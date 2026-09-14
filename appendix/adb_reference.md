# ADB コマンドリファレンス

本端末の調査・デバッグでよく使う ADB コマンドをまとめた実用リファレンスです。

本端末は `userdebug` ビルドかつ `su` が最初から入っているため、`adb root` / `adb remount` がそのまま通ります。USB を挿すと ADB と RNDIS が自動で有効になるため、無線が死んでいても PC からは常にフルアクセスできます。

!!! tip "シリアル番号の指定"
    以下の例では `-s <SERIAL>` を明示していますが、端末が 1 台しか繋がっていない場合は省略できます。

---

## 1. 基本操作

```powershell
# 接続確認
adb devices -l

# シェル
adb shell

# root 化 & /system, /vendor を書き込み可能に
adb root
adb remount
```

## 2. ログ取得

```powershell
# リアルタイムログ
adb logcat -v time

# ファイルへ保存
adb logcat -v time > logcat.txt

# クラッシュのみ
adb logcat -b crash

# 無線まわりのログ（セルラー調査用）
adb logcat -b radio

# カーネルログ
adb shell dmesg > dmesg.log
```

## 3. 画面操作・スクリーンショット

```powershell
adb shell screencap -p /sdcard/screen.png
adb pull /sdcard/screen.png .

# 画面録画
adb shell screenrecord /sdcard/rec.mp4
```

## 4. アプリ・アクティビティ管理

```powershell
# 最前面のアプリ/アクティビティ
adb shell "dumpsys window | grep -E 'mCurrentFocus|mFocusedApp'"

# サードパーティアプリ一覧
adb shell pm list packages -3

# 全パッケージ（インストールパス付き）
adb shell pm list packages -f

# APK パスの確認と取得
adb shell pm path com.hikvision.alcoholtest
adb pull /data/app/com.hikvision.alcoholtest-<hash>/base.apk ./alcoholtest.apk

# 設定画面など、ランチャーから消されている Activity を直接起動する
adb shell am start -n com.android.settings/.Settings\$WifiSettingsActivity
```

!!! note "設定メニューが消えている件"
    本端末は Settings.apk から通信系メニューが削除されています。上記の `am start` や Activity Launcher 系アプリで直接 Activity を叩くのが定石です。詳細は [Wi-Fi 恒久有効化手順](../network/wifi_enablement.md) を参照してください。

## 5. ハードウェア・センサー状態

```powershell
# I2C デバイス一覧（Goodix タッチ、PCF8563 RTC、AW2013 LED などが見える）
adb shell "ls -l /sys/bus/i2c/devices/"

# 入力イベントのモニタリング（物理ボタンの死活確認に有用）
adb shell getevent -lt

# GPIO の状態（ホームボタンの GPIO 88 特定に使用）
adb shell cat /sys/kernel/debug/gpio

# バッテリー状態
adb shell dumpsys battery

# ロード中カーネルモジュール
adb shell cat /proc/modules
```

## 6. シリアルポート / プリンター

```powershell
# アルコールセンサー基板の UART
adb shell ls -l /dev/ttyMSM1

# サーマルプリンターのキャラクタデバイス
adb shell ls -l /dev/prn-dev

# 権限が足りない場合（本端末は最初から 777 のことが多い）
adb shell su -c "chmod 777 /dev/ttyMSM1 /dev/prn-dev"
```

プロトコルの詳細は [シリアル通信プロトコル](../alcohol_checker/serial_protocol.md) と [プリンター制御仕様](../printer/printer.md) を参照してください。

## 7. セキュリティ状態の確認

```powershell
adb shell getenforce                        # -> Permissive
adb shell getprop ro.boot.veritymode        # -> disabled
adb shell getprop ro.boot.verifiedbootstate # -> orange (unlocked)
adb shell getprop ro.build.type             # -> userdebug
```

詳細な評価は [セキュリティ状態](../software/security.md) を参照してください。

## 8. バックアップ

パーティションの RAW ダンプなど、本格的なバックアップ手順は [バックアップ手順](../software/backup_strategy.md) にまとめています。

```powershell
# ポートフォワード + nc による高速ダンプ（約33MB/s）
adb forward tcp:9999 tcp:9999
```
