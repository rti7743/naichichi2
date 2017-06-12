フューチャーホームコントローラー(FHC)
=============

音声認識によるホームコントロールを実現するフューチャーホームコントローラーのソースコードです。  
音声アシスタントとか、スマートスピーカーとかです。  
音声で動作するスマートホームを実現できます。  


ソフトウェアは GPL でライセンスで公開します。  
ご自由にお使いください。  
2012年から開発しているので、若干古いものもあります。  
古くても、opensslはバグ修正のパッチをちゃんとあててますのでご安心ください。  


動作環境
=============

windowsと Linux で動作します。(クロスプラットフォームです)  
VS2010以降でコンパイルできます。  
gcc は、ver4系だったら動くと思います。  

VS2013未満は、 std::threadがないので boost::threadで代用します。  
それ以外の処理系では、 std::thread を利用します。  

含まれる内容  
=============
* 音声認識エンジン  
* 合成音声エンジン  
* webサーバと画面  
* 同期エンジン  
* 複数台同期  
* javascript実行処理系  
* Homekitエミュレーション  
* HEMS(EcoNetLite)  
* UPNP(SSDP)  
* sip(諸々の理由により無効にしています)  
* 温度光度センサー(ただしハードがないと動かない)と、グラフ
* 学習リモコンの受信部、送信部(ただしハードがないと動かない)
* その他いろいろ  
* 非力なarmでも動きます。  


使い方  
=============
以下の環境で、コンパイルしてください。  
Windows + VS2010 + boost(VS2010には std::threadがないためboostで代用)  
Windows + VS2013以降  
Linux + gcc ver4  


windowsでは、 naichichi2.sln を開いて F5ビルドしてください。  


linux ubuntuであれば、必要なソフトウェアを入れた後で、  
apt-get install gcc g++  
apt-get install flex uuid uuid-dev libasound2 libasound2-dev mplayer  
  
sh build.sh  

or  

sh build.sh arm  
sh build.sh x86  
sh build.sh ia32  

長い長いコンパイルが終わったら、  

cd naichichi2  
./naichichi2_client  

or  

cd naichichi2  
./naichichi2_client --nostdout  

or  

もし、あなたが、以下に入れているならば、これが利用できます。  
 /home/rti/naichichi2/  
  
cd /home/rti/naichichi2/naichichi2/config/linuxboot  
./naichichi2_zaoriku_client.sh  


コンパイルして生成したプロセス  
naichichi2_client を実行したら、webサーバが起動するので、ブラウザからアクセスしてください。  
http://127.0.0.1/  


web画面にアクセスするには、アカウント登録が必要です。  
https://fhc.rti-giken.jp/welcome/newregist  

アカウントは無料です。  
ただ、そのうち無償版だけは広告でも出すかも・・・  


マニュアル
=============
マニュアルはこちらにあります。  
チュートリアル  
https://rti-giken.jp/fhc/help/howto/  
  
画面リファレンス  
https://rti-giken.jp/fhc/help/ref/  
  
スクリプトリファレンス  
https://rti-giken.jp/fhc/help/ref/  
  
WebAPIリファレンス  
https://rti-giken.jp/fhc/help/ref/setting_webapi.html  


物理的にデバイスがないとできない学習リモコン以外の処理を動かすことができます。  

楽をしたい人は・・・
=============

学習リモコンが搭載されたすぐに動作するハードウェアを、こちらから買えます。  
楽をしたい人は、こちらからどうぞ。 
今ならキャンペーンで 26,800円です。

https://rti-giken.jp/  


作者 :: rti (rti@rti-giken.jp)  
