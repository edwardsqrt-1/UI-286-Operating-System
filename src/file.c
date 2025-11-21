#include "file.h"

// Find a file on the disk
struct FATEntry* FindFile(char* name, struct FATEntry* fileSource) {

    // Pre-declare indices
    short match_base, match_ext; 
    char* dest;
    char* src;
    char c;
    unsigned short e = 0, i = 0, n = 0;
    for (i = 0; i < 12 && name[i] != 0; i++) if (name[i] >= 'a' && name[i] <= 'z') name[i] -= 0x20;

    // Find an entry that matches the pattern
    for (e = 0; e < MAX_ENTRIES && fileSource->name[0] != 0; e++, fileSource++) {

        // Skip deleted entries (starts with 0xE5)
        if (fileSource->name[0] == 0xE5) continue;

        // Initialize indices
        match_base = 1;
        dest = fileSource->name;
        src = name;
        i = 0;

        // Compare the two basenames
        while ((*dest != ' ') && (i < 8)) {
            if (*src != *dest) {
                match_base = 0;
                break;
            }

            // Increment indices
            ++i; ++src; ++dest;

            // Compare end of string characters
            if ((*dest == ' ' || i == 8) && (*src != ' ' && *src != '.' && *src != 0)) {
                match_base = 0;
                break;
            }
        }

        // If the base name does not match, move on to the next file
        if (match_base != 1) continue;

        // Re-initialize indexes to begin comparing the extensions
        match_ext = 1;
        dest = fileSource->extension;
        src = name+1;
        e = 0;
        c = *(src-1);
        while (c != '.' && c != 0) {
            ++src;
            c = *(src-1);
        }

        // Compare the extensions
        match_ext = 1;
        while (e < 3 && *dest != ' ') {
        
            // Compare characters
            if (*src != *dest) {
                match_ext = 0;
                break;
            }

            // Increment indices
            ++e; ++src; ++dest;
            c = *dest;

            // Ensure that if the end of the extension is reached, 
            if (c == ' ' || e == 3) {
                c = *src;
                if (c != 0) {
                    match_ext = 0;
                    break;
                }
            }
        

        }
          
        // Continue if entry doesn't match, proceed out of loop if there is a match
        if (match_ext != 1) continue;
        else break;

    }

    // Return the pointer to the file entry if one exists
    if (match_base && match_ext) return fileSource;
    else return 0;

    
}

// Create an empty file on the disk
short CreateFile(char* name, struct FATEntry* fileSource) {

    short res;
    struct FATEntry* fileEntry = fileSource; 
    unsigned short e = 0, i = 0, n = 0;

    // Capitalize all letters
    for (i = 0; i < 12 && name[i] != 0; i++) if (name[i] >= 'a' && name[i] <= 'z') name[i] -= 0x20;

    // Load disk sector containing the table of files
    res = ReadSector(SPF+3, ptr(fileEntry), (MAX_ENTRIES*0x40) / 512, 0);
    if (res < 0) return -1;

    // Find deleted or empty entry
    for (e = 0; e < MAX_ENTRIES && fileEntry->name[0] != 0; e++, fileEntry++) 
        if (fileEntry->name[0] == 0xE5 || fileEntry->name[0] == 0) break;

    // If maximum entries have been used, error
    if (e == MAX_ENTRIES) return -4;

    // Create new entry
    // Focus on lower 32 bytes first that contains the file information
    // Place file's base name
    for (n = 0, i = 0; n < 8 && name[n] != '.' && name[n] != 0 && name[n] != ' '; n++, i++) fileEntry->name[n] = name[n];
    while (i < 8) {
        fileEntry->name[i] = ' ';
        ++i;
    }

    // Place extension of the file
    ++n;
    if (name[n-1] == '.') for (e = 0; e < 3 &&  name[n+e] != 0; e++) fileEntry->extension[e] = name[n+e];
    else e = 0;
    while (e < 3) {
        fileEntry->extension[e] = ' ';
        ++e;
    }

    // Set the different attributes (0 will be used as a placeholder)
    fileEntry->attr = 0;
    for (e = 0; e < 10; e++) fileEntry->notUsed[e] = 0;
    fileEntry->rawTime = 0;
    fileEntry->rawDate = 0;
    fileEntry->sector = 0;
    fileEntry->size = 0;
    
    // Focus on upper 32 bytes for long file name support
    // Start character
    fileEntry->start = 'A';

    // Copy entire file's name into the long file name (split into 3 segments of 5, 6, and 2 characters)
    // Once completed, the rest are filled with 0's
    n = 1;
    for (e = 0; e < 13; e++) {

        if (name[e] == 0) n = 0; // Find end of string

        if (n) { // File's name is still printed
            if (e < 5) fileEntry->longName1[e] = (unsigned short) name[e];
            else if (e < 11) fileEntry->longName2[e-5] = (unsigned short) name[e];
            else fileEntry->longName3[e-11] = (unsigned short) name[e];
        } else { // File's name has already ended
            if (e < 5) fileEntry->longName1[e] = 0;
            else if (e < 11) fileEntry->longName2[e-5] = 0;
            else fileEntry->longName3[e-11] = 0;
        } 
        
    }

    // Set long file name entry attributes (checksum is calculated from the name bytes)
    fileEntry->longAttr = 0x0F;
    fileEntry->longType = 0;
    fileEntry->checksum = 0;
    for (e = 0; e < 11; e++) fileEntry->checksum += fileEntry->name[e];
    fileEntry->zero = 0;

    // Update table on disk
    res = WriteSector(SPF+3, ptr(fileSource), (MAX_ENTRIES*0x40) / 512, 0);
    if (res < 0) return -3;
    return 0;
}

// Delete a file from the disk
short DeleteFile(char* name, struct FATEntry* fileSource) {

    // Pre-declare entries
    short res;
    struct FATEntry* fileEntry; 
    unsigned short e = 0, i = 0, n = 0;

    // Load disk sector containing the table of files
    res = ReadSector(SPF+3, ptr(fileSource), (MAX_ENTRIES*0x40) / 512, 0);
    if (res < 0) return -1;

    // Locate file
    fileEntry = FindFile(name, fileSource);
    if (fileEntry == 0) return -2;

    // "Delete" entry by setting the first character of the name to 0xE5
    fileEntry->name[0] = 0xE5;

    // Update table on disk
    res = WriteSector(SPF+3, ptr(fileSource), (MAX_ENTRIES*0x40) / 512, 0);
    if (res < 0) return -3;
    return 0;
}
