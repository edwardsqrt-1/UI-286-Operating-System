#include "string.h"
#include "disk.h"

// Get a character from the keyboard without blocking execution
char GetChar() {

    // Passively get keyboard input and if input exists
    char res = 0;
    char pressed = 0;
    __asm {
        mov ah, 0x1
        xor al, al
        int 0x16
        jz keydone

        xor ah, ah
        int 0x16
        mov res, al
        mov pressed, 1

        keydone:
    }

    // Return the resulting character if a key was pressed to begin with
    if (pressed) return res;
    return 0;

}

// Wait for a character from the keyboard 
char GetChar_H() {

    // Use the BIOS interrupt to get a character from the keyboard
    char res = 0;
    __asm {
        xor ah, ah
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

// Concatenates str2 at the end of str1
void StrCat(char* str1, char* str2) {

    // Find null character
    char* beginConcat = str1;
    while (*beginConcat != 0) ++beginConcat;

    // Copy from string 2 to the end of string 1
    while (*str2 != 0) {
        *beginConcat = *str2;
        ++beginConcat;
        ++str2;
    }

    // Terminate string
    *beginConcat = 0;

}
