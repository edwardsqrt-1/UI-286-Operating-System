#ifndef MOUSE_H
#define MOUSE_H
#define SERIAL_MOUSE_BAUD_RATE 1200
#define MICROSOFT_2_MOUSE 2
#define MICROSOFT_3_MOUSE 3
#define MICROSOFT_S_MOUSE 5
#define UNKNOWN_MOUSE 0
#include <graphicsmode.h>

// Structure to contain mouse information
struct MouseInfo {
    char type;
    short x;
    short y;
    unsigned char left_clicked;
    unsigned char right_clicked;
};

// Structure for a cursor
struct Cursor {
    unsigned short x;
    unsigned short y;
    unsigned char* icon;
};

// Function to initialize the PS/2 mouse.
// Returns -1 on failure, or the mouse ID on success
char PS2_MouseInit();

// Function to poll mouse information 
char PS2_MousePoll(struct MouseInfo __far* mouse);

// Move the cursor with the given mouse information
void PlaceCursor(struct MouseInfo __far* mouse, unsigned char* icon, unsigned char* screen_buff);

#endif