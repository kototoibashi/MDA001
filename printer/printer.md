# プリンター制御仕様 (Printer Control)

DS-MDA001 に内蔵されたサーマルプリンターの制御アーキテクチャ、JNI / Native コマンド仕様、ラスタライズ方式、印字フォーマットをまとめます。

本ページの内容は、元アプリ `com.hikvision.alcoholtest` の逆コンパイル結果に加え、**ネイティブライブラリ `libgzds_utils.so` (armeabi, 18,000 bytes) の逆アセンブル**によって裏取りしています。

!!! info "ソースの所在"
    逆コンパイル済みソースはリポジトリの `apks/decompiled/com.hikvision.alcoholtest/sources/` 以下にあります（再配布はしていません）。

---

## 1. システムアーキテクチャ

```mermaid
graph TD
    A["Hikvision Activity<br/>(NormalTestModeActivity)"] -->|PrintUtil.printAll()| B["PrintUtil<br/>(com.hikvision.alcoholtest.util)"]
    B -->|High-Level API| C["Printer / CPrint<br/>(com.gzds.utils)"]
    C -->|JNI: Command cmd,len,data,transport| D["libgzds_utils.so"]
    D -->|write / ioctl| F["内蔵サーマルプリンター<br/>(/dev/prn-dev)"]
```

| モジュール | パッケージ / パス | 役割 |
| :--- | :--- | :--- |
| **PrintUtil** | `com.hikvision.alcoholtest.util.PrintUtil` | 許可証レシート・顔写真の非同期印刷タスク生成 |
| **Printer** | `com.gzds.utils.Printer` | `CPrint` のラッパー。戻り値を Boolean へ変換 |
| **CPrint** | `com.gzds.utils.CPrint` | テキストの Bitmap 描画、モノクロ化・ビットパック、JNI 呼出 |
| **CSerialPort** | `com.gzds.utils.CSerialPort` | **プリンターではなく UART** (`/dev/ttyMSM1`) 用。`su` によるパーミッション昇格を含む |
| **libgzds_utils.so** | `System.loadLibrary("gzds_utils")` | パケット組み立てとデバイスへの書き込み |

`libgzds_utils.so` がエクスポートしている JNI シンボルは 5 つだけです。

```text
Java_com_gzds_utils_CPrint_Open        (どの Java クラスからも宣言されていない — dead export)
Java_com_gzds_utils_CPrint_Close
Java_com_gzds_utils_CPrint_Command
Java_com_gzds_utils_CSerialPort_open
Java_com_gzds_utils_CSerialPort_close
```

`/dev/prn-dev` は `Command` の中で遅延オープン (`open(path, O_RDWR)`) されます。`Open()` は使われていません。デバイスノードの権限は `vendor/etc/init/hw/init.target.rc` の `chmod 0777 /dev/prn-dev` により最初から全開です。

---

## 2. JNI インターフェース

```java
private static native int  Command(int cmd, int param1, byte[] data, int printerType);
private static native void Close();
```

### 2.1 第4引数はプリンター機種ではなく「トランスポート選択」

逆アセンブルの結果、`printerType` は**パケットの組み立て方と送信方法を選ぶスイッチ**であることが判明しました。

| 値 | 動作 | 実使用 |
| :-: | :--- | :--- |
| **0** | `ioctl(fd, op, {dataPtr, param1})`。オペコードごとに `IOCTL_PRINTER_TASK` 等へ分岐 | **どこからも呼ばれていない（dead）** |
| **1** | `[cmd(1B)][param1(LE32)][param1 × 48 バイトのデータ]` を組み立てて **1 回の `write(2)`** | 通常の印字はすべてこれ |
| **2** | `cmd` と `param1` を**完全に無視**し、`[0x67][固定120バイト]` の 121 バイトを `write(2)` | グレースケール専用 |
| その他 | `-1` を返して何もしない | — |

```text
; libgzds_utils.so  Java_com_gzds_utils_CPrint_Command
0x1242: cmp.w sb, #2  ; beq 0x129c    -> type 2 : 0x67 固定パケット
0x1248: cmp.w sb, #1  ; beq 0x12c6    -> type 1 : raw write
0x124e: cmp.w sb, #0  ; bne 0x1296    -> type 0 : ioctl
0x1296: mov.w r6, #-1
```

`PrinterInit(int)` は単に `PrinterType` フィールドへ代入するだけで、実行時に渡される値は **1 のみ**です。グレースケールの `2` は `sendPrintPictureData()` の呼び出し側でハードコードされており、`PrinterType` を経由しません。

### 2.2 戻り値は `write(2)` / `ioctl(2)` の戻り値そのもの

ネイティブ側にプロトコル応答のパース処理は存在しません。`getPrinterStatus()` が `== 1` と比較しているのは `write()` の戻り値です。ステータスの 0/1 の意味は `/dev/prn-dev` のカーネルドライバ側の仕様であり、**本リポジトリの資料からは検証できていません**（実挙動としてビジー中に 1 が返ることは確認済み）。

