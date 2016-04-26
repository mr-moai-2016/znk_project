# libZnk
===================================

## Overview
-----------------------------------
  libZnkはC/C++上で使う汎用の基本処理を集めたよくあるタイプのライブラリである.  
  このバージョンにおいては具体的には以下のような処理をサポートする.  

*   処理系間の違いを吸収するヘッダ(Znk_base.h)
*   汎用のバイナリ配列および文字列処理
*   ファイルIOおよびディレクトリ操作
*   エラー処理
*   ソケット通信およびHTTPに関する処理
*   zipおよびgzipの解凍処理(zlibを使用)
*   動的ロード補助機構
*   環境変数操作
*   汎用変数、プリミティブ型、およびその動的配列
*   汎用の設定ファイル(myf)パーサ
*   ミューテックスおよびスレッド
*   数学およびMD5計算など  

  このライブラリは敢えて比較的小規模に押さえてある.
  とりあえずCでプログラミングする場合、最低限このくらいはないと話にならないといった
  レベルのものに留めてある(C用のライブラリだが一応C++からでも使え、それ用のユーティリティも
  若干だが提供する. C++専用のヘッダについては拡張子をhppとしてある).
  またこのライブラリを使うのに他のライブラリをインストールする必要は基本的にない.
  内部でzlibを使用しているが、このディレクトリ配下に同梱しているため、Windowsにおいて
  わざわざこれをインストールする必要がない.
  Linuxにおいてはさらにlibpthread, libdl, libstdc++を使用しているが、これらはたとえ
  あなたが最小構成でLinuxをインストールしていたとしても、まず確実にシステムに最初から
  入っているものばかりである.
  尚、MacOSでの動作確認はしていないが、サポートしている処理は基本的にBSD系でも問題ない
  ものなので対応させるのはそれほど難しくはないとは思う.


## License
-----------------------------------

  このライブラリは zlib 以外の部分についてはNYSL(煮るなり焼くなり好きにしろ)である.  
  zlibの部分については以下のzlibライセンスに従う.

~~~
  zlib 1.2.3 : Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
~~~

  つまりzlibライブラリを使うソフトウェアに上記の著作権表示を記入する必要はないが、
  もし記入するなら、正しく記入しておかなければならないといった程度の意味である.
  そしてもしこのライブラリのソースコードを改変して使用している場合は、
  オリジナルとの混乱を避けるため一応そのことについて一言書いておけということである.

  よくわからなければこれらについてはあなたのソフトウェアには何も記入せずに
  ライブラリについてはそのままを使用すればよい.


## 付属zlibについて
-----------------------------------

  libZnkはzlibを静的に含める形で使用する.  
  同梱してあるzlibは、ソースコード自体はオリジナルから一切改変していないが、
  付属のexampleやドキュメントファイル類は冗長なので、ライセンス条項(README)以外は
  削除してある.

  libZnk use zlib as statically sub-object.
  zlib in here, its source code itself is not at all modified from its original version,
  but accessory of example and document files are deleted except for the license terms
  (README) to avoid redundancy.


## 免責事項
-----------------------------------

  本ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、
  何らの保証もなく提供されます. 本ソフトウェアの使用によって生じるいかなる損害についても、
  作者は一切の責任を負わないものとします.

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising
  from the use of this software.

