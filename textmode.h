#ifndef TEXTMODE_H
#define TEXTMODE_H

// Defining the width and height of text mode as 80x25
#define TM_HEIGHT 25
#define TM_WIDTH  80

// Procedure to blank the screen out with a color
void TM_BlankScreen(unsigned char attr);

// Procedure to print out a single character
void TM_PutChar(char c, unsigned char x, unsigned char y, unsigned char attr);

// Procedure to clear a particular character
void TM_ClearChar(unsigned char x, unsigned char y, unsigned char attr);

// Procedure to print out a string
void TM_PutStr(char* str, unsigned char x, unsigned char y, unsigned char attr);

// Procedure to scroll the screen up by 1
void ScrollConsoleDown(char attr);

// Procedure to change the location of the text cursor
void TM_SetCursor(unsigned char x, unsigned char y);

#endif
