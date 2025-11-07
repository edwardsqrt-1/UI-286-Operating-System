# UI-286-Operating-System
This is a simple operating system for some of the oldest PCs that have long since been abandoned support by modern operating systems such as Linux and in some cases some BSD distributions. While it is not intended to be a UNIX-like system, many commands will be more familiar to said systems such as the `ls` and `cd` commands for example. 

## Tenative Installation Steps
I intend to have a more elegant setup upon closer to completion, however these are the steps to build (tested on Windows 10 with OpenWatcom and NASM):

- Create Disk
`nasm -f bin boot.asm -o boot.img -dfloppy=<360|1200|1440>`

- Compile Kernel Sources
    - `wcc -0 -s -zl -zld -zls .\kernel.c`
    - `wcc -0 -s -zl -zld -zls .\speaker.c`
    - `wcc -0 -s -zl -zld -zls .\textmode.c`
    - `wcc -0 -s -zl -zld -zls .\time.c`

- Link object files together from compiling kernel
`wlink name '.\kernel.bin' output raw offset=0x700 format dos file '*.obj' order clname CODE offset=0x700 clname DATA offset=0x900`

- The generated kernel.bin file must then be placed within the floppy disk image boot.img. This can be done with a program such as PowerISO.
