#!/bin/bash

# Create libraries
for file in src/*.cpp; do
    wpp -0 -s -zl -zld -zls -iinclude -fo./obj/$(basename ${file%.*}).obj ${file} -dfloppy=$1
done

# Create GUI Demo
wpp -0 -s -zl -zld -zls -iinclude -fogui.obj gui.cpp -dfloppy=$1
wlink name 'gui.286' output raw offset=0x2300 format dos file 'gui.obj' $(printf "file %s " obj/*.obj) order clname CODE segaddr=0x0230 offset=0x0 clname DATA segaddr=0x0480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

# Create Desktop Application
wpp -0 -s -zl -zld -zls -iinclude -fogui.obj desktop.cpp -dfloppy=$1
wlink name 'desktop.286' output raw offset=0x2300 format dos file 'desktop.obj' $(printf "file %s " obj/*.obj) order clname CODE segaddr=0x0230 offset=0x0 clname DATA segaddr=0x0480 offset=0x0 clname STACK segaddr=0x0980 offset=0x0

# Delete stray object files and create the disk image
rm *.obj
