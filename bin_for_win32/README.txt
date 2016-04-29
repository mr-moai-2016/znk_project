＠Virtual USERSとは？(超簡易版)
-----------------------------------

  Virtual User-agent Screen-size and Everything Randomize Systemの略.


  まず初めてVirtual USERSを使う場合は、以下の手順でサイトへアクセスできるかを確認しよう.

  1. virtual_users(Windowsならvirtual_users.exe)を起動する.

  2. ブラウザのプロキシ設定ダイアログでプロキシサーバとして localhost:8124 を指定する.
    (これでうまくいかない場合は 127.0.0.1:8124 で試してみよう!) 
    これは一般にループバック接続と呼ばれ、自分自身への接続を意味する.

  3. ブラウザから目的のサイトへアクセス！

  これで目的のサイトが無事表示されれば成功である.


  さて、Virtual USERSを介して別人に生まれ変わるには次のようにする.

  1. IPを変える(たとえばルーカチする).  

  2. virtual_users(Windowsならvirtual_users.exe)を起動する.  
     既に起動している場合は一旦終了し、もう一度起動する.  

  以上である.  


  **【おまけ】**  
  Virtual USERSは簡易Webサーバとしても機能する.  
  ブラウザのURL指定欄に http://localhost:8124 または http://127.0.0.1:8124 を指定してアクセスしてみよう.  


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
