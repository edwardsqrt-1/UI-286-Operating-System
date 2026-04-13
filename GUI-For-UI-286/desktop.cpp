/*
 * Program: GUI for the UI(286) Operating System
 * Author: Edward Bierens
 * Date Created: Sunday April 12th, 2026
 * 
 * Description: A Command Line Interface is not enough for the modern day; as such UI(286) has an
 * optional Graphical User Interface Shell that it can run. For now, this displays a demo screen with a character 
 * map for 10 seconds, then exits. 
 * 
 */

#include <gmodes.h>
#include <graphicsmode.h>
#include <time.h>
#include <panel.h>
#include <string.h>
#include <mouse.h>
#include <window.h>

// Externally define the cursor and background maps
extern unsigned char default_cursor[80];
extern unsigned char left_click_cursor[80];
extern unsigned char right_click_cursor[80];
extern unsigned char pattern[64];

// Entry point (do not modify, copy and paste this to the beginning of your program after your includes)
void guiroot();
void __far entrypoint() {

    // Set the data segment
    __asm {
        mov ax, 0x0480
        mov ds, ax
    }

    // Call the program's main function
    guiroot();

    // Exit back into the kernel
    return;
}

// Driver program for the GUI
void guiroot() { 

    // Initialize position and color indices to be 0
    unsigned int i = 0;
    unsigned int y = 324;
    unsigned int x = 280;
    char c, m, n;
    unsigned char color = 0;
    struct MouseInfo* mouse_state;
    unsigned short clicked_x = 0xFFFF;
    unsigned short clicked_y = 0xFFFF;
    unsigned char mouse_down = 0;

    // Initialize Panel
    Panel p(28);

    // Test widget 0 on the left
    PanelWidget label(0, 150, p.Height());
    label.SetColor(0x7);
    p.AddWidget(&label);

    // Test widget 1 on the 3rd furthest from the right
    PanelWidget shutdown(440, 50, p.Height());
    shutdown.SetColor(0x4);
    p.AddWidget(&shutdown);

    // Test widget 2 on the 2nd furthest from the right
    PanelWidget ret(490, 50, p.Height());
    ret.SetColor(0x5);
    p.AddWidget(&ret);

    // Test widget 3 on the right side of the panel
    PanelClockWidget panel_clock(540, 100, p.Height());
    panel_clock.SetColor(0x3);
    p.AddWidget(&panel_clock);

    // Set the screen mode to be Mode 0x12 and point a buffer at memory
    SetGraphicsMode(MODE_640x480x16);

    // Add a cool gradient at the top
    GM_BlankScreen(0x1);

    // Draw the desktop background
    for (y = 0; y < 480 - p.Height(); y++) {
        for (x = 0; x < 640; x++) {
            GM_PutPixel(x, y, pattern[(y & 0x7) * 8 + (x & 0x7)]);
        }
    }

    // Draw panel and widgets
    p.Draw();

    // Initialize Welcome Window
    Window hello_world;
    hello_world.Draw();

    // Initialize the mouse
    Cursor cur(320, 240, default_cursor);
    m = PS2_MouseInit();
    if (m >= 0) cur.SetType(m);

    // Test out desktop widgets
    DesktopWidget dw1("Test #1", 4, 4);
    DesktopWidget dw2("Test #2", 78, 4);
    DesktopWidget dw3("Test #3", 152, 4);
    DesktopWidget dw4("Test #4", 226, 4);

    // Draw said desktop widgets
    dw1.Draw();
    dw2.Draw();
    dw3.Draw();
    dw4.Draw();

    // Wait for the Control + X key stroke, and then go back to the CLI
    c = 0;
    while (c != 'X' && c != 'x') {

        // Get character if one exists in the buffer and get time
        c = GetChar();
        panel_clock.UpdateTime();

        // Poll the mouse
        mouse_state = cur.AccessState();
        m = PS2_MousePoll(mouse_state);

        // Move the cursor if a new poll was successfully retreived
        if (m != -1) {

            // Address mouse not clicked 
            if (!mouse_state->left_clicked && !mouse_state->right_clicked) {

                GM_PutStr("              ", 520, 0, 0xF, 0x1);
                // Change cursor icon to default
                cur.ChangeIcon(default_cursor);

                // If the mouse is no longer pressed from a click, send an event
                if (mouse_down == 1) {
                    mouse_down = 0;
                    GM_PutStr("Mouse Clicked!", 520, 0, 0xF, 0x1);
                    Delay(500000);
                }

            } else { // The mouse has a button down

                // Change icon depending on what button was clicked
                if (mouse_state->left_clicked) cur.ChangeIcon(left_click_cursor);
                else if (mouse_state->right_clicked) cur.ChangeIcon(right_click_cursor);

                // Log the coordinates originally clicked
                mouse_down = 1;
                clicked_x = mouse_state->x;
                clicked_y = mouse_state->y;

            }
            cur.PlaceCursor();
        }
    }

    // Before exiting entirely, set the graphics mode back to text
    ResetGraphicsMode();

}

// Default mouse cursor icon for the GUI
unsigned char default_cursor[80] = {
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x0E, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x0E, 0x0E, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x0E, 0x0C, 0x04, 0x01, 0xFF, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x01, 0x01, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// Left-click mouse cursor icon for the GUI
unsigned char left_click_cursor[80] = {
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x03, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x03, 0x03, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x03, 0x0C, 0x04, 0x01, 0xFF, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x01, 0x01, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// Right-click mouse cursor icon for the GUI
unsigned char right_click_cursor[80] = {
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x0A, 0x0A, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x0A, 0x0C, 0x04, 0x01, 0xFF, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x01, 0x01, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// Background pattern
unsigned char pattern[64] = {
    0x5,0x5,0x1,0x1,0x1,0x1,0x3,0x3,
    0x5,0x1,0x1,0x9,0x9,0x1,0x1,0x3,
    0x1,0x1,0x9,0x9,0x9,0x9,0x1,0x1,
    0x1,0x9,0x9,0x9,0x9,0x9,0x9,0x1,
    0x1,0x9,0x9,0x9,0x9,0x9,0x9,0x1,
    0x1,0x1,0x9,0x9,0x9,0x9,0x1,0x1,
    0x3,0x1,0x1,0x9,0x9,0x1,0x1,0x5,
    0x3,0x3,0x1,0x1,0x1,0x1,0x5,0x5
};