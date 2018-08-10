@echo off
win64\docgen.exe html_doc ../src/docgen/doc_in ../src/moai/doc_root

call cp_from_src_to_install_dir.bat
