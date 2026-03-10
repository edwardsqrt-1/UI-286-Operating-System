#include <graphicsmode.h>

// Place a single pixel on the screen at the specified coordinates with foreground and background
void GM_PutPixel(unsigned short x, unsigned short y, unsigned char c) {

    // Testing code only with BIOS Interrupts; intending to replace with direct VGA control
    __asm {
        mov ah, 0x0C
        mov al, c
        xor bx, bx
        mov cx, x
        mov dx, y
        int 0x10
    }

}

// Blank out the entire screen with a background color
void GM_BlankScreen(unsigned char bg) {

    // Loop through every coordinate and set pixel
    unsigned short x, y;
    for (x = 0; x < WIDTH; x++) 
        for (y = 0; y < HEIGHT; y++) 
            GM_PutPixel(x, y, bg);

}

// Print out a single character at the coordinates with the given foreground and background
// Note: 255 = transparent background (applies also for string and integer functions)
void GM_PutChar(unsigned char c, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg) {

    // Prepare variables and choose corresponding array of the character chosen
    unsigned char row, col;
    unsigned char* map = ui286_character_map_8x10 + ((c - 0x20)*10);
    unsigned char tmp;

    // Look at each of the 10 bytes corresponding to a row
    for (row = 0; row < 10; row++) {
        
        // Get current byte representing row, and examine each bit
        tmp = map[row];
        for (col = 8; col > 0; col--) {

            // See whether the current bit is on or off to determine whether to use foreground or background
            if ((tmp & 0x1) == 1) GM_PutPixel(x + col - 1, y + row, fg);
            else if (bg != 255) GM_PutPixel(x + col - 1, y + row, bg);
            tmp = tmp >> 1; 

        }
    }

}

// Prints out a number of a given base on a graphics screen
void GM_PutUIntB(unsigned short num, unsigned short base, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg) {

    // Division operations
    unsigned short res;
    unsigned char count = 0;
    unsigned char charToPrint;

    // Make a special exception for 0; algorithm only works on non-zero integers (print 0 directly)
    if (num == 0) {
        GM_PutChar('0', x, y, fg, bg);
        return;
    }

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

    // Pop digits from stack and display them on screen for the number of times the number was divided
    while (count != 0) {

        __asm {
            pop bx
            mov charToPrint, bl
        }
        GM_PutChar(charToPrint, x, y, fg, bg);
        count--;
        x += 8;
    }

}

// Special default function for base 10
void GM_PutUInt(unsigned short num, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg) {
    GM_PutUIntB(num, 10, x, y, fg, bg);
}

// Print a string at the coordinates with the given foreground and background
void GM_PutStr(char* str, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg) {

    unsigned short i;
    for (i = 0; str[i] != 0; i++) {
        if (str[i] >= ' ') {
            GM_PutChar(str[i], x, y, fg, bg);
            x += 8;
        } else continue;
    }
}