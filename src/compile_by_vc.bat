@echo off

call bat_utils\setenv_vc.bat

echo ====
cd libZnk
nmake -f Makefile_vc.mak
if errorlevel 1 goto Error
cd ..
echo ====
echo;

echo ====
cd moai
nmake -f Makefile_vc.mak
if errorlevel 1 goto Error
cd ..
echo ====
echo;

echo ====
cd http_decorator
nmake -f Makefile_vc.mak
if errorlevel 1 goto Error
cd ..
echo ====
echo;

echo ====
cd plugin_futaba
nmake -f Makefile_vc.mak
if errorlevel 1 goto Error
cd ..
echo ====
echo;

echo ====
cd plugin_2ch
nmake -f Makefile_vc.mak
if errorlevel 1 goto Error
cd ..
echo ====
echo;

echo All compilings are done successfully.
goto End
:Error
cd ..

:End

pause
