@echo off
rmdir /S /Q package

mkdir package\hotair
mkdir package\hotair\base
copy build\bin\Release\engine.exe package\hotair\
copy build\bin\Release\SDL2.dll package\hotair\
copy build\bin\release\game.dll package\hotair\base\

rename package\hotair\engine.exe hotair.exe

rcedit-x64 "package\hotair.exe" --set-icon "xmas\icon.ico"

"c:\Program Files\7-Zip\7z.exe" a -tzip package\hotair\base\pak00.pk3 .\base\*
"c:\Program Files\7-Zip\7z.exe" u -tzip package\hotair\base\pak00.pk3 .\xmas\* -xr!.git -xr!.vscode

"c:\Program Files\7-Zip\7z.exe" a -tzip package\hotair.zip .\package\*