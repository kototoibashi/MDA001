# 🌐 03. ネットワーク・通信仕様 (Network & Connectivity)

本ドキュメントでは、接続端末のネットワークインターフェース、IPアドレス設定、Wi-Fi/Bluetooth通信サブシステムについてまとめています。

---

## 📡 1. ネットワークインターフェース一覧 (`ip addr`)

| インターフェース | 状態 | IP アドレス | 説明 |
| :--- | :--- | :--- | :--- |
| **`rndis0`** | **UP** | `192.168.42.129/24` | USBテザリング・PC接続用ネットワーク |
| **`lo`** | **UP** | `127.0.0.1/8` | ループバック |
| **`rmnet_data0~7`** | **DOWN** | 未割当 | モバイルデータ通信 (LTE/3G Modem) |
| **`wlan0`** | **DOWN** | 未割当 | Wi-Fi 通信 |

---

## 📶 2. Wi-Fi & Bluetooth サブシステム

* **サブシステム名**: Qualcomm WCNSS (Pronto) Subsystem (`wcnss_wlan`)
* **統合ファームウェア**: `CNSS-PR-4-0-3-0000260`
* **ファームウェアバイナリ**: `wcnss.b02`, `wcnss.b06`, `wcnss.b09`, `wcnss.b11`, `wcnss.b12`
* **対応規格**:
  * Wi-Fi: IEEE 802.11 a/b/g/n/ac
  * Bluetooth: Bluetooth 4.x / 5.0 (HCI Device & L2CAP Layer)

---

## 💻 3. USBテザリング・通信構成

端末はPCとUSB経由で接続されており、`rndis0` (RNDIS: Remote Network Driver Interface Specification) が有効化されています。

```
PC (Host) <--- USB RNDIS (192.168.42.x) ---> Android Pad (192.168.42.129)
```

---

## 📻 4. Radio（Wi-Fi / BT / セルラー）無効化原因 ＆ 実動検証 ＆ 永続化方針

端末上で Radio 系機能が無効化されていた原因、Volatile（非永続）実動テスト結果、および恒久有効化については以下のドキュメントを参照してください。

* **Wi-Fi有効化手順と最終報告書**: [wifi_enablement.md](wifi_enablement.md)

### 実証・永続化サマリー
* **Wi-Fi**: ドライバ (`pronto_wlan.ko`) を自動ロードし `wlan0` を自動出現させ、周辺アクセスポイント (`[MASKED_SSID]` 等) のリアルタイム実測スキャンに**100% 成功**！
* **永続化**: `/vendor/bin/init.class_main.sh` の `noril` 抑止スクリプト解除 ＋ `init.radio_enable.rc` へのドライバ自動 `insmod` 定義 ＋ 機内モード OFF を適用完了。

---

## 📡 ハードウェアに関する重要事項: アンテナの追加

システム設定上での各種無線モジュール（Wi-Fi, Bluetooth, セルラー）の有効化に加え、**物理的なアンテナの後付け**が必須となります。
本端末の基板にはU.FL / IPEXコネクタのみが実装されており、アンテナ本体は搭載されていません。
詳細な実装状況については、[ハードウェア構成: RFアンテナ物理実装状況](../hardware/components.md#5-rf-) を参照してください。
