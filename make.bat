@echo off
wcc -0 -s -zl -zld -zls -i.\include kernel.c -dfloppy=%1
for %%f in (src\*.c) do wcc -0 -s -zl -zld -zls -i.\include %%f -dfloppy=%1
wlink name '.\kernel.bin' output raw offset=0x700 format dos file '*.obj' order clname CODE offset=0x700 clname DATA offset=0x3000
nasm -f bin boot.asm -o boot.img -dfloppy=%1
rem del *.obj