(Japanese, UTF-8)

# ffscrnsave --- スクリーンセーバー ビュア

## これは何？

位置とサイズを指定してスクリーンセーバーを起動できるWindowsソフトです。
デジタルサイネージなどにお使いください。

## 使い方

```txt
使用方法: ffscrnsave [オプション] your_file.scr
オプション:
  -i INPUT.scr            入力ファイルを指定します。
  -x WIDTH                表示される幅を設定します。
  -y HEIGHT               表示される高さを設定します。
  -left LEFT              ウィンドウの左端の位置を指定します。
                          (デフォルトでは中央に配置されます)
  -top TOP                ウィンドウの上端の位置を指定します。
                          (デフォルトでは中央に配置されます)
  -fs                     全画面モードで開始します。
  -noborder               枠のないウィンドウを作成します。
  -window_title TITLE     ウィンドウのタイトルを設定します。
                          (デフォルトでは入力ファイル名が使用されます)
  -help                   このヘルプメッセージを表示します。
  -version                バージョン情報を表示します。
```

## ライセンス

- MIT

## 連絡先

- 片山博文MZ <katayama.hirofumi.mz@gmail.com>

---
(English)

# ffscrnsave --- A screensaver viewer

## What's this?

This is a Windows software that allows you to launch a screensaver with specified position and size.
It can be used for digital signage and other purposes.

## Usage

```txt
Usage: ffscrnsave [Options] your_file.scr

Options:
  -i INPUT.scr          Specify the input file.
  -x WIDTH              Set the displayed width.
  -y HEIGHT             Set the displayed height.
  -left LEFT            Specify the x position of the window's left edge
                        (default is centered).
  -top TOP              Specify the y position of the window's top edge
                        (default is centered).
  -fs                   Start in fullscreen mode.
  -noborder             Create a borderless window.
  -window_title TITLE   Set the window title (default is the input
                        filename).
  -help                 Display this help message.
  -version              Display version information.
```

## License

- MIT

## Contact

- Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
