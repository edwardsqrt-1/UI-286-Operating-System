#ifndef DISK_H
#define DISK_H
#define DATA_SEGMENT 0x300 // Data segment value 
#define mem(x) (x - DATA_SEGMENT*16) // Apply data offset to get the location in relation to the data segment
#define ptr(x) ((unsigned short)x + DATA_SEGMENT*16) // Convert pointer into a memory address as an integer

#if floppy == 360 // Define parameters for 5.25" 360 KB FDD

#define SPT 9
#define HEADS 2
#define MAX_ENTRIES 55
#define ROOT_DIRS 7
#define SPF 2

#elif floppy == 1440 // Define parameters for 3.5" 1.44 MB FDD

#define SPT 18
#define HEADS 2
#define MAX_ENTRIES 111
#define ROOT_DIRS 14
#define SPF 9

#else // Default is 5.25" 1.2 MB FDD

#define SPT 15
#define HEADS 2
#define MAX_ENTRIES 111
#define ROOT_DIRS 14
#define SPF 7

#endif

// Function to load a sector from the disk (written as LBA); 0 = success and -1 = failure
short ReadSector(unsigned short lba, unsigned long addr, unsigned char sectors, unsigned char drive);

// Function to store a sector on the disk at an LBA address; 0 = success and -1 = failure
short WriteSector(unsigned short lba, unsigned long addr, unsigned char sectors, unsigned char drive);

#endif
