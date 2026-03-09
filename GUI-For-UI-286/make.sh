#!/bin/bash

# Create libraries
for file in src/*.cpp; do
    wpp -0 -s -zl -zld -zls -iinclude -fo./obj/$(basename ${file%.*}).obj ${file} -dfloppy=$1
done

# Create kernel
wpp -0 -s -zl -zld -zls -iinclude -fogui.obj gui.cpp -dfloppy=$1
wlink name 'gui.286' output raw offset=0x2300 format dos file 'gui.obj' $(printf "file %s " obj/*.obj) order clname CODE offset=0x2300 clname DATA offset=0x4800

# Delete stray object files and create the disk image
rm *.obj
