#ifndef FILE_H
#define FILE_H
#include "disk.h"

// Template to easily read FAT12 filesystem entries
struct FATEntry {
    char spacer[32];
    char name[8];
    char extension[3];
    unsigned char attr;
    char not_used[10];
    unsigned short raw_time;
    unsigned short raw_date;
    unsigned short sector;
    unsigned long size;
};


#endif
