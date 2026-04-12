@echo off

rem Create libraries
for %%f in (src\*.c) do wcc -0 -s -zl -zld -zls -i.\include -fo.\obj\%%~nf.obj %%f -dfloppy=%1

rem Create kernel
wcc -0 -s -zl -zld -zls -i.\include kernel.c -dfloppy=%1
wlink name '.\kernel.bin' output raw offset=0x500 format dos file 'kernel.obj' file '.\obj\*.obj' order clname CODE segaddr=0x50 offset=0x0 clname DATA segaddr=0x1E0 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

rem Create snake game
wcc -0 -s -zl -zld -zls -i.\include -fosnake.obj snake.c -dfloppy=%1
wlink name '.\snake.286' output raw offset=0x2300 format dos file 'snake.obj' file '.\obj\*.obj' order clname CODE segaddr=0x230 offset=0x0 clname DATA segaddr=0x480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

rem Create demo
wcc -0 -s -zl -zld -zls -i.\include -fodemo.obj demo.c -dfloppy=%1
wlink name '.\demo.286' output raw offset=0x2300 format dos file 'demo.obj' file '.\obj\*.obj' order clname CODE segaddr=0x230 offset=0x0 clname DATA segaddr=0x480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0
rem Create text editor
wcc -0 -s -zl -zld -zls -i.\include -fotext.obj text.c -dfloppy=%1
wlink name '.\text.286' output raw offset=0x2300 format dos file 'text.obj' file '.\obj\*.obj' order clname CODE segaddr=0x230 offset=0x0 clname DATA segaddr=0x480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

rem Delete stray object files and create the disk image
del *.obj
nasm -f bin boot.asm -o boot.img -dfloppy=%1