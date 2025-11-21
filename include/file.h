#pragma pack(1)
#ifndef FILE_H
#define FILE_H
#include "disk.h"

// Template to easily read FAT12 filesystem entries
struct FATEntry {
    char start;
    unsigned short longName1[5];
    unsigned char longAttr;
    unsigned char longType;
    unsigned char checksum;
    unsigned short longName2[6];
    unsigned short zero;
    unsigned short longName3[2];
    char name[8];
    char extension[3];
    unsigned char attr;
    char notUsed[10];
    unsigned short rawTime;
    unsigned short rawDate;
    unsigned short sector;
    unsigned long size;
};

// Function to find a file on the disk; -1 = no file, 0..MAX_ENTRIES-1 = index of file
struct FATEntry* FindFile(char* name, struct FATEntry* fileSource);

// Functions to create and delete files on the disk
// 0 = Success
// 1 = Disk read error
// 2 = File not found
// 3 = Disk write error
// 4 = Disk full
short CreateFile(char* name, struct FATEntry* fileSource);
short DeleteFile(char* name, struct FATEntry* fileSource);

#endif
