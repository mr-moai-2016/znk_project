# libZnk
===================================

## Overview
-----------------------------------
  libZnkはC/C++上で使う汎用の基本処理を集めたよくあるタイプのライブラリである.  
  このバージョンにおいては具体的には以下のような処理をサポートする.  

  <ul>
  <li>処理系間の違いを吸収する基本ヘッダ(すべてのZnkヘッダからincludeされる):<br>
	Znk_base.h
  </li>
  <li>汎用のバイナリ配列および文字列処理:<br>
	Znk_bfr.h Znk_vpod.h Znk_bfr_ary.h<br>
	Znk_str.h Znk_str_ex.h Znk_str_pth.h Znk_str_ary<br>
	Znk_s_base.h<br>
	Znk_txt_filter.h
  </li>
  <li>標準または準標準関数のラッパー群(Znk_snprintfなど):<br>
	Znk_stdc.h Znk_s_posix.h Znk_missing_libc.h 
  </li>
  <li>ファイルIOおよびディレクトリ操作:<br>
	Znk_str_fio.h Znk_dir.h
  </li>
  <li>エラー処理:<br>
	Znk_err.h Znk_sys_errno.h<br>
	Znk_s_atom.h
  </li>
  <li>ネットワークAPI、ソケット通信およびHTTPに関する基本処理:<br>
	Znk_net_base.h Znk_net_ip.h<br>
	Znk_socket.h Znk_server.h Znk_fdset.h<br>
	Znk_htp_hdrs.h Znk_htp_rar.h<br>
	Znk_cookie.h
  </li>
  <li>zipおよびgzipの解凍処理(zlibを使用):<br>
	Znk_zlib.h
  </li>
  <li>動的ロード補助機構:<br>
	Znk_dlink.h Znk_dlhlp.h
  </li>
  <li>環境変数操作:<br>
	Znk_envvar.h
  </li>
  <li>汎用変数、プリミティブ型、およびその動的配列:<br>
	Znk_var.h Znk_prim.h Znk_varp_ary.h
  </li>
  <li>汎用の設定ファイル(myf)パーサ:<br>
	Znk_myf.h
  </li>
  <li>ミューテックスおよびスレッド:<br>
	Znk_mutex.h Znk_thread.h
  </li>
  <li>数学およびMD5計算など:<br>
	Znk_math.h Znk_md5.h 
  </li>
  </ul>

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

