# znk_project
===================================

This is Japanese overview. English version is [here][7].

## znk_projectとは?
-----------------------------------

"znk_project" は我々**日本HTTP研究所(通称NHK)**によるフリーかつオープンソースなアプリケーション
およびライブラリを開発/提供するリポジトリです.
これらはすべてlibZnkという基本ライブラリによってWindows またはLinux向けに開発されたものであります.

このページの右上にあります「Clone or download」というボタンをクリックすると
さらに「Download ZIP」と書かれた部分が現れるので、これをクリックすれば
プロジェクト全体をアーカイブした最新のzipファイルがダウンロードできます.

あるいは、以下のリンクからもこれを直接ダウンロード可能です.

  https://github.com/mr-moai-2016/znk_project/archive/master.zip


現在このリポジトリで提供しておりますものは以下の通り:

* libZnk : 基本的なC/C++用ライブラリ  
* Moai: また一つ生まれた(ばかりの)ローカルプロキシエンジン  
* Moai plugins: 掲示板用のMoaiプラグイン.
* http_decorator : ~~マスコット~~シンプルなHTTPクライアント.  

以下はこれらに関するドキュメントへのリンク一覧となります: 

* [Moaiについて: 基本事項][2]
* [Moai/Virtual USERS機能: Moaiを使って掲示板のユーザ情報を仮想化する方法について][4]  
* [Moai Reference Manual: 全機能の解説][3]
* [Moai FAQ][8]
* [ソースコードからのコンパイル方法][6]
* [ソース改造および機能拡張のためのガイドライン: プログラマ向け][9]
* [libZnkについて][1]
* [リリースノート][5]


## What's new?
-----------------------------------

* 2016/10/07 Version 1.1.8 リリース.  
  ふたばのスレの下部にある[リロード]を押すと、正常に応答しなくなるバグを修正.
  またFirefox+赤福においてふたばに画像レス投稿などを行うと同様に応答しなくなるか
  あるいはその応答が著しく遅くなるバグを修正.

  リリースノートは[こちら][5]です.

* 2016/10/07 Version 1.1.7 リリース.  
  ふたばの新しい仕様に対応.
  ふたばプラグインを修正.  

* 2016/07/13 Version 1.1.6 リリース.  
  recv_filterにおいてcss_addtional というセクションを新たに導入.
  この部分にユーザ独自のcssを追加で記述できるようになりました.
  これにより閲覧しているページのスタイルを比較的容易にカスタマイズできます.

  Virtual USERS Initiate機能におけるuser_agentのデフォルト生成値を若干改良.   
  ふたばの新しいサーバ test.2chan.net への試験的対応.

* 2016/07/07 Version 1.1.5 リリース.  
  ふたばの新しい仕様に対応.
  ふたばプラグインを修正.  

  **現在、ふたばの仕様が頻繁かつ早いベースで変更されています.**  
  **これより古いバージョンでは正常に表示されなかったりレス投稿などができない可能性が考えられます.**    
  **その場合は最新版をお使いください.**  

  **【参考】**  
  ふたばの仕様変更のペースは早いときには数時間おきという単位で行われるケースも確認されています.  
  我々Moai開発チームも、とりあえず「特別措置」として可能な場合にはこの変更にこのペースで追従します.   
  しかし、数時間おきに新バージョンリリース扱いなどにするとキリがありませんので  
  このWhat's new やリリースノートでわざわざそのような修正を告知しない場合があります.  
  ここでの告知はよほど緊急のものを除いては最短でも１日置きとします.  
  つまり同じ最新バージョンでも、例えば数時間後にダウンロードしたzipでは問題の修正が行われている
  といったケースもあり得ますので参考にしてください. 

* 2016/07/06 Version 1.1.4 リリース.  
  ふたばの新しい仕様に対応.
  ふたばプラグインおよびfilterファイルを修正.  

* 2016/07/04 Version 1.1.3 リリース.  
  ふたばプラグインを修正.
  現状、ふたばにはscriptタグ内におけるコメントアウト内に、ある条件下で正常な表示を阻害するパターンが含まれています.
  これを除去するサニタイズ機構を導入しました.

