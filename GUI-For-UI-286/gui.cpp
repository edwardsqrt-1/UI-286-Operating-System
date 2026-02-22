/*
 * Program: GUI for the UI(286) Operating System
 * Author: Edward Bierens
 * Date Created: Thursday February 17th, 2026
 * 
 * Description: A Command Line Interface is not enough for the modern day; as such UI(286) has an
 * optional Graphical User Interface Shell that it can run. For now, this only displays a striped screen
 * for 3 seconds, then exits. 
 * 
 * Developer's note: This is a subproject of UI(286); the header and source files that exist in the UI(286)
 * directory can and will be copied and pasted in the respective include and src directories (but must be renamed 
 * and adapted to C++ from C).
 * 
 */

#include <graphics_modes.h>
#include <time.h>

// Entry point (do not modify, copy and paste this to the beginning of your program after your includes)
void guiroot();
void __far entrypoint() {

    // Set the data segment
    __asm {
        mov ax, 0x0400
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
    unsigned char color = 0;

    // Set the screen mode to be Mode 0x13 and point a buffer at memory
    SetGraphicsMode(MODE_320x200x256);
    char __far * video = ADDR_VGA_GRAPHICS;

    // Fill the screen with vertical stripes of color
    for (i = 0; i < 64000; i++, color++) {
        video[i] = color; 
        if (i % 320 == 0) color = 0;
    }

    // Wait 3 seconds and reset to text mode
    delay(3000000);
    ResetGraphicsMode();

}