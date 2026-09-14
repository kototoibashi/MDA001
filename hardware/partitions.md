# 💾 07. パーティションテーブル解析仕様書 (Partition Table Specification)

本ドキュメントでは、ADBツールおよび`sgdisk`コマンドにより取得したAndroid端末（シリアル番号: `********` / Qualcomm MSM8953 SLM758）の全56個のGPTパーティション構造、デバイスマッピング (`device-mapper`)、および各パーティションの役割とマウント状態について詳細にまとめています。

---

## 📊 1. ストレージ概要 (Storage Summary)

* **ストレージデバイス**: eMMC 16GB (`/dev/block/mmcblk0`)
* **総セクタ数**: `30,535,680` セクタ (論理セクタサイズ: 512 bytes)
* **実効実容量**: 約 `14.56 GiB` (`15,634,268,160` bytes)
* **パーティション方式**: GPT (GUID Partition Table)
* **パーティション数**: 56 個 (`mmcblk0p1` 〜 `mmcblk0p56`)
* **RPMB領域**: `/dev/block/mmcblk0rpmb` (4.0 MiB)

---

## 🗂️ 2. 主要システム・データパーティション一覧 (Key Partitions)

| パーティション名 | ブロックデバイス | サイズ | マウントポイント / 用途 | マウント形式 / 状態 |
| :--- | :--- | :--- | :--- | :--- |
| **`system`** | `/dev/block/mmcblk0p29` | `3.0 GiB` | `/` (`dm-0`) | `ext4` (ro, dm-verity) |
| **`vendor`** | `/dev/block/mmcblk0p30` | `1.0 GiB` (1024 MiB) | `/vendor` (`dm-1`) | `ext4` (ro, dm-verity) |
| **`userdata`** | `/dev/block/mmcblk0p56` | `9.1 GiB` | `/data` (`dm-2`) | `ext4` (rw, 暗号化) |
| **`cache`** | `/dev/block/mmcblk0p31` | `256 MiB` | `/cache` | `ext4` (rw) |
| **`persist`** | `/dev/block/mmcblk0p33` | `32 MiB` | `/mnt/vendor/persist` | `ext4` (rw, 固有データ) |
| **`modem`** | `/dev/block/mmcblk0p1` | `88 MiB` | `/vendor/firmware_mnt` | `vfat` (ro, モデムFW) |
| **`dsp`** | `/dev/block/mmcblk0p12` | `16 MiB` | `/vendor/dsp` | `ext4` (ro, ADSP/CDSP FW) |
| **`boot`** | `/dev/block/mmcblk0p27` | `64 MiB` | カーネル (boot.img) | RAWイメージ |
| **`recovery`** | `/dev/block/mmcblk0p28` | `64 MiB` | リカバリ (recovery.img) | RAWイメージ |
| **`dtbo` / `bak`**| `p23` / `p24` | `8.0 MiB` ×2 | Device Tree Overlay | RAWイメージ |
| **`vbmeta` / `bak`**| `p25` / `p26` | `64 KiB` ×2 | Android Verified Boot | AVB メタデータ |
| **`aboot` / `bak`**| `p21` / `p22` | `1.0 MiB` ×2 | アプリケーションブートローダー| LK (Little Kernel) |
| **`modemst1/2`**| `p13` / `p14` | `1.5 MiB` ×2 | EFS / NVアイテム | モデム固有調整値 |
| **`fsg`** | `/dev/block/mmcblk0p16` | `1.5 MiB` | モデムFSGゴールデンコピー | EFSバックアップ |
| **`splash`** | `/dev/block/mmcblk0p20` | `11 MiB` | 起動ロゴ画像 | RAWビットマップデータ |

---

## 📋 3. 完全GPTパーティションテーブル (`sgdisk` 抽出全リスト)

以下の表は、`sgdisk --print /dev/block/mmcblk0` から抽出した全56エントリの完全な詳細リストです。

