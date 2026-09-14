# バックアップ戦略・手順ガイド (Backup Strategy Guide)

本ドキュメントでは、本端末（`********`）の特性（`userdebug` ビルド・`Permissive` モード・Qualcomm MSM8953）に最適なバックアップ手法をまとめています。

---

## 🎯 目的別の最適なバックアップ手法まとめ

| アプローチ | バックアップ対象 | 手法 | 特徴・メリット |
| :--- | :--- | :--- | :--- |
| **1. 完全RAWパーティション (完全バックアップ)** | 全パーティション (`system`, `vendor`, `boot`, `userdata` 等) | `adb shell dd` パイプ出力 | 端末が文鎮化・破損しても100%完全に元の状態へ完全復元可能 |
| **2. NV / EFS領域 (端末固有データ保護)** | `modemst1`, `modemst2`, `fsg`, `persist` | `dd` または QPST (Qualcomm Diag) | シリアル、IMEI、MACアドレス、無線・モジュール調整パラメータの安全な保護 |
| **3. アプリ・データ (設定・DB解析用)** | `/data/data/com.hikvision.alcoholtest` 等 | `adb pull` | **APKは既に179個全抽出済み**。アプリ内部DBや設定ファイルの保存 |

---

## 🛠️ 各バックアップの具体的実行方法

### 1. 完全RAWパーティション・バックアップ

端末のストレージを消費せず、PC側のHDD/SSDへ直接RAWイメージを書き出します。

> [!IMPORTANT]
> **PowerShellにおけるバイナリ書き出しの注意点**:
> PowerShellの標準リダイレクト（`>`）を使うと、バイナリデータがUTF-16等にエンコードされて破損するかファイルサイズが倍加します。
> **`cmd /c`** 経由でリダイレクトするか、以下の **TCP Pipe方式（推奨・最速）** を使用してください。

---

#### 🚀【最速】TCP Pipe方式（`adb forward` + `nc`） — **約33.4 MB/s (標準ADBの約3.7倍速)**

RNDIS（USBテザリング）を設定しなくても、ADBポートフォワードと端末内の `nc` (netcat) を組み合わせることで、USB 2.0の物理限界に近い転送速度が得られます。

```powershell
# 1. ADBポートフォワード設定
.\tools\platform-tools\adb.exe -s ******** forward tcp:9999 tcp:9999

# 2. 端末側で待機＆PC側で受信用PowerShellワンライナー例 (bootイメージ例)
$job = Start-Job -ScriptBlock {
    .\tools\platform-tools\adb.exe -s ******** shell "nc -l -p 9999 < /dev/block/bootdevice/by-name/boot"
}
Start-Sleep -Milliseconds 300
$client = New-Object System.Net.Sockets.TcpClient("127.0.0.1", 9999)
$stream = $client.GetStream()
$fs = [System.IO.File]::Create(".\backup_images\boot.img")
$stream.CopyTo($fs); $fs.Close(); $client.Close()
Remove-Job $job -Force
```

---

#### ℹ️ 標準 `cmd /c` 方式 — **約9.0 MB/s**

```powershell
# 保存先フォルダの作成
New-Item -ItemType Directory -Path ".\backup_images" -Force

# boot (カーネル)
cmd /c ".\tools\platform-tools\adb.exe -s ******** exec-out dd if=/dev/block/bootdevice/by-name/boot bs=1M > .\backup_images\boot.img"

# recovery (リカバリ)
cmd /c ".\tools\platform-tools\adb.exe -s ******** exec-out dd if=/dev/block/bootdevice/by-name/recovery bs=1M > .\backup_images\recovery.img"

# system (OS本体)
cmd /c ".\tools\platform-tools\adb.exe -s ******** exec-out dd if=/dev/block/bootdevice/by-name/system bs=1M > .\backup_images\system.img"

# vendor (Qualcomm/モジュール依存領域)
cmd /c ".\tools\platform-tools\adb.exe -s ******** exec-out dd if=/dev/block/bootdevice/by-name/vendor bs=1M > .\backup_images\vendor.img"
```

> **💡 `conv=sync,noerror` オプションについての補足**:
> * **`noerror`**: 読み込みエラー（不良セクタ等）が発生しても処理を中断せず続行します。
> * **`sync`**: 不良セクタ等で読み込めなかったブロックを NUL バイト (`0x00`) でパディング補填し、イメージ全体のオフセット（アドレスのズレ）を防止します。
> 正常なeMMC領域のバックアップでは通常エラーは発生しませんが、破損領域がある場合の安全策として `conv=sync,noerror` を併用するのがLinux/Androidバックアップのベストプラクティスです。


---

### 2. Qualcomm EFS / 固有パラメータのバックアップ (最重要保護対象)

万一の際、端末固有のMACアドレスやNVデータ破損を防ぐため、以下のコマンドでEFS関連パーティションを必ずバックアップしておくことを強く推奨します。

```powershell
# EFS / NV パラメータ
.\tools\platform-tools\adb.exe -s ******** shell "dd if=/dev/block/bootdevice/by-name/modemst1 bs=4096" > .\backup_images\modemst1.img
.\tools\platform-tools\adb.exe -s ******** shell "dd if=/dev/block/bootdevice/by-name/modemst2 bs=4096" > .\backup_images\modemst2.img
.\tools\platform-tools\adb.exe -s ******** shell "dd if=/dev/block/bootdevice/by-name/fsg bs=4096" > .\backup_images\fsg.img
.\tools\platform-tools\adb.exe -s ******** shell "dd if=/dev/block/bootdevice/by-name/persist bs=4096" > .\backup_images\persist.img
```

また、QPST ツールを使用して QCN ファイル (`.qcn`) として保存する場合は、以下で Diag モードを有効化して QPST (Software Download -> Backup) を使用します：
```powershell
.\tools\platform-tools\adb.exe -s ******** shell "setprop sys.usb.config diag,adb"
```

---

### 3. アプリデータのバックアップ

サードパーティアプリ（アルコールチェッカーアプリ等）の内部データベースおよび設定ファイルを抽出します。

```powershell
# アルコール測定アプリの内部データ取得
.\tools\platform-tools\adb.exe -s ******** pull /data/data/com.hikvision.alcoholtest/ .\app_data\com.hikvision.alcoholtest\
```
