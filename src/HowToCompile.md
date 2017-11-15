# ソースコードのコンパイル手順
-----------------------------------

## <a name="index"></a>目次
-----------------------------------
* [Windowsな方へ、はじめてのコマンドプロンプト](#user-content-windows_first)
* [Windowsな方へ、はじめてのMinGW             ](#user-content-windows_next)
* [MinGWの入手方法         ](#user-content-get_mingw)
* [WindowsでMinGWを使う場合](#user-content-use_mingw_on_windows)
* [WindowsでVCを使う場合   ](#user-content-use_vc_on_windows)
* [Linuxの場合    ](#user-content-compile_on_linux)
* [Cygwinの場合   ](#user-content-compile_on_cygwin)
* [MSYS1.0でMinGWを使用する場合](#user-content-compile_on_msys10)
* [WindowsでBCC5.5(Borland C++ Compiler 5.5)またはDMC(Digital Mars C/C++)を使う場合(おまけ)](#user-content-use_others_on_windows)
* [Androidについて](#user-content-about_android)
* [MacOSについて  ](#user-content-about_macos)

## <a name="windows_first"></a>Windowsな方へ、はじめてのコマンドプロンプト
-----------------------------------

  まずスタートメニューからコマンドプロンプトを開く方法からである.
  **そこからかよッ！** って方はこの項はスルーして[次へ](#user-content-windows_next)進むこと！

  「**スタートメニュー？なんですかそれは？**」な方は画面一番左下にあるWindowsのロゴが
  入ったボタンがそれである.  「そんなものは見当たらない…」な方は…やむを得まい.
  「コマンドプロンプト Windows8」をキーワードにしてググって欲しい.

  スタートメニューを開いたら「プログラム」=>「アクセサリ」=>「コマンドプロンプト」の順で選ぶ.  
  すると真っ黒なウィンドウが表示されるはずである.  これがコマンドプロンプトである！

  Windows7の場合はこれが「すべてのプログラム」=>「アクセサリ」=>「コマンドプロンプト」の順となる.  
  Windows10の場合は「すべてのアプリ」=>「Windows システムツール」=>「コマンドプロンプト」の順となる.  
  Windows8/8.1の場合は、スタートメニューに該当するボタンないのだが、右クリックした時に
  画面一番右下に現れる「すべてのアプリ」ボタンを押して欲しい. すると画面上に「コマンドプロンプト」
  のアイコンが現れるのですぐにわかると思う. どうしてもわからない場合はここであれこれ言うより
  やはりググってもらった方が早いだろう.  

  コマンドプロンプトを閉じるにはキーボードから exit と入力してEnterキーを押すか
  普通にウィンドウ右上にある x ボタンを押す.

  以上がコマンドプロンプトを開く方法だ！
  全くの初心者の方は**10回くらい開けて閉じてを繰り返し**、確実にマスターしておくことォ！

  <a href="#user-content-index">目次へ戻る</a>


## <a name="windows_next"></a>Windowsな方へ、はじめてのMinGW
-----------------------------------

  コマンドプロンプトを開けるようになったであろうか？
  まだな方はもう**10回くらい**練習して確実にマスターしておくことォ！

  では次にコンパイラをインストールしよう.
  **MinGW**と呼ばれている最もメジャーなものの一つを使う.

  「[MinGWの入手方法](#user-content-get_mingw)」の項へと進もう.
  (既にインストールしてある方はこの項はスルーすること)

  尚、MinGWではなくVCを使いたいという方は「[WindowsでVCを使う場合](#user-content-use_vc_on_windows)」を参照していただきたい.

  <a href="#user-content-index">目次へ戻る</a>


## <a name="get_mingw"></a>MinGWの入手方法:
-----------------------------------
  メジャーなだけにググればいくらでも入手およびインストール方法が解説されたサイトはあるのだが
  ここでも簡単に説明しておく.

  まずダウンロードサイトである https://sourceforge.net/projects/mingw へアクセス.
  ( MinGWの公式サイトは http://www.mingw.org なのであるがダウンロードするファイル群は
   すべてsourceforgeの方へ置かれてある )
  「Download mingw-get-setup.exe」と書かれた緑のボタンがあるのでそれをクリック.
  mingw-get-setup.exe をダウンロードする. たとえばC:\MinGWというフォルダを作って
  その中に保存しておくとよいだろう(これのファイルサイズは85KBと極めて小さい).

  次にmingw-get-setup.exe を実行する.
  このプログラムはMinGWのインストーラを自動でダウンロードするといういわば
  **インストーラをインストールするインストーラ**である(わかりにくい言い回しだが).
  次の手順で行う.

  1. 多分最初に**アホな**Windowsが「セキュリティの警告」を表示してくるかもしれないが
     **MinGWにその種の問題があろうはずもない**ので「実行」.

  2. 「MinGW Installation Manager Setup Tool」ウィンドウが表示されるので「Install」を押す.

  3. 次の画面でインストール先を指定できる. デフォルトでは C:\MinGWとなっているが「Change」で
     これを変えることもできる(その場合は以下の説明でC:\MinGWとなっている部分をそのフォルダで
     読み替えて欲しい). 最終的に180MB程度必要であるので、万一余裕がなければ他のドライブに変える
     などすること. その他特に弄る必要はないのでこれを設定し終えたら「Continue」を押す.

  4. するとC:\MinGW配下へファイル群が自動でダウンロードされる. ここでダウンロードされるのは
     MinGW本体ではなくそのインストーラであり、合計サイズにしておよそ2MBほどである. 100%と表示されたら、
    「Continue」を押す. ここで間違って「Quit」を押すとウィンドウが閉じてしまうが…
	
     ** C:\MinGW\bin\mingw-get.exeを実行すれば続きが表示されます **のでご安心を！
	
     この C:\MinGW\bin\mingw-get.exe こそがMinGWの本当のインストーラであり、Installation Manager
     と呼ばれるものである.

  5. 「MinGW Installation Manager」と書かれたウィンドウが表示されるのでここからインストールしたい
     項目を選ぶ. 全部選んでいたらとんでもない分量になるし、その必要も全くないのでここでは
     mingw32-base と mingw32-g++だけを選ぶ(これだけでも合計180MB程度となり、実のところ結構な
     分量にはなる.
     まず左側のペインにおいて「Basic Setup」を選んだ状態にし、右側のペインでmingw32-baseとある
     行の四角形のところをクリックする(あるいは右クリックでもよい).
     このときプルダウンメニューが表示されるので「Mark for Installation」を選ぶ.
     四角形のところにオレンジ色の矢印マークが表示されれば選択された状態になったことを意味する.
     mingw32-g++についても同様にして選択状態にしておこう.
	
     繰り返すがこれら以外はいらない. 必要最小限こそ我々のモットーである.
     しかし万が一その他が必要になることもあるかもしれないし…これだけで本当に大丈夫なのか？
	
     ** 後でもう一度C:\MinGW\bin\mingw-get.exeを実行すればいつでも追加インストールできます **のでご安心を！
	
     このあたりに関して心配なプログラマの方は下記の[補足事項1](#user-content-note1)も参照されたい.

  6. さて、準備は整ったのでいよいよ実際のパッケージ本体をダウンロードおよびインストールする.
     一番左上にある「Installation」メニューを開き、「Apply Changes」を選ぶ.
    「Schedule of Pending Actions」というウィンドウが現れるので「Apply」ボタンを押せば実際に
     パッケージ群がダウンロードおよびインストールされる.
	
     パッケージは勿論圧縮されてあるのでダウンロードは大して時間は掛からない.
     ネットワークが特別混雑していないなら、筆者の貧弱な回線でも３分ほどで終る分量である.
     またインストールといってもC:\MinGW配下に必要な圧縮ファイルが解凍されるだけのことである.
     上でも述べた通り、mingw32-baseだけの場合、展開後は合計150MBほどになる.

  7. 「Applying Scaduled Changes」というウィンドウが出て「All changes were applied successfully;」
     などと書いてあれば成功である. 「Close」ボタンを押そう.
    「MinGW Installation Manager」ウィンドウがまだ表示されていると思うがこれも右上のxボタンで閉じてよい.

  これにて一件落着インストール完了である.
  「[WindowsでMinGWを使う場合](#user-content-use_mingw_on_windows)」の項へと進もう.


<a name="note1"></a>**【補足事項1】プログラマな方へ**
~~~
    mingw32-baseによりインストールされるのは以下である.

    * コンパイラgcc(Cのみ)
    * リンカld
    * ライブラリアンarおよびranlib、
    * makeコマンド(MinGWではmingw32-make.exeという名前である)
    * デバッガgdb
    * 標準Cライブラリ(このとき動的リンクされるのはデフォルトではmsvcrt.dllである)
    * MinGW用のWindowAPI SDK
    * リソースコンパイラwindres(iconなどの設定に利用)
    * プロファイラgprof
    * その他若干のGNUユーティリティ

    一方、mingw32-g++によりインストールされるのは以下である.

    * コンパイラgcc(C++用の内部コマンドg++)
    * 標準C++ライブラリ(libstdc++)
 
    よってもしC++は全く扱わず、つまり本当にC言語だけしか扱わないのであれば、
    実のところmingw32-baseだけで十分であり、つまりmingw32-g++は不要である.
    (mingw32-baseだけの場合、30MBほど節約できて全部で合計150MB程度になる). 
    とはいえ、実際には C++ のコードも C と一緒くたになって巷に多く溢れかえっており、
    これがコンパイルできないようでは困るので今回はこれも必須項目とした.

    尚、gccは様々な言語のフロントエンドになっており、C++をコンパイルする際も
    コマンドとしてはgccコマンドを使えばよい(gccは拡張子によってCかC++かを自動的に判断する).
    もしmingw32-g++をインストールしていない状態でC++をコンパイルしようとすると
    gccは以下の非常にわかりにくいエラーメッセージを出すので注意が必要だ. 

    gcc: error: CreateProcess: No such file or directory

    もしもこれが表示されたなら mingw32-g++をインストールし忘れているか、
    失敗しているかしているのでもう一度mingw-get.exeを起動してこれを再インストールしよう.
~~~


<a name="note2"></a>**【補足事項2】MinGWのアンインストール、移動、バックアップについて**
~~~
    mingw32-base および mingw32-g++ はクリーンかつハンディである.
    これのインストールにおいてはレジストリなどの書き込みなどは行われない.
    従ってアンインストールしたい場合は単にC:\MinGWをそのフォルダ毎削除すればよい.
    
    またインストールした後、そのフォルダを丸ごと別の場所へ移動してもかまわない.
    あるいはあなたの２台目のマシンに、そのフォルダをコピーするだけでそのマシンでも
    使うことができる. つまり２台目のマシン上で再び上記の手順でインストールする必要は
    ないということである(してもよいけれども).
    
    そしてバックアップについても同様に行える. 例えばMinGWが用済みになってしばらく使う
    予定がないので今はPCから削除したいが、今度また使うこともあるかもしれないと思われるなら、
    MinGWフォルダ全体を一枚のCDに焼くなどしてとっておくとよい. 次回必要になった時
    そのフォルダをそのまま丸ごとPCへコピーすれば即座に復元できるからである.
    
    ダウンロードされたアーカイブは、C:\MinGW\var\cache\mingw-get\packages内に一時保存
    されているので、気になるならそちらも確認されるとよい. インストールが完全に終った後であれば、
    このフォルダ内のアーカイブファイル(拡張子がxz、lzmaとなっているもの)は削除してもよい.
    また万一、通信の過程でこれらのアーカイブが壊れているかダウンロードが不完全であった場合は、
    再ダウンロードが必要となるが、その場合もこのフォルダ内の該当アーカイブを手動で消す必要がある.
    さらにはその場合はデフォルトでお使いのブラウザのキャッシュも消去しておく必要がある
    ( mingw-get.exeはダウンロード時、お使いのブラウザの機能を内部で利用しているためである ).
~~~

  <a href="#user-content-index">目次へ戻る</a>


## <a name="use_mingw_on_windows"></a>WindowsでMinGWを使う場合
-----------------------------------

  src/bat_utils フォルダにsetenv_mingw.bat というファイルがある.
  これをテキストエディタ(メモ帳など)で開き、以下の行を必要に応じて適切に修正する.

~~~
    set ZNK_MINGW_DIR=C:\MinGW
~~~

  C:\MinGWの部分をあなたがインストールしたMinGWのフォルダのパスに修正するだけである.
  (C:\MinGWにインストールされている場合は修正の必要はない)
  修正し終ったらこれを上書き保存し、srcフォルダにある compile_by_mingw.batを
  クリックして実行しよう.

  MinGWが適切にインストールされているならば、すべてのコンパイルが自動で行われ、
  それぞれのプロジェクトのフォルダ内にout_dirというフォルダも自動的に生成され、
  その中に目的のexeファイルやdllが生成されているはずである.

  後はこれらをsrcフォルダの一つ上にあるbin_for_win32フォルダへとコピーするだけなのだが
  面倒なら srcフォルダにある install_bin_for_win32.bat を実行すればこのコピーまで自動的に
  行ってくれるので活用するとよい.

  **ハイ！初心者の方はここまでッ！**
  これ以降は中級者の方向けの内容となっております.


  **【参考】compile_by_mingw.bat が内部で行っていること**

  ここではcompile_by_mingw.bat を使わずに設定する方法を述べる.
  換言すればcompile_by_mingw.bat が内部で行っていることの解説でもある.

  基本的にはコマンドプロンプトを開き、mingw32-make.exe が実行できるように環境変数を整えた上で、
  以下を実行するとビルド可能である.
~~~
    mingw32-make -f Makefile_mingw.mak
~~~

  整えなければならない環境変数は、PATHである. 

  環境変数PATHに関しては、以下のコマンドが存在するフォルダのパスを ; 区切りで追加する.
~~~
    gcc.exe  ld.exe  ar.exe  ranlib.exe  mingw32-make.exe
~~~
  例えば これらが C:\MinGW\bin内にあるならば、コマンドプロンプト上で以下のように入力して
  Enterキーを押す.

~~~
    set PATH=C:\MinGW\bin;%PATH%
~~~

  これで上で挙げた gcc.exe などがフルパスを指定することなく単にgcc.exe(gccでもよい)と
  入力するだけで実行可能となる. 次に実際にgccと入力してEnterキーを押してみよう.
  「gcc: fatal error: no input files」と出たら成功である.
  一方、「gccは内部コマンドまたは外部コマンド～なんたらかんたら～として認識されていません」
  と出たら失敗である. 上記のPATHの設定において何かを間違えている(例えば打ち間違えなど).

  <a href="#user-content-index">目次へ戻る</a>


## <a name="use_vc_on_windows"></a>WindowsでVCを使う場合
-----------------------------------
  これを使われているような方は既にある程度わかっておられる方だとは思うが...

  srcフォルダにある compile_by_vc.batをクリックして実行しよう.
  VCが適切にインストールされているならば、すべてのコンパイルが自動で行われ、
  out_dir内に目的のexeファイルやdllが生成されるはずである.


  **【参考】compile_by_vc.bat が内部で行っていること**

  ここではcompile_by_vc.bat を使わずに設定する方法を述べる.
  換言すればcompile_by_vc.bat が内部で行っていることの解説でもある.

  基本的にはコマンドプロンプトを開き、環境変数を整えた上で、以下を実行すると
  ビルド可能である.

~~~
    nmake -f Makefile_vc.mak
~~~

  整えなければならない環境変数は、PATH, INCLUDE, LIBである.  
  これらに関する詳細はググッた方が早いかもしれないが、一応説明する.  

  環境変数PATHに関しては、以下のコマンドが存在するフォルダのパスを ; 区切りで追加する.
~~~
    cl.exe  link.exe  lib.exe  nmake.exe
~~~
  
  しかしこれだけでは実は不十分でさらにこれらのコマンドが依存するdllが含まれるフォルダも
  同様に追加しなければならない. 厄介なことにそのフォルダはこれらのコマンドの置いて
  あるフォルダとは別(大抵はCommon7配下のどこか)であったりする.

  例えば 上記のコマンドが C:\Program Files\Microsoft Visual Studio 8\VC\bin 内にあるならば
  コマンドプロンプト上で以下のように入力する(行の最後毎にEnterキーを押すこと).

~~~
    set PATH=C:\Program Files\Microsoft Visual Studio 8\VC\bin;%PATH%
    set PATH=C:\Program Files\Microsoft Visual Studio 8\Common7\IDE;%PATH%
    set PATH=C:\Program Files\Microsoft Visual Studio 8\Common7\Tools%PATH%
    set PATH=C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\Bin;%PATH%
~~~

  これで上で挙げた cl.exe などがフルパスを指定することなく単にcl.exe(clでもよい)と
  入力するだけで実行可能となる. 実際にclと入力してEnterキーを押してみよう.
  「Microsoft.. Optimize Compiler...うんたらかんたら」と出たら成功である.

  しかし、上記はかなり長い…  
  こんなもの毎回入力してはいられないので、ここで一つテクニックを紹介しよう.
  テキストエディタ(メモ帳など)を開き、上記のコマンドをコピペして、例えばmy_set_path.bat
  という名前でこれを保存する.
  あとはコマンドプロンプト上から以下を入力すれば、上記のコマンド群を一括して実行できる.
~~~
    call my_set_path.bat
~~~

  環境変数INCLUDEに関しては、ヘッダの存在するフォルダのパスを環境変数PATHと同様の形式で設定する.
  さしあたって必要なのは標準Cライブラリのヘッダ(stdio.hなど)の存在するフォルダと
  WindowsAPI(これは一般にPlatformSDKと呼ばれる)のヘッダ(windows.hなど)の存在するフォルダ
  の２つである. 例えば以下のようになる.

~~~
    set INCLUDE=C:\Program Files\Microsoft Visual Studio 8\VC\include;%INCLUDE%
    set INCLUDE=C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\include;%INCLUDE%
~~~

  環境変数LIBに関しては、ライブラリ(拡張子lib)の存在するフォルダのパスを環境変数PATHと同様の形式で設定する.
  さしあたって必要なのは標準Cライブラリ(libcmt.libなど)の存在するフォルダと
  WindowsAPI(これは一般にPlatformSDKと呼ばれる)のライブラリ(Kernel32.libなど)の存在するフォルダ
  の２つである. 例えば以下のようになる.

~~~
    set LIB=C:\Program Files\Microsoft Visual Studio 8\VC\lib;%LIB%
    set LIB=C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\lib;%LIB%
~~~

  ところで通常はこれらを我々が手動で全部設定する必要はなく、VCのインストールディレクトリ配下に
  これらを自動設定するためのbatファイルが存在するのでそれを使う. 
  これはVCのバージョンによって中身が異なるがVC6.0を除き、vsvars32.bat という名前になっている.
  尚、VC7.0以降、VCがインストールされた場合は VS??COMNTOOLS (??にはバージョン番号が入る)という
  形式の環境変数が設定される. これを利用してvsvars32.batの場所を特定することができる.
  同梱してあるスクリプト setenv_vc.batはこれを実現するものである.
  そしてcompile_by_vc.batは内部でこれを呼び出す.

  <a href="#user-content-index">目次へ戻る</a>


## <a name="compile_on_linux"></a>Linuxの場合:
-----------------------------------
  これを使われているような方は既にある程度わかっておられる方だとは思うが...

  gcc, g++, make, ld, ar, ranlib コマンドが実行できるように必要に応じてこれらをインストールする.
  これらはどれも C/C++での開発において最低限必要となる基本コマンドばかりである.
  個別にインストールしてもよいが、Development系のカテゴリにある基本パッケージをインストールすれば
  まずすべて自動でインストールされると思う（例えばArch Linuxなら pacman で base-devel をインストールする). 
  具体的なインストール方法はそれぞれのLinuxディストリビューションによって千差万別なのでここでは記載しない. 
  ディストリビューションによっては最初からインストールされている可能性も高い.

  その上でターミナル(xterm, urxvtなどなんでもよいが)を開き、以下のシェルスクリプトを実行する.

    ./compile_on_linux.sh

  このシェルスクリプトにより、Makefile_linux.mak の存在する全ディレクトリへ自動的に移動しつつ
  make -f Makefile_linux.mak が実行される. この時点でコンパイル後のバイナリは、各ディレクトリ内の
  out_dirに格納されている.
  (Permission deniedのようなエラーメッセージが表示されてこのシェルスクリプトが実行できない場合は、
  まず chmod 755 compile_on_linux.sh で「実行権限」を与え、再度試してみよう！)

  次に、上でコンパイルした全バイナリや設定ファイルをインストールするには、以下を実行すればよい.

	./install_bin_for_linux.sh

  これにより、このディレクトリの一つ上にある ../bin_for_linux へ必要なすべてのファイルがインストールされる.
  (Permission deniedのようなエラーメッセージが表示されてこのシェルスクリプトが実行できない場合は、
  上と同様に chmod 755 install_bin_for_linux.sh で「実行権限」を与え、再度試してみよう！)

  以上でコンパイルおよびインストールは完了である.

  ../bin_for_linuxへ移動し、moaiを起動しよう.
  以下のようなメッセージが表示されればmoaiは無事起動している.

~~~
  Moai : config load OK.
  Moai : target load OK.
  Moai : analysis load OK.
  Moai : Filter Loading [filters/….myf]
  Moai : Plugin Loading [plugins/….so]
  …
  Moai : acceptable_host=[LOOPBACK]
  Moai : blocking_mode=[0]
  Moai : AutoGet PrivateIP=[192.168.…].
  Moai : Listen port 8124...

  Moai : ObserveR : 1-th sock events by select.
~~~

  この状態でブラウザにおけるプロキシ設定で接続先を127.0.0.1:8124に設定すれば、
  moaiをローカルプロキシとして機能させることができるはずである.

  <a href="#user-content-index">目次へ戻る</a>


## <a name="compile_on_cygwin"></a>Cygwinの場合:
-----------------------------------
  既にCygwinを愛用されており、MinGWなどの競合開発環境をわざわざインストールしたくない方もおられると思う.
  この項目はそのような方向けである.

> Cygwinではややこしいことに、この上で動作するMinGWをインストールすることもでき、その場合gccオプションとして
>  -mno-cygwinを使うことでCygwin版gccではなくMinGW版gccを呼び出すこともできる.
> ここでは設定ファイルなどムダにややこしくしたくないため、そのような使い方については対応しない.
> つまり純粋なWindowsプログラムを作る用途ではなく、UNIXエミュレータとしてプログラムを作るというCygwin本来の
> 趣旨に限定するものとする. MinGWを使いたい場合は、上記での説明のようにコマンドプロンプト上で使用するか、
> あるいはMSYS1.0を使って欲しい. ( どうしてもCygwin上でMinGWを使いたい方はMakefile_cygwin.makを各自修正する必要がある.
> 生成されるdll名の微調整やコマンドラインオプション -mno-cygwin を追加する程度の変更で済むとは思う ).

  gcc-core, gcc-g++, make, binutils(ld, ar, ranlib, asコマンドなど)をインストールする.
  これらはどれも C/C++での開発において最低限必要となる基本パッケージである.
  setup-x86.exe においてこれらを個別にインストールしてもよいが、Develカテゴリにこれらは全て含まれているので
  面倒ならこのカテゴリごと一括でインストールしてもよい.

  その上でCygwinターミナルを開き、以下のシェルスクリプトを実行する.

    ./compile_on_cygwin.sh

  このシェルスクリプトにより、Makefile_cygwin.mak の存在する全ディレクトリへ自動的に移動しつつ
  make -f Makefile_cygwin.mak が実行される. この時点でコンパイル後のバイナリは、各ディレクトリ内の
  out_dirに格納されている.
  尚、Cygwinの場合、出来上がるdllの名前にはcygプリフィックスをつけている.
  これば別に必須というわけではないのだが、Cygwin用のdllはVCやMinGWにより生成する普通のdllとは一部異なるため、
  区別するためにこのようにしてある. 

  次に、上でコンパイルした全バイナリや設定ファイルをインストールするには、以下を実行すればよい.

	./install_bin_for_cygwin.sh

  これにより、このディレクトリの一つ上にある ../bin_for_cygwin へ必要なすべてのファイルがインストールされる.

  以上でコンパイルおよびインストールは完了である.

  ../bin_for_cygwinへ移動し、moaiを起動しよう.
  以降はLinux版と同じである.

  <a href="#user-content-index">目次へ戻る</a>


## <a name="compile_on_msys10"></a>MSYS1.0の場合:
-----------------------------------

  作成中.

  <a href="#user-content-index">目次へ戻る</a>


## <a name="use_others_on_windows"></a>WindowsでBCC5.5(Borland C++ Compiler 5.5)またはDMC(Digital Mars C/C++)を使う場合(おまけ)
-----------------------------------
  あなたがMinGWでもVCでもなくあえてこれらを選ぶというなら、もはや我々は何も言うまい.

  これ用のmakefileは、かさばるので同梱していないが、実のところソースコード自体は
  これらでもビルドできるように移植性に一応配慮してある.
  ヒントを示しておくので興味がある奇特な方は各自makefileを作成して欲しい.

  **BCC5.5**  
  objを作る場合の指定方法:
~~~
    bcc32 -w -RT -c -oYourFile.obj YouFile.c
~~~

  exeを作る場合の指定方法(OBJSは+記号で各objファイルを連結したものを指定するという変態仕様である):
~~~
    ilink32 -w -Gn -C -Tpe -ap c0x32.obj $(OBJS) $(LFLAGS),YourApplication.exe,,import32.lib cw32mt.lib,,YourResource.res
~~~

  静的ライブラリ(lib)を作る場合の指定方法:
~~~
    tlib YourStaticLib.lib /P4096 $(OBJS)
~~~

  dllおよびそれに対応するインポートライブラリを作る場合の指定方法(この方法は__stdcallには対応しない):
~~~
    ilink32 -w -Gn -C -Tpd c0d32x.obj $(OBJS) $(LFLAGS),YourDLL.dll,,import32.lib cw32mt.lib,YourDLL.def
    implib -f -a YourDLL.lib YourDLL.dll
    または
    implib -f -a YourDLL.lib YourDLL.def
~~~

  **DMC(最新バージョンのみ)**  
  objを作る場合の指定方法:
~~~
    dmc -HP99 -Bj -j0 -Ab -Ae -Ar -w6 -c -oYourFile.obj YouFile.c
~~~

  exeを作る場合の指定方法:
~~~
    LINK -EXET:NT $(OBJS),YourApplication.exe,,ws2_32.lib user32.lib kernel32.lib,,YourResource.res
~~~

  静的ライブラリ(lib)を作る場合の指定方法:
~~~
    lib -c -p4096 YourStaticLib.lib $(OBJS)
~~~

  dllおよびそれに対応するインポートライブラリを作る場合の指定方法:
~~~
    LINK -EXET:NT $(OBJS),YourDLL.dll,,ws2_32.lib user32.lib kernel32.lib,YourDLL.def
    implib /system YourDLL.lib YourDLL.dll
~~~

  <a href="#user-content-index">目次へ戻る</a>


## <a name="about_android"></a>Androidについて:
-----------------------------------
  Androidは基本的にLinuxであるが、PC Linuxとは大きく異なる部分もある.
  そして通常、AndroidにおけるアプリはJava VMをベースとして(エントリポイントとして)提供される.
  Java VMベースなアプリでは、グラフィカルなアイコンがホーム画面に表示され、
  それをタップすることで起動する.

  しかしながら、今回ビルドする Moai for Android はJava VMベースではなくAndroidのLinux上で
  直接走るネイティブアプリとしてC言語で開発する. この場合ホーム画面にMoai用の特別なアイコンが表示されることはない.
  Moaiを起動するには、まず端末エミュレータを開き、cdコマンドにより moai実行バイナリが存在するディレクトリ
  (bin_for_android)へと移動してからmoai実行バイナリを直接起動する形になる.
  要するに本来のLinux環境やWindowsのコマンドプロンプトなどからプログラムを起動する要領で行う.

  さて、前置きが長くなったが、ネイティブアプリとしてC言語のソースコードをコンパイルするにはAndroid NDKを使う.
  Android NDKは https://developer.android.com/ndk/downloads/index.html よりダウンロードできる.
  また開発はAndroid上では行えないため、PCが必要である. つまりAndroid NDKをPCへダウンロードし、PC上にて開発を行う.

  しかし Android NDKのファイルサイズはかなり大きいので注意してほしい. ダウンロードするzipファイルは約750MB程度もあり、
  さらにこれを解凍展開すると、なんと3GBほどにもなる！ つまりこれらを単純に合計しただけでも
  ディスクの空きが4GB程度必要となる. さらにWindows上でコンパイルする場合、今回はCygwinを使うことを想定する.
  (Cygwinを使わずにコンパイルすることもおそらく可能だが、これに関してはまだ調査/調整中である)  
  尚、Android Studio および android-sdk は今回は全く使わない. インストールさえも不要である.
  同様にJDK(Java Development Kit)やJREなども必要ない.

  Android NDKをダウンロードしたら、これをディスクへ解凍展開する.
  無事解凍できただろうか？なにせファイルサイズが馬鹿デカイため、解凍ツールでエラーなどが表示されていないことを念のため確認しよう.

  次に android_setting_ndk.sh をテキストエディタで開き、ZNK_NDK_HOMEの値をAndroid NDKが解凍展開されたディレクトリ
  のパスに書きかえる. 例えばWindowsでC:\android-ndk-r12フォルダに解凍展開されているなら、Cygwin上でのこれを示すパスは
  /cygdrive/c/android-ndk-r12となり、以下のように書き換える形となる.
  

~~~
   export ZNK_NDK_HOME=/cygdrive/c/android-ndk-r12
~~~

  後は以下のコマンドを順番に実行していけばよい.
  (Windowsの場合はCygwinを起動してこれを実行しよう.)
~~~
  chmod 755 *.sh
  source ./android_setting_ndk.sh
  ./android_compile.sh
  ./android_install_bin.sh
~~~

  src/libZnk, src/moai などにandroidディレクトリが存在するが、この内部にあるjniディレクトリに
  Android NDKでコンパイルするためのMakefileが格納されてある.
  android_compile.shを実行すると、まずこのandroid ディレクトリへ移動してndk-buildコマンドが呼び出される.
  このとき、同ディレクトリに libs, obj ディレクトリが生成され、ビルドしたバイナリやライブラリなどが
  ここに出力される.
  ./android_install_bin.sh を実行することで、これらのバイナリが ../bin_for_androidへとインストールされる形となる.


  最後に出来上がった bin_for_android を実機に転送した上でそれを実行する.
  これには色々方法があるが、とりあえずはbin_for_androidを一旦zipファイルに固めてWeb上にアップし
  実機からこれをダウンロードするというクソ回りくどい方法が~~ここで説明するには~~一番簡単だ.
  まあどんな方法でもよいがとりあえずなんとかこれを実機へと転送して欲しい.

  実行するにあたっては次の点に注意していただきたい.

  * ホームディレクトリ以外では書き込み権限および実行権限の関係で実行できない.<br>
    よってまず「端末エミュレータ」を開き、cd コマンドよりホームディレクトリへ移動する.
    (単にcdと入力してEnterを押せばよい).<br>
    そしてbin_for_android.zipをダウンロードなどした場合は、Downloadディレクトリからホームディレクトリへ
    これをコピーしなければならない.
    Downloadディレクトリは通常、/storage/sdcard0/Download などのパスに存在し、その配下にダウンロードした
    ファイルも置かれている.
    以下のように実行することでこれをホームディレクトリへコピーしよう.
~~~
      cp /storage/sdcard0/Download/bin_for_android.zip ~/
~~~
  * bin_for_android.zip をホームディレクトリへコピーしたら、unzip bin_for_android.zip でこれを解凍展開しよう.
    次に cd bin_for_android/armeabi で実行バイナリのあるディレクトリ内へと移動し、
    moai および http_decorator に実行権限を与える. 
    以下を実行しておこう.
~~~
      chmod 755 moai http_decorator
~~~
  * moaiは起動時にlibZnk.soを動的ロードする.
    これを成功させるためにLD_LIBRARY_PATHを適切に指定しておかなければならない.
    以下を実行しておこう.
~~~
      export LD_LIBRARY_PATH=. 
~~~

  これでようやく準備が整った.
  ./moai と入力し、上記「Linuxの場合」で説明したのと同様のメッセージが表示されれば成功である.


  <a href="#user-content-index">目次へ戻る</a>


## <a name="about_macos"></a>MacOSについて:
-----------------------------------
  現状では未サポートである.
  仮にコンパイルを試みるなら(手元にないので未確認であるが)Darwinを使うことになると思う.
  これにgcc, makeコマンドなどLinuxと同様のツールが含まれており、多分Makefile_linux.makを
  少し手直しすれば同様の方法でビルドできるはずである.
  (ダイナミックリンクライブラリの拡張子はsoではなくdylibやtbdといったものになるようである.
  その他リンカldのオプションもおそらくMacOS独自のものを指定しなければならないだろう)

  <a href="#user-content-index">目次へ戻る</a>


