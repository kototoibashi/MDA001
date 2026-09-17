# 不具合の修正記録 (Known Defects & Fixes)

本ドキュメントは、DS-MDA001 を汎用 Android 端末として使おうとしたときに突き当たる「壊れている / 殺されている機能」と、その原因調査および修正手法をまとめたものです。

いずれも **専用端末として作り込まれた結果、汎用 Android としては設定が欠落している** ことに起因しており、ハードウェアの故障ではありません。

---

## 1. 物理ホームボタンが一切反応しない

### 症状
本体の物理ホームボタンを押しても OS が全く反応しない。

### 原因
`adb shell getevent -l` で監視しても押下時に入力イベントが発生せず、Android フレームワーク以前の **カーネルレベルで信号が捨てられている** ことが判明しました。

`/sys/kernel/debug/gpio` を総当たりで監視した結果、**ホームボタンは物理的に GPIO 88 に結線されている**ことを特定しました。押下で電圧が `1` → `0` に遷移する Active-Low 構成です。

!!! bug "訂正(2026-09-15): 「記述が欠落」ではなく「番号が1つズレている」"
    当初「DTBにホームボタンの記述が存在しない」としていましたが、`dtbo.img`(`boot.img`ではない)を
    展開して確認した結果、`gpio_keys`ノード配下に**`home`エントリ自体はすでに存在**していました。
    ただし`gpios`プロパティが`GPIO 87`(`0x57`)を指しており、実際の配線である`GPIO 88`と
    **1つズレていた**のが真因、という当初の分析。この後さらに2つの問題が発覚したため、
    最終的な結論は下の「解決済み」ノートを参照。

!!! success "解決済み(2026-09-16〜18 実機検証): 3段階のバグが重なっていた"
    GPIO番号の1ズレを直すだけでは動かず、調査の結果**3つの独立したバグが積み重なっていた**ことが判明した。

    1. **番号ズレ**: `gpios`が`GPIO 87`(`0x57`)を指していたが実配線は`GPIO 88`(`0x58`)。
    2. **そもそも死んでいるドライバ**: `home`エントリは無印`gpio_keys`(割り込み版)ノードにあったが、
       この端末のカーネルは`CONFIG_KEYBOARD_GPIO`が無効(`CONFIG_KEYBOARD_GPIO_POLLED`のみ有効)で、
       割り込み版`gpio-keys`ドライバ自体がビルドされていない。DTをどう直しても`/sys/bus/platform/devices/soc:gpio_keys/`に
       `driver`シンボリックリンクが付かず(=probeされない)ため、**`gpio_keys_polled`ノード側に`home`エントリを
       移設する**必要がある。
    3. **キーコードの取り違え**: `linux,code = <0x66>`(Linux `KEY_HOME`=PCキーボードの「文頭移動」)を
       指定していたが、Android標準`/system/usr/keylayout/Generic.kl`ではこのスキャンコードは
       `KEYCODE_MOVE_HOME`にマップされ、システムのHome画面遷移(`KEYCODE_HOME`)には紐付かない。
       `KEYCODE_HOME`に化けるのは`linux,code = <0xac>`(Linux `KEY_HOMEPAGE`=172)の方だった。
       この結果、カーネルもInputReaderも正常に動作している(`dumpsys input`で該当デバイスの
       `KeyDowns`/`DownTime`が実際の押下と一致して更新される)のに、`PhoneWindowManager`の
       `mHomePressed`まで届かず**画面上は何も起きない**という、切り分けが最も厄介なパターンになっていた。

    最終的な修正内容は対策Cの手順を参照。切り分けの決め手は`dumpsys input`の該当デバイスの
    `KeyDowns`/`DownTime`(カーネル→InputReaderは正常)と`dumpsys window policy`の
    `mHomePressed`(InputReader→PhoneWindowManagerで止まっている)を比較したこと。
    詳細な抽出手順・fragment構造は [パーティションテーブル解析 §7](../hardware/partitions.md) を参照。

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

### 対策C: DTBO を修正（根本解決・実機検証済み）

編集対象は `boot.img` ではなく **`dtbo` パーティション**(`/dev/block/by-name/dtbo`)です。`dtbo.img`は
Android標準の`DT_TABLE_MAGIC`ヘッダを持つテーブル形式なので、生の`dtc`ではなく`mkdtboimg.py`
(AOSP `system/libufdt/utils/src/mkdtboimg.py`)でunpack/repackする必要があります。

1. `dtbo`パーティションを抽出・バックアップ(`dtbo`・`dtbobak`両方。手順は[パーティションテーブル解析 §7](../hardware/partitions.md)参照)
2. `mkdtboimg.py dump dtbo_orig.img -o dtbo_dump -b dtbo_` でunpack(実際のdtbバイナリは`-b`で指定した接頭辞の方、`dtbo_.0`)
3. `dtc -I dtb -O dts dtbo_.0 -o dtbo.dts` でデコンパイル(overlay特有の`avoid_default_addr_size`等の警告は無視してよい、`FATAL ERROR`でなければ成功)
4. **`gpio_keys_polled`**(`gpio_keys`無印ではない。理由は上の「解決済み」ノート参照)ノードに`home`エントリを新規追加

    ```diff
      gpio_keys_polled {
          ...
          key_switch { ... };
    +
    +     home {
    +         label = "home";
    +         gpios = <0xffffffff 0x58 0x01>;  /* GPIO 88, Active-Low */
    +         linux,input-type = <0x01>;
    +         linux,code = <0xac>;             /* KEY_HOMEPAGE → Android KEYCODE_HOME */
    +         debounce-interval = <0x0f>;
    +     };
      };
    ```

