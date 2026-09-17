# カーネル `uyu_printer.c` 逆コンパイル解析

対象: `backup_images/boot.img` から抽出したカーネルイメージ(Linux 4.9.193, msm-4.9, `LA.UM.8.6.2`)。
ソースは `/home3/build3/clarence/zthj_758/LA.UM.8.6.2/kernel/msm-4.9/drivers/uyu/uyu_printer.c`(非公開、入手不可)。
本ドキュメントは、ビルド済みバイナリを逆コンパイルして得た結果のみに基づく再構成である。

## 0. 手法・ツール

- `vmlinux-to-elf` で `backup_images/derived/kernel/kernel_raw.bin`(gzip解凍済みの生 ARM64 Image)から
  `kernel.elf` を生成。カーネル内蔵の kallsyms を利用して **124,717個のシンボル**(関数名含む)を
  完全復元できた。これにより `uyu_printer.c` 由来と思われる関数がほぼ全て個別のシンボル名で
  判別可能になった(静的関数含む)。**さらにグローバル変数のシンボル名(`HEAT_DELAY`, `TK`, `TB`,
  `Heat_En`, `PRN_MT_A` 等)も kallsyms から実名で復元できており、以下の記述はオフセット推測ではなく
  実シンボル名に基づく。**
- 当初 r2ghidra(Ghidraベースの逆コンパイラプラグイン)は本環境(WSL)でのビルドに必要な
  radare2 開発用ヘッダ/pkg-config ファイルが欠けており断念し、**radare2 の生アセンブリ(`pd`/`pdf`)を
  人手で読む**方針で1次調査を行った(2節・3節の一部はその結果)。
  その後、**radare2 6.2.2 をソースから `$HOME/.local` にビルドし直し(`meson`+`ninja`、sudo不要)、
  `r2pm -ci r2ghidra` で r2ghidra 本体を、`r2ghidra_sleigh-6.1.4.zip`(事前ダウンロード済みのsleigh仕様
  データ、`unzip`コマンドが無かったため Python の `zipfile` で手動展開)で sleigh アーキテクチャ定義を
  導入し、`pdg`(Ghidra擬似コード出力)による正式な逆コンパイルに成功した**。4節以降の記述は
  この `pdg` 出力で得られた、より正確な情報に基づいて更新・訂正している。
- 使用コマンド例:
  ```
  vmlinux-to-elf kernel_raw.bin kernel.elf
  r2 -q -e scr.color=0 -c 'pD <size> @ <addr>' kernel.elf        # 生アセンブリ
  r2 -q -e scr.color=0 -e bin.relocs.apply=true \
     -c 'af @ sym.<func>; pdg @ sym.<func>' kernel.elf           # r2ghidra 擬似コード
  ```
  (`af` だけでは関数境界の自動判定を誤り隣接関数と結合されることがあったため、必要に応じて
  `afu <end_addr>` で関数終端を明示した。)
- 本ファイルの「事実」は上記の手法で得たアセンブリ/擬似コードから直接読み取れる内容、
  「推測」は複数箇所の状況証拠から筆者(Claude)が組み立てた解釈。**r2ghidra導入後に判明した内容で
  当初の推測が訂正された箇所は、その旨を明記している。**

## 1. 復元された関数一覧(事実)

kallsyms から復元された、本ドライバに属すると判断できる関数(アドレスは仮想アドレス、
ELF内 `kernel.elf` 基準):

| アドレス | シンボル | 種別 |
|---|---|---|
| 0xffffff8008e15460 | `gpio_init` | GLOBAL FUNC |
| 0xffffff8008e156e0 | `Print_On` | GLOBAL FUNC |
| 0xffffff8008e15708 | `Print_Off` | GLOBAL FUNC |
| 0xffffff8008e15730 | `Heat_On` | GLOBAL FUNC |
| 0xffffff8008e15778 | `Heat_Off` | GLOBAL FUNC |
| 0xffffff8008e157b8 | `motor_step1` | GLOBAL FUNC |
| 0xffffff8008e15840 | `motor_step2` | GLOBAL FUNC |
| 0xffffff8008e15890 | `motor_step3` | GLOBAL FUNC |
| 0xffffff8008e158e8 | `motor_step4` | GLOBAL FUNC |
| 0xffffff8008e15940 | `motor_step5` | GLOBAL FUNC |
| 0xffffff8008e15990 | `motor_step6` | GLOBAL FUNC |
| 0xffffff8008e159e8 | `motor_step7` | GLOBAL FUNC |
| 0xffffff8008e15a40 | `motor_step8` | GLOBAL FUNC |
| 0xffffff8008e15a98 | `motor_forward_one_step` | GLOBAL FUNC |
| 0xffffff8008e15ba8 | `motor_backward_one_step` | GLOBAL FUNC |
| 0xffffff8008e15cc8 | `motor_move_lines` | GLOBAL FUNC |
| 0xffffff8008e15d20 | `PaperCheck` | GLOBAL FUNC |
| 0xffffff8008e15d88 | `printer_dowork` | GLOBAL FUNC |
| 0xffffff8008e15dc0 | `LineColorRankData_Manage` | GLOBAL FUNC |
| 0xffffff8008e15e58 | `IdlePrint` | GLOBAL FUNC |
| 0xffffff8008e15f70 | `LineColorRank_Print` | GLOBAL FUNC |
| 0xffffff8008e161f8 | `pic_motor_forward_one_step` | GLOBAL FUNC |
| 0xffffff8008e162e0 | `Print_Picture` | GLOBAL FUNC |
| 0xffffff8008e16328 | `printer_work` | GLOBAL FUNC(workqueue本体) |
| 0xffffff8008e166a0 | `uyu_prn_probe` | LOCAL FUNC |
| 0xffffff8008e16a38 | `uyu_prn_remove` | LOCAL FUNC |
| 0xffffff8008e16ad0 | `prn_dev_read` | LOCAL FUNC |
| 0xffffff8008e16b08 | `prn_dev_write` | LOCAL FUNC |
| 0xffffff8008e16c50 | `prn_dev_unlocked_ioctl` | LOCAL FUNC |
| 0xffffff8008e170b8 | `prn_dev_open` | LOCAL FUNC |
| 0xffffff8008e17128 | `prn_dev_release` | LOCAL FUNC |
| 0xffffff8008e14df0 | `slm758_dev_probe` | LOCAL FUNC(アルコールボード/ライト等の親デバイス) |

