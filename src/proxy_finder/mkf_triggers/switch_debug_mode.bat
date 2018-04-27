@echo off

if exist ..\mkf_triggers cd ..
if not exist mkf_triggers goto Error_InvalidTrigger

set DEBUG_MODE=o
if exist mkf_triggers\__DEBUG__ set DEBUG_MODE=d
if exist mkf_triggers\__BOTH__  set DEBUG_MODE=b

if not "%DEBUG_MODE%" == "o" goto EndOf_o
	set DEBUG_MODE=b
	echo d>mkf_triggers\__BOTH__
	if exist mkf_triggers\__DEBUG__ del mkf_triggers\__DEBUG__
goto End
:EndOf_o

if not "%DEBUG_MODE%" == "b" goto EndOf_b
	set DEBUG_MODE=d
	echo d>mkf_triggers\__DEBUG__
	if exist mkf_triggers\__BOTH__ del mkf_triggers\__BOTH__
goto End
:EndOf_b

if not "%DEBUG_MODE%" == "d" goto EndOf_d
	set DEBUG_MODE=o
	if exist mkf_triggers\__DEBUG__ del mkf_triggers\__DEBUG__
	if exist mkf_triggers\__BOTH__  del mkf_triggers\__BOTH__
goto End
:EndOf_d

:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.
pause

:End