5. **`__fixups__`ノードの`tlmm`文字列リストに新しいパスを追記する(これを忘れると他のボタンまで巻き添えで死ぬ)**。
   `gpios`の第1セルは外部(ベースDT側)の`tlmm`ノードへのphandle参照で、dtcはこのデコンパイル済みdtsから
   自動でfixupを生成できない(overlay対象が別blob=ベースツリー側のため)。手動で1行追加が必要:

    ```diff
    - .../gpio_keys_polled/key_switch:gpios:0\0/fragment@46/__overlay__/meig-hwversion:...
    + .../gpio_keys_polled/key_switch:gpios:0\0/fragment@46/__overlay__/gpio_keys_polled/home:gpios:0\0/fragment@46/__overlay__/meig-hwversion:...
    ```

    追記を忘れると、追加した`home`の`gpios`が実機上で無効なphandle(`0xffffffff`)のまま残り、
    `gpio-keys-polled`ドライバの`of_get_named_gpio()`がその1エントリで失敗して**probe関数自体が
    エラーリターンし、同じノード内の`vol_up`/`vol_down`/`key_back`等まで全滅する**(実機で再現済み)。

6. `dtc -@ -I dts -O dtb dtbo.dts -o dtbo_new.dtb` でリコンパイル → `mkdtboimg.py create dtbo_new.img dtbo_new.dtb` で再パック → `dtbo`パーティションへ書き戻し

### 切り分けに使った確認コマンド

「`getevent`ではイベントが出るのにOSが無反応」という状態を切り分けるには、`InputReader`(カーネル直後)と
`PhoneWindowManager`(Android側ポリシー層)のどちらで止まっているかを見るのが決め手になった。

```bash
# InputReaderが実際に押下を受理しているか(カーネルのタイムスタンプと一致するはず)
adb shell dumpsys input | grep -A 15 "Device N: soc:gpio_keys_polled"   # KeyDowns / DownTime を見る

# PhoneWindowManagerのHome判定まで届いているか
adb shell dumpsys window policy | grep mHomePressed   # 押している間だけtrueになるはず

# Android側のHome処理自体が生きているかの独立確認(仮想注入)
adb shell input keyevent KEYCODE_HOME
```

`KeyDowns`/`DownTime`は更新されるのに`mHomePressed`が常に`false`なら、キーコードのマッピング
(`linux,code`と`/system/usr/keylayout/Generic.kl`の対応、`adb shell su 0 cat /system/usr/keylayout/Generic.kl`で確認)を疑う。

!!! warning "dtbo パーティションの書き換えは文鎮化リスクがあります"
    実行前に必ず [バックアップ手順](backup_strategy.md) に従って `dtbo.img`(および念のため `boot.img`)を退避してください。書き込みイメージのサイズは元パーティションと同一に保つこと。

---

## 2. 通話が一切できない（Telephony / IMS）— 未解決・断念

**要約(技術詳細は読み飛ばしてOK): SMSは送受信できるが、着信・発信の音声通話が一切ダメ。原因不明のまま断念した。**

!!! danger "未解決(2026-09-18時点): 断念"
    **そもそもマイクが無いから、ソフトウェア側をどれだけ直しても実用的な通話は成立しない。**
    IMS Registration(SIP `REGISTER`によるP-CSCF/S-CSCFへの登録)・SMS送受信は正常に動作しており、
    **発信側から見た呼び出し待ちビヘイビアも正常**——つまり回線・IMS登録・着信シグナリングまでは
    ネットワーク/ベースバンド側で問題なく処理されている。しかし**着信・発信を問わず音声通話が
    まるごと成立しない**(着信は鳴動・着信画面表示ともに無し、発信側も通話にならない)。
    下記の「デフォルトダイヤラー設定」はTelecomの着信ルーティングという一因を潰しただけで、
    通話自体は復旧していない。この端末は業務上通話機能を使わないため、これ以上の追及はせず断念した。
    **そもそも本体にマイクが実装されていない可能性が高く**、SMS(データ経路)は生きているのに
    音声だけがまるごと死んでいる状況から見ても、**通話用の音声パス(ベースバンドのオーディオ経路、
    または関連ハードウェアの欠如)に根本的な問題がある**可能性が高いと見ている。


### 症状
IMS Registration(SIP `REGISTER`)自体は成功しており、SMSの送受信も正常に動作する。この端末に電話をかけると、**発信側は正常な端末に対してと同じ呼び出し待ち(呼出音・呼び出し中表示)のビヘイビアになる**——つまり回線・IMS登録・着信シグナリングまではネットワーク側で正常に処理されている。しかし**端末側では着信音が鳴らず、着信画面も一切表示されない**。さらに、**この端末から発信した場合も通話(音声)が成立しない**——着信・発信どちらの方向でも音声通話がまるごと機能しない。「Telephony.apk が入っていないのでは？」と疑われる症状だが、後述の通りコンポーネント自体は揃っている。

### 分かったこと(部分的な原因)
Android の電話機能は単一の `Telephony.apk` ではなく複数コンポーネントで構成されますが、本端末には `TeleService.apk` (`com.android.phone`)、`Telecom.apk`、`QtiTelephonyService.apk`、`ims.apk` などの必須コンポーネントはすべて正常に存在していました。

`dumpsys telecom` を解析した結果、**デフォルトの電話アプリ (Default Dialer) が未設定 (`null`)** であることが判明。Telecom システムが着信のルーティング先を見つけられず、着信自体が破棄されていました。

### 試した対策(効果は限定的)

端末にインストール済みのダイヤラーをデフォルトに指定します。

```bash
adb shell telecom set-default-dialer org.codeaurora.dialer
```

これでTelecom側のルーティング未設定という問題は解消したはずだが、それでも着信は最終的に復旧しなかった。マイク実装の疑いも含め、原因は1つではない可能性が高い。

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
