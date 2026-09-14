# 不具合の修正記録 (Known Defects & Fixes)

本ドキュメントは、DS-MDA001 を汎用 Android 端末として使おうとしたときに突き当たる「壊れている / 殺されている機能」と、その原因調査および修正手法をまとめたものです。

いずれも **専用端末として作り込まれた結果、汎用 Android としては設定が欠落している** ことに起因しており、ハードウェアの故障ではありません。

---

## 1. 物理ホームボタンが一切反応しない

### 症状
本体の物理ホームボタンを押しても OS が全く反応しない。

### 原因
`adb shell getevent -l` で監視しても押下時に入力イベントが発生せず、Android フレームワーク以前の **カーネルレベルで信号が捨てられている** ことが判明しました。

カーネルログ (`dmesg`) には起動時に `gpio_keys_polled` ドライバの `gpio_to_desc` エラーが出ており、これは Device Tree (DTB) における GPIO ピン割り当ての記述が誤っている／欠落している場合に発生します。

`/sys/kernel/debug/gpio` を総当たりで監視した結果、**ホームボタンは物理的に GPIO 88 に結線されている**ことを特定しました。押下で電圧が `1` → `0` に遷移する Active-Low 構成です。カーネルの DTB に「GPIO 88 がホームボタンである」という記述がないため、ボタンが死んでいます。

### 対策A: ソフトウェアナビゲーションバーで代替（最も手軽）

物理ボタンを使わず、Android 標準の画面内ナビゲーションバーを強制表示させます。

```bash
adb shell setprop qemu.hw.mainkeys 0
adb shell pkill SystemUI
```

恒久化する場合は `/system/build.prop` に `qemu.hw.mainkeys=0` を追記します。

### 対策B: GPIO 監視スクリプトでエミュレート（DTB を触らない安全策）

Device Tree を書き換えず、バックグラウンドスクリプトで GPIO 88 の電圧を直接監視してホームキーイベントを発行します。

```sh
#!/system/bin/sh
echo 88 > /sys/class/gpio/export 2>/dev/null
echo in > /sys/class/gpio/gpio88/direction

while true; do
  if [ "$(cat /sys/class/gpio/gpio88/value)" = "0" ]; then
    input keyevent 3  # KEYCODE_HOME
    while [ "$(cat /sys/class/gpio/gpio88/value)" = "0" ]; do
      sleep 0.1
    done
  fi
  sleep 0.1
done
```

### 対策C: DTB にノードを追記（根本解決）

1. `magiskboot` 等で `boot.img` から `dtb` を抽出
2. `dtc` でデコンパイルし、`gpio_keys_polled` ノードへ以下を追記

    ```dts
    button@home {
        label = "Home";
        linux,code = <102>;      /* KEY_HOME */
        gpios = <&tlmm 88 0x01>; /* GPIO 88 (Active-Low) */
    };
    ```

3. リコンパイルして `boot.img` を再構築しフラッシュ

!!! warning "boot パーティションの書き換えは文鎮化リスクがあります"
    実行前に必ず [バックアップ手順](backup_strategy.md) に従って `boot.img` を退避してください。

---

## 2. 着信しない（Telephony / IMS）

### 症状
IMS 接続（VoLTE 等）は正常に確立できているのに、外部からの着信で画面が出ない・鳴らない。「Telephony.apk が入っていないのでは？」と疑われる症状です。

### 原因
Android の電話機能は単一の `Telephony.apk` ではなく複数コンポーネントで構成されますが、本端末には `TeleService.apk` (`com.android.phone`)、`Telecom.apk`、`QtiTelephonyService.apk`、`ims.apk` などの必須コンポーネントはすべて正常に存在していました。

`dumpsys telecom` を解析した結果、**デフォルトの電話アプリ (Default Dialer) が未設定 (`null`)** であることが判明。Telecom システムが着信のルーティング先を見つけられず、着信自体が破棄されていました。

### 対策

端末にインストール済みのダイヤラーをデフォルトに指定します。

```bash
adb shell telecom set-default-dialer org.codeaurora.dialer
```

---

## 3. 無線が全部死んでいる

Wi-Fi / Bluetooth / セルラーは出荷時にソフトウェア・ハードウェア両面で無効化されています。原因の切り分けと復旧手順は独立したドキュメントにまとめています。

- [Wi-Fi 恒久有効化手順](../network/wifi_enablement.md) — **成功**
- [Bluetooth 有効化の試行記録](../network/bluetooth.md) — **失敗（Qualcomm スタック非互換）**
- [ネットワークインターフェース一覧](../network/interfaces.md)

いずれのケースでも、**基板にアンテナ本体が実装されていない**というハードウェア側の制約が最後に残ります。詳細は [基板・IC・ドライバ仕様](../hardware/components.md) を参照してください。

---

## 4. USB では充電できない

本体の USB ポートは ADB / RNDIS 用であり、充電には使えません。給電は専用 AC アダプタ（**12V 2A / DC ジャック 外径 3.5mm・内径 1.1mm**）が必要です。

汎用の 12V アダプタを使う場合はジャック径に注意してください（一般的な 5.5mm/2.1mm とは異なります）。
