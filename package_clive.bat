@echo off
rmdir /S /Q package

mkdir package\base
copy build\bin\Release\engine.exe package\
copy build\bin\Release\SDL2.dll package\
copy build\bin\release\game.dll package\base\

"c:\Program Files\7-Zip\7z.exe" a -tzip package\base\pak00.pk3 .\base\*
"c:\Program Files\7-Zip\7z.exe" u -tzip package\base\pak00.pk3 .\clive\*

"c:\Program Files\7-Zip\7z.exe" a -tzip package\game.zip .\package\*