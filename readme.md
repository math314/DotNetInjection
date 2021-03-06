
目的
=====

箱庭XSSをバイナリとして攻略．


手段
=====
箱庭XSSの使っている正規表現による置換を抑制する．
これを実現するために，.Netの関数を別の関数に置き換える必要が出てきた．

* ネイティブなx86,x64アプリケーションならば，dll injectionを行う事で関数の置き換えが出来る．
* ネイティブでのdll injectionはサンプルが豊富
* .Netでは同様の方法が通用しない
* 中間言語をJIT compileして実行しているため．


制約
----

* ターゲットとなるアプリケーションを書き換えてから実行しない(箱庭XSSの不正検知が働く)
    * 実行中に書き換えるのはOK
    * hoge.exeにパッチを当てて実行，をしないという意味
* IsDebuggerPresentがfalseを返すような方法を取る

[5],[6]は有用な手法と感じたが，ターゲットとなるアプリケーションを書き換える必要があるため不可
[1-4]のProfilingAPIを使い，JITCompileのタイミングを受け取ってメソッドを置き換える手法を取ることに．


はまりどころ
==========

* .net 4.0 以下を対象としているので，4.5とかだと動かない，はず
    * 試してない

* 対象アプリケーションのプラットフォームターゲットがx86じゃないと動かない
    * 何故ならHakoniwaProfiler.dllがx86なので
    * x64にも対応したいなら，それ用のdllを作る必要がある
    * デフォルトでは"AnyCPU" だが，これでは64bitのOSで32bitアプリケーションとして実行は"出来ない"
          * 余談だが，.net4.5以降では "AnyCPU + 32bit優先" というオプションを付けることで，32bitでも動くようになる
    * CorFlags.exe というコマンドで，"AnyCPU"を"x86"に変更できる(exeを直接書き換える)．
    * アウトプロセスサーバでプロファイラを作成すれば苦労が少ない？
          * 実はアウトプロセスサーバ+.netの場合，プロファイラのプラットフォームターゲットをAnyCPUにしてはいけない模様
          * ハングアップするらしい
    * 参考[7]

* mCorProfilerInfo2->SetEventMask で COR_PRF_USE_PROFILE_IMAGES を指定する必要がある
    * System.*** は NGENでJITコンパイル済みなので，普通はJITCompilationStartedが呼び出される事はない
    そこでJITコンパイルを無効化したprofile用のdllを読み込む様にEventMaskで設定する必要がある
    * 参考[4]

* System.MethodAccessExceptionが発生する
    * 今回発生した理由は2つ
    * publicでない関数をcallしようとしたことにより発生した例外
          * あるクラスから，別クラスのprivateな関数の呼び出しをするように書き換えていた
          * publicな関数を呼び出すコードに書き変える事で対処
    * .net4.0から導入されたセキュリティモデルにより発生する例外
          * mscorlibのassemblyにはSystem.Security.SecurityTransparent属性が宣言されている
          * これが宣言されたアセンブリからは，SecurityTransparent，SecuritySafeCriticalな関数しか呼べない
          * デフォルトではSystem.Security.AllowPartiallyTrustedCallersが暗に宣言されているが，これではダメらしい
    * 参考[8]

* System.Security.SecurityTransparentを指定したアセンブリから，System.Security.AllowPartiallyTrustedCallersのアセンブリを呼び出せない
    * AllowPartiallyTrustedCallersのアセンブリのメソッドを置き換えるにはAllowPartiallyTrustedCallers，
    SecurityTransparentなアセンブリのメソッドを置き換えるにはSecurityTransparentのアセンブリを使う必要がある？
    * メソッドにSecuritySafeCritical属性をつけて試してみたが，セキュリティの例外が発生してしまった．
    * 置き換え先のアセンブリを2つ用意すれば解決するが，本質的ではないため，ConsoleAppTestはSecurityTransparent属性をつけている．

* System.BadImageFormatException: バイナリ シグネチャが不適切です。(HRESULT = 0x80131192)
    * metaDataEmit->DefineMemberRef で設定したsignatureBlobにバグがあった

