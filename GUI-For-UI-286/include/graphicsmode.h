#ifndef GRAPHICSMODE_H
#define GRAPHICSMODE_H
#include <charmap.h>

// Defining the width and height of text mode as 80x25
#define HEIGHT 480
#define WIDTH 640
#define TM_BYTES_PER_PIXEL 2

// Procedure to place a pixel on the screen
void GM_PutPixel(unsigned char x, unsigned char y, unsigned char fg, unsigned char bg);

// Blank out the screen to a specific color
void GM_BlankScreen(unsigned char bg);

// Procedure to print out a single character in graphics mode
void GM_PutChar(unsigned char c, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg);

// Procedure to print a number in graphics mode
void GM_PutUInt(unsigned short num, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg);

// Procedure to print out a string in graphics mode
void GM_PutStr(char* str, unsigned short x, unsigned short y, unsigned char fg, unsigned char bg);

#endif
