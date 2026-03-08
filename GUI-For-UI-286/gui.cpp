/*
 * Program: GUI for the UI(286) Operating System
 * Author: Edward Bierens
 * Date Created: Sunday March 8th, 2026
 * 
 * Description: A Command Line Interface is not enough for the modern day; as such UI(286) has an
 * optional Graphical User Interface Shell that it can run. For now, this only displays a character 
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

    // Set the screen mode to be Mode 0x13 and point a buffer at memory
    SetGraphicsMode(MODE_640x480x16);

    // Add a title
    GM_BlankScreen(0x1);
    GM_PutStr("286GUI TEST BUILD...", 240, 5, 0xA, 0x1);

    // Print each implemented character to the screen
    GM_PutStr("CHARACTER MAP:", 264, 300, 0xC, 0x1);
    for (c = ' '; c <= 'Z'; c++) {
        
        // Go to new row
        if (((c - ' ') & 0x7) == 0 && c != ' ') {
            y += 10;
            x = 280;
        } else if (c != ' ') x += 8;

        // Print character after a delay of 0.15 seconds
        delay(150000);
        GM_PutChar(c, x, y, 0xA, 0x0);

    }

    // Wait 10 seconds and reset to text mode
    delay(10000000);
    ResetGraphicsMode();

}