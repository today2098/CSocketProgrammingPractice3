# CSocketProgrammingPractice3

ソケットプログラミングを勉強した際の成果物．

|ID|Project|Summary|Model|Keyword|
|:---|---|---|---|---|
|0.|Name Resolution|与えられたホスト名から名前解決を行う．|-|名前解決|
|1-1.|Simple TCP|TCP を用いて短いメッセージの送受信を行う．|One to one|TCP|
|1-2.|TCP File Transfer|TCP を用いてファイル転送を行う．|One to one|TCP, ファイル転送|
|1-3.|File Server|ファイルサーバ．任意のファイルのダウンロードを実現する．|One to one|TCP, ファイル転送|
|2-1.|Simple UDP|UDP を用いて短いメッセージの送受信を行う．|One to one|UDP|
|2-2.|UDP File Transfer|UDP を用いてファイル転送を行う．|One to one|UDP, ファイル転送|
|3.|Simple Multicast|マルチキャストにより，特定の複数の端末に対し同一のメッセージを送信する．|One to many|UDP, マルチキャスト|
|4.|Multi TCP Client|スレッドを用いた並列処理により，1対多の通信を行う．|One to many|TCP, マルチクライアント|
|a.|My Library|一部プロジェクトで用いる自作ライブラリ群|-|-|


## Environment

以下の環境で動作確認しました．

- Ubuntu 20.04.6 LTS @ WSL1 @ Windows 11
- GCC 9.4.0
- Cmake 3.16.3


### Prerequisites

前提として，C/C++ に関する一式のビルドツールがインストールされている必要があります．

```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install build-essential
```


## Usage

### Build

初めにシェルスクリプト `build.sh` を用いてビルドします．
ビルドが完了すると，各プロジェクトの実行ファイル `*.out` が作成されます．

```bash
$ pwd  # ~/workspace/CSocketProgrammingPractice3
$ ls
CMakeLists.txt  LICENSE  README.md  build.sh  clear.sh  data  lib  src
$ ./build.sh
...
$ ls
CMakeLists.txt  build     data                   simple_tcp-server.out    src
LICENSE         build.sh  lib                    simple_udp-receiver.out
README.md       clear.sh  simple_tcp-client.out  simple_udp-sender.out
```


### Execution


### Initialization

シェルスクリプト `clean.sh` を用いると，以下の作成されたファイルを削除し初期化します．

- Cmake のビルドツリー `build/`
- 実行ファイル `*.out`
- 実験の出力先ディレクトリ `output/`

```bash
$ pwd  # ~/workspace/CSocketProgrammingPractice3
$ ls
CMakeLists.txt  build     data    simple_tcp-client.out    simple_udp-sender.out
LICENSE         build.sh  lib     simple_tcp-server.out    src
README.md       clear.sh  output  simple_udp-receiver.out
$ ./clean.sh
...
$ ls
CMakeLists.txt  LICENSE  README.md  build.sh  clear.sh  data  lib  src
```


## References

1. M.J. Donahoo & K.L. Calvert. TCP/IP ソケットプログラミング C言語編. 小高知宏監訳, オーム社, 2003, 181p.
1. M.J. Donahoo & K.L. Calvert. TCP/IP Sockets in C. Seconds Edition. Morgan Kaufmann Publishers, 2009, 196p.
1. 富永和人・権藤克彦. 例解 UNIX/Linux プログラミング教室. オーム社, 2018, 507p.


## サンプルデータについて

ディレクトリ `data/` にある画像及び動画は，すべて Creative Commons Zero (CC0) に基づいて配布されたものです．


### 画像

1. Vincent van Gogh. "Self-Portrait". 1887. The Art Institute of Chicago, <https://www.artic.edu/artworks/80607/self-portrait>, (参照 2023-10-22).
1. Claude Monet. "Water Lilies". 1906. The Art Institute of Chicago, <https://www.artic.edu/artworks/16568/water-lilies>, (参照 2023-10-21).


### 動画

1. Kelly. "Pouring Hot water In A Coffee Filter Cup". Pexels, <https://www.pexels.com/video/pouring-hot-water-in-a-coffee-filter-cup-2853793/>, (参照 2023-10-22).
1. Sakura Studio Videoproduktion. "Drone Footage Of The Ocean". Pexels, <https://www.pexels.com/video/drone-footage-of-the-ocean-4232194/>, (参照 2023-10-22).