グローバル変数(データオブジェクト、`kallsyms` の OBJ エントリ。r2ghidra の `pdg` 出力で
実際の読み書き箇所まで確認済み):

| シンボル | アドレス | 役割(事実) |
|---|---|---|
| `Heat_En` | `0xa91f000+0x1d4`域 | devicetree `en-heat` の GPIO番号キャッシュ |
| `Print_En` | 同域 | devicetree `en-gpio` の GPIO番号キャッシュ |
| `PRN_MT_A` / `PRN_MT_A_0` / `PRN_MT_B` / `PRN_MT_B_0` | 同域 | devicetree `mt-a`/`mt-a0`/`mt-b`/`mt-b0` のGPIO番号キャッシュ(ステッピングモーター4相) |
| `PRN_STB` | 同域 | devicetree `stb`(ストローブ)のGPIO番号キャッシュ |
| `P_Paper` | 同域 | devicetree `p-paper`(用紙検知)のGPIO番号キャッシュ |
| `task_busy` | `0xa91f050` | ヘッド発熱中/デバイスビジーを表すフラグ。`Heat_On`が1、`Heat_Off`が0にする。ioctl `0x44`はこの値を読むだけ |
| `Motor_Cycle` | `0xa91f1f4` | ステッピングモーターの半歩位相を切り替えるトグルカウンタ(2値、`motor_forward_one_step`内で±1して2で割った余りのように振る舞う) |
| `MOTOR_DELAY` | `0xedb0ab8` | モーター1相あたりのウェイト時間(µs, `__udelay`直結)。ioctl `0x55`で設定 |
| `HEAT_DELAY` | `0xedb0abc` | ioctl `0x88`(濃度設定)で書き込まれる値(1100〜3000, 詳細は3.3節)。**2値印字モード(`printer_work`のタグ`0xf0`経路、ioctl `0x66`)のストローブ幅として使われている(3.5節で確定)。16階調モード(`0x67`/`Print_Picture`)では使われない** |
| `TB` | `0xedb0ac0` | 初期値 `6`(実バイナリの `.data` から直接確認)。`LineColorRank_Print`内でストローブ幅の一部として使用 |
| `TK` | `0xedb0ac4` | 初期値 `144`(同上)。`LineColorRank_Print`内でストローブ幅の基準値として使用 |
| `printer_work_lock` | `0xedb0ac8` | `printer_work`のための `mutex`実体、および「作業をキューする際にロック中かどうか」の判定に使用 |
| `gPrn` | (probe内で確保) | デバイス本体構造体(SPI/GPIO/kfifo/workqueueをまとめた `kmem_cache_alloc_trace(size=0x138)` の動的確保構造体)へのグローバルポインタ |
| `LineColorRankData` | `0xa91f054` | 1ラインぶんの4bit階調バッファ(384エントリ) |

`MOTOR_DELAY`, `HEAT_DELAY`, `TB`, `TK`, `printer_work_lock` が
`0xedb0ab8`〜`0xedb0ac8`に連続して並んでいることから、これらは1つの構造体
(または連続したstatic変数群)のフィールドであると判断できる。

## 2. デバイス初期化(`uyu_prn_probe`)で判明した GPIO 対応表(事実)

`of_get_named_gpio_flags()` の呼び出し順序と、直前にセットされる devicetree プロパティ名文字列
(`.rodata` から直接読み取り)を突き合わせた結果:

| シンボル名(r2ghidra復元) | devicetree プロパティ名 | 用途(後続コードから判明) |
|---|---|---|
| `Heat_En` | `en-heat` | サーマルヘッド発熱イネーブル(マスタースイッチ、`Heat_On`/`Heat_Off`が操作。3.4節参照) |
| `Print_En` | `en-gpio` | `prn_dev_open`/`gpio_init`で出力0/1に固定操作。ioctl `0x77`でも状態読取に使われる |
| `PRN_MT_A` | `mt-a` | ステッピングモーター相A |
| `PRN_MT_B` | `mt-b` | ステッピングモーター相B |
| `PRN_MT_A_0` | `mt-a0` | ステッピングモーター相A0(相補相) |
| `PRN_MT_B_0` | `mt-b0` | ステッピングモーター相B0(相補相) |
| `PRN_STB` | `stb` | **ヘッドストローブ(発熱パルス)信号**。`LineColorRank_Print`内でSPI転送直後にHigh→delay→Lowされる |
| `P_Paper` | `p-paper` | 用紙検出センサー入力 |

これはプロンプトに記載の devicetree ノード(`en-gpio`, `en-heat`, `mt-a/a0/b/b0`, `p-paper`, `stb`)と完全に一致する。
(シンボル名は r2ghidra による正式デコンパイルで実名を確認済み。当初はオフセット `+0x1d4`等で
記述していたが、実際のCソースコードの変数名がそのままkallsymsに残っていたため、それに置き換えた。)

### 2.1 実機での GPIO 番号確定(事実、`devicetree.dts`+実機`gpiochip0`ダンプで裏取り済み)

`hardware-notes/devicetree.dts` の `spi@7af7000/uyu-prn` ノード(`<phandle 0x103 pin flags>`形式、
`0x103` = `pinctrl@1000000`(`qcom,msm8953-pinctrl`, TLMM本体))と、実機`cat /sys/kernel/debug/gpio`の
`gpiochip0: GPIOs 0-141, parent: platform/1000000.pinctrl`(base=0)を突き合わせて確定:

| シンボル名 | devicetreeプロパティ | GPIO番号 |
|---|---|---|
| `PRN_STB` | `stb` | **48** |
| `PRN_MT_A` | `mt-a` | **0** |
| `PRN_MT_A_0` | `mt-a0` | **93** |
| `PRN_MT_B` | `mt-b` | **131** |
| `PRN_MT_B_0` | `mt-b0` | **28** |
| `Print_En` | `en-gpio` | **128** |
| `Heat_En` | `en-heat` | **18** |
| `P_Paper` | `p-paper` | **17** |

idle時(印字ジョブなし)は全て`value:0`(`Print_En`/`Heat_En`はactive-high、`Print_On()`/`Heat_On()`が
`gpiod_direction_output_raw(desc, 1)`を呼ぶことと整合)。

## 3. `prn_dev_unlocked_ioctl` のコマンド一覧(事実+一部推測)

### 3.1 引数の実体(事実)

`ioctl(fd, cmd, arg)` の `arg`(第3引数、ユーザ空間ポインタ)は次の構造体を指す:

```c
struct uyu_prn_ioctl_arg {
    void   *buf;   // +0x0: ユーザバッファへのポインタ
    size_t  len;   // +0x8: バッファ長 or サブコマンド識別に使う値
};
```
(アセンブリで `ldr x19,[x20,8]`(len) → `ldr x20,[x20]`(buf) と読み出している。フィールド名・型は推測)

### 3.2 switch 構造(事実)

コンパイラはコマンド値 `0x55`〜`0x77`(35個, `0x23`)の範囲をジャンプテーブル化し、
それ以外の `0x44`, `0x88`, `0x99` は個別の `cmp`/`b.eq` で分岐している。
ジャンプテーブル実体はデータ領域 `0xffffff80091ec9d4`(35バイト、各バイトが
`(target - 0xffffff8008e16cf0) / 4` のオフセット)。

デコード結果、テーブル中で実装があるのは **4個のコマンドのみ**で、残りは
「未実装(何もせず0を返す)」だった(r2ghidra `pdg` による正式デコンパイルで確定):