| # | パーティション名 (`Name`) | ブロックデバイス | 開始セクタ (`Start`) | 終了セクタ (`End`) | 容量 (`Size`) | 役割・分類 |
| :-: | :--- | :--- | :-: | :-: | :-: | :--- |
| 1 | `modem` | `mmcblk0p1` | 131,072 | 311,295 | 88.0 MiB | Baseband / Modem Firmware (VFAT) |
| 2 | `fsc` | `mmcblk0p2` | 393,216 | 393,217 | 1.0 KiB | Fastcard Sync Channel |
| 3 | `ssd` | `mmcblk0p3` | 393,224 | 393,239 | 8.0 KiB | Secure Software Download |
| 4 | `sbl1` | `mmcblk0p4` | 393,240 | 394,263 | 512.0 KiB | Secondary Boot Loader (Primary) |
| 5 | `sbl1bak` | `mmcblk0p5` | 394,264 | 395,287 | 512.0 KiB | Secondary Boot Loader (Backup) |
| 6 | `rpm` | `mmcblk0p6` | 395,288 | 396,311 | 512.0 KiB | Resource Power Manager (Primary) |
| 7 | `rpmbak` | `mmcblk0p7` | 396,312 | 397,335 | 512.0 KiB | Resource Power Manager (Backup) |
| 8 | `tz` | `mmcblk0p8` | 397,336 | 401,431 | 2.0 MiB | Qualcomm TrustZone (Primary) |
| 9 | `tzbak` | `mmcblk0p9` | 401,432 | 405,527 | 2.0 MiB | Qualcomm TrustZone (Backup) |
| 10 | `devcfg` | `mmcblk0p10` | 405,528 | 406,039 | 256.0 KiB | Device Config (Primary) |
| 11 | `devcfgbak` | `mmcblk0p11` | 406,040 | 406,551 | 256.0 KiB | Device Config (Backup) |
| 12 | `dsp` | `mmcblk0p12` | 406,552 | 439,319 | 16.0 MiB | Hexagon DSP Firmware (EXT4) |
| 13 | `modemst1` | `mmcblk0p13` | 439,320 | 442,391 | 1.5 MiB | Modem NV Storage 1 (IMEI/EFS) |
| 14 | `modemst2` | `mmcblk0p14` | 442,392 | 445,463 | 1.5 MiB | Modem NV Storage 2 (IMEI/EFS) |
| 15 | `DDR` | `mmcblk0p15` | 524,288 | 524,351 | 32.0 KiB | DDR Calibration Data |
| 16 | `fsg` | `mmcblk0p16` | 524,352 | 527,423 | 1.5 MiB | Modem FSG Golden Copy |
| 17 | `odm` | `mmcblk0p17` | 527,424 | 528,447 | 512.0 KiB | Original Design Manufacturer |
| 18 | `ssign` | `mmcblk0p18` | 528,448 | 529,471 | 512.0 KiB | Secure Signature |
| 19 | `sec` | `mmcblk0p19` | 529,472 | 529,503 | 16.0 KiB | Security Configuration |
| 20 | `splash` | `mmcblk0p20` | 655,360 | 677,887 | 11.0 MiB | Boot Logo Splash Screen |
| 21 | `aboot` | `mmcblk0p21` | 786,432 | 788,479 | 1.0 MiB | Android Bootloader (LK) |
| 22 | `abootbak` | `mmcblk0p22` | 788,480 | 790,527 | 1.0 MiB | Android Bootloader (Backup) |
| 23 | `dtbo` | `mmcblk0p23` | 790,528 | 806,911 | 8.0 MiB | Device Tree Overlay |
| 24 | `dtbobak` | `mmcblk0p24` | 806,912 | 823,295 | 8.0 MiB | Device Tree Overlay (Backup) |
| 25 | `vbmeta` | `mmcblk0p25` | 823,296 | 823,423 | 64.0 KiB | Verified Boot Metadata |
| 26 | `vbmetabak` | `mmcblk0p26` | 823,424 | 823,551 | 64.0 KiB | Verified Boot Metadata (Backup) |
| 27 | `boot` | `mmcblk0p27` | 823,552 | 954,623 | 64.0 MiB | Android Boot Image (Kernel + Ramdisk)|
| 28 | `recovery` | `mmcblk0p28` | 954,624 | 1,085,695 | 64.0 MiB | Recovery Image |
| 29 | `system` | `mmcblk0p29` | 1,085,696 | 7,377,151 | 3.0 GiB | Android System OS Partition |
| 30 | `vendor` | `mmcblk0p30` | 7,377,152 | 9,474,303 | 1.0 GiB | Qualcomm Vendor Partition |
| 31 | `cache` | `mmcblk0p31` | 9,568,256 | 10,092,543 | 256.0 MiB | Android Cache Partition |
| 32 | `devinfo` | `mmcblk0p32` | 10,092,544 | 10,094,591 | 1.0 MiB | Device Information / Unlocked state |
| 33 | `persist` | `mmcblk0p33` | 10,223,616 | 10,289,151 | 32.0 MiB | Calibration / DRM / Persist Data |
| 34 | `misc` | `mmcblk0p34` | 10,289,152 | 10,291,199 | 1.0 MiB | Misc / Bootloader Commands |
| 35 | `keystore` | `mmcblk0p35` | 10,291,200 | 10,292,223 | 512.0 KiB | Keystore / Key Master Data |
| 36 | `config` | `mmcblk0p36` | 10,292,224 | 10,292,287 | 32.0 KiB | Hardware Config |
| 37 | `oem` | `mmcblk0p37` | 10,292,288 | 10,816,575 | 256.0 MiB | OEM Customizations |
| 38 | `limits` | `mmcblk0p38` | 10,878,976 | 10,879,039 | 32.0 KiB | Thermal / Power limits |
| 39 | `mota` | `mmcblk0p39` | 11,010,048 | 11,011,071 | 512.0 KiB | Modem OTA Update |
| 40 | `dip` | `mmcblk0p40` | 11,011,072 | 11,013,119 | 1.0 MiB | Deep Image Processing / DSP |
| 41 | `mdtp` | `mmcblk0p41` | 11,013,120 | 11,078,655 | 32.0 MiB | Qualcomm SafeSwitch (MDTP) |
| 42 | `syscfg` | `mmcblk0p42` | 11,078,656 | 11,079,679 | 512.0 KiB | System Config |
| 43 | `mcfg` | `mmcblk0p43` | 11,079,680 | 11,087,871 | 4.0 MiB | Modem Configuration File |
| 44 | `lksecapp` | `mmcblk0p44` | 11,141,120 | 11,141,375 | 128.0 KiB | LK Secure App |
| 45 | `lksecappbak`| `mmcblk0p45` | 11,141,376 | 11,141,631 | 128.0 KiB | LK Secure App Backup |
| 46 | `cmnlib` | `mmcblk0p46` | 11,141,632 | 11,143,679 | 1.0 MiB | 32-bit Common Library TrustZone |
| 47 | `cmnlibbak` | `mmcblk0p47` | 11,143,680 | 11,145,727 | 1.0 MiB | 32-bit Common Library (Backup) |
| 48 | `cmnlib64` | `mmcblk0p48` | 11,145,728 | 11,147,775 | 1.0 MiB | 64-bit Common Library TrustZone |
| 49 | `cmnlib64bak`| `mmcblk0p49` | 11,147,776 | 11,149,823 | 1.0 MiB | 64-bit Common Library (Backup) |
| 50 | `keymaster` | `mmcblk0p50` | 11,149,824 | 11,151,871 | 1.0 MiB | Hardware Keymaster |
| 51 | `keymasterbak`| `mmcblk0p51` | 11,151,872 | 11,153,919 | 1.0 MiB | Hardware Keymaster (Backup) |
| 52 | `apdp` | `mmcblk0p52` | 11,272,192 | 11,272,703 | 256.0 KiB | Application Debug Policy |
| 53 | `msadp` | `mmcblk0p53` | 11,272,704 | 11,273,215 | 256.0 KiB | Modem Security Debug Policy |
| 54 | `dpo` | `mmcblk0p54` | 11,273,216 | 11,273,231 | 8.0 KiB | Display Pulse Optimization |
| 55 | `logdump` | `mmcblk0p55` | 11,273,232 | 11,404,303 | 64.0 MiB | Hardware Crash Dump Log Partition |
| 56 | `userdata` | `mmcblk0p56` | 11,404,304 | 30,535,646 | 9.1 GiB | User Data & Internal SDCard (`/data`) |

