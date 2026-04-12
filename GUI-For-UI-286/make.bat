@echo off

rem Create libraries
for %%f in (src\*.cpp) do wpp -0 -s -zl -zld -zls -i.\include -fo.\obj\%%~nf.obj %%f -dfloppy=%1
for %%f in (src\*.c) do wcc -0 -s -zl -zld -zls -i.\include -fo.\obj\%%~nf.obj %%f -dfloppy=%1

rem Create GUI
wpp -0 -s -zl -zld -zls -i.\include gui.cpp -dfloppy=%1
wlink option nodefaultlib name '.\gui.286' output raw offset=0x2300 format dos file 'gui.obj' file '.\obj\*.obj' order clname CODE segaddr=0x0230 offset=0x0 clname DATA segaddr=0x0480 offset=0x0

rem Delete stray object files and create the disk image
del *.obj