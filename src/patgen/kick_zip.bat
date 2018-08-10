@echo off

set old_parent_ver=2.1
set old_ver=2.1
set new_parent_ver=2.1
set new_ver=2.1.1
if exist set_ver.bat call set_ver.bat

call _zip_one.bat moai-v%old_parent_ver%-patch-v%new_ver%-windows moai-v%old_parent_ver%-patch-v%new_ver%-windows
call _zip_one.bat moai-v%old_parent_ver%-patch-v%new_ver%-cygwin  moai-v%old_parent_ver%-patch-v%new_ver%-cygwin
call _zip_one.bat moai-v%old_parent_ver%-patch-v%new_ver%-android moai-v%old_parent_ver%-patch-v%new_ver%-android
