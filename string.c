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