| cmd (hex) | 実装 | 内容 |
|---|---|---|
| `0x44` | ● | グローバル `task_busy` フラグを読み取って返すだけ(`Heat_On`/`Heat_Off`が書き込む。3.4節で訂正あり) |
| `0x55` | ● | **モーター速度テーブル参照**。ユーザバッファ先頭1バイト`(byte-1)`が`0x27`(39)以下なら`0x91f3000+0xcf8`の40エントリテーブルを引き、結果をグローバル`MOTOR_DELAY`(µs, `__udelay`直結)に書き込む。範囲外なら固定値`1000`。`len`(バッファ長フィールド)が0でなければ、続けてタグ`0xf1`+4バイトを`prn_fifo`に`kfifo_in`し、`printer_work_lock==1`なら`queue_work_on`。テーブルの実データ内容は未解読。 |
| 0x56〜0x65, 0x68〜0x76 | ✗未実装 | 何もせず戻り値0(switch内でdefault相当に`goto`するだけ) |
| `0x66` | ● | **複数コマンドの一括投入**。`kmalloc(len*0x30)`(0x30=48バイト)し、ユーザバッファから`len*48`バイトを`__memcpy`。各48バイトレコードを先頭から走査し、**非ゼロバイトが1つでもあれば**タグ`0xf0`+レコード全体(48バイト)を、**全48バイトがゼロなら**タグ`0xf1`+4バイト値`1`を代わりに`kfifo_in`する。各レコード処理後`printer_work_lock==1`なら`queue_work_on`。最後にバッファを`kfree`し、処理件数・`HEAT_DELAY`・`MOTOR_DELAY`の現在値を`printk`でログ出力する(これは`0x66`固有の処理であり、当初「`0x76`/`0x77`のデバッグダンプ」と推測していたのは誤りだった — 訂正)。**48バイトレコードの内部レイアウトは未解読**。 |
| `0x67` | ● | **単一印刷ジョブ投入**。`kmem_cache_alloc_trace`で固定長**120バイト(0x78)**の構造体を確保し、ユーザバッファから120バイトをそのままコピー。タグ`0xf3`+120バイトを`prn_fifo`に`kfifo_in`し、確保したバッファは直後に`kfree`(fifoへコピー済みのため)。`printer_work_lock==1`なら`queue_work_on`。**120バイト構造体の内部レイアウトは未解読**。 |
| `0x77` | ● | GPIO `Print_En`(devicetree`en-gpio`)と`P_Paper`(`p-paper`)のraw値をそれぞれ読んで`printk`し、`P_Paper==0`なら真(1)を返す。**用紙切れ/検知コマンド**とほぼ断定できる。 |
| `0x88` | ● | **濃度(darkness)/発熱レベル設定コマンド**。詳細は3.3節。 |
| `0x99` | ● | `len`が0でなければ、タグ`0xf2`+`len`自身の値(4バイト)を`kfifo_in`する(`0x55`と合流する共有コード経由)。`len==0`なら`printer_work_lock`のチェックのみ行い、必要なら`queue_work_on`する(既存キューを起こす「キック専用」コマンドの可能性)。 |
| 上記以外 | ✗ | `printk`でログ出力のみ、戻り値0(未知/無効コマンド) |

> **訂正メモ**: r2ghidra導入前の手動アセンブリ解読では、ジャンプテーブルのバイトオフセット計算を
> 誤り、GPIOセンサー読み取りコマンドを`0x75`、デバッグダンプを`0x76`/`0x77`固有の処理と記載していたが、
> 正式デコンパイルの結果、GPIO読み取りは`0x77`のみであり、デバッグダンプの`printk`は`0x66`処理の
> 末尾に過ぎないことが判明した。上表は訂正済みの内容。

### 3.3 濃度(darkness)設定コマンド `0x88` の演算式(事実、r2ghidraで再確認)

r2ghidra `pdg` が出力した実際の擬似コード(シンボル名は実名):

```c
if (cmd == 0x88) {
    printk("...", *puVar11);   // *puVar11 = level (ユーザ指定, 1バイト, buf[0])
    HEAT_DELAY = ((*puVar11 - 1) & 0xff) * 100 + 0x44c;   // 1100 + 100*(level-1)
    if (0x13 < *puVar11 - 1) {           // (level-1) > 19  つまり level > 20 または level == 0
        HEAT_DELAY = 2000;               // 範囲外は安全値にクランプ
    }
    goto <0x77と共有するGPIO読み取りコードへ>;
}
```

- **事実**: `level` 1〜20 に対し `HEAT_DELAY = 1100 + 100*(level-1)` (単位はコード上は不明だが、
  `MOTOR_DELAY`・`TK`・`TB`が全て`__udelay`の引数としてマイクロ秒で使われていることから、
  同じ単位系(µs)である可能性が高い)が計算され、グローバル変数`HEAT_DELAY`に格納される。
  範囲外入力時は`2000`にクランプされる。
- `0x88`のハンドラは処理の最後に**`0x77`と全く同じGPIO読み取りコード(`Print_En`/`P_Paper`)へ
  合流**する(=`0x88`コマンドも副作用として用紙センサー状態を返す)。これは今回r2ghidraで
  初めて明確になった制御フローで、手動アセンブリ解読の段階でも合流先アドレスの一致には
  気づいていたが、`pdg`によって「同一コードの共有」であることが確定した。
- **訂正の経緯**: 当初は「この`HEAT_DELAY`の値がそのまま`LineColorRank_Print`のストローブ幅に
  なっている」と推測していたが、`LineColorRank_Print`を正式にデコンパイルした結果、
  そちらで使われているのは`HEAT_DELAY`ではなく固定値`TK`/`TB`(144µs/6µs)だった。
  その後`printer_work`のタグ別ディスパッチ(3.5節)を追加調査した結果、
  **`HEAT_DELAY`は`LineColorRank_Print`(16階調モード)ではなく、別の"2値モード"専用コードパス
  (タグ`0xf0`、ioctl `0x66`経由)でストローブ幅として使われている**ことが判明した。
  つまり`0x88`の濃度設定は無効・デッドコードだったわけではなく、**2値印字モードの濃度制御として
  ちゃんと機能している**(詳細は3.5節)。

