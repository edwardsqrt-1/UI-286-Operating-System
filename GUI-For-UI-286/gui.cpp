/*
 * Program: GUI for the UI(286) Operating System
 * Author: Edward Bierens
 * Date Created: Tuesday March 10th, 2026
 * 
 * Description: A Command Line Interface is not enough for the modern day; as such UI(286) has an
 * optional Graphical User Interface Shell that it can run. For now, this displays a demo screen with a character 
 * map for 10 seconds, then exits. 
 * 
 * Developer's note: This is a subproject of UI(286); the header and source files that exist in the UI(286)
 * directory can and will be copied and pasted in the respective include and src directories (but must be renamed 
 * and adapted to C++ from C).
 * 
 */

#include <gmodes.h>
#include <graphicsmode.h>
#include <time.h>
#include <panel.h>
#include <string.h>
#include <mouse.h>

// Externally define the cursor maps
extern unsigned char default_cursor[80];
extern unsigned char left_click_cursor[80];
extern unsigned char right_click_cursor[80];

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
    struct rtc_time __far* clock = (struct rtc_time __far *) 0x10000000L; // At address 0x10000

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
    for (y = 0; y < 100; y++) {
        for (x = 0; x < 640; x++) {
            if (y < 12) GM_PutPixel(x, y, 0x5);
            else if ((x + 25) % ((y - 7) / 5) == 0) GM_PutPixel(x, y, 0x5);
        }
    }

    // Set title on the top of screen
    GM_PutStr("286GUI TEST BUILD...", 240, 4, 0xF, 255);
    
    // Test number printing out
    for (int i = 0; i*3 < 100; i++) {
        GM_PutUInt(i*3, 100 + (i % 3) * 200, 120 + i * 3, 0xB, 255);
    }

    // Test time
    GetTime(clock);
    GM_PutStr("Time the program was loaded at:", 150, 250, 0xE, 0x1);

    /* Display the time the program was loaded */
    // Print hour
    if (clock->hour < 10) {
        GM_PutUInt(0, 408, 250, 0xF, 0x1);
        GM_PutUInt(clock->hour, 416, 250, 0xF, 0x1);
    } else GM_PutUInt(clock->hour, 408, 250, 0xF, 0x1);

    // Print the colon seperating hour and minute
    GM_PutChar(':', 424, 250, 0xF, 0x1);

    // Print the minute
    if (clock->minute < 10) {
        GM_PutUInt(0, 432, 250, 0xF, 0x1);
        GM_PutUInt(clock->minute, 440, 250, 0xF, 0x1);
    } else GM_PutUInt(clock->minute, 432, 250, 0xF, 0x1);

    // Test date
    GM_PutStr("Day the program was loaded at:", 150, 260, 0xE, 0x1);

    /* Display the day the program was loaded */
    // Print month
    if (clock->month < 10) {
        GM_PutUInt(0, 400, 260, 0xF, 0x1);
        GM_PutUInt(clock->month, 408, 260, 0xF, 0x1);
    } else GM_PutUInt(clock->month, 400, 260, 0xF, 0x1);

    // Print the slash seperating month and day
    GM_PutChar('/', 416, 260, 0xF, 0x1);

    // Print the minute
    if (clock->day < 10) {
        GM_PutUInt(0, 424, 260, 0xF, 0x1);
        GM_PutUInt(clock->day, 432, 260, 0xF, 0x1);
    } else GM_PutUInt(clock->day, 424, 260, 0xF, 0x1);

    // Holiday easter eggs (pun not intended) :-)
    if (clock->month == 3 && clock->day == 14) GM_PutStr("Happy PI Day!", 150, 276, 0xE, 0x1);
    if (clock->month == 12 && clock->day == 25) GM_PutStr("Merry Christmas!", 150, 276, 0xA, 0x1);
    if (clock->month == 2 && clock->day == 14) GM_PutStr("Made with love!", 150, 276, 0xD, 0x1); 

    // Draw panel and widgets
    p.Draw();

    // Initialize the mouse and create a box for it
    Cursor cur(320, 240, default_cursor);
    m = PS2_MouseInit();
    if (m >= 0) cur.SetType(m); 
    GM_PutUInt(m, 2, 428, 0xA, 0x1);
    GM_PutRect(450, 300, 80, 80, 0x2, 0xF);
    GM_PutStr("X:", 452, 302, 0xF, 0x2);
    GM_PutStr("Y:", 452, 322, 0xF, 0x2);
    GM_PutStr("LB:", 452, 342, 0xF, 0x2);
    GM_PutStr("RB:", 452, 362, 0xF, 0x2);

    // Add a message to exit the interface
    GM_PutStr("Press X to exit this demo...", 2, 438, 0xA, 0x1); 

    // Print each implemented character to the screen
    y = 324;
    x = 240;
    GM_PutStr("CHARACTER MAP:", 248, 300, 0xC, 0x1);
    for (c = ' '; c <= '~'; c++) {
        
        // Go to new row
        if (((c - ' ') & 0xF) == 0 && c != ' ') {
            y += 10;
            x = 240;
        } else if (c != ' ') x += 8;

        // Print character after a delay of 0.15 seconds
        Delay(1000);
        GM_PutChar(c, x, y, 0xA, 0x0);

    }

    // Wait for the Control + X key stroke, and then go back to the CLI
    c = 0;
    while (c != 'X' && c != 'x') {

        // Get character if one exists in the buffer and get time
        c = GetChar();
        panel_clock.UpdateTime();

        // Poll the mouse
        m = PS2_MousePoll(cur.AccessState());
        GM_PutStr("     ", 470, 302, 0xF, 0x2);
        GM_PutStr("     ", 470, 322, 0xF, 0x2);
        GM_PutStr("  ", 478, 342, 0xF, 0x2);
        GM_PutStr("  ", 478, 362, 0xF, 0x2);

        // Update mouse coordinates if successfully polled; place the cursor on screen
        if (m != -1) {
            GM_PutUInt(cur.AccessState()->x, 470, 302, 0xF, 0x2);
            GM_PutUInt(cur.AccessState()->y, 470, 322, 0xF, 0x2);
            GM_PutUInt(cur.AccessState()->left_clicked, 478, 342, 0xF, 0x2);
            GM_PutUInt(cur.AccessState()->right_clicked, 478, 362, 0xF, 0x2);
            if (cur.AccessState()->left_clicked) cur.ChangeIcon(left_click_cursor);
            else if (cur.AccessState()->right_clicked) cur.ChangeIcon(right_click_cursor);
            else cur.ChangeIcon(default_cursor);
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
