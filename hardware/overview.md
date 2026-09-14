# 端末基本情報・OS仕様 (Device & OS Overview)

本ドキュメントでは、接続されたAndroid端末（シリアル番号: `********`）の基本システム情報、OSバージョン、ビルド仕様、ディスプレイ、バッテリーおよびストレージ状態についてまとめています。

---

## 📌 1. 端末概要 (Device Overview)

| 項目 | 詳細スペック |
| :--- | :--- |
| **シリアル番号** | `********` |
| **SoC / プロセッサ** | Qualcomm MSM8953 (Snapdragon 625) / `msm8953 for arm64` |
| **スマートモジュール** | MeiG Mobility SLM758 (`slm758`) |
| **製造元 (Manufacturer)** | QUALCOMM |
| **ブランド (Brand)** | `qti` |
| **CPU アーキテクチャ** | `arm64-v8a` (64-bit ARMv8 Cortex-A53 8コア @ 2.0 GHz) |
| **GPU** | Qualcomm Adreno 506 |
| **RAM (メモリ) 容量** | 2 GB (システム認識量: `1,828 MB`) |
| **調査日時** | 2026年8月6日 |

---

## 🤖 2. OS・システムバージョン (OS Version & Build)

| 項目 | 詳細情報 |
| :--- | :--- |
| **Android バージョン** | `Android 10` |
| **API レベル** | `29` |
| **ビルド Display ID** | `msm8953_64-userdebug 10 QKQ1.191215.002 eng.build3.20220623.201448 test-keys` |
| **ビルドタイプ** | `userdebug` (開発・デバッグ権限有効化ビルド) |
| **ビルド日時** | 2022年6月23日 |
| **SELinux モード** | `Permissive` (アクセス制限緩和状態) |

---

## 🖥️ 3. ディスプレイ仕様 (Display)

* **物理画面解像度**: `480 × 800` pixels
* **画面密度 (Density)**: `240` dpi (hdpi)
* **フレームバッファデバイス**: `/sys/class/graphics/fb0` (Qualcomm MDSS DSI)

---

## 🔋 4. バッテリー・電源管理 (Power & Battery)

* **電源供給状態**: USB給電中 (`USB powered: true`)
* **バッテリー残量**: `43%`
* **バッテリー温度**: `25.0 °C`
* **バッテリー健康状態**: 正常 (`Health: Good / 2`)
* **充電規格**: Max current 500mA / Max voltage 5.0V

---

## 💾 5. ストレージ・メモリ使用状況 (Storage & Memory)

### 内蔵ストレージ (`/data`)
* **マウントポイント**: `/data` (`/storage/emulated`)
* **全体容量**: `8.9 GB`
* **使用量**: `143 MB` (使用率 2%)
* **空き容量**: `8.6 GB`

### メモリ / Swap
* **Total RAM**: `1,828 MB`
* **Used RAM**: `1,805 MB` (キャッシュ・バッファ含む)
* **Free RAM**: `22 MB`
* **Swap 容量**: `4,095 MB` (使用量: 44 MB)