## 3.5 【重要】2値(binary)モードと16階調モードの併存(事実、`printer_work`のタグ分岐を追加解析して判明)

3.3節・4.3節で「`HEAT_DELAY`を読み出す箇所が見つからない」としていたのは誤りで、
`printer_work`本体のタグ別ディスパッチ(`kfifo`から取り出した先頭1バイトのタグ値で分岐する
4値のジャンプテーブル、5節参照)を追加調査した結果、**このドライバには全く異なる2つの印字経路が
存在し、片方だけが`HEAT_DELAY`を使っている**ことが判明した。

| タグ | 投入元コマンド | 処理 | 濃度制御方式 |
|---|---|---|---|
| `0xf0` | ioctl `0x66`(全ゼロでない48バイトレコード) | 48バイト(=384ビット)を**そのまま1回だけ**`spi_sync`。ストローブ(`PRN_STB`)を**`HEAT_DELAY`(=ioctl `0x88`で設定、1100〜3000µs)ぶんだけ1回High**にしてから紙送り。 | **2値(ON/OFFのみ)。濃度はストローブのパルス幅=`HEAT_DELAY`で直接制御** |
| `0xf1` | ioctl `0x66`(全ゼロの48バイトレコード、値=1が代入) | 4バイト値を取り出し、その回数だけ`motor_forward_one_step`(印字なし・紙送りのみ) | (印字なし) |
| `0xf2` | (詳細未確認、符号付き4バイト値で前後に紙送り) | 正負に応じ`motor_forward_one_step`/`motor_backward_one_step`をN回 | (印字なし) |
| `0xf3` | ioctl `0x67`(単一印刷ジョブ、120バイト) | 120バイトを`LineColorRank_Print(0x10, ptr)`に渡す(第1引数`0x10`=16固定) | **16階調(15パスのマルチパス階調)。ストローブ幅は固定値`TK`(144µs、初回のみ+`TB`=6µs)で、`HEAT_DELAY`は使われない** |

`Print_Picture`(画像印刷用の別エントリポイント)も`LineColorRank_Print(0x10, ...)`を固定値で
240回呼び出しており、常に16階調モードを使う。

**つまり本ドライバは「2値モード」と「16階調(疑似階調)モード」の両方を実装しており、**
- **2値モード**(`0x66`ioctl経由): 1ドットあたり1ビットのバイナリ印字。**濃度はユーザが`0x88`で
  設定した`HEAT_DELAY`(単発ストローブ幅、1100〜3000µs)で直接制御**される。
- **16階調モード**(`0x67`ioctl経由、または画像印刷`Print_Picture`): 1ドットあたり4bit(0〜15)の
  階調値を、最大15回のマルチパス多重ストローブで疑似階調表現する。**この経路のストローブ幅は
  ユーザ設定`HEAT_DELAY`ではなく、コード中に埋め込まれた固定値`TK`/`TB`(144µs/6µs)を使う**ため、
  `0x88`で濃度を変えても16階調モードの印字結果には影響しない(現状のバイナリを見る限り)。

どちらのモードを使うかは、**呼び出し元アプリケーションがどのioctl(`0x66` vs `0x67`)を使うかで
決まる**ため、テキスト印字は2値+濃度調整可能、写真/画像印字は16階調固定という役割分担に
なっていると推測できる(この役割分担自体は「推測」だが、コードの構造とそれぞれのバッファサイズ
(48バイト=1bpp384ドット vs 120バイト=4bpp展開データ)から見て妥当性が高い)。

## 4. 発熱(Heat)・モーター駆動の実装方式(事実+推測、根拠付き。r2ghidraで大幅更新)

### 4.1 結論(先出し、更新版)

**サーマルヘッドの駆動は「2値ON/OFFの単純な点灯制御」ではなく、
「SPIで1ライン分のドットパターンを転送 → ストローブ(`PRN_STB`)GPIOを一定時間Highにして発熱 → Lowで消灯、
を複数パス(階調段数ぶん)繰り返す」方式であり、各パスのストローブ時間はソフトウェアの`udelay`
(ビジーウェイト)で作られている。** これはr2ghidraによる正式デコンパイル後も変わらない結論だが、
**「ストローブ時間がユーザ設定の濃度値と連動しているか」については、r2ghidra導入前の推測が
誤りだったことが判明した**(4.3節)。

- ハードウェアPWMペリフェラルは使っていない(`pwm_config`/`pwm_enable`等の呼び出しは
  当該関数群から一切呼ばれていない — **事実**、`pdg`出力の呼び出し一覧に存在しないことを確認済み)。
- 単なる2値(0/1しかない)描画でもない。**「複数回の弱いストローブパルスの多重照射」による
  疑似階調(マルチパス halftoning)**を行っている(4.2, 4.3節)。
- 一方、当初推測していた「パルス幅そのものをユーザ設定の濃度値(`0x88`)で変える」という
  **2種類目の階調制御については、r2ghidraの結果、根拠が失われた**(4.3節で訂正)。

