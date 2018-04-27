@echo off 

if "x%1" == "x" goto Error

set LIST=
if exist ..\proj_list.bat call ..\proj_list.bat

for %%a in ( %LIST% ) do call _make_one.bat %1 %2 %%a %3

goto End
:Error
echo Usage : _make_kick.bat MAKE_CMD MKID subdir MAKE_ARGS
pause
:End

