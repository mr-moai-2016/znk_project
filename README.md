# znk_project
===================================

This is Japanese overview. English version is [here][7].

## znk_projectとは?
-----------------------------------

"znk_project" は我々**日本HTTP研究所(通称NHK)**によるフリーかつオープンソースなアプリケーション
およびライブラリを開発/提供するリポジトリです.
これらはすべてlibZnkという基本ライブラリによってWindows またはLinux向けに開発されたものであります.

現在このリポジトリで提供しておりますものは以下の通り:

* libZnk : 基本的なC/C++用ライブラリ  
  ドキュメントは[こちら][1]です.
* Moai: また一つ生まれた(ばかりの)ローカルプロキシエンジン  
  ドキュメントは[こちら][2]です.  
  リファレンスマニュアルは[こちら][3]です.  
  FAQは[こちら][8]です.  
* Moai/Virtual USERS機能: Moaiを使って掲示板のユーザ情報を仮想化する方法について  
  ドキュメントは[こちら][4]です.
* http_decorator : ~~マスコット~~シンプルなHTTPクライアント.  
...などなど 

これらのアプリケーションおよびライブラリのソースコードはVC(Ver6.0からVer12.0)、MinGW、(linux上の)gccなど向けに
書かれたものです. コンパイル方法に関しては[こちら][6]をご覧ください.  
また、これらのツールの改造を試みたいといったプログラマな方のために、そのガイドラインを[こちら][9]にまとめてあります.

## What's new?
-----------------------------------

* 2016/05/27 : **Ver1.0.2においてロードが極端に遅くなる不具合がある**ことがわかったため、
  その修正版を急遽Ver1.0.3としてリリース.

  リリースノートは[こちら][5]です.

* 2016/05/27 : Cookieのフィルタ処理が場合によってはうまく機能しないことが有り得る問題を修正.
  ここまでの修正版をVer1.0.2としてリリース.

* 2016/05/27 : libZnkにあった重大なバグを修正.
  **特にVer1.0.1をお使いの方は、このバグによってMoaiがクラッシュすることがあるため、
  新しいZnk-1.0.dllまたはlibZnk.soに置き換えるか、Ver1.0.3を使うようにしてください**.
  このバグはZnk_memrchrの実装ミスとなります.

* 2016/05/25 : アップローダ「斧」においてファイルのダウンロード開始に導く最終ページにおいて
  発生する不具合を修正. ここまでの修正版をVer1.0.1としてリリース.

* 2016/05/23 : IE7以前で画像投稿時、POST部に含まれるfilenameがフルパスのままになっておりイヤーンバカーン&hearts;な不具合を修正.
  (mIpN0ZXK0さん、kQKv21nt0さんご指摘ありがとうございます).

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
      (勿論、某G●Uライセンスのようなソースコード公開の義務なんてものはありません).</li>
  <li>改造などしてあなたの名前で公開できる.
      (ほんのちょっとの改造であっても全然構いません).</li>
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