### 4.2 1ライン分の階調データ生成(`LineColorRank_Print`, 事実、r2ghidra `pdg` で確認)

先頭ループは、入力バッファ(1バイトに2ドットぶんパックされた4bit値、上位ニブル・下位ニブルの順)を
**1バイト=2ドットの4bit階調値(0〜15)に展開**し、グローバル配列`LineColorRankData`
(384エントリ = `0x180`、ヘッド全幅ぶんのライン バッファ)に書き込む。

**訂正**: 当初「384ドット全体が入力バッファから埋まる」かのように記載していたが、
展開ループの条件式を精査した結果、実際に入力バッファ(`arg2`、ioctl `0x67` の120バイト構造体)
から埋められるのは**インデックス`0x48`(72)〜`0x137`(311)の240要素だけ**で、それ以外
(先頭72ドット・末尾72ドット、計144ドット)は無条件に`0`(消灯)でゼロ埋めされる:

```c
uVar14 = 0; uVar11 = 0;
do {
    if (uVar14 - 0x48 < 0xf0) {                     // 0x48(72) <= uVar14 < 0x48+0xf0(312)
        LineColorRankData[uVar11]   = (*arg2) >> 4;  // 上位ニブル
        LineColorRankData[uVar11+1] = (*arg2) & 0xf;  // 下位ニブル
        arg2++;
    } else {
        LineColorRankData[uVar11] = 0x0;              // 範囲外は常に消灯
    }
    uVar11 += 2; uVar14 = uVar11;
} while (uVar14 < 0x180);                             // 0x180 = 384
```

120バイト(ioctl `0x67`のペイロードサイズ)× 1バイト2ドット = ちょうど240ドット分のデータ量で
あり、このサイズが範囲(72〜311, 240要素)を過不足なく埋め切ることからも、この読み方が
正しいことが裏付けられる。

**つまり `LineColorRank_Print`(階調モード)が実際に画像データとして使うのはヘッド全幅384ドットの
うち中央240ドットのみで、左右72ドットずつは常にOFF(余白)である。** 一方 `0x66`(2値モード)は
48バイト=384bitをそのままSPI転送するため、こちらは**フル384ドット幅**を使用する
(3.5節参照)。ヘッド自体の物理解像度は384ドットだが、階調モードのアプリ側は240ドット幅の
画像しか渡していない(または渡せない)ことになる。

### 4.3 多重パス(階調)ループとSPI転送・ストローブ制御(事実、r2ghidra `pdg` の実際の擬似コード)

第1引数(呼び出し元 `printer_work` から渡る、階調段数/濃度ランク数と思われる値)の下位バイトが
2以上のとき、`pass = 1` から `pass == (arg1 & 0xff)` まで(=`pass`は`1`〜`arg1-1`)以下をループする
(r2ghidra `pdg`出力をそのまま整形、シンボル名は実名):

```c
uint8_t pass = 1;
do {
    // 384ドットを8ドットずつ処理し、1バイト(8ドット)のビットマップを作る
    for (group = 0; group < 0x30; group++) {           // 48回 = 384/8
        // 各ドットについて、"そのドットの階調値 >= pass" ならビットを立てる
        // (階調値が大きい=濃いドットほど、より多くのパスで発熱される)
        packed_byte[group] = pack_8_bits(dot => (pass <= LineColorRankData[dot]));
    }

    spi_sync(gPrn->spi, &xfer_of_48_bytes);   // 384ビット=48バイトをSPIでヘッドへシフト

    // ストローブ(PRN_STB)を一定時間Highにして加熱
    uint32_t strobe_us = TK;
    if (pass == 1) {
        strobe_us += TB;      // 初回パスだけ TB ぶん余分に加熱(TK=144, TB=6 が初期値)
    }
    gpiod_direction_output_raw(gpio_to_desc(PRN_STB), 1);
    __udelay(strobe_us);
    gpiod_direction_output_raw(gpio_to_desc(PRN_STB), 0);

    pass++;
} while (pass != (arg1 & 0xff));

motor_forward_one_step();   // 全パス終了後に紙送り1ステップ
```

- **事実として確認できること(r2ghidraで確定)**:
  - `pass <= LineColorRankData[dot]`という比較でビット列を作っている
    (=濃いドットほど、より多くのパス回数で発熱される多重パス階調方式)。
  - `spi_sync()`の呼び出しがパスのたびに行われ、直後に`PRN_STB`GPIOへの
    `gpiod_direction_output_raw(1)` → `__udelay()` → `gpiod_direction_output_raw(0)`という
    「SPI転送→ストローブパルス」のペアが実行される。
  - **ストローブ幅は`TK`(初期値`144`)を基準とし、パス1回目のみ`TB`(初期値`6`)を加算した
    `TK+TB`(=150)を使う**。2回目以降は`TK`(=144)固定。これは`.data`セクションの初期値を
    直接読み取って確認した実測値。
  - `TK`・`TB`はグローバル変数`MOTOR_DELAY`・`HEAT_DELAY`と連続したアドレス
    (`0xedb0ac0`/`0xedb0ac4`)に配置された、同じ制御構造体らしきものの一部。
  - モーター(紙送り)は**1ライン=複数パスの後に1回だけ**前進させる(`motor_forward_one_step`)。
    パスごとに紙を動かしているわけではない(=多重パスは同一ラインの重ね書きであり、
    紙送りを伴わない)。

