#ifndef MOUSE_H
#define MOUSE_H
#define SERIAL_MOUSE_BAUD_RATE 1200
#define MICROSOFT_2_MOUSE 2
#define MICROSOFT_3_MOUSE 3
#define MICROSOFT_S_MOUSE 5
#define UNKNOWN_MOUSE 0
#include <graphicsmode.h>

// Structure to contain mouse information as the mouse reads
struct MouseInfo {
    char type;
    short x;
    short y;
    unsigned char left_clicked;
    unsigned char right_clicked;
};

// Structure for a cursor
class Cursor {

    private:
        unsigned short last_x;
        unsigned short last_y;
        struct MouseInfo mouse;     // Mouse information struct that acts as the base for the cursor
        unsigned char icon[80];     // Icon the cursor uses to display onto the screen
        unsigned char screen[80];   // Pixels the cursor will cover up

    public:
        Cursor(unsigned short x, unsigned short y, unsigned char* mouse_bitmap);    // Initialize the cursor with coordinates and icon
        struct MouseInfo* AccessState();            // Access the mouse state directly
        void SetType(unsigned char type);           // Set the type of mouse the cursor is using
        void ChangeIcon(unsigned char* new_bitmap); // Change the icon of the cursor
        void PlaceCursor();                         // Place the cursor on the screen

};

// Function to initialize the PS/2 mouse.
// Returns -1 on failure, or the mouse ID on success
char PS2_MouseInit();

// Function to poll mouse information 
// Returns -1 on failure, or 0 on success
char PS2_MousePoll(struct MouseInfo* mouse);

#endif