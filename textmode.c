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
    for (i = 0; i < TM_HEIGHT * TM_WIDTH; i++) {
        __asm {
            mov byte ptr [es:di], 0
            mov [es:di+1], al
            add di, 2
        }
    }

}

// Places a character in the video memory
void TM_PutChar(char c, unsigned char x, unsigned char y, unsigned char attr) {

    // Calculate offset
    unsigned short off = (y * TM_WIDTH + x) * 2;
    
    // Initialize pointer to video memory and place the character and attribute bytes
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov di, off
        mov ah, c
        mov al, attr
        mov [es:di], ah
        mov [es:di+1], al
    }

    // Set Cursor to character in video memory directly in front
    TM_SetCursor(x+1, y);

}

// Clears a character from the video memory
void TM_ClearChar(unsigned char x, unsigned char y, unsigned char attr) {

    // Similar to TM_PutChar, but with the character byte 0.
    unsigned short off = (y * TM_WIDTH + x) * 2;
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov di, off
        mov al, attr
        mov byte ptr [es:di], 0
        mov [es:di+1], al
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
        x++;
    }

    // When printing is complete, set cursor after string
    TM_SetCursor(x, y);

}

// Scrolls the entire console window in a specific direction by 1
void ScrollConsoleDown(char attr) {

    // Pre-declaring variables
    unsigned short i;
    unsigned short off = 2 * TM_WIDTH;

    // Start at beginning; arm attribute byte
    __asm {
        mov ax, 0xB800
        mov es, ax
        xor di, di
        mov si, off
    }
    
    // Loop through each pixel and set the null character and attribute byte in video memory
    for (i = 0; i < (TM_HEIGHT - 1) * TM_WIDTH; i++) {
        __asm {
            mov ax, [es:si]
            mov [es:di], ax
            add di, 2
            add si, 2
        }
    }

    // Add empty line below
    for (i = 0; i < TM_WIDTH; i++) {
        __asm {
            mov al, attr
            mov byte ptr [es:di], 0
            mov [es:di+1], al
            add di, 2
        }
    }

}

// Sets the cursor in text mode
void TM_SetCursor(unsigned char x, unsigned char y) {

    // Calculate offset and split into two bytes
    unsigned short off = y * TM_WIDTH + x;
    unsigned char off_h = off >> 8;
    unsigned char off_l = off & 0xFF;

    // Select the VGA register for the upper byte of the cursor
    // Send the upper byte of the offset value
    __asm {
        mov dx, 0x3D4
        mov al, 0xE
        out dx, al
        inc dx
        mov al, off_h
        out dx, al
    }

    // Select the VGA register for the lower byte of the cursor
    // Send the lower byte of the offset value
    __asm {
        mov dx, 0x3D4
        mov al, 0xF
        out dx, al
        inc dx
        mov al, off_l
        out dx, al
    }

}
