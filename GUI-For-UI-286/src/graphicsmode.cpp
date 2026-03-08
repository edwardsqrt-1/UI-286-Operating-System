#include <graphicsmode.h>

// Place a single pixel on the screen at the specified coordinates with foreground and background
// Note: 255 = transparent background (applies for other functions)
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

    unsigned short x, y;
    for (x = 0; x < WIDTH; x++) 
        for (y = 0; y < HEIGHT; y++) 
            GM_PutPixel(x, y, bg);

}

// Print out a single character at the coordinates with the given foreground and background
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

// Print an unsigned integer at the coordinates with the given foreground and background
void GM_PutUInt(unsigned short num, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg) {

}

// Print a string at the coordinates with the given foreground and background
void GM_PutStr(char* str, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg) {

}