@echo off
set name=%1

rmdir /S /Q package

mkdir build\package\%name%\base
copy build\bin\x86_64_Release\slate2d.exe build\package\%name%\%name%.exe
copy build\bin\x86_64_Release\SDL2.dll build\package\%name%\
copy build\bin\x86_64_Release\libopenmpt.dll build\package\%name%\
copy build\bin\x86_64_Release\openmpt-mpg123.dll build\package\%name%\
copy build\bin\x86_64_Release\openmpt-ogg.dll build\package\%name%\
copy build\bin\x86_64_Release\openmpt-vorbis.dll build\package\%name%\
copy build\bin\x86_64_Release\openmpt-zlib.dll build\package\%name%\
copy build\bin\x86_64_Release\game.dll build\package\%name%\base\

rcedit-x64 "build\package\%name%\%name%.exe" --set-icon "%name%\icon.ico"

"c:\Program Files\7-Zip\7z.exe" a -tzip build\package\%name%\base\pak00.pk3 .\base\*
"c:\Program Files\7-Zip\7z.exe" u -tzip build\package\%name%\base\pak00.pk3 .\%name%\*

"c:\Program Files\7-Zip\7z.exe" a -tzip build\package\%name%.zip .\build\package\*