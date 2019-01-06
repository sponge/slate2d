@echo off
set name=%1

rmdir /S /Q package

mkdir package\%name%\base
copy build\bin\Release\engine.exe package\%name%\%name%.exe
copy build\bin\Release\SDL2.dll package\%name%\
copy build\bin\Release\game.dll package\%name%\base\

rcedit-x64 "package\%name%\%name%.exe" --set-icon "%name%\icon.ico"

"c:\Program Files\7-Zip\7z.exe" a -tzip package\%name%\base\pak00.pk3 .\base\*
"c:\Program Files\7-Zip\7z.exe" u -tzip package\%name%\base\pak00.pk3 .\%name%\*

"c:\Program Files\7-Zip\7z.exe" a -tzip package\%name%.zip .\package\*