---

## 3. オペコード一覧 {#3}

| Opcode | Hex | 定数名 | Java 側の呼び出し元 | `printerType=1` での実際の効果 |
| :---: | :---: | :--- | :--- | :--- |
| **68** | `0x44` | `PRINTER_STATUS` | `getPrinterStatus()` | `write(fd, {0x44,0,0,0,0}, 5)`。`param1=0` のため**データ `{20}` は送られない** |
| **85** | `0x55` | `PRINTER_PRINTER_FEED` | `PrintLineFeed(n>=0)` / `speed(byte[])` | 紙送り。`param1=50` なら 5 + 2400 バイトのゼロ埋め |
| **102** | `0x66` | `PRINTER_TASK` | `PrintPixel()` / `PrintText()` | **1bit ラスタ印字**。`param1` = 行数、48 バイト/行 |
| **103** | `0x67` | (定数なし) | `sendPrintPictureData()` | **16階調グレースケール**。`printerType=2` 固定、常に 121 バイト |
| **119** | `0x77` | `PRINTER_PAPERCHECK` | `PrintCheckPaper()` | `write(fd, {0x77,0,0,0,0}, 5)`。**データ `{20}` は送られない** |
| **136** | `0x88` | `PRINTER_DARKNESS` | `PrinterSetDarkness(byte)` | `write(fd, {0x88,0,0,0,0}, 5)`。**濃度値が送信されていない**（後述） |
| **153** | `0x99` | `PRINTER_BACKWARD` | `PrintLineFeed(n<0)` | 逆送り。`0x55` と同形式 |

!!! bug "ダークネス設定・用紙チェックはデータが落ちている"
    `PrinterSetDarkness(b)` は `Command(0x88, 0, new byte[]{b}, 1)` を呼びますが、`printerType=1` のパケット組み立ては**データ長を `param1 × 48` で計算する**ため、`param1 == 0` だとペイロードが 0 バイトになります。つまり濃度値そのものがプリンターに届いていません。`PrintCheckPaper()` (`0x77`) と `getPrinterStatus()` (`0x44`) も同様に `{20}` が落ちます。

    これらが機能しているように見えるのは、**オペコードだけで完結するコマンドだから**と考えられます。濃度を実際に変えたい場合は `param1` の与え方を含めた別途検証が必要です。

!!! warning "`speed(byte[])` は範囲外読み出しになる"
    `speed()` は `Command(0x55, 1, bArr, 1)` を呼びます。`param1=1` なので native は **48 バイト**をコピーしますが、渡される配列は 1 バイトです。使わないでください。

---

## 4. 2つのラスタパス — 1bit と グレースケール {#4}

プリンターには **1bit モノクロ**と **16階調グレースケール**の 2 系統があり、**印字幅が異なります**。

| | 1bit モノクロ | グレースケール |
| :--- | :--- | :--- |
| Opcode | `0x66` (102) | `0x67` (103) |
| トランスポート | `printerType = 1` | `printerType = 2`（固定） |
| **印字幅** | **384 px** | **240 px** |
| 1行あたり | 48 バイト（8px/バイト） | 120 バイト（2px/バイト） |
| bit/px | 1 | **4** |
| `param1` | 行数 | **native 側で無視される** |
| 1コマンドのバイト数 | `param1 × 48 + 5` | 常に 121（＝1 ライン） |

グレースケールは**印字幅が 384 → 240 px と狭くなります**。顔写真のプリントを想定した機能と思われ、元アプリでの用途も測定者の顔写真のみです。

### 4.1 1bit パス（384 px） {#raster-1bit}

`CPrint.PrintText()` は `ARGB_8888` ビットマップの全ピクセルを走査し、白以外 (`pixel != -1`) を黒ドットとして MSB 先頭で 1 バイトに 8 px パックします。

```java
int i2 = 7;
byte b = 0;
int i3 = 0;
for (int i4 = 0; i4 < mTextHeight; i4++) {
    for (int i5 = 0; i5 < mTextWidth; i5++) {   // mTextWidth = 384
        if (mBitmap.getPixel(i5, i4) != -1) {
            b = (byte) (b | (1 << i2));
        }
        i2--;
        if (i2 < 0) { bArr[i3++] = b; i2 = 7; b = 0; }
    }
}
Command(102, mTextHeight, bArr, PrinterType);
```

!!! note "384 という数字の出どころ"
    逆コンパイル結果では `mTextWidth = Function.USE_VARARGS;` と表示されます。これは JNA の定数で、**値は 384** です（`com/sun/jna/Function.java` の `public static final int USE_VARARGS = 384;`）。jadx が元 APK のクラスパスにあった JNA の定数で置換したもので、native 側が `param1 × 48` バイトを読むことと完全に一致します。

