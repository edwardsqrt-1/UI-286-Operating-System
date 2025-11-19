# UI-286-Operating-System
This is a simple operating system for some of the oldest PCs that have long since been abandoned support by modern operating systems such as Linux and in some cases some BSD distributions. While it is not intended to be a UNIX-like system, many commands will be more familiar to said systems such as the `ls` and `cd` commands for example. 

## Tenative Installation Steps
I intend to have a more elegant setup upon closer to completion, however these are the steps to build (tested on Windows 10 with OpenWatcom and NASM):

- Create Disk
    - `nasm -f bin boot.asm -o boot.img -dfloppy=<360|1200|1440>`

- Compile Kernel Sources
    - `wcc -0 -s -zl -zld -zls .\kernel.c -dfloppy=<360|1200|1440>`
    - `wcc -0 -s -zl -zld -zls .\disk.c -dfloppy=<360|1200|1440>`
    - `wcc -0 -s -zl -zld -zls .\file.c`
    - `wcc -0 -s -zl -zld -zls .\parser.c`
    - `wcc -0 -s -zl -zld -zls .\speaker.c`
    - `wcc -0 -s -zl -zld -zls .\string.c`
    - `wcc -0 -s -zl -zld -zls .\textmode.c`
    - `wcc -0 -s -zl -zld -zls .\time.c`

- Link object files together from compiling kernel
    - `wlink name '.\kernel.bin' output raw offset=0x700 format dos file '*.obj' order clname CODE offset=0x700 clname DATA offset=0x3000`

- The generated kernel.bin file must then be placed within the floppy disk image boot.img. This can be done with a program such as PowerISO, just as long as kernel.bin is the first file ever placed on the disk (the bootloader will not look past the first entry). Files and folders can then be added as necessary.

## Commands Implemented

UI(286) will have 7 commands available in the CLI:

- `clear` to clear the CLI window
- `ls` to get a list of all of the files
- `create <file>` makes a file [NOT IMPLEMENTED] 
- `rm <file>` deletes a file [NOT IMPLEMENTED] 
- `print [sentence]` takes the input afterwards and prints it to the screen
- `info` displays basic system information
- `help` brings up the list of commands

These were mainly based off of the commands in UNIX, but some have been slightly modified to make more sense to the user.

## Credits

This project would have not been possible without the sound advice given by OSDev.org https://wiki.osdev.org/Expanded_Main_Page

For an in-depth guide as to how different computer components work, the book *The Indispensable PC Hardware Book* by Hans-Peter Messmer is an extremely helpful resource. The edition used in UI(286) development is the 1994 edition.

Special thanks to the following pieces of software for emulating and debugging UI(286):

- 86Box @ https://86box.net/ for providing a close-to-authentic emulator for potential machines
- DOSBox-X @ https://dosbox-x.com/ for providing the debugging tools to solve issues
- PowerISO @ https://www.poweriso.com/ for quick and easy access to manipulating the file system on virtual floppy disks