---

## 🔗 4. デバイスマッパー構成 (`device-mapper`)

本端末では、`dm-verity` および暗号化レイヤーによって以下のマッピングが行われています。

* **`dm-0`** (`/dev/block/dm-0`): `/dev/root`
  * 元パーティション: `system` (`mmcblk0p29`, 3.0 GiB)
  * 用途: Android OSルートファイルシステム (`dm-verity` 読み取り専用保護)
* **`dm-1`** (`/dev/block/dm-1`): `/vendor`
  * 元パーティション: `vendor` (`mmcblk0p30`, 1.0 GiB)
  * 用途: Qualcomm / OEM ドライバ・HALファイルシステム (`dm-verity` 読み取り専用保護)
* **`dm-2`** (`/dev/block/dm-2`): `/data`
  * 元パーティション: `userdata` (`mmcblk0p56`, 9.1 GiB)
  * 用途: アプリデータおよび内部ストレージ (`ext4` 暗号化マウント)

---

## 💡 5. バックアップ時の注意点 (Backup Notes)

1. **EFS / IME 情報の保護**:
   * `modemst1` (`p13`), `modemst2` (`p14`), `fsg` (`p16`), `fsc` (`p2`) に固有の通信・キャリブレーションパラメータが含まれるため、個別バックアップが必須です。