### 4.2 グレースケールパス（240 px / 4bpp） {#raster-gray}

1 バイトに 2 ピクセルを詰め、**上位ニブル = 左（偶数 x）、下位ニブル = 右（奇数 x）**。値は反転輝度（0 = 白／無加熱、15 = 最黒）です。

```java
// ImageUtils.java — BT.601 を反転チャンネルに適用
luma  = (((255-r)*66 + (255-g)*129 + (255-b)*25 + 128) >> 8) + 16;
level = luma / 15;
bArr2[i3] = (byte) ((levelLeft << 4) + levelRight);
```

送信は **1 スキャンラインずつ**です。240×240 の顔写真なら 240 回の呼び出しになります。

```java
// PrintUtil.java
for (int i3 = 0; i3 < 240; i3++) {
    CPrint.sendPrintPictureData(120, PrintUtil.getData(i2, bArrImageTo16Gray));
    i2 += 120;   // 120 バイト = 240 px
}
```

!!! bug "元コードのニブルオーバーフロー"
    `ImageUtils.java` の `int i8 = i7 / 15;` は `i7` が 0〜255 のため **0〜17** を返し、4bit のニブル (`17 << 4` = 272) を溢れさせます。自作の [PrinterServerApp](print_server.md) 側では `(adjustedLuma / 15).coerceIn(1, 15)` として修正済みです。

---

## 5. レシート（許可証・測定結果）印字フォーマット

`PrintUtil.printAll()` が出力するレイアウト構成です。

```text
+------------------------------------------+
|             許可証                        | <- タイトル (40pt)
|             スタンドアロンモード           | <- 動作モード (30pt, 条件付き)
| [顔写真 16階調グレースケール 240px幅]      | <- グレースケールパス
| ID：12345                                 |
| 氏名：山田 太郎                            |
| 時間：2026-08-06 20:00:00                 |
| 車両出帰状態：出庫                         |
| 車両番号：品川 500 あ 12-34                |
| アルコール濃度:0.00mg/L                    |
| 温度：36.5℃                               |
| 判定：OK                                   |
|                                          |
|                                          | <- 紙送り 3行 (PrintLineFeed(3))
+------------------------------------------+
```

判定は **0.15 mg/L**（日本の道交法基準）が閾値です。

!!! bug "`print()` と `printAll()` で境界値の扱いが違う"
    - `PrintUtil.print()` … `d > 0.15` で NG、`d <= 0.15` で OK → **0.15 は OK**
    - `PrintUtil.printAll()` および DB 記録 … `d >= 0.15` で NG、`d < 0.15` で OK → **0.15 は NG**

    ちょうど 0.15 mg/L のとき両者が食い違います。判定ロジック全体については [測定モードと運用仕様](../alcohol_checker/test_modes.md) を参照してください。

---

## 6. ハードウェア・デバイスノード

- **プリンター**: `/dev/prn-dev`（`init.target.rc` で `chmod 0777`）
- **アルコールセンサー UART**: `/dev/ttyMSM1`（プリンターとは無関係）
- 用紙: 紙幅 57〜58mm、ロール外径 25mm まで

`CSerialPort` のコンストラクタは、権限が足りない場合に `su` を実行して `chmod 777` を発行します。

```java
if (!file.canRead() || !file.canWrite()) {
    Process processExec = Runtime.getRuntime().exec("/system/bin/su");
    processExec.getOutputStream().write(("chmod 777 " + file.getAbsolutePath() + "\nexit\n").getBytes());
    processExec.waitFor();
}
```

---

## 7. API リファレンス (`com.gzds.utils.Printer`)

| メソッド | 引数 | 戻り値 | 説明 |
| :--- | :--- | :--- | :--- |
| `PrinterInit(int type)` | `int` | `int` | トランスポート選択の設定（実質 `1` 固定）。詳細は §2.1 |
| `PrinterSetDarkness(byte)` | `byte` (1-20) | `Boolean` | 印字濃度設定。**現状の呼び出し方では値が送信されません**（§3） |
| `PrinterSetFont(Typeface)` | `Typeface` | `Boolean` | 使用フォントの指定 |
| `PrinterSetFontZoom(float)` | `float` | `Boolean` | フォント横方向倍率 (`mZoomWidth`) |
| `PrintData(String, int)` | `String`, `int` | `Boolean` | テキスト印刷（内部で 384px 幅にラスタライズ） |
| `PrintLineFeed(int)` | `int` | `Boolean` | 紙送り（負数でバックフィード） |
| `PrintCheckPaper()` | なし | `Boolean` | 用紙有無チェック |
| `PrinterClose()` | なし | `Boolean` | リソース解放 |

---

## 次に読む

- [印字ノウハウ・ハマりどころ](printing_tips.md) — 実際にきれいに印字するための知見
- [PrinterServerApp 開発記録](print_server.md) — 自前アプリからの制御
