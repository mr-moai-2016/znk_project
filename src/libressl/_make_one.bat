@echo off
if not exist %3 goto Error

setlocal
echo ======
echo === Entering [%3] ===
cd %3
%1 -f Makefile_%2.mak %4
if errorlevel 1 goto Error_Make
echo === Leaving [%3] ===
echo ======
echo:
endlocal
REM endlocalによりdirectoryも元に戻る.
goto End

:Skip
echo Skip : directory [%3] is skipped.
pause
goto End

:Error
echo Error : directory [%3] does not exist.
pause
goto End

:Error_Make
echo Error : %1 -f Makefile_%2.mak %4 abort.
pause
:End
