@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
cd %1
msbuild /property:GenerateFullPaths=true /t:build /p:Platform=Win32 /p:Configuration=Debug build/game.sln