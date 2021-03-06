@echo off

set CommonCompilerFlags=-O2 -FC -GR- -Oi -MP -WL -Z7 -nologo -EHsc -Fobin/ -DDEBUG -DPLATFORM_WINDOWS /Ithirdparty 
set CommonLinkerFlags=-incremental:no -opt:ref /Subsystem:Windows /LIBPATH:thirdparty/ "User32.lib" "Gdi32.lib" "kernel32.lib" "raylib.lib"

cl %CommonCompilerFlags% src/main.cpp /Febin/ld49 /Fdbin/ld49 /link %CommonLinkerFlags%
