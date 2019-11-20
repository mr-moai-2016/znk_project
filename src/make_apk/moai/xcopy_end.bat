@echo off

set CP=xcopy /H /C /Y
if not exist %DST%\ mkdir %DST%
for %%a in ( %LIST% ) do %CP% %SRC%\%%a %DST%\
for %%a in ( %WILD% ) do %CP% %%a       %DST%\