- **`HEAT_DELAY`はこの16階調モードでは使われない**: このモードのストローブ幅は`TK`/`TB`という
  別のグローバル変数(初期値144µs/6µs)で決まっており、`TK`/`TB`に書き込みを行っている関数は
  今回デコンパイルした範囲(`printer_work`, `uyu_prn_probe`, `gpio_init`, `Print_Picture`,
  `LineColorRankData_Manage`, `IdlePrint`, `PaperCheck`, `pic_motor_forward_one_step`)の
  どこにも見つからず、`.data`の初期値がそのまま使われていると考えられる。
  一方`HEAT_DELAY`(ioctl `0x88`で設定)は**別の"2値印字モード"(3.5節、タグ`0xf0`経路)で
  ちゃんと使われている**ことが`printer_work`本体の追加調査で判明した。つまり
  「濃度設定`0x88`はデッドコードでは全くなく、2値モードの濃度制御として機能している。
  ただし16階調モード(このセクションの`LineColorRank_Print`)には影響しない」という
  **役割分担がある**、というのが最終的な結論(3.5節も参照)。
- `rank_count`(`arg1`、多重パスの段数)は、今回追跡できた2つの呼び出し元(`printer_work`の
  タグ`0xf3`ハンドラ、および`Print_Picture`)がいずれも**`0x10`(16)を固定で渡している**ことを
  確認した。したがって現状把握している範囲では、`LineColorRank_Print`は常に16階調
  (15パス)で呼ばれており、「呼び出し元によってパス数が可変」という仕組みは確認できていない。

### 4.4 「PWMか2値か」への回答(更新版。3.5節の2モード併存を踏まえた最終結論)

このドライバは**2値モードと16階調モードを両方持ち、印字経路(ioctlコマンド)によって
使い分けている**(3.5節)。それぞれの駆動方式は:

- **2値モード**(`0x66`ioctl / タグ`0xf0`): 1ラインにつき**SPI転送1回+ストローブ1回のみ**。
  ドット自体はON/OFFの2値だが、**その単発ストローブの持続時間(`HEAT_DELAY`, 1100〜3000µs)を
  ユーザが`0x88`で調整できる**。古典的な高周波PWM(デューティ比変調)ではなく、
  `__udelay()`による単発ワンショットパルスの幅そのものを変える方式。
- **16階調モード**(`0x67`ioctl / `Print_Picture`): 同一ラインに対し最大15回のSPI転送と
  ストローブを繰り返し、ドットごとの閾値比較で参加パス数を変える**多重パス階調方式**。
  各パスのストローブ幅は固定値`TK`/`TB`(144µs/6µs)で、`0x88`の濃度設定の影響を受けない。

したがって、「2値かPWMか」という問いへの答えは**両方**であり、かつ**モードによって
濃度制御の方式が異なる**(2値モード=パルス幅変調、16階調モード=多重パス空間階調)という
のが、バイナリから直接確認できる最も正確な記述である。

## 5. 印刷ジョブの全体フロー(事実、r2ghidraで更新)

```
ユーザ空間アプリ
   │ ioctl(cmd=0x67, {buf, len})  … 1ジョブ(120B)投入 (タグ0xf3)
   │ ioctl(cmd=0x66, {buf, len})  … 複数ジョブ一括投入 (タグ0xf0/0xf1)
   │ write()                     … prn_dev_write() が内部で
   │                                prn_dev_unlocked_ioctl() を直接呼び出す
   ▼
prn_fifo (kfifo, gPrn+0x70) に [タグ1バイト + ペイロード] 形式で投入
   │
   │ queue_work_on(8, wq, &gPrn->work)   … gPrn->work.func = printer_work (probe時にセット)
   ▼
printer_work()  [workqueueコンテキスト]
   │ mutex_lock(printer_work_lock)
   │ Heat_En を GPIO出力1にし、task_busy=1  ← ジョブ全体の開始時に1回だけ実行(事実、pdgで確認)
   │ __kfifo_out()でタグ1バイトを取り出し続ける限りループ:
   │   タグ値(0xf0〜0xf3、3.5節)に応じたジャンプテーブルで分岐
   │     - 0xf0: 48バイト1回のみSPI転送 + ストローブ幅=HEAT_DELAYで1回発熱 → 2値印字(3.5節)
   │     - 0xf1: 4バイト値ぶんmotor_forward_one_step (紙送りのみ)
   │     - 0xf2: 符号付き4バイト値に応じ前後に紙送り (詳細未解読)
   │     - 0xf3: 120バイトをLineColorRank_Print(16, ...)へ → 16階調印字(3.5節・4節)
   │ kfifoが空になったら: Heat_En を GPIO出力0にし、task_busy=0  ← ジョブ全体の終了時に1回だけ
   │ mutex_unlock
```

