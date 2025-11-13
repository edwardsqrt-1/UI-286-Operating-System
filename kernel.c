#include "speaker.h"
#include "time.h"
#include "textmode.h"
#include "string.h"

// Main kernel function
void main286() {

    // Pre-declare variables
    short i;
    char* welcome = "Welcome to the UI(286) Operating System v0.05!";
    char* note = "UI(286) Shell v0.1";
    char* prompt = "286sh @ ";
    char* keybuff;
    char x = 9, y = 4;
  
    // Beep for a specified amount of times while increasing the frequency
    for (i = 0; i < 10; i++) {
        beep(1046-i*i*5, 125000);
        delay(1000);
    }

    // Clear screen
    TM_BlankScreen(0x30);

    // Add two strings; one for welcome message and one as a note (also includes prompt)
    TM_PutStr(welcome, 1, 1, 0x3F);
    TM_PutStr(note, 1, 2, 0x39);
    TM_PutStr(prompt, 1, 4, 0x3F);

    // (TEMPORARY!) This buffer has to be initialized somewhere reasonable, OpenWatcom will NOT initialize it!
    keybuff = (char*) 0x1000;
    
    // Command line
    while (1) {

        // Get keyboard input for 50 characters and echo; "execute" on enter
        for (i = 0; i < 50; i++) {

            // Get the current character
            keybuff[i] = GetChar();

            // Execute on carriage return (enter pressed)
            if (keybuff[i] == 0xD) break;

            // Delete last character
            if (keybuff[i] == 0x8) {

                if (i > 0) { // Proceed with deleting last character if one exists

                    x--;
                    TM_ClearChar(x, y, 0x30);
                    keybuff[--i] = 0;
                    TM_SetCursor(x, y);

                } else if (i == 0) { // If at beginning already, make a noise

                    beep(1046, 25000);
                    beep(1196, 25000);
                    beep(1046, 25000);

                }

                // Cancel out the i++ to keep i the same value and continue
                i--;
                continue;
            }

            // Show character on screen and increment the value of x
            TM_PutChar(keybuff[i], x, y, 0x3A);
            x++;
        }

        // Terminate string and go to new line (scroll down if at bottom of screen)
        keybuff[i] = 0;
        y++;
        if (y >= TM_HEIGHT) {
            ScrollConsoleDown(0x30);
            y = TM_HEIGHT - 1;
        }

        // Print out the buffer
        TM_PutChar(0xAF, 1, y, 0x3E);
        TM_PutStr(keybuff, 3, y, 0x3E);

        // Go to next line again (scroll if needed)
        y++;
        if (y >= TM_HEIGHT) {
            ScrollConsoleDown(0x30);
            y = TM_HEIGHT - 1;
        }

        // Place prompt again
        TM_PutStr(prompt, 1, y, 0x3F);
        x = 9;

    }
}
