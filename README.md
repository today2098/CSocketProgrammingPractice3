# CSocketProgrammingPractice3

ソケットプログラミングを勉強した際の成果物．


## Environment

以下の環境で動作を確認しました．

- Ubuntu 20.04.6 LTS @ WSL1 @ Windows 11
- GCC 9.4.0
- Cmake 3.16.3


### Prerequisites

前提として，一式のビルドツールがインストールされている必要があります．

```bash
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install build-essential
```


## Build

初めにシェルスクリプト `build.sh` を利用してビルドします．
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


## Usage

## Project

|プロジェクト名|概要|
|---|---|
|Simple TCP|TCP を用いて短いメッセージの送受信を行う．|
|Simple UDP|UDP を用いて短いメッセージの送受信を行う．|
|TCP File Transfer|TCP を用いてファイル転送を行う．|
|UDP File Transfer|UDP を用いてファイル転送を行う．|


## サンプルデータについて

フォルダ `data/` にある画像及び動画は，すべて Creative Commons Zero (CC0) に基づいて配布されたものです．


### 画像

1. Vincent van Gogh. "Self-Portrait". 1887. The Art Institute of Chicago. <https://www.artic.edu/artworks/80607/self-portrait>.
1. Claude Monet. "Water Lilies". 1906. The Art Institute of Chicago. <https://www.artic.edu/artworks/16568/water-lilies>.


### 動画

1. Kelly. "Pouring Hot water In A Coffee Filter Cup". Pexels. <https://www.pexels.com/video/pouring-hot-water-in-a-coffee-filter-cup-2853793/>.
1. Sakura Studio Videoproduktion. "Drone Footage Of The Ocean". Pexels. <https://www.pexels.com/video/drone-footage-of-the-ocean-4232194/>.
