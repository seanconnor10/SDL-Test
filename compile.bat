@echo Building!

gcc src/main.c -IC:\Users\Student\workspace\personal\mingw64\mingw64\include -LC:\Users\Student\workspace\personal\mingw64\mingw64\lib -lmingw32 -lSDL2main -lSDL2 -lm -o test.exe

@echo off

pause

REM "g++ 01_hello_SDL.cpp -IC:\mingw_dev_lib\include\SDL2 -LC:\mingw_dev_lib\lib -w -Wl,"
REM "-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o 01_hello_SDL"
