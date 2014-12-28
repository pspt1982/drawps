DrawPS
======
DrawPSはC言語用のライブラリです。

プログラムを実行しながら簡単に2D/3Dのアニメーションをリアルタイムに表示することができます。従来の数値計算ではすべての計算を終えてから、その結果をgnuplotやExcelなどを使ってグラフ化することで物理現象を考察していました。DrawPSでアニメーション化をすることで運動の時間発展などを直感的に理解しやすくなります。

## デモ
![](/demo.gif)

## 必要
- [Visual Studio](http://www.visualstudio.com/downloads/download-visual-studio-vs)
- [GLUT for Win32](http://user.xmission.com/~nate/glut.html)

## 使い方
- [マニュアル](http://www23.atwiki.jp/pspt/pages/34.html)
- [リファレンス](http://www23.atwiki.jp/pspt/pages/35.html)
- [サンプル解説](http://www23.atwiki.jp/pspt/pages/36.html)

## インストール
1. __Visual Studio__を導入する. (→[Visual Studio のダウンロード](http://www.visualstudio.com/downloads/download-visual-studio-vs))
2. __GLUT for Win32__を導入する. 
  1. [Nate Robins - OpenGL GLUT for Win32](http://user.xmission.com/~nate/glut.html)から__glut-3.7.6-bin.zip__をダウンロードし, 解凍する.
  2. __glut-3.7.6-bin.zip__内の3つのファイルをそれぞれのコピー先にコピーする.

    |ファイル名|コピー先|
    |---|---|---|
    |glut32.dll|C:\Windows\SysWOW64|
    |glut.h|C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\gl|
    |glut32.lib|C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib|

3. [pspt1982/drawps](https://github.com/pspt1982/drawps)の__Download ZIP__のボタンをクリックし, __drawps-master.zip__をダウンロード, そして解凍する.
4. __drawps.sln__をダブルクリックして__Visual Studio__を起動する.
5. __F5__キーを押して実行する.

## ライセンス
This software is released under the MIT License, see [LICENSE](/LICENSE).

## 作成
日本大学理工学部 サークル「[P.S.P.T. 計算物理研究会](http://www23.atwiki.jp/pspt/)」