* 2016/07/03 Version 1.1.2 リリース.  
  ふたばプラグインを修正.
  現状、ふたばにはHTMLコメントアウト内に、ある条件下で正常な表示を阻害するパターンが含まれています.
  これを除去するサニタイズ機構を導入しました.

* 2016/06/30 Version 1.1 リリース.  
  久しぶりのメジャーバージョン番号引き上げリリースとなります.

  プラグインにおけるon_post関数の仕様を変更.
  ふたばの新しい仕様に対応.
  libZnkのバージョンを1.1に変更.

* 2016/06/21 Version 1.0.6 リリース.  
  **Androidユーザの皆様！おまたせしました**(iPhoneな方はサーセンｗ)！
  **ついに Moai for Android (試験的バージョン)の登場です.**
  今回我々開発チームがAndroid向けにコンパイルしたものをbin_for_androidとしてご提供します.
  そしてまた、あなたがAndroid向けにコンパイルするための指針をHowToCompileにて解説してあります.

* 2016/06/13 : Version 1.0.5 リリース.  
  Cygwin向けのMakefileおよびコンパイル/インストールスクリプトなどを追加.
  Cygwin上でMoaiをコンパイルして使いたいがうまくいかなかったという方、
  かなり簡単になったと思いますので、是非ともお試しください.

* 2016/05/21 : 正式バージョン1.0がリリースされました.  

## <a name="license">ライセンス
-----------------------------------

  Copyright (c) Nippon HTTP Kenkyujo(NHK)

  このリポジトリで提供されるツールおよびライブラリは基本的にはすべて
  NYSL(煮るなり焼くなり好きにしろ)というライセンスで提供されます.
  ( NYSLについては http://www.kmonos.net/nysl を参照. )  

  NYSLはパブリックドメインとほぼ同義で、例えば以下のことが自由に行えます.
  またこれらを行うにあたって我々に許可をとる必要は全くないし、我々作者の名前や
  このリポジトリ由来であることを明記する必要もありません.(明記してもよいけれども)
  <ul>
  <li>再配布などが自由に行える.</li>
  <li>商用/非商用に関わらずあなたの開発/使うツールなどで自由に利用できる.
      (勿論、某G●Uライセンスのようなソースコード公開の義務なんてものはない).</li>
  <li>改造などしてあなたの名前で公開できる.
      ほんのちょっとの改造であっても全然構わない.
	  それどころか単に関数のプリフィックスやライブラリ名を変えただけのものであってもOK.
	  むしろ混同を避けるため、そのようにしてくれた方が有難い. </li>

  </ul>

  まあ要するに一言で言えば「**なんでもOKだ!おまえらの好きにしろ**」ということです.

  ただし、libZnkにおいてはzlibというライブラリがそのまま丸ごと含まれており、
  その部分においてのみzlibのライセンス(このライセンスもほとんど制限が全くないようなものですが)
  に従うものとします.


## <a name="as_is">免責事項
-----------------------------------

  本ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、
  何らの保証もなく提供されます. 本ソフトウェアの使用によって生じるいかなる損害についても、
  作者は一切の責任を負わないものとします.

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising
  from the use of this software.



[1]: https://github.com/mr-moai-2016/znk_project/blob/master/src/libZnk/README.md
[2]: https://github.com/mr-moai-2016/znk_project/blob/master/src/moai/README.md
[3]: https://github.com/mr-moai-2016/znk_project/blob/master/src/moai/Reference.md
[4]: https://github.com/mr-moai-2016/znk_project/blob/master/src/virtual_users/VirtualUSERS.md
[5]: https://github.com/mr-moai-2016/znk_project/blob/master/src/ReleaseNote.md
[6]: https://github.com/mr-moai-2016/znk_project/blob/master/src/HowToCompile.md
[7]: https://github.com/mr-moai-2016/znk_project/blob/master/README_en.md
[8]: https://github.com/mr-moai-2016/znk_project/blob/master/src/moai/FAQ.md
[9]: https://github.com/mr-moai-2016/znk_project/blob/master/src/Hacking.md
