#include "textmode.h"
#include "string.h"

// Character layout for the top and bottom bars
char bar[TM_WIDTH] = {
    0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,
    0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,
    0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,
    0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,0xB0,
    0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,0xB1,
    0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,
    0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,0xDB
};

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
            add di, TM_BYTES_PER_PIXEL
        }
    }

}

// Procedure to blank the screen out with a color
void TM_ClearConsole(unsigned char attr) {

    // Create necessary offsets
    unsigned short off = TM_WIDTH * TM_BYTES_PER_PIXEL;
    unsigned short i;

    // Initialize graphics
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov di, off
        mov bl, attr
    }

    // Clear everything except for the top and bottom bars
    for (i = off; i < TM_WIDTH * TM_HEIGHT; i++) {
        __asm {
            mov byte ptr [es:di], 0
            mov [es:di+1], bl
            add di, TM_BYTES_PER_PIXEL
        }
    }

}

// Looks at what character value is set at a given location
char TM_PeekChar(unsigned char x, unsigned char y) {

    // Calculate offset
    char __far * screen = (char __far *) 0xB8000000L; // ES = 0xB800; BX = 0x0
    unsigned short off = (y * TM_WIDTH + x) * TM_BYTES_PER_PIXEL;
    screen += off;
    return *screen;

}

// Places a character in the video memory
void TM_PutChar(char c, unsigned char x, unsigned char y, unsigned char attr) {

    // Calculate offset
    unsigned short off = (y * TM_WIDTH + x) * TM_BYTES_PER_PIXEL;
    
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

// Prints out a number of a given base into the video memory
void TM_PutUIntB(unsigned short num, unsigned short base, unsigned char x, unsigned char y, unsigned char attr) {

    // Calculate offset
    unsigned short off = (y * TM_WIDTH + x) * TM_BYTES_PER_PIXEL;

    // Division operations
    unsigned short res;
    unsigned char count = 0;

    // Keep dividing number and pushing to the stack until the number reaches 0
    while (num != 0) {

        res = num % base;
        num /= base;
        __asm {
            mov bx, res
            add bx, '0'
            push bx
        }        
        count++;

    }

    // Initialize video memory
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov di, off
        mov al, attr
    }

    // Pop digits from stack and display them on screen for the number of times the number was divided
    while (count != 0) {

        __asm {
            pop bx
            mov ah, bl
            mov [es:di], ah
            mov [es:di+1], al
            add di, TM_BYTES_PER_PIXEL
        }
        count--;

    }
}

// Special function to print base 10 numbers
void TM_PutUInt(unsigned short num, unsigned char x, unsigned char y, unsigned char attr) {
    TM_PutUIntB(num, 10, x, y, attr);
}


// Clears a character from the video memory
void TM_ClearChar(unsigned char x, unsigned char y, unsigned char attr) {

    // Similar to TM_PutChar, but with the character byte 0.
    unsigned short off = (y * TM_WIDTH + x) * TM_BYTES_PER_PIXEL;
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
    unsigned short off = (y * TM_WIDTH + x) * TM_BYTES_PER_PIXEL;
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
            add di, TM_BYTES_PER_PIXEL
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
    unsigned short below = 2 * TM_BYTES_PER_PIXEL * TM_WIDTH;
    unsigned short above = TM_BYTES_PER_PIXEL * TM_WIDTH;

    // Start at beginning; arm attribute byte
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov di, above
        mov si, below
    }
    
    // Loop through each pixel and set the null character and attribute byte in video memory
    for (i = 0; i < (TM_HEIGHT - 3) * TM_WIDTH; i++) {
        __asm {
            mov ax, [es:si]
            mov [es:di], ax
            add di, TM_BYTES_PER_PIXEL
            add si, TM_BYTES_PER_PIXEL
        }
    }

    // Add empty line below
    for (i = 0; i < TM_WIDTH; i++) {
        __asm {
            mov al, attr
            mov byte ptr [es:di], 0
            mov [es:di+1], al
            add di, TM_BYTES_PER_PIXEL
        }
    }

    // Re-add bars and title
    TM_TUIInit("\xDB UI(286) CLI \xDB", 0x19);

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

// Create the title bars and title
void TM_TUIInit(char* title_text, unsigned char attr) {

    // Create necessary calculation helpers
    unsigned short i = 0;
    unsigned short title_len = StrLen(title_text);
    unsigned short position = (TM_WIDTH - title_len) / 2;

    // Print the two bars and title
    TM_PutStr(bar, 0, 0, attr);
    TM_PutStr(bar, 0, TM_HEIGHT-1, attr);
    TM_PutStr(title_text, position, 0, (attr & 0xF0) | 0x0F);

}
