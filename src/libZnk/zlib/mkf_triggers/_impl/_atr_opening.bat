@echo off
echo:
set DEBUG_MODE=optimize
if exist mkf_triggers/__DEBUG__ set DEBUG_MODE=debug
if exist mkf_triggers/__BOTH__  set DEBUG_MODE=both
if exist mkf_triggers/__MKF_ID_mingw__   set MKF_ID=mingw
if exist mkf_triggers/__MKF_ID_vc__      set MKF_ID=vc
if exist mkf_triggers/__MKF_ID_android__ set MKF_ID=android
if exist mkf_triggers/__MACHINE_x86__         set MACHINE=x86
if exist mkf_triggers/__MACHINE_x64__         set MACHINE=x64
if exist mkf_triggers/__MACHINE_armeabi__     set MACHINE=armeabi
if exist mkf_triggers/__MACHINE_armeabi-v7a__ set MACHINE=armeabi-v7a
if not exist %MKFSYS_DIR% goto Error_InvalidMkfSysDir
echo === Welcome to auto_trigger[%~n1] console menu ===
echo === You can move the cursor by pressing arrow or HJKL key, 
echo === select item by number key, and exit by Q key!
echo === DEBUG_MODE=[%DEBUG_MODE%] MKF_ID=[%MKF_ID%] MACHINE=[%MACHINE%]          
