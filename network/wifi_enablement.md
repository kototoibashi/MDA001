# 📶 16. Wi-Fi 機能の完全恒久有効化・復旧手順書 (Final Report)

本ドキュメントは、メーカーによってシステムレベルで無効化されていた本端末（`********`）の Wi-Fi 機能を、**完全な恒久有効化** に成功した際の全手順とトラブルシューティングの記録です。（※標準の設定アプリのメニューには復活しませんが、サードパーティのランチャーアプリ等から直接設定画面を呼び出すことで自由にON/OFF可能です）

---

## 📌 1. Wi-Fi 無効化の全貌と 5 つの障壁
調査の結果、本端末ではソフトウェア（OS）レベルでの4重のロックに加え、物理的なハードウェアの制約という**合計5つの障壁**によってWi-Fi機能が封じられていました。

1. **UI の隠蔽 (ソフトウェア)**: `ro.radio.noril=true` やその他システムレベルの設定により、設定アプリ（Settings.apk）から通信系のメニューが完全に消去されている。（※後述の通り、プロパティを修正しても標準メニューは復活しないため、ActivityLauncher等による直接呼び出しが必要となります）
2. **HAL 許可リストからの除外 (ソフトウェア)**: `manifest.xml` から Wi-Fi 関連の HAL が削除されており、フレームワークが Wi-Fi デバイスにアクセスしようとすると `hwservicemanager` に通信をブロックされる。
3. **サービス定義の削除 (ソフトウェア)**: `wpa_supplicant`（Wi-Fi制御デーモン）を起動するための `wpa_supplicant.rc` が `/vendor/etc/init/` から削除されており、システムが Wi-Fi をオンにしようとしてもプロセスを起動できない。
4. **ディレクトリ構造の欠落 (ソフトウェア)**: `/data/vendor/wifi` など、`wpa_supplicant` が動作するために必須となる設定ファイルディレクトリが存在しない。
5. **アンテナの非搭載 (ハードウェア)**: Wi-Fiモジュール自体は基板上に存在するものの、コネクタのみで**アンテナ本体が物理的に搭載されておらず**、ソフトウェア側を復旧しても実用的な距離で電波を拾うことができない。

---

## 🚀 2. 完全恒久有効化のための全ステップ（実行手順）

以下の手順を順に実行することで、ゼロから完全に Wi-Fi を復旧させることができます。（※すべて `adb root` および `adb remount` で `/vendor` を書き込み可能にしてから実行します）
### Step 1: プロパティの修正と機内モード解除
起動時に強制的に無効化プロパティを注入するスクリプトを無効化し、システム全体の通信遮断を解除します。

1. **`/vendor/bin/init.class_main.sh` の修正** (Line 40付近)
   ```bash
   # 修正前: setprop ro.vendor.radio.noril yes
   # 修正後:
   setprop ro.vendor.radio.noril no
   ```
2. **機内モードの解除**
   ```powershell
   adb shell settings put global airplane_mode_on 0
   ```

> [!NOTE]
> プロパティを修正しても、標準の「設定 (Settings)」アプリ内に Wi-Fi メニューは表示されません。Wi-Fi の設定画面を開くには、PlayストアやAPKから「Activity Launcher」等のアプリをインストールし、`com.android.settings.Settings$WifiSettingsActivity` などを直接起動してください。

### Step 2: データディレクトリの作成と権限設定
`wpa_supplicant` が動作するためのディレクトリと設定ファイル（空）を作成し、Wi-Fi ユーザー (`wifi:wifi`) に権限を付与します。

```powershell
adb shell mkdir -p /data/vendor/wifi/wpa/sockets
adb shell cp /vendor/etc/wifi/wpa_supplicant.conf /data/vendor/wifi/wpa/wpa_supplicant.conf
adb shell chown -R wifi:wifi /data/vendor/wifi
adb shell chmod 770 /data/vendor/wifi
adb shell chmod 770 /data/vendor/wifi/wpa
adb shell chmod 660 /data/vendor/wifi/wpa/wpa_supplicant.conf
```

### Step 3: VINTF Manifest への Wi-Fi HAL 登録
`/vendor/etc/vintf/manifest.xml` を開き、ファイルの**最後にある `<sepolicy>` タグの直前**に、以下の Wi-Fi 関連 HAL 宣言を挿入します。

> [!CAUTION]
> **XMLスキーマの厳密な順序規則**
> `<hal>` タグは必ず `<sepolicy>` タグよりも前に配置する必要があります。`<sepolicy>` の後ろに追記すると、XML全体がパースエラーとなり、`vold` 等が連鎖クラッシュして致命的なブートループに陥ります。

