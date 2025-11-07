#include "textmode.h"

// Defining the procedure to blank out the screen
void TM_BlankScreen(unsigned char attr) {

    // Pre-declaring variables
    unsigned short i = 0;

    // Start at beginning; arm attribute byte
    __asm {
        mov ax, 0xB800
        mov es, ax
        xor di, di
        mov al, attr
    }
    
    // Loop through each pixel and set the null character and attribute byte in video memory
    for (i = 0; i < 80*25; i++) {
        __asm {
            mov byte ptr [es:di], 0
            mov [es:di+1], al
            add di, 2
        }
    }

}

// Defining the procedure to put a string
void TM_PutStr(char* str, unsigned char x, unsigned char y, unsigned char attr) {

    // Calculate offset for the video memory and declare a character value
    unsigned short off = (y * TM_WIDTH + x) * 2;
    char c;

    // Reinitialize video memory by pointing the offset value at the needed location
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov di, off
    }

    // Iterate until a null character is reached
    while (*str != 0) {

        // Assign the current character value (will not work without this)
        c = *str;

        // Print the character and its attribute in video memory
        __asm {
            mov ah, c
            mov al, attr
            mov [es:di], ah
            mov [es:di+1], al
            add di, 2
        }

        // Increment the string variable
        str++;
    }
}
