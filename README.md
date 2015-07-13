Lumino.Core
===========

##はじめに
Lumino.Core は、C++ によるアプリケーション開発を支援するためのユーティリティクラスライブラリです。
文字列、動的配列、ファイルI/O、文字コード変換、XML などの、ユーザーインターフェイスから独立した多数の機能を提供しています。


##ビルド方法
複数のプラットフォーム向けのビルドシステムとして、CMake を使用します。
CMake がインストールされていなければ、ご使用の OS 用の CMake をダウンロードしてインストールする必要があります。
- http://www.cmake.org/

ビルドに成功すると、"lib" フォルダにライブラリファイルがコピーされます。

###VisualStudio でビルド (Windows)
1. CMake GUI を起動します。
2. "Where is the source code:" に、このファイルがあるディレクトリパスを指定します。
3. エクスプローラで "build" フォルダを作成し、"Where to build the binaried:" にパスを指定します。
4. "Configure" ボタンをクリックして VisualStudio のバージョンを選択し、"Finish" をクリックします。
5. "Generate" ボタンをクリックします。
6. "build" フォルダ内に生成されたソリューションファイル(.sln) を開き、ビルドします。

###XCode でビルド (Mac)
$ cd <このファイルがあるディレクトリ>  
$ mkdir build  
$ cd build  
$ cmake .. -G "Xcode"  
$ open LuminoMath.xcodeproj  

###コマンドラインからのビルド (Windows/Mac/Linux)
$ cd <このファイルがあるディレクトリ>  
$ mkdir build  
$ cd build  
$ cmake ..  
$ make  


##ロードマップ
####


##Licence
[zlib/libpng License](http://opensource.org/licenses/zlib-license.php)
([日本語訳](http://sourceforge.jp/projects/opensource/wiki/licenses/zlib_libpng_license))
