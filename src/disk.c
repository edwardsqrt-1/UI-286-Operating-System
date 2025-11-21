#include "disk.h"

// Within a given memory address and the drive to load from, it will load a certain amount of sectors from a starting point 
// Given in Linear Block Addressing for easy conversion to Cylinders Heads Sectors format.
short ReadSector(unsigned short lba, unsigned long addr, unsigned char sectors, unsigned char drive) {

    // Pre-declare variables
    unsigned short segm, off;
    unsigned char cyl, head, sect;
    unsigned char s = 0;
    unsigned int res;

    // At least load one single sector
    do {

        // Prepare parameters
        segm = addr >> 4;
        off = addr & 0xF;
        cyl = lba / (HEADS * SPT);
        head = (lba / SPT) % HEADS;
        sect = ((lba % SPT) + 1) & 0x3F;

        // Load parameters and use BIOS interrupt 13h
        __asm {
            mov bx, segm
            mov es, bx
            mov bx, off
            mov ah, 0x02
            mov al, 1
            mov cl, sect
            mov ch, cyl
            mov dh, head
            mov dl, drive
            int 0x13

            jnc floppy_disk_done
            mov ax, 0xFFFF
            mov res, ax

            floppy_disk_done:
            mov ax, 0x0
            mov res, ax
        }

        // Increment to next sector and to the next block of memory
        lba++;
        addr += 512;
        s++;

    } while (s < sectors); // Stop when desired sector amount is reached

    return res;
}

// Within a given memory address and the drive to load from, it will store the data in the specified sectors of the disk
// Given in Linear Block Addressing for easy conversion; similar process to the function ReadSector.
short WriteSector(unsigned short lba, unsigned long addr, unsigned char sectors, unsigned char drive) {

    // Pre-declare variables
    unsigned short segm, off;
    unsigned char cyl, head, sect;
    unsigned char s = 0;
    unsigned int res;

    // At least load one single sector
    do {

        // Prepare parameters
        segm = addr >> 4;
        off = addr & 0xF;
        cyl = lba / (HEADS * SPT);
        head = (lba / SPT) % HEADS;
        sect = ((lba % SPT) + 1) & 0x3F;

        // Load parameters and use BIOS interrupt 13h
        __asm {
            mov bx, segm
            mov es, bx
            mov bx, off
            mov ah, 0x03
            mov al, 1
            mov cl, sect
            mov ch, cyl
            mov dh, head
            mov dl, drive
            int 0x13

            jnc floppy_write_done
            mov ax, 0xFFFF
            mov res, ax

            floppy_write_done:
            mov ax, 0x0
            mov res, ax
        }

        // Increment to next sector and to the next block of memory
        lba++;
        addr += 512;
        s++;

    } while (s < sectors); // Stop when desired sector amount is reached

    return res;
}