* 署名をしていないアセンブリを，署名済みアセンブリに読み込ませることが出来なかった
    * 置き換え先のメソッドが定義されているアセンブリに署名をした
    * これにより，HakoniwaProfiler.MethodHook から， ConsoleAppTest を参照できなくなった．
    * 解決方法は，ConsoleAppTestに署名をするか，ConsoleAppTestで定義されている型を一切参照しないか，のどちらかである
    * 今回は簡単のため，前者の署名をした．

実行方法
=================

    ...\DotNetInjection>REM "普通に実行する"
    ...\DotNetInjection> cd Release
    ...\DotNetInjection\Release>ConsoleAppTest.exe

    ...\DotNetInjection>REM "関数を置き換えて実行する"
    ...\DotNetInjection> cd Release
    ...\DotNetInjection\Release>Injector.exe ConsoleAppTest.exe


実行例
=============

普通に実行した場合
-------------------------

    ...\DotNetInjection\Release>ConsoleAppTest.exe
    2015/03/11 22:55:31
    poyohugapoyoxxxx
    poyohugapoyoxxxx
    ConsoleAppTest.Program.getStr1
    getStr1
    aa + bb
    3
    a + b + c + 2 + 3 + 4
    TestClass.test2 : 1 , aaa
    aaa
    TestClass.test2 : aaa
    aaa

関数を置き換えて実行した場合
-----------------------------------

    ...\DotNetInjection\Release>Injector.exe ConsoleAppTest.exe
    ConsoleAppTest started.
    [!] HakoniwaProfiler.MethodHook.MethodHook.get_Now
    2000/01/01 0:00:00
    [!] HakoniwaProfiler.MethodHook.MethodHook.Replace
    ------------------------------
    input = poyohugapoyopiyo
    pattern = piyo,pattern_length = 4
    ------------------------------
    poyohugapoyopiyo
    [!] HakoniwaProfiler.MethodHook.MethodHook.Replace
    ------------------------------
    input = poyohugapoyopiyo
    pattern = piyo,pattern_length = 4
    ------------------------------
    poyohugapoyopiyo
    [!] HakoniwaProfiler.MethodHook.MethodHook.getStr1
    HHHH
    [!] HakoniwaProfiler.MethodHook.MethodHook.haveArguments
    aa + bb
    3
    [!] HakoniwaProfiler.MethodHook.MethodHook.haveManyArguments
    a + b + c + 2 + 3 + 4
    [!] HakoniwaProfiler.MethodHook.MethodHook.test1

    [!] HakoniwaProfiler.MethodHook.MethodHook.test2

参考にした記事達
==================

* http://www.codeproject.com/Articles/17275/Really-Easy-Logging-using-IL-Rewriting-and-the-NET [1]
    * idlファイルがおかしい，サンプルがコンパイル出来ない
    * 別の記事を探すことに

* http://www.codeproject.com/Articles/453065/ILRewriting-for-beginners [2]
    * 上記の[1]を読みやすくしたもの，作者は別

* https://msdn.microsoft.com/en-us/magazine/cc188743.aspx [3]
    * ．Net Internalsの記事
    * サンプルは一部書き換えるとコンパイルは通るが，[2]で間に合っているため使用せず
    * [1]にある説明をより詳細にしたもの

* http://blogs.msdn.com/b/davbr/archive/2007/03/06/creating-an-il-rewriting-profiler.aspx [4]
    * NGENで既にコンパイルされているILもflagを設定することで再度JITコンパイルされるようになる，という内容が書かれている．

* http://www.codeproject.com/Articles/463508/NET-CLR-Injection-Modify-IL-Code-during-Run-time [5]
    * 書き換え対象のアプリケーションが自身を書き換える場合どうすればよいか，という記事

* http://www.codeproject.com/Articles/37549/CLR-Injection-Runtime-Method-Replacer[6]
    * [5]と同様

* http://qiita.com/mima_ita/items/57d7c1101543e214b1d6 [7]
    * .netアプリケーションと，COMのインプロセスサーバとアウトプロセスサーバの関係

* http://www.atmarkit.co.jp/fdotnet/special/dotnet4security_01/dotnet4security_01_01.html [8]
    * .NET 4のセキュリティ透過性モデル（レベル2）について