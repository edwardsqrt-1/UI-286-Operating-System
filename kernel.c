#include "speaker.h"
#include "time.h"
#include "textmode.h"

// Main kernel function (will just print off a C)
void main286() {

    // Pre-declare variables
    short i;
    char* welcome = "Welcome to the UI(286) Operating System v0.01!";
    char* note = "Note: For now, only a Command Line Interface is shown; non-functional";
    char* prompt = "UI(286) #";

    
    // Beep for a specified amount of times while increasing the frequency
    for (i = 0; i < 10; i++) {
        beep(1046-i*i*5, 125000);
        delay(1000);
    }

    // Clear screen
    TM_BlankScreen(0x30);

    // Add two strings; one for welcome message and one as a note (also includes prompt)
    TM_PutStr(welcome, 1, 1, 0x3F);
    TM_PutStr(note, 1, 2, 0x3C);
    TM_PutStr(prompt, 1, 4, 0x3F);
    
    // Halt here
    while (1) {
        __asm {
            nop
            nop
        }
    }

}