2. **システムイメージ (`system`/`vendor`) のダンプ**:
   * `dd if=/dev/block/mmcblk0p29 of=/sdcard/system.img bs=4M` または `adb pull` で完全イメージを保存可能です。

---

## 🔒 6. 参考情報: パーティションチェックサム (SHA-256)

バックアップイメージと、Android実機上の生パーティションから取得したSHA-256の照合結果（対象端末シリアル: `********`）です。
整合性確認や文鎮化時の復旧の参考にしてください。

| パーティション名 | バックアップファイル名 | ファイルサイズ | 端末側 SHA-256 (RAW) | PC側 SHA-256 (解凍・生比較) | 照合結果 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **`modemst1`** | `modemst1.img` | 1.50 MB | `a304b755a5777c08bf2276c2f3bc0ed91e3085c08e2d0a47b704bf305ee0a5e6` | `a304b755a5777c08bf2276c2f3bc0ed91e3085c08e2d0a47b704bf305ee0a5e6` | ✅ **100% 完全一致** |
| **`modemst2`** | `modemst2.img` | 1.50 MB | `1b7c3b990cddc52a8d34de12124a1427f187113a30bb4ecec44504c20989db0f` | `1b7c3b990cddc52a8d34de12124a1427f187113a30bb4ecec44504c20989db0f` | ✅ **100% 完全一致** |
| **`fsg`** | `fsg.img` | 1.50 MB | `8088a569ce3fd4bd3bc477722bf980cea82d5473808b71675155c7d2384e2a42` | `8088a569ce3fd4bd3bc477722bf980cea82d5473808b71675155c7d2384e2a42` | ✅ **100% Complete** |
| **`persist`** | `persist.img` | 32.00 MB | `2926be27211d0e95bc8becfd1f5d54dc329eca615276921113aee6d631bc04dd` | `2926be27211d0e95bc8becfd1f5d54dc329eca615276921113aee6d631bc04dd` | ✅ **100% 完全一致** |
| **`boot`** | `boot.img` | 64.00 MB | `8e34fda39251e23245155d81cefbdcf2f20a7a3757d59334d956c516a8567446` | `8e34fda39251e23245155d81cefbdcf2f20a7a3757d59334d956c516a8567446` | ✅ **100% 完全一致** |
| **`recovery`** | `recovery.img` | 64.00 MB | `7259b45dc2423906e8ade72ee84c6255dba8b6b2b6839cca1e1d4ec54284cc46` | `7259b45dc2423906e8ade72ee84c6255dba8b6b2b6839cca1e1d4ec54284cc46` | ✅ **100% 完全一致** |
| **`vendor`** | `vendor.img` | 1,024.00 MB | `c37c013d6526d751c04f9e4f24e1fab34f5f6f16fd500743fa3cfd47611be290` | `c37c013d6526d751c04f9e4f24e1fab34f5f6f16fd500743fa3cfd47611be290` | ✅ **100% 完全一致** |
| **`system`** | `system.img` | 3,072.00 MB | `c546f14fa2ee7dddd69c63b4f1481947604b8b36d79a6b00b41a5cb619429ad5` | `c546f14fa2ee7dddd69c63b4f1481947604b8b36d79a6b00b41a5cb619429ad5` | ✅ **100% 完全一致** |
| **`userdata`** | `userdata.img.gz` | 344.21 MB (解凍時 9.1GB) | `969818ff0d4cc1e67a1300544134b60d67c0c3e1555ca19bde7a8ac526a43a04` | `63fa51090a282c82c5a44b4dbddc357b8456e91555473fb9c0fe85edd2a47e73` | ⚠️ **動的変化あり (仕様)** |
