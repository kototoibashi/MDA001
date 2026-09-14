# Hikvision DS-MDA001 解析レポート

アルコールチェッカーと感熱プリンターを内蔵した Android 10 端末 **Hikvision DS-MDA001** の解析記録です。

**📖 ドキュメント本体: [https://kototoibashi.github.io/MDA001/](https://kototoibashi.github.io/MDA001/)**

## 内容

| 章 | 内容 |
| :--- | :--- |
| [端末の素性と出自](https://kototoibashi.github.io/MDA001/identity/) | 中国警察向け取り締まり端末としての出自、日本流通版で殺されている機能 |
| ハードウェア | 基板・IC・ドライバ構成、パーティションテーブル |
| アルコールチェッカー | シリアル通信プロトコル全解析、測定モード、AD値→濃度の換算 |
| サーマルプリンター | 1bit / 16階調グレースケールの制御仕様、印字ノウハウ |
| 無線・ネットワーク | Wi-Fi 恒久有効化（成功）、Bluetooth 移植（失敗） |
| システム改造・運用 | セキュリティ状態、不具合の修正、バックアップ手順 |

## リポジトリ構成

```
docs/         ドキュメントソース (Markdown)
publish/      mkdocs のビルド成果物 = GitHub Pages 公開ブランチ
mkdocs.yml    サイト定義
scripts/      ビルドフック・補助スクリプト

AlcoholTestDebugger/  アルコールセンサーのデバッグ用アプリ (Kotlin)
PrinterServerApp/     プリンターの HTTP サーバー化アプリ (Kotlin)
MDA001Tweak/          端末設定ツール（開発中）
```

## ビルド

```bash
pip install mkdocs mkdocs-material
python -m mkdocs serve     # ローカルプレビュー
python -m mkdocs build     # publish/ へ出力
```

## 注意

個人が入手した中古端末を対象としたリバースエンジニアリングの記録であり、メーカーの公式情報ではありません。端末から抽出した APK・ネイティブライブラリの再配布は行っていません。
