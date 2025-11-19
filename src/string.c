#include "string.h"

// Get a character from the keyboard
char GetChar() {

    // Use the BIOS interrupt to get a character from the keyboard automatically
    // Will be subject to change
    char res;
    __asm {
        xor ax, ax
        int 0x16
        mov res, al
    }

    // Return the resulting character
    return res;

}

// Get the length of a string
unsigned short StrLen(char* str) {

    unsigned short l;
    for (l = 0; str[l] != 0; l++) continue;
    return l;
    
}

// Compares two different strings; will return 0 if different and 1 if same
unsigned char StrCompare(char* str1, char* str2) {
    
    // Check if sizes are the same
    unsigned char i = 0;
    if (StrLen(str1) != StrLen(str2)) return 0;

    // Check individual characters
    while (i < StrLen(str1)) {
        if (str1[i] != str2[i]) return 0;
        i++;
    }
    return 1;

}
