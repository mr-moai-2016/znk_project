@echo off
win64\docgen.exe source_doc ../src/docgen ../src/moai/doc_root/source_doc

call cp_from_src_to_install_dir.bat
pause
