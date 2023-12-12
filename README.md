# MZDiskExplorer
これは、MZ用のD88ディスクイメージファイルを操作するためのプログラムです。

https://github.com/kuran-kuran/MZDiskExplorer

## はじめに
D88 ディスクイメージファイルからファイルを取り出すことができます。
D88 ディスクイメージファイルにファイルを記録することができます。
D88 ディスクイメージファイルからブートプログラムを取り出すことができます。
D88 ディスクイメージファイルにブートプログラムを記録することができます。

## 対応ディスク
- MZ-80B/700/1500/2000/2200/2500用のD88ディスクイメージファイル
- MZ-80KのSP-6010形式のD88ディスクイメージファイル
- MZ-80KのSP-6110形式のD88ディスクイメージファイル
- MZ-80AのD88ディスクイメージファイル

## 65536バイトの起動ファイル対応
IPLS09bKai対応のため65536バイトのファイル書き込みに対応しました。  
ところがmztファイルは65535バイトまでしか対応していないためそのままだと65535バイトまでしか書き込めません。  
そこでファイルサイズが65536バイトの場合はファイルサイズを65535にしてディレクトリ未使用領域に最後の1バイトを設定する事によって65536バイトに対応しています。  
ディレクトリの未使用領域編集はMZDiskExplorer 2.1.1にて対応しています。  
また、65536バイト丁度のバイナリファイルをドロップする事でも未使用領域が設定されます。  

## ダウンロード
https://github.com/kuran-kuran/MZDiskExplorer/releases

### 古いバージョンはこちら
https://github.com/kuran-kuran/MZDiskExplorer/tree/main/Download

## ファイル構成
```
-\
+ MZDiskExplorer32.exe : 32ビット版
+ MZDiskExplorer64.exe : 64ビット版
+ MZDiskExplorer.txt   : 説明
```

## ライセンス
MZDiskExplorerはMIT Licenseとします。

## 作者について
- Twitter

https://twitter.com/kuran_kuran

- アルゴの記憶

https://daimonsoft.info/argo/

