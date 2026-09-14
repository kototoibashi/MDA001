# 🔌 02. ハードウェア基板・IC型番・ドライバ解析 (Hardware & Drivers)

本ドキュメントでは、接続端末のメイン基板（SOM）、各種搭載ICチップの型番、バス接続（I2C/Sysfs）、および読み込まれているカーネルドライバ・モジュールについてまとめています。

---

## 🧩 1. 主要ハードウェア IC・コンポーネント一覧

| コンポーネント | 型番・IC名称 | 接続バス / アドレス | 対応ドライバ / モジュール | 機能・詳細 |
| :--- | :--- | :--- | :--- | :--- |
| **スマートモジュール (SOM)** | **MeiG SLM758** | Platform / Device Tree | `slm758`, `meig-hwver` | MeiG Mobility製 LTEスマートモジュール基板 |
| **SoC (プロセッサ)** | **Qualcomm MSM8953** | Platform | `cpu-clock-8953`, `msm` | Snapdragon 625 (Octa-core Cortex-A53 2.0GHz) |
| **電源管理 (PMIC)** | **Qualcomm PMI632 / PM8953** | SPMI / Platform | `qcom,qpnp-smb5`, `qpnp_pon` | 電源管理、充電制御、電源キー (`qpnp_pon`) |
| **eMMC ストレージ** | **Samsung QE63BB** | SDHCI (Manf ID: `0x000015`) | `sdhci_msm`, `mmcblk0` | 16GB/32GB eMMC 5.1 (2022年4月製造) |
| **タッチパネル (Touch IC)** | **Goodix GT9xx シリーズ** | I2C `3-005d` (0x5D) | `goodix-ts`, `synaptics_dsxv26` | 静電容量式タッチパネル (`/dev/input/event2`) |
| **リアルタイムクロック (RTC)** | **NXP / TI PCF8563** | I2C `2-0051` (0x51) | `pcf8563` | 独立時計/カレンダー保持IC |
| **LED コントローラ** | **Awinic AW2013** | I2C `8-0045` (0x45) | `aw2013` | 3チャンネル I2C LEDインジケータ制御IC |
| **オーディオコーデック** | **Qualcomm WCD9335** | Sound / SLIMbus | `wcd9335_dlkm`, `wcd_core_dlkm` | Hi-Fi Audio Codec |
| **スピーカーアンプ** | **Qualcomm WSA881x** | Sound / SWR | `wsa881x_dlkm`, `wsa881x_analog` | Smart Speaker Amplifier |
| **Wi-Fi / Bluetooth** | **Qualcomm WCNSS (Pronto)** | PIL / Subsystem | `wcnss_wlan` | Wi-Fi 802.11 b/g/n/ac & Bluetooth 4.x/5.0 |
| **外部通信ポート (UART)** | Qualcomm MSM UART | High-speed UART | `/dev/ttyMSM1` | 外部センサー（アルコールチェッカー等）用シリアル |

---

## 🕹️ 2. 入力デバイス (`/proc/bus/input/devices`)

1. **`qpnp_pon`** (`event0`): 電源キー (PMIC Power-on)
2. **`soc:gpio_keys_polled`** (`event1`): サイド物理ボタン (GPIO Key)
3. **`goodix-ts`** (`event2`): Goodix タッチパネル (Multi-touch)
4. **`msm8953-snd-card-mtp Headset/Button Jack`** (`event3`, `event4`): オーディオジャック検出

---

## 📦 3. ロード中の主要カーネルモジュール (`/proc/modules`)

```
machine_ext_dlkm 147456 0
wcd9335_dlkm 393216 1 machine_ext_dlkm          # Qualcomm WCD9335 オーディオコーデック
wcd_cpe_dlkm 122880 2 wcd9335_dlkm,cpe_lsm_dlkm
analog_cdc_dlkm 618496 2 machine_dlkm
wsa881x_dlkm 57344 2 machine_ext_dlkm,wsa881x_analog_dlkm  # WSA881x アンプ
wcd_core_dlkm 147456 8                          # WCD Core
platform_dlkm 2023424 33 native_dlkm            # MSM プラットフォームコア
q6_dlkm 1114112 11                              # Hexagon DSP Q6 コア
apr_dlkm 278528 7                               # Asynchronous Packet Router
```

---

## 🔌 4. 外部シリアルインターフェース (Serial UART)

* **デバイスノード**: `/dev/ttyMSM1`
* **パーミッション**: `crwxrwxrwx` (全ユーザー読込・書込可)
* **用途**: アルコール測定用周辺機器モジュールとのUART通信ポートとして機能

---

## 📡 5. RF アンテナ物理実装状況

実機を分解・確認した結果、基板上にアンテナ接続用の同軸コネクタ（U.FL または IPEX MHF1）自体は**実装済み（ハンダ付けされている）**ですが、そこから伸びるはずのアンテナ本体（ケーブルやフィルムアンテナ等）は**すべて非搭載**の状態であることが判明しました。

*   **Wi-Fi 2.4GHz & Bluetooth アンテナ**: コネクタ 1基実装済み（アンテナ本体は非搭載）
*   **Wi-Fi 5GHz アンテナ**: SOM側のパッドがNC（Not Connected）となっており、コネクタや配線パターン自体が存在しません。そのため5GHz帯は物理的に利用不可（SOMから直接ジャンパー線を引き出せば利用可能な可能性はあるが未検証）。
*   **セルラー (LTE/3G) アンテナ**: コネクタ 2基実装済み（メイン・サブアンテナ用と思われるが、アンテナ本体は非搭載）
*   **GPS アンテナ**: 未検証（コネクタ有無の特定待ち）
*   **NFC アンテナ**: 未検証（コネクタ有無の特定待ち）

> **Note:** ソフトウェア（OS）レイヤーでWi-Fiやセルラーを有効化しても、物理的なアンテナが接続されていないため、ルーター至近距離以外では電波を拾うことができません。運用にあたっては汎用のIPEX/U.FLアンテナを自前で取り付ける必要があります。
