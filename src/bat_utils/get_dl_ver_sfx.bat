@echo off

REM 詳細:
REM   やや技巧的な処理を行っている.
REM   template_bat__prefix に '%_BB_PREFIX%' という文字列が
REM   一番最後に改行なしで(ここが重要!)記述されている.
REM   (echoコマンドでは最後に自動的に改行が挿入されてしまうため、
REM   このテンプレートファイルを自動生成することはできない)
REM   これを type コマンドで一時ファイルへ出力した後、
REM   type で追加出力し、コマンドを動的に生成させる.
REM   あとは、その一時ファイルを bat ファイルとして callするだけである.

set _BB_PREFIX=set
set _BB_BAT_UTILS_DIR=%1
set _BB_VAL_FILE=%2
set _BB_SUFFIX=

echo _BB_VAL_FILE=[%_BB_VAL_FILE%]
if not exist %_BB_VAL_FILE% goto EndOfGen
	type %_BB_BAT_UTILS_DIR%\template_bat__prefix>  __temp_bb_cmd.bat
	type %_BB_BAT_UTILS_DIR%\template_bat__space>>  __temp_bb_cmd.bat
	type %_BB_VAL_FILE%>>                           __temp_bb_cmd.bat
	if not "%_BB_SUFFIX%" == "" echo %_BB_SUFFIX%>> __temp_bb_cmd.bat
	call __temp_bb_cmd.bat
	del  __temp_bb_cmd.bat
:EndOfGen

set DL_VER_SFX=
if not "%DL_VER%" == "" set DL_VER_SFX=-%DL_VER%
