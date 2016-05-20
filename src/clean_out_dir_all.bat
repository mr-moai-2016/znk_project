@echo off

call bat_utils\clean_out_dir_one.bat libZnk
call bat_utils\clean_out_dir_one.bat libZnk\zlib
call bat_utils\clean_out_dir_one.bat moai
call bat_utils\clean_out_dir_one.bat http_decorator
call bat_utils\clean_out_dir_one.bat plugin_futaba
call bat_utils\clean_out_dir_one.bat plugin_2ch

echo All cleanings are done successfully.
pause