```xml
    <!-- Wi-Fi HAL -->
    <hal format="hidl">
        <name>android.hardware.wifi</name>
        <transport>hwbinder</transport>
        <version>1.0</version>
        <interface>
            <name>IWifi</name>
            <instance>default</instance>
        </interface>
        <fqname>@1.0::IWifi/default</fqname>
    </hal>
    <!-- Wi-Fi Supplicant HAL -->
    <hal format="hidl">
        <name>android.hardware.wifi.supplicant</name>
        <transport>hwbinder</transport>
        <version>1.1</version>
        <interface>
            <name>ISupplicant</name>
            <instance>default</instance>
        </interface>
        <fqname>@1.1::ISupplicant/default</fqname>
    </hal>
    <!-- Wi-Fi Hostapd HAL (テザリング用) -->
    <hal format="hidl">
        <name>android.hardware.wifi.hostapd</name>
        <transport>hwbinder</transport>
        <version>1.0</version>
        <interface>
            <name>IHostapd</name>
            <instance>default</instance>
        </interface>
        <fqname>@1.0::IHostapd/default</fqname>
    </hal>
    <sepolicy>
        <!-- 既存のタグ -->
```

### Step 4: `wpa_supplicant` サービス定義 (`.rc`) の復元
`init` プロセスが `wpa_supplicant` を起動できるように、削除されていた `.rc` ファイルを新規作成します。

1. **`/vendor/etc/init/wpa_supplicant.rc` の作成**
   ```rc
   service wpa_supplicant /vendor/bin/hw/wpa_supplicant \
       -O/data/vendor/wifi/wpa/sockets -pclass=main -g@android:wpa_wlan0
       interface android.hardware.wifi.supplicant@1.0::ISupplicant default
       interface android.hardware.wifi.supplicant@1.1::ISupplicant default
       interface android.hardware.wifi.supplicant@1.2::ISupplicant default
       interface vendor.qti.hardware.wifi.supplicant@2.0::ISupplicantVendor default
       interface vendor.qti.hardware.wifi.supplicant@2.1::ISupplicantVendor default
       class main
       socket wpa_wlan0 dgram 660 wifi wifi
       disabled
       oneshot
   ```
2. **権限の設定**
   ```powershell
   adb shell chmod 644 /vendor/etc/init/wpa_supplicant.rc
   ```

### Step 5: (超重要) 手動ドライバロードスクリプトの削除
過去の検証で `/vendor/etc/init/init.radio_enable.rc` などに `insmod /vendor/lib/modules/pronto_wlan.ko` を記述していた場合は、**必ず削除**します。
Android の仕様では、Wi-Fi を ON にした瞬間に Wi-Fi HAL 自らがドライバを動的にロードします。起動時に手動でロードしてしまうと、HAL がドライバをロードした際に「File exists（既に存在する）」エラーとなり、Wi-Fi が即座に強制 OFF されてしまいます。

---

## 🛠️ トラブルシューティング（発生した問題と解決策）

### 1. `wpa_supplicant` 起動 5秒タイムアウト問題
* **症状**: UI でトグルを ON にすると、5秒間 `Turning on...` になり OFF に戻る。
* **原因**: `wpa_supplicant.rc` が欠落していたため、Framework の起動要求に対して何も起きずタイムアウトしていた。
* **解決**: Step 4 の通り `.rc` ファイルを新規作成して再起動。

### 2. VINTF パースエラーによるブートループ
* **症状**: 起動ロゴのまま先に進まず、裏でプロセスがクラッシュループする。
* **原因**: `manifest.xml` に追記した Wi-Fi HAL タグを、XML の末尾（`<sepolicy>` の後ろ）に書いてしまったためのスキーマ順序違反。
* **解決**: ブートループ中は `adb shell` に入り `stop` コマンドで暴走サービスを止め、`mount -o rw,remount /vendor` で書き込み権限を得て、`<sepolicy>` の前に正しく書いた `manifest.xml` をプッシュして復旧。

### 3. トグル ON 直後の即落ち（File exists）問題
* **症状**: UI で Wi-Fi を ON にすると一瞬で OFF に戻る。
* **原因**: 起動時スクリプトで `pronto_wlan.ko` を強制ロードしていたため、Wi-Fi HAL が自身でロードしようとして競合エラー (`finit_module return: -1: File exists`) が発生。
* **解決**: 強制ロード用の `init.radio_enable.rc` を削除し、ドライバ管理を完全に Android に委譲した。

---

## 📡 ハードウェアに関する重要事項: アンテナの追加

OS上の設定でWi-Fiの有効化に成功しても、本端末の基板には初期状態で**アンテナ本体が搭載されていません**（コネクタのみ実装されています）。
ルーター至近距離以外で実用的な電波を送受信するためには、汎用のIPEX/U.FLアンテナを自前で取り付ける必要があります。
詳細については、[ハードウェア構成: RFアンテナ物理実装状況](../hardware/components.md#5-rf-) を参照してください。
