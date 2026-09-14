# 🚫 17. Bluetooth 完全恒久有効化 失敗レポートおよび詳細解析 (Failed Attempt)

本ドキュメントは、メーカーによって無効化されていた Bluetooth 機能を復旧する試みが失敗に終わった原因と、その技術的な詳細解析の記録です。

---

## 📌 1. 事前調査で判明していた事実

Wi-Fi の恒久有効化（[wifi_enablement.md](./wifi_enablement.md) 参照）の成功例に倣い、Bluetooth の状態を調査した結果、以下の「希望が持てる状態」であることが確認できていました。

1. **Bluetooth HAL は存在**: `/vendor/etc/vintf/manifest.xml` に `android.hardware.bluetooth` などの記述が既に存在する。
2. **Bluetooth サービスは稼働中**: `/vendor/etc/init/android.hardware.bluetooth@1.0-service-qti.rc` が存在し、バックグラウンドプロセスとして `vendor.bluetooth-1-0-qti` が正常稼働している。
3. **設定ディレクトリが存在**: `/data/misc/bluetooth` ディレクトリが適切な権限（`bluetooth:bluetooth`）で既に存在している。

**唯一の欠落要素**として、AndroidフレームワークとBluetoothハードウェアを繋ぎ、UIを提供する大元のシステムアプリである **`Bluetooth.apk` (`com.android.bluetooth`) がシステムから物理的に削除（完全消去）されている**ことが判明しました。

---

## 🚀 2. 実施した復旧アプローチ (GSIからの移植)

オリジナルファームウェアのバックアップが存在しなかったため、同じ Android 10 (arm64) の **AOSP Generic System Image (GSI)** から標準の Bluetooth アプリを抽出し、移植するアプローチを取りました。

1. `phhusson` 氏の Android 10 GSI (`system-quack-arm64-aonly-vanilla.img.xz`) をダウンロード。
2. `7-Zip` を用いて EXT4 イメージ内から以下の2ファイルを抽出。
   - `/app/Bluetooth/Bluetooth.apk`
   - `/lib64/libbluetooth_jni.so`
3. 端末の `/system` 領域を Read-Write でマウントし、上記ファイルをプッシュ。
4. 適切な権限（chmod 644）を付与し、端末を再起動。

結果として、OSは新しく追加された `com.android.bluetooth` パッケージを認識し、設定メニューから Bluetooth のトグルUIにアクセスできるようになりました。

---

## 💥 3. 発生した障害 (詳細解析)

Bluetooth を ON にした直後、「Bluetooth が繰り返し停止しています」というクラッシュダイアログが発生しました。
`adb logcat -b crash` を用いてクラッシュダンプを詳細解析した結果、以下の致命的なエラー（FATAL EXCEPTION）が記録されていました。

```log
08-06 22:33:31.010  3979  3979 E AndroidRuntime: FATAL EXCEPTION: main
08-06 22:33:31.010  3979  3979 E AndroidRuntime: Process: com.android.bluetooth, PID: 3979
08-06 22:33:31.010  3979  3979 E AndroidRuntime: java.lang.UnsatisfiedLinkError: dlopen failed: library "libbluetooth-binder.so" not found
08-06 22:33:31.010  3979  3979 E AndroidRuntime: 	at java.lang.Runtime.loadLibrary0(Runtime.java:1071)
08-06 22:33:31.010  3979  3979 E AndroidRuntime: 	at com.android.bluetooth.btservice.AdapterApp.<clinit>(AdapterApp.java:36)
```

### 🔍 エラーの根本原因 (Root Cause)

この `UnsatisfiedLinkError` は、GSIから抽出した汎用的な `Bluetooth.apk` が起動時に **`libbluetooth-binder.so`** という共有ライブラリをロードしようとして失敗したことを示しています。

端末内の共有ライブラリ構成を再調査（`find /system /vendor -name "*bluetooth*.so"`）したところ、本端末特有の致命的な事実が判明しました。

*   **AOSP (標準のAndroid) の仕様**:
    標準の Android では、Bluetooth スタックとして `libbluetooth.so` および `libbluetooth-binder.so` が使用されます。
*   **本端末 (Qualcomm SoC) の仕様**:
    本端末は Qualcomm のプロプライエタリな Bluetooth スタックを採用しており、システム内には **`libbluetooth_qti.so`** というカスタマイズされたライブラリしか存在しません。（※プロパティ `ro.bluetooth.library_name=libbluetooth_qti.so` にも明記されています）

**結論**:
移植した汎用的な `Bluetooth.apk` は「標準の AOSP ライブラリ」を要求して設計されていますが、本端末のファームウェアは「Qualcomm 独自の Bluetooth ライブラリ (`libbluetooth_qti.so`)」専用に構築されています。この **JNI (Java Native Interface) レイヤーの依存関係の不一致**が原因で、アプリがネイティブライブラリをロードできず即座にクラッシュしています。

---

## 🚫 4. 結論と今後の展望

今回のアプローチでは、**Qualcomm 独自実装と AOSP 標準実装の間のバイナリ互換性の壁**により、Bluetooth の有効化は失敗に終わりました。

Wi-Fi の場合は、Android フレームワークの標準コードで十分動作する範囲だったため設定ファイルの復元のみで成功しましたが、Bluetooth においてはベンダー（Qualcomm）固有の拡張が深く入り込んでいるため、単純な APK のポン付けでは動作しません。

### 今後 Bluetooth を復旧するための現実的な選択肢

1.  **同型機・同ファームウェアからの完全なバックアップの入手**:
    この端末本来の `Bluetooth.apk` (Qualcomm 独自ライブラリを参照するようにコンパイルされたもの) を探し出し、プッシュするしかありません。
2.  **Qualcomm 製 Android 10 (Snapdragon 625 世代) の他機種 ROM からの流用**:
    他メーカーであっても、同じく Qualcomm のスタック (`libbluetooth_qti.so`) を使用している Android 10 機種（例: Xiaomi等の MIUI ベース等）から `Bluetooth.apk` を抽出すれば、依存関係が一致し動作する可能性が僅かに残されています。

---

## 📡 ハードウェアに関する重要事項: アンテナの追加

ソフトウェア・OS上の設定でBluetoothの有効化に成功しても、本端末の基板には初期状態で**アンテナ本体が搭載されていません**（コネクタのみ実装されています）。
実運用において他のBluetooth機器と通信するためには、汎用のIPEX/U.FLアンテナを自前で取り付ける必要があります。
詳細については、[ハードウェア構成: RFアンテナ物理実装状況](../hardware/components.md#5-rf-) を参照してください。
