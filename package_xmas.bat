@echo off
rmdir /S /Q package

mkdir package\base
copy build\bin\Release\engine.exe package\
copy build\bin\Release\SDL2.dll package\
copy build\bin\release\game.dll package\base\

rename package\engine.exe hotair.exe

rcedit-x64 "package\hotair.exe" --set-icon "xmas\icon.ico"

"c:\Program Files\7-Zip\7z.exe" a -tzip package\base\pak00.pk3 .\base\*
"c:\Program Files\7-Zip\7z.exe" u -tzip package\base\pak00.pk3 .\xmas\* -xr!.git -xr!.vscode

"c:\Program Files\7-Zip\7z.exe" a -tzip package\hotair.zip .\package\*