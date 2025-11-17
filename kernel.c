#include "speaker.h"
#include "time.h"
#include "textmode.h"
#include "string.h"
#include "parser.h"
#define VERSION_NUMBER "0.1"
#define DATA_SEGMENT 0x300

// Forward declaration of printing a list of commands
void PrintHelp(unsigned char x, unsigned char y);

// Forward declaration of displaying information
void PrintInfo(unsigned char x, unsigned char y);

// Main kernel function
void main286() {

    // Pre-declare variables
    short i;
    char* welcome = "Welcome to the UI(286) Operating System v0.1!";
    char* note = "UI(286) Shell v0.15";
    char* prompt = "286sh @ ";
    char* unknown = "ERROR: Could not recognize command!";
    char* keybuff; // I would use char keybuff[50] here, but for some reason it only works with this
    char x = 9, y = 5;
  
    // Beep for a specified amount of times while increasing the frequency
    for (i = 0; i < 5; i++) {
        beep(1046-i*i*10, 75000);
        delay(500);
    }

    // Clear screen
    TM_BlankScreen(0x30);

    // Add two strings; one for welcome message and one as a note (also includes prompt)
    TM_PutStr(bar, 0, 0, 0x19);
    TM_PutStr(bar, 0, 24, 0x19);
    TM_SetTitle("\xDB UI(286) CLI \xDB");
    TM_PutStr(welcome, 1, 2, 0x3F);
    TM_PutStr(note, 1, 3, 0x39);
    TM_PutStr(prompt, 1, 5, 0x3F);
    
    // (TEMPORARY!) This buffer has to be initialized somewhere reasonable, OpenWatcom will NOT initialize it!
    keybuff = (char*) 0x7000 - DATA_SEGMENT;
    
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

                    --x;
                    TM_ClearChar(x, y, 0x30);
                    keybuff[--i] = 0;
                    TM_SetCursor(x, y);

                } else if (i == 0) { // If at beginning already, make a noise

                    beep(1046, 25000);
                    beep(1196, 25000);
                    beep(1046, 25000);

                }

                // Cancel out the i++ to keep i the same value and continue
                --i;
                continue;
            }

            // Show character on screen and increment the value of x
            TM_PutChar(keybuff[i], x, y, 0x39);
            x++;

        }

        // Check for empty command
        if (i == 0) {

            // Scroll down another line if needed
            ++y;
            if (y >= TM_HEIGHT - 1) {
                ScrollConsoleDown(0x30);
                y = TM_HEIGHT - 2;
            }

            // Place prompt again
            TM_PutStr(prompt, 1, y, 0x3F);
            x = 9;
            continue;
            
        }

        // Terminate string and clear anything afterwards
        while (i < 50) keybuff[i++] = 0;

        // Choose the command given
        switch (ChooseCommand(keybuff)) {

            case CLEAR_SCREEN: // Clear screen command

                // Just clear the console
                TM_ClearConsole(0x30);
                y = 2;
                TM_PutStr(prompt, 1, y, 0x3F);
                x = 9;
                continue;

            case LIST_FILES: // List files command
            case CREATE_FILE: // Create a file command
            case DELETE_FILE: // Delete a file command

                // Reserve the next line (scroll if needed)
                ++y;
                if (y >= TM_HEIGHT - 1) {
                    ScrollConsoleDown(0x30);
                    y = TM_HEIGHT - 2;
                }

                // Print out the buffer
                TM_PutChar(0xAF, 1, y, 0x3E);
                TM_PutStr(keybuff, 3, y, 0x3D);
                break;

            case PRINT_STR: // Print a given string command

                // Reserve the next line (scroll if needed)
                ++y;
                if (y >= TM_HEIGHT - 1) {
                    ScrollConsoleDown(0x30);
                    y = TM_HEIGHT - 2;
                }

                // Print out the argument
                TM_PutChar(0xAF, 1, y, 0x3E);
                TM_PutStr(keybuff+6, 3, y, 0x3E);
                break;

            case HELP: // Print a list of commands

                // Reserve lines for the number of commands + one more
                y += NUM_COMMANDS+3;
                if (y >= TM_HEIGHT - 1) {
                    while (y > TM_HEIGHT - 2) {
                        ScrollConsoleDown(0x30);
                        --y;
                    }
                }

                // Print help information
                PrintHelp(1, y-(NUM_COMMANDS+3));
                break;

            case VERSION: // Print version of the system and system information

                // Reserve the next 3 lines (scroll if needed)
                y += 7;
                if (y >= TM_HEIGHT - 1) {
                    while (y > TM_HEIGHT - 2) {
                        ScrollConsoleDown(0x30);
                        --y;
                    }
                }

                // Print out the version information
                PrintInfo(1, y-7);
                break;
                
            default: // Unknown command
                
                // Scroll down to the next line
                ++y;
                if (y >= TM_HEIGHT - 1) {
                    ScrollConsoleDown(0x30);
                    y = TM_HEIGHT - 2;
                }

                // Print out the buffer
                TM_PutStr(unknown, 1, y, 0x34);
                break;
        }   
        
        // Scroll down another line if needed
        ++y;
        if (y >= TM_HEIGHT - 1) {
            ScrollConsoleDown(0x30);
            y = TM_HEIGHT - 2;
        }

        // Place prompt again
        TM_PutStr(prompt, 1, y, 0x3F);
        x = 9;

    }
}

