@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" > nul
custom/bin/buildsuper_x64-win.bat custom/czapa_4coder.cpp release
rem custom/bin/buildsuper_x64-win.bat custom/czapa_4coder.cpp