/*
 * Program: GUI for the UI(286) Operating System
 * Author: Edward Bierens
 * Date Created: Sunday March 8th, 2026
 * 
 * Description: A Command Line Interface is not enough for the modern day; as such UI(286) has an
 * optional Graphical User Interface Shell that it can run. For now, this only displays some text
 * for 3 seconds, then exits. 
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
    unsigned char c;
    unsigned char color = 0;

    // Set the screen mode to be Mode 0x13 and point a buffer at memory
    SetGraphicsMode(MODE_640x480x16);
    
    // Print each implemented character to the screen (wait a second between each character)
    GM_BlankScreen(0x1);
    for (c = ' '; c <= '?'; c++) {
        delay(1000000);
        GM_PutChar(c, 1+((c-' ')*8), 1, 0xA, 0x0);
    }

    // Wait 10 seconds and reset to text mode
    delay(10000000);
    ResetGraphicsMode();

}