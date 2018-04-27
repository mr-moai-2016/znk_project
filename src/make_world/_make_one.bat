@echo off
if not exist ..\%3 goto Error
setlocal
echo ======
echo === Entering [%3] ===
cd ..\%3
%1 -f Makefile_%2.mak %4
echo === Leaving [%3] ===
echo ======
echo:
endlocal
REM endlocal‚É‚æ‚èdirectory‚àŒ³‚É–ß‚é.
goto End
:Error
echo Error : directory [%3] does not exist.
pause
:End
