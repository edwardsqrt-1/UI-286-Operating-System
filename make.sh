#!/bin/bash

# Create libraries
for file in src/*.c; do
	wcc -0 -s -zl -zld -zls -iinclude -fo./obj/$(basename ${file%.*}).obj ${file} -dfloppy=$1
done

# Create kernel
wcc -0 -s -zl -zld -zls -iinclude -fokernel.obj kernel.c -dfloppy=$1
wlink name 'kernel.bin' output raw offset=0x500 format dos file 'kernel.obj' $(printf "file %s " obj/*.obj) order clname CODE segaddr=0x50 offset=0x0 clname DATA segaddr=0x1E0 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

# Create snake game
wcc -0 -s -zl -zld -zls -iinclude -fosnake.obj snake.c -dfloppy=$1
wlink name 'snake.286' output raw offset=0x2300 format dos file 'snake.obj' $(printf "file %s " obj/*.obj) order clname CODE segaddr=0x230 offset=0x0 clname DATA segaddr=0x480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

# Create demo
wcc -0 -s -zl -zld -zls -iinclude -fodemo.obj demo.c -dfloppy=$1
wlink name 'demo.286' output raw offset=0x2300 format dos file 'demo.obj' $(printf "file %s " obj/*.obj) order clname CODE segaddr=0x230 offset=0x0 clname DATA segaddr=0x480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

# Create text editor
wcc -0 -s -zl -zld -zls -iinclude -fotext.obj text.c -dfloppy=$1
wlink name 'text.286' output raw offset=0x2300 format dos file 'text.obj' $(printf "file %s " obj/*.obj) order clname CODE segaddr=0x230 offset=0x0 clname DATA segaddr=0x480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

# Delete stray object files and create the disk image
rm *.obj
nasm -f bin boot.asm -o boot.img -dfloppy=$1

# Set up the disk image
udisksctl loop-setup -f boot.img
udisksctl mount -b /dev/loop0
cp kernel.bin /media/ebierens/disk/
cp *.286 /media/ebierens/disk/
udisksctl unmount -b /dev/loop0
udisksctl loop-delete -b /dev/loop0
