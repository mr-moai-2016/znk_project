@echo off

call bat_utils\setenv_mingw.bat

echo ====
mingw32-make -f Makefile_mingw.mak -C libZnk
if errorlevel 1 goto Error
echo ====
echo;

echo ====
mingw32-make -f Makefile_mingw.mak -C moai
if errorlevel 1 goto Error
echo ====
echo;

echo ====
mingw32-make -f Makefile_mingw.mak -C http_decorator
if errorlevel 1 goto Error
echo ====
echo;

echo ====
mingw32-make -f Makefile_mingw.mak -C plugin_futaba
if errorlevel 1 goto Error
echo ====
echo;

echo ====
mingw32-make -f Makefile_mingw.mak -C plugin_2ch
if errorlevel 1 goto Error
echo ====
echo;

echo All compilings are done successfully.
:Error

pause
