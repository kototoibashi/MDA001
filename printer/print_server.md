# PrinterServerApp 開発記録とトラブルシューティング

## 概要
専用端末に内蔵された感熱式プリンター (`/dev/prn-dev`) を、ネットワーク経由（PCブラウザ等）およびローカルで制御するためのハイブリッドAndroidアプリ `PrinterServerApp` の開発記録です。

元の業務アプリ (`com.hikvision.alcoholtest`) からプリンター制御用のネイティブライブラリとJNIラッパーを抽出し、HTTPサーバーと高度な画像2値化処理を持つPPT風Webエディタを新規に構築しました。

プロトコル自体の仕様は [プリンター制御仕様](printer.md)、実践的な印字ノウハウは [印字ノウハウ・ハマりどころ](printing_tips.md) を参照してください。

## 1. プロジェクト構造と移植内容

### ネイティブライブラリの移植
- **JNIラッパー**: `CPrint.java`, `CSerialPort.java` を元APKから抽出し、JNIのシグネチャ整合性を保つために元のパッケージ名 `com.gzds.utils` のままプロジェクトに組み込みました。
- **SOファイル**: 抽出した `libgzds_utils.so` を `app/src/main/jniLibs/armeabi-v7a/` に配置しました。
- **ハードウェアアクセス**: `CSerialPort` 内部で `su` コマンドを用いて `/dev/prn-dev` に対する `chmod 777` が実行され、直接キャラクタデバイスへデータを送信できるアーキテクチャになっています。

### 新規構築機能
- **NanoHTTPD サーバー (`PrinterHttpServer.kt`)**:
  - ポート `8080` で動作。
  - `GET /api/projects`, `POST /api/project`: JSON形式でのレイアウトプロジェクト保存・読み込み。
  - `POST /api/print`: Base64エンコードされた画像の直接印刷（1bit / 384px）。
  - `POST /api/print_raw_gray`: 16階調グレースケール印刷（240px）。
  - `assets/web/` 内の静的Webアセットのホスティング。
- **Webレイアウトエディタ (`Fabric.js`)**:
  - PPT感覚でテキストや画像を自由に配置できるHTML5 Canvasベースのエディタ。
  - 感熱紙の白黒印刷に最適化するため、画像ごとに「単純2値化(Threshold)」「ベイヤー(Bayer)」「フロイド-スタインバーグ(Floyd-Steinberg)」アルゴリズムをJS上で適用し、リアルタイムプレビュー可能にしました。
- **ローカルWebView (`WebEditorActivity.kt`)**:
  - Android端末単体でもローカルホスト (`http://localhost:8080/`) のエディタをWebViewで操作できるように構成。

---

## 2. 開発中に発生した問題と解決策 (トラブルシューティング)

### ① ネイティブライブラリ読み込みエラー (UnsatisfiedLinkError)
**現象:**
アプリ起動時に `java.lang.UnsatisfiedLinkError: couldn't find "libgzds_utils.so"` が発生。

**原因:**
端末が64-bit (arm64-v8a) であり、アプリプロセスがデフォルトで64-bitとして起動したため。抽出したSOファイルは古い32-bit (`armeabi`) であったため、64-bitプロセスからはロードできませんでした。

**解決策:**
`build.gradle.kts` の `defaultConfig` ブロックに ABI フィルタリングを明示的に追加し、アプリが32-bit環境で動作するように強制しました。また、非推奨の `armeabi` フォルダを `armeabi-v7a` にリネームしました。
```kotlin
ndk {
    abiFilters.add("armeabi-v7a")
}
```

### ② WebView での ERR_CLEARTEXT_NOT_PERMITTED
**現象:**
Androidアプリ内の WebView で `http://localhost:8080/` を開こうとするとエラーが発生し、画面が表示されない。

**原因:**
Android 9 (API 28) 以降、デフォルトでHTTP（暗号化されていないCleartextトラフィック）の通信がOSレベルでブロックされる仕様になっているため。

**解決策:**
`AndroidManifest.xml` の `<application>` タグに `android:usesCleartextTraffic="true"` を追加し、ローカルHTTPへのアクセスを許可しました。

