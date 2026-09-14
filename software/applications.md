# 内蔵アプリ・抽出APKアーカイブ (Applications & Extracted APKs)

本ドキュメントでは、端末にインストールされているパッケージ、サードパーティアプリケーション、およびローカル環境へ全抽出した179個のAPKファイルの分類構造についてまとめています。

---

## 📦 1. 抽出済み APK アーカイブ一覧 (Extracted APK Summary)

端末から全 **179 個** の APK ファイルを取得し、以下のカテゴリごとに分類・保存しています。

* **ローカル保存場所**: `[MASKED_PATH]`

### 保存ディレクトリ構造
```
apks/
├── data_3rdparty/   # 1 個  (サードパーティ追加アプリ)
├── product/         # 35 個 (プロダクト標準アプリ・テーマオーバーレイ)
├── system/          # 131 個 (Android OSシステム標準・サービス)
└── vendor/          # 12 個 (Qualcomm / ベンダー固有サービス)
```

---

## 🍺 2. サードパーティアプリケーション (`apks/data_3rdparty/`)

* **パッケージ名**: `com.hikvision.alcoholtest`
* **APK パス**: `apks/data_3rdparty/com.hikvision.alcoholtest.apk`
* **元のインストール場所**: `/data/app/com.hikvision.alcoholtest-yU0d63z910i3rOOrkaWqhA==/base.apk`
* **概要**: Hikvision製 アルコール測定・チェッカー管理アプリケーション

---

## 🏢 3. ベンダー・システム主要サービスアプリ (`apks/vendor/`, `apks/system/`)

### 主要ベンダーアプリ (`apks/vendor/`)
1. `com.qualcomm.qti.qdma`: QDMA (Qualcomm Diagnostic Monitoring Agent)
2. `com.qualcomm.qti.cne`: CNE (Connectivity Engine)
3. `com.qualcomm.qti.sensors.qsensortest`: QSensorTest (Qualcomm センサーテストツール)
4. `com.qualcomm.timeservice`: TimeService (時間同期サービス)
5. `com.qualcomm.qti.qms.service.trustzoneaccess`: TrustZone Access Service

### 主要システムアプリ (`apks/system/`, `apks/product/`)
1. `com.android.settings`: 設定 (`Settings.apk`)
2. `com.android.systemui`: システムUI (`SystemUI.apk`)
3. `org.codeaurora.snapcam`: Snapdragon Camera (`SnapdragonCamera.apk`)
4. `com.android.dialer`: 電話アプリ
5. `com.android.inputmethod.latin`: Latin IME キーボード
