@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" > nul
call custom/bin/build_one_time.bat custom/languages/czapa_cpp_lexer_gen.cpp
one_time.exe
build.bat
