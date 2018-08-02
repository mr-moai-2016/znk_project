@echo off

set CP=xcopy /H /C /Y
%CP% moai-win32\birdman\birdman.exe .\
%CP% moai-common\birdman\birdman.myf .\

birdman.exe install win32 2.0
birdman.exe install win64 2.0

pause
