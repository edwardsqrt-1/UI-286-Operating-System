#ifndef DISK_H
#define DISK_H
#define DATA_SEGMENT 0x1B0 // Data segment value 
#define PROGRAM_LOAD_SEGMENT 0x200 // Data segment value for program loading
#define PROGRAM_LOAD_OFFSET 0x0
#define mem(x) (x - DATA_SEGMENT*16) // Apply data offset to get the location in relation to the data segment
#define ptr(x) ((unsigned short)x + DATA_SEGMENT*16) // Convert pointer into a memory address as an integer

#if floppy == 360 // Define parameters for 5.25" 360 KB FDD

#define SPT 9
#define HEADS 2
#define MAX_ENTRIES 55
#define ROOT_DIRS 7
#define SPF 2
#define SPC 2

#elif floppy == 1440 // Define parameters for 3.5" 1.44 MB FDD

#define SPT 18
#define HEADS 2
#define MAX_ENTRIES 111
#define ROOT_DIRS 14
#define SPF 9
#define SPC 1

#else // Default is 5.25" 1.2 MB FDD

// General disk information true for all:
#define SPT 15
#define HEADS 2
#define MAX_ENTRIES 111
#define ROOT_DIRS 14
#define SPF 7
#define SPC 1

#endif

// Floppy disk signatures
#define FD_1440_KB 0xF0
#define FD_1200_KB 0xF9
#define FD_360_KB 0xFD

// Function to load a sector from the disk (written as LBA); 0 = success and -1 = failure
short ReadSector(unsigned short lba, unsigned long addr, unsigned char sectors, unsigned char drive);

// Function to store a sector on the disk at an LBA address; 0 = success and -1 = failure
short WriteSector(unsigned short lba, unsigned long addr, unsigned char sectors, unsigned char drive);

#endif