// Prints out a list of commands and explains what they do
void PrintHelp(unsigned char x, unsigned char y) {
 
    // Prints top boundary
    ++y;
    TM_PutStr("----------------------------------------------------", x, y, 0x3B);
    
    // This is the header
    ++y;
    TM_PutStr("Available Commands:", x, y, 0x3E);

    // Explain what the "clear" command does
    ++y;
    TM_PutStr("clear", x, y, 0x3D);
    TM_PutStr("\xAF Clears the CLI", x+8, y, 0x3F);

    // Explain what "ls" does
    ++y;
    TM_PutStr("ls", x, y, 0x3D);
    TM_PutStr("\xAF Lists files in the current directory", x+8, y, 0x3F);

    // Explain what "create" does
    ++y;
    TM_PutStr("create", x, y, 0x3D);
    TM_PutStr("\xAF Creates a file with the given name", x+8, y, 0x3F);

    // Explain what "rm" does
    ++y;
    TM_PutStr("rm", x, y, 0x3D);
    TM_PutStr("\xAF Deletes a given file", x+8, y, 0x3F);

    // Explain what "print" does
    ++y;
    TM_PutStr("print", x, y, 0x3D);
    TM_PutStr("\xAF Prints a string in the CLI", x+8, y, 0x3F);

    // Explain what "info" does
    ++y;
    TM_PutStr("info", x, y, 0x3D);
    TM_PutStr("\xAF Displays basic system information", x+8, y, 0x3F);

    // Finally, explain what the "help" command does
    ++y;
    TM_PutStr("help", x, y, 0x3D);
    TM_PutStr("\xAF Displays this help message", x+8, y, 0x3F);

    // Print 
    ++y;
    TM_PutStr("----------------------------------------------------", x, y, 0x3B);
}

// Display system information
void PrintInfo(unsigned char x, unsigned char y) {

    // Determine what the memory and the disk signature are using code from the boot sector
    unsigned short RAM = *((unsigned short*) (0x7DFA - DATA_SEGMENT*16));
    unsigned char sig = *((unsigned char*) (0x7C15 - DATA_SEGMENT*16));

    // Print a header bar
    ++y;
    TM_PutStr("----------------------------------------------------", x, y, 0x3B);
    
    // Display UI(286) version
    ++y;
    TM_PutStr("UI", x, y, 0x3C);
    TM_PutStr("(", x+2, y, 0x3F);
    TM_PutStr("286", x+3, y, 0x3B);
    TM_PutStr(")", x+6, y, 0x3F);
    TM_PutStr("Operating System", x+8, y, 0x3F);
    TM_PutStr("v", x+25, y, 0x3F);
    TM_PutStr(VERSION_NUMBER, x+26, y, 0x3C);

    // Print the amount of memory in KB
    ++y;
    TM_PutStr("Conventional RAM detected:", x, y, 0x3F);
    TM_PutUInt(RAM, x+27, y, 0x3E);
    TM_PutStr("KB", x+31, y, 0x3F);

    // Print what disk UI(286) is using
    ++y;
    TM_PutStr("Loaded from:", x, y, 0x3F);
    switch (sig) {
        case 0xFD:
            TM_PutStr("5.25\" 360 KB Floppy Disk", x+13, y, 0x3F);
            break;
        case 0xF9:
            TM_PutStr("5.25\" 1.2 MB Floppy Disk", x+13, y, 0x3F);
            break;
        case 0xF0:
            TM_PutStr("3.5\" 1.44 MB Floppy Disk", x+13, y, 0x3F);
            break;
    }

    // Print the author
    y += 2;
    TM_PutStr("A hobbyist operating system made by Edward Bierens", x, y, 0x3B);
    
    // Print footer bar
    ++y;
    TM_PutStr("----------------------------------------------------", x, y, 0x3B);
    
}
