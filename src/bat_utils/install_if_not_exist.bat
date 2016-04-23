@echo off
set CP_=xcopy /H /C /Y

set FILE_=%1
set SRC_DIR_=%2
set DST_DIR_=%3
if "%FILE_%"    == "" goto End
if "%SRC_DIR_%" == "" goto End
if "%DST_DIR_%" == "" goto End

if not exist %SRC_DIR_%\%FILE_% goto End
if not exist %DST_DIR_%\%FILE_% %CP_% %SRC_DIR_%\%FILE_% %DST_DIR_%\

:End