**訂正**: 当初「`Heat_On`/`Heat_Off`は各印字ラインの直前直後に呼ばれる」かのように読める書き方を
していたが、r2ghidraで`printer_work`本体を正式デコンパイルした結果、**`Heat_En`(発熱マスタースイッチ)
のON/OFFはジョブ全体(=`printer_work`が1回のkfifo枯渇まで処理する区間)の最初と最後に1回ずつだけ
実行される**ことが分かった。ライン単位の実際の発熱制御は、4.3節の`PRN_STB`ストローブパルスが
担っている。つまり`Heat_En`は「印字ヘッド電源の大元のスイッチ(かつ`task_busy`状態フラグ)」、
`PRN_STB`は「1パスごとの実際の発熱トリガー」という**2段構成**である。

## 6. 未解明・要追加調査の項目(正直な棚卸し、r2ghidra導入後に更新)

- `0x55`テーブル(`0x91f3000+0xcf8`, 40エントリ)の実データ値。
- `0x66`/`0x67`で投入される48バイト・120バイトレコードの内部フィールドレイアウト
  (`0x66`の「全ゼロか否か」で2値印字/紙送りコマンドに分岐する理由・仕様は3.5節で解明したが、
  48バイトが具体的にどのアプリ層フォーマット(フォント/文字コード等)から生成されるかは未解読)。
- `TK`/`TB`(16階調モードのストローブ幅、固定値144µs/6µs)に書き込みを行う関数が見つかっていない。
  ユーザやアプリ側からこれらを調整する手段(sysfs属性や別のioctl等)が存在するのかどうかは未解明。
- タグ`0xf2`(符号付き4バイト値による紙送り)の正確な意味論(`0xf1`との使い分け)は未解読。
- `LineColorRank_Print`の第1引数(多重パス段数)は、今回追跡できた2つの呼び出し元(`printer_work`
  タグ`0xf3`、`Print_Picture`)ではいずれも`16`固定だった。可変にする呼び出し元が別途存在するかは未確認。
- `slm758_dev_probe`(指紋センサー/カメラライト/アルコールボード)は本調査のスコープ外につき未着手。

r2ghidraによる正式な擬似Cコード生成が実現したことで、当初の手動アセンブリ解読より格段に
正確な情報(実変数名・正しいswitch構造・関数境界)が得られたが、それでも上記の項目は
「値は追えるがそれがどう使われるかのコード」までは到達できておらず、追加調査が必要。

## 7. 実機検証: `unbind`+`spidev`化によるユーザーランド直叩きは不成立(事実、2026-09-15実機テスト)

[userland_grayscale_hack.md](../printer/userland_grayscale_hack.md) が前提としていた
「`unbind`→`driver_override`→`spidev`強制bind」を実機(`spi7.0`, カーネル Linux 4.9.193)で試した結果:

- **`/sys/bus/spi/devices/spi7.0/driver_override` というファイルが存在しない**。SPIバスの
  `driver_override`対応はmainlineでは4.9より新しいカーネルの機能で、この端末にはバックポートされていない。
- `driver_override`無しで`echo spi7.0 > /sys/bus/spi/drivers/spidev/bind`を直接試しても、
  `compatible = "qcom,prn-dev"`が`spidev`の`of_device_id`/`spi_device_id`テーブル
  (`rohm,dh2228fv`等の標準プレースホルダーのみ)にマッチせず、`bind`は静かに失敗する
  (`probe()`自体が呼ばれない)。
- `unbind`→(bind失敗)→再度`unbind`→`bind`を短時間に繰り返したところ、2回目の`prn-dev`への
  再bindで`gpio_init`が`Heat_En`/`Print_En`の`gpio_request`で`-EBUSY(-16)`を返し、
  `/dev/prn-dev`が消失。**再起動でのみ復旧できた**(sysfs操作だけでは直せなかった)。
  `uyu_prn_remove()`のGPIO解放処理が、char device側がまだopen中(`PrinterServerApp`が
  ステータスポーリング等で掴んでいた可能性)だと正しく完了しない、という推測はあるが未検証。

**結論**: この端末では`driver_override`頼みのユーザーランド直叩きは成立しない。代替案として
「DTBOのオーバーレイfragmentで`uyu-prn`ノードの`compatible`に`"qcom,prn-dev", "rohm,dh2228fv"`のように
`spidev`が拾える文字列を追加し、通常起動時と手動`unbind`/`bind`後のどちらでも該当ドライバが
bindできるようにする」というアイデアがあるが、**起動時に`prn-dev`と`spidev`のどちらが自動bindを
勝ち取るかはカーネルのリンク順(`device_initcall`のリンク順)次第で不確定**であり、実機で
検証しない限り「通常の1bit印字が起動するたびに死ぬ」リスクが残る。詳細は
[userland_grayscale_hack.md](../printer/userland_grayscale_hack.md) を参照。
