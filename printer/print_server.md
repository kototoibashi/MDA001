# 18. PrinterServerApp 開発とトラブルシューティング記録

## 概要
専用端末に内蔵された感熱式プリンター (`/dev/prn-dev`) を、ネットワーク経由（PCブラウザ等）およびローカルで制御するためのハイブリッドAndroidアプリ `PrinterServerApp` の開発記録です。

元の業務アプリ (`com.hikvision.alcoholtest`) からプリンター制御用のネイティブライブラリとJNIラッパーを抽出し、HTTPサーバーと高度な画像2値化処理を持つPPT風Webエディタを新規に構築しました。

## 1. プロジェクト構造と移植内容

### ネイティブライブラリの移植
- **JNIラッパー**: `CPrint.java`, `CSerialPort.java` を元APKから抽出し、JNIのシグネチャ整合性を保つために元のパッケージ名 `com.gzds.utils` のままプロジェクトに組み込みました。
- **SOファイル**: 抽出した `libgzds_utils.so` を `app/src/main/jniLibs/armeabi-v7a/` に配置しました。
- **ハードウェアアクセス**: `CSerialPort` 内部で `su` コマンドを用いて `/dev/prn-dev` に対する `chmod 777` が実行され、直接キャラクタデバイスへデータを送信できるアーキテクチャになっています。

### 新規構築機能
- **NanoHTTPD サーバー (`PrinterHttpServer.kt`)**:
  - ポート `8080` で動作。
  - `GET /api/projects`, `POST /api/project`: JSON形式でのレイアウトプロジェクト保存・読み込み。
  - `POST /api/print`: Base64エンコードされた画像の直接印刷。
  - `assets/web/` 内の静的Webアセットのホスティング。
- **Webレイアウトエディタ (`Fabric.js`)**:
  - PPT感覚でテキストや画像を自由に配置できるHTML5 Canvasベースのエディタ。
  - 感熱紙の白黒印刷に最適化するため、画像ごとに「単純2値化(Threshold)」「ベイヤー(Bayer)」「フロイド-スタインバーグ(Floyd-Steinberg)」アルゴリズムをJS上で適用し、リアルタイムプレビュー可能にしました。
- **ローカルWebView (`WebEditorActivity.kt`)**:
  - Android端末単体でもローカルホスト (`http://localhost:8080/`) のエディタをWebViewで操作できるように構成。

---

## 2. 開発中に発生した問題と解決策 (トラブルシューティング)

### ① ネイティブラブラリ読み込みエラー (UnsatisfiedLinkError)
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

## 3. 今後の拡張アイデア
- **フォントの追加**: プレビュー環境(Web)と実際の印刷(Android Canvas)でフォントのレンダリング差を出さないため、同一のカスタムTTFフォントを組み込む。
- **用紙幅の動的変更**: プリンターのピクセル幅 (今回384pxと推測) を動的に変更できるようにする設定画面の追加。
- **QRコード生成**: Webフロントエンド側に `qrcode.js` 等を導入し、エディタ上で直接QRコードを生成・配置する機能。