### ③ Fabric.js で画像が真っ白（透明）になる問題
**現象:**
レイアウトエディタ上で画像をロードし2値化フィルタをかけると、印刷自体は正常にされるのに、ブラウザの Canvas 上では画像が真っ白（見えない状態）になってしまう。

**原因:**
1. Fabric.js (v5) はカスタムフィルタの処理にデフォルトで WebGL (GPU) を使おうとしますが、提供した処理は 2D CPU向け (`applyTo2d`) のみだったため、WebGL側での描画が空回りして透明としてレンダリングされていました。
2. また、元画像のアルファチャンネル（透明度）の処理が考慮されていなかったため、透過PNGなどが黒ベタになってしまう可能性がありました。

**解決策:**
- `filters.js` の冒頭で、フィルタバックエンドを明示的に Canvas2D に固定しました。
  ```javascript
  fabric.filterBackend = new fabric.Canvas2dFilterBackend();
  ```
- `applyTo2d` 関数内で、アルファ値が低い（透明な）ピクセルを「白 (255, 255, 255)」に変換し、全てのピクセルのアルファ値を強制的に 255 (不透明) に設定する処理を追加しました。

---

## 3. 2つの印字パスの実装

`PrintUtil.kt` は、[プリンター制御仕様](printer.md#4) の 1bit / グレースケールの 2 系統をそれぞれ実装しています。切り替えは `PrinterSettings.isGradientMode`（設定画面のトグル、既定 OFF）です。

### 3.1 1bit パス — できるだけ大きなチャンクで流し込む

```kotlin
val chunkHeight = bitmap.width * 4          // 384 * 4 = 1536 行
while (y < bitmap.height) {
    val currentHeight = minOf(chunkHeight, bitmap.height - y)
    Bitmap.createBitmap(bitmap, 0, y, bitmap.width, currentHeight).use { chunk ->
        res = CPrint.PrintWaterMarkBmp(chunk, "", 0, 24)
    }
    y += currentHeight
    while (CPrint.getPrinterStatus() == 1) { Thread.sleep(50) }
}
```

1 回の `write(2)` で流し込む最大サイズは **1536 行 × 48 バイト + 5 = 73,733 バイト**になります。`Command` の呼び出しを分割すると継ぎ目に横線が出るため、チャンクはできる限り大きく取っています（[印字ノウハウ §1](printing_tips.md)）。

### 3.2 グレースケールパス (`printGradientNative`)

```kotlin
val targetWidth = 240                       // グラデーション印刷はプリンタ仕様で 240px 固定
val widthBytes  = (width + 1) / 2           // = 120 バイト／ライン
val chunkLength = targetWidth * 2           // = 480 行（ディザ・ステータス監視の単位）
...
for (y in 0 until currentChunkHeight) {
    System.arraycopy(chunkBytes, y * widthBytes, sendBytes, 0, widthBytes)
    CPrint.sendPrintPictureData(widthBytes, sendBytes)   // 1 ライン = 120 バイト
}
```

こちらはネイティブ側でペイロード長 120 バイトが固定されているため、**1 スキャンラインずつしか送れません**。`chunkLength` はまとめ送りのためではなく、Floyd–Steinberg 誤差拡散とステータス監視の処理単位です。

!!! note "元コードのニブルオーバーフローを修正済み"
    元アプリの `ImageUtils.java` は `luma / 15` が 0〜17 を返してニブルを溢れさせます。本実装では `(adjustedLuma / 15).coerceIn(1, 15)` としてクランプしています。下限を 1 にしているため、グラデーションモードでは純白が出力されない点に注意してください。

---

## 4. 今後の拡張アイデア
- **フォントの追加**: プレビュー環境(Web)と実際の印刷(Android Canvas)でフォントのレンダリング差を出さないため、同一のカスタムTTFフォントを組み込む。
- **用紙幅の動的変更**: 1bit パスの 384px はネイティブの逆アセンブルで確定済み（[根拠](printer.md#raster-1bit)）だが、設定画面から扱えるようにしておきたい。
- **QRコード生成**: Webフロントエンド側に `qrcode.js` 等を導入し、エディタ上で直接QRコードを生成・配置する機能。
