＠MoaiのVirtual USERS機能とは？(超簡易版)
-----------------------------------

  Virtual USERSとはVirtual User-agent Screen-size and Everything Randomize Systemの略である.
  Virtual USERS機能はIPを除くすべてのあなたのマシンの情報をランダムに仮想化する.
  これに踏まえ、ルーカチ(ルータリセット)などによりIPを変えることで、掲示板などにおいて
  完全に別のユーザに生まれ変わることができるというものである.

  具体的な手順は以下である.

  1. IPを変えるためルーカチする.
     (あるいは同目的で外部プロキシを変えるなどしてもよい)

  2. moai(Windowsならmoai.exe)を起動し、
     ブラウザからhttp://127.0.0.1:8124へアクセスする.

  3. Moai Web Configuration画面を開き、「Virtual USERS Initiation」ボタンを押す.
    「Initiate futaba : Success : Virtual USERS done. Your futaba_send.myf is randomized successfully.」
     と表示されれば成功である. 

  以上である.  


＠続きはWebで！
-----------------------------------

  …というか以下が正式なドキュメントになります.
  ブラウザよりアクセスしてご覧ください

  https://github.com/mr-moai-2016/znk_project/blob/master/src/virtual_users/README.md

  テキストエディタで読む方が好みな方は、src/virtual_users/README.mdを開いてご覧ください.
  md形式ですが、テキスト文書として読む分には差し支えないと思われます.


＠免責事項
-----------------------------------
  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising
  from the use of this software.
