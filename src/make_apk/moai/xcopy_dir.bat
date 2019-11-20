@echo off

REM
REM xcopy /E を「安全に」使う方法.
REM
REM   必ずコピー元がディレクトリであるかどうかをまず判定する.
REM   (コピー元としてファイルを指定してしまった場合は xcopyコマンドの実行を確実に防ぐ).
REM   万一コピー元としてファイルを指定してしまった場合、xcopyコマンドは邪悪な挙動をするので注意する.
REM
REM ディレクトリ/ファイル判定法
REM
REM   まず最後尾に \ を付けて exist判定する.
REM   これでディレクトリの場合を抽出.
REM   次に \ を付けて exist判定する.
REM   これで残ったファイルの場合を抽出.
REM   最後にすべてに合致しない場合は指定したパスは存在しないと判断すればよい.
REM

if exist %1\ goto SrcIsDirectory
if exist %1  goto SrcIsFile
goto SrcNotFound

:SrcIsDirectory
if exist %2\ goto DstIsDirectory
if exist %2  goto DstIsFile
if not exist %2\ mkdir %2

:DstIsDirectory
xcopy /H /C /Y /E %1 %2
goto End

:SrcNotFound
echo xcopy_dir Error : Src %1 is not found.
goto End

:SrcIsFile
echo xcopy_dir Error : Src %1 is not directory.
goto End

:DstIsFile
echo xcopy_dir Error : Dst %2 is not directory.
goto End

:End
