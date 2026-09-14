# 自作ツール一覧

本プロジェクトの解析過程で作成した Android アプリ／スクリプト群です。いずれも DS-MDA001 実機上で動作させることを前提にしています。

---

## AlcoholTestDebugger

アルコールセンサー基板と直接会話するためのデバッグ用 Android アプリ (Kotlin / Jetpack Compose)。

**できること**

- `/dev/ttyMSM1` を開いて、[コマンドカタログ](alcohol_checker/serial_protocol.md#3-command-catalog) の全コマンドを手動送信
- 標準測定シーケンス（`startTest` → `findAlcoholAD` → `blow` → `exitBlow` → `AlcoholContent`）の自動実行と状態表示
- Passive Monitor（`passiveTest` + `findAlcoholAD` のループ）による常時監視と最大値保持
- センサー値の 1 秒周期オートポーリング
- **TCP ブリッジ (`0.0.0.0:8000`)** — PC から生のフレームを投げて生の応答を受け取る。プロトコル解析はほぼこれで行いました

**元アプリからの流用**

JNI シグネチャの整合性を保つため、`com.gzds.utils.CSerialPort` と `com.hikvision.alcoholtest.comassistant.SerialHelper` / `util.OrderTools` / `util.DataUtils` は元 APK のパッケージ名のまま取り込んでいます。

**PC 側スクリプト** (`tools/tcp_tests/`)

| スクリプト | 用途 |
| :--- | :--- |
| `log_breath_test.py` | 呼気テスト中の全フレームを CSV へ記録（[AD値解析](alcohol_checker/alcohol_ad_values.md) の元データ） |
| `test_passive.py` | パッシブ測定サイクルの挙動確認 |
| `test_async.py` | 非同期応答（`flag=0` 中間応答）のレイテンシ測定 |
| `test_blow_interleave.py` / `test_interleave.py` | 吹込中の並行コマンド送信によるフリーズ再現 |
| `parse_responses.py` | 収集したフレームのデコード |
| `test_ordertools.py` | コマンド生成・LRC 計算の検証 |

!!! warning "TCP ブリッジ使用時の注意"
    アプリ内部の自動ポーリングが動いたままだと、その応答が TCP クライアント向けストリームに混入します。クライアント接続中は内部ポーリングを Mutex で排他停止するようにしてあります。詳細は [実測挙動](alcohol_checker/measurement_behavior.md) を参照してください。

---

## PrinterServerApp

内蔵サーマルプリンターをネットワーク経由・ローカル双方から叩くためのハイブリッドアプリ。

- **NanoHTTPD サーバー (ポート 8080)** — `POST /api/print` で Base64 画像を直接印刷、`/api/project` でレイアウト JSON を保存・読込
- **Fabric.js ベースの Web レイアウトエディタ** — PPT 感覚でテキスト・画像を配置し、2値化（Threshold / Bayer / Floyd–Steinberg）をブラウザ上でリアルタイムプレビュー
- **輪郭抽出ブレンド** — ラプラシアンでエッジを抽出し、ディザ前に重み付き合成してシャープな印字を得る
- **電圧降下シミュレーション** — 1 ラインあたりの黒画素率が 30% を超えると薄く描画し、実機のかすれを事前に可視化（プレビュー専用）
- **フォアグラウンドサービス常駐** — 画面を閉じてもサーバーとプリンタ接続を維持

詳細な開発記録とハマりどころは [PrinterServerApp 開発記録](printer/print_server.md) を参照してください。

---

## MDA001Tweak

端末側の設定を書き換えるためのツールアプリ（開発中）。

`AndroidManifest.xml` で `android:sharedUserId="android.uid.shell"` を宣言し `WRITE_SECURE_SETTINGS` を要求するため、**プラットフォーム鍵での署名が必要**です。本端末はブートローダーアンロック済み・`Permissive` のため、抽出したプラットフォーム鍵 (`platform.pk8` / `platform.x509.pem`) で署名すればそのままインストールできます。

現時点では UI スケルトンのみで、機能実装はこれからです。
