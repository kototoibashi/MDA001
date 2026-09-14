# Hikvision DS-MDA001 解析レポート

**Hikvision DS-MDA001** — アルコールチェッカーと感熱プリンターを内蔵した Android 10 端末の、ハードウェアからシリアルプロトコルまでの全解析記録です。

中古市場で **1.2〜1.3 万円程度**から入手できる一方、出荷状態では無線が全部殺されており、素性も表向きの「運送会社の始業前点呼用アルコールチェッカー」とは少し違います。本サイトはその中身を端から順に開けていった記録です。

---

## この端末は何なのか

| 項目 | 内容 |
| :--- | :--- |
| **製品名** | Hikvision DS-MDA001 |
| **SoC** | Qualcomm Snapdragon 625 (MSM8953) / Cortex-A53 ×8 @2.0GHz |
| **SOM** | MeiG Mobility SLM758 |
| **RAM / ストレージ** | 2 GB / eMMC 16 GB |
| **OS** | Android 10 (API 29) — `userdebug` ビルド |
| **画面** | 480 × 800 (240dpi) |
| **内蔵デバイス** | 電気化学式アルコールセンサー基板（UART 接続）、感熱プリンター（紙幅 57〜58mm / ロール径 25mm まで） |
| **無線** | Wi-Fi / Bluetooth / LTE — **すべて出荷時に無効化**（アンテナ本体も非実装） |
| **電源** | 専用 AC アダプタ 12V 2A（DC ジャック 外径 3.5mm / 内径 1.1mm）。**USB 充電不可** |
| **セキュリティ状態** | 購入時点で root 済み・BL アンロック済み・SELinux `Permissive`・dm-verity 無効 |

日本では運送事業者向けのアルコール検査端末として流通していますが、実体は **中国国家標準 GB/T 21254-2017「呼出气体酒精含量检测仪」に適合する、中国警察向けに開発された取り締まり用デバイス**です。詳しくは [端末の素性と出自](identity.md) を参照してください。

---

## 何ができるようになったか

| やったこと | 結果 |
| :--- | :--- |
| [Wi-Fi の恒久有効化](network/wifi_enablement.md) | ✅ 成功（4 重のソフトロックを解除。要アンテナ後付け） |
| [Bluetooth の復活](network/bluetooth.md) | ❌ 失敗（Qualcomm 独自スタックと AOSP APK の非互換） |
| [アルコールセンサーのプロトコル解析](alcohol_checker/serial_protocol.md) | ✅ 全コマンド解明・自前アプリから測定可能 |
| [プリンターの制御解析](printer/printer.md) | ✅ 1bit / グレースケール両モードを外部から制御可能 |
| [プリンターサーバー化](printer/print_server.md) | ✅ PC ブラウザからレイアウト印刷できる Web エディタを実装 |
| [物理ホームボタンの修理](software/fixes.md) | ✅ DTB 記述欠落を特定し 3 通りの回避策を確立 |
| [全パーティションのバックアップ](software/backup_strategy.md) | ✅ SHA-256 一致で完全復元可能な状態を確保 |

---

## ドキュメントの歩き方

<div class="grid cards" markdown>

- **まずはこれ**

    [端末の素性と出自](identity.md) — 何者なのか、なぜこの仕様なのか

- **中身を知りたい**

    [基本情報・OS 仕様](hardware/overview.md) / [基板・IC・ドライバ](hardware/components.md) / [パーティション構成](hardware/partitions.md)

- **アルコールチェッカーを叩きたい**

    [測定モードと運用仕様](alcohol_checker/test_modes.md) → [シリアル通信プロトコル](alcohol_checker/serial_protocol.md) → [実測挙動と非同期仕様](alcohol_checker/measurement_behavior.md)

- **プリンターを使いたい**

    [プリンター制御仕様](printer/printer.md) → [印字ノウハウ](printer/printing_tips.md) → [PrinterServerApp](printer/print_server.md)

- **汎用 Android として使いたい**

    [Wi-Fi 有効化](network/wifi_enablement.md) / [不具合の修正記録](software/fixes.md) / [セキュリティ状態](software/security.md)

- **手を動かす前に**

    [バックアップ手順](software/backup_strategy.md) / [ADB コマンドリファレンス](appendix/adb_reference.md)

</div>

---

## 関連リンク

- ブログ記事: [アルコールチェッカー付きAndroid端末](https://kuriuzu.io/2026/08/20/%e3%82%a2%e3%83%ab%e3%82%b3%e3%83%bc%e3%83%ab%e3%83%81%e3%82%a7%e3%83%83%e3%82%ab%e3%83%bc%e4%bb%98%e3%81%8dandroid%e7%ab%af%e6%9c%ab/) — 本プロジェクトの概説
- [自作ツール一覧](tools.md) — 解析に使ったアプリとスクリプト

!!! warning "免責"
    本サイトの内容は個人が入手した中古端末を対象としたリバースエンジニアリングの記録であり、メーカーの公式情報ではありません。記載の手順はブートローダー・システム領域の書き換えを含み、実行は自己責任です。また、端末から抽出した APK やネイティブライブラリの再配布は行っていません。
