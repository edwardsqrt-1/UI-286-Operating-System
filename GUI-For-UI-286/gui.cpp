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
    unsigned char c;
    unsigned char color = 0;
    struct rtc_time __far* clock = (struct rtc_time __far *) 0x10000000L; // At address 0x10000

    // Set the screen mode to be Mode 0x13 and point a buffer at memory
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
    gettime(clock);
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
        delay(150000);
        GM_PutChar(c, x, y, 0xA, 0x0);

    }

    // Wait 10 seconds and reset to text mode
    delay(10000000);
    ResetGraphicsMode();

}