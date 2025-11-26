/*
 * Program: UI(286) Kernel / Command Line Interface (286 Shell)
 * Author: Edward Bierens
 * Date Created: Monday November 24th, 2025
 * 
 * Description: This is a piece of command line software that acts as the kernel for UI(286).
 * The possible commands that can be given are as follows (cross-listed from README.md):
 *  - `clear` to clear the CLI window
 *  - `ls` to get a list of all of the files
 *  - `create <file>` makes a file
 *  - `rm <file>` deletes a file
 *  - `print [sentence]` takes the input afterwards and prints it to the screen
 *  - `info` displays basic system information
 *  - `help` brings up the list of commands
 * 
 * Alternatively, a 286 file can be loaded in the filesystem. Such files are automatically
 * viewed as executables by UI(286).
 * 
 */

#include <speaker.h>
#include <time.h>
#include <textmode.h>
#include <string.h>
#include <parser.h>
#include <disk.h>
#include <file.h>

// Parameters
#define VERSION_NUMBER "0.15"
#define DATA_SEGMENT 0x300
#define FD_1440_KB 0xF0
#define FD_1200_KB 0xF9
#define FD_360_KB 0xFD

// Stub to tell the program to go to the main loop
void main286();
void GoToMain() {
    main286();
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

// Function to call an app
void CallApp() {

    
    // Initialize variables and the segment + offset of the program to load
    int i;
    unsigned short seg = 0x1000;
    unsigned short off = 0;

    // Create a function definition at that faraway address
    typedef void (__far * prog)(void);
    prog main = (prog) (void __far *) (((unsigned long)(seg) << 16) + (unsigned long)(off));

    // Create backup of video memory at 0x5000
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov di, 0x5000
        xor si, si
    }
    for (i = 0; i < 80*25*2; i++) {
        __asm {
            mov al, [es:si]
            mov [ds:di], al
            inc si
            inc di
        }
    }

    // Load program
    main();

    // Restore the data segment
    __asm {
        mov ax, 0x300
        mov ds, ax
    }

    // Restore video memory
    __asm {
        mov ax, 0xB800
        mov es, ax
        mov si, 0x5000
        xor di, di
    }
    for (i = 0; i < 80*25*2; i++) {
        __asm {
            mov al, [ds:si]
            mov [es:di], al
            inc si
            inc di
        }
    }

}

// Display system information
void PrintInfo(unsigned char x, unsigned char y, unsigned char sig, unsigned short RAM) {

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
        case FD_360_KB:
            TM_PutStr("5.25\" 360 KB Floppy Disk", x+13, y, 0x3F);
            break;
        case FD_1200_KB:
            TM_PutStr("5.25\" 1.2 MB Floppy Disk", x+13, y, 0x3F);
            break;
        case FD_1440_KB:
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

// Main kernel function
void main286() {

    // Pre-declare variables
    char* welcome = "Welcome to the UI(286) Operating System v0.15!";
    char* note = "UI(286) Shell v0.3";
    char* prompt = "286sh @ ";
    char* unknown = "ERROR: Could not recognize command!";
    char* keybuff = (char*) mem(0x7000);
    char* namebuff = (char*) mem(0x7032);
    short* memsize_loc = (short*) mem(0x7BFD);
    char* disk_descriptor_loc = (char*) mem(0x7BFF);
    struct FATEntry* ent;
    unsigned char x = 9, y = 5;
    short res, i, j, k;
    unsigned long sectors;
    char c;

    // Copy values from boot sector to the kernel data
    *memsize_loc = *((unsigned short*) mem(0x7DFA));
    *disk_descriptor_loc = *((unsigned char*) mem(0x7C15));

    // Beep for a specified amount of times while increasing the frequency
    for (i = 0; i < 5; i++) {
        beep(1046-i*i*10, 75000);
        delay(500);
    }

    // Clear screen
    TM_BlankScreen(0x30);

    // Add two strings; one for welcome message and one as a note (also includes prompt)
    TM_TUIInit("\xDB UI(286) CLI \xDB", 0x19);
    TM_PutStr(welcome, 1, 2, 0x3F);
    TM_PutStr(note, 1, 3, 0x39);
    TM_PutStr(prompt, 1, 5, 0x3F);
    
    // Command line
    while (1) {

        // Clear buffers
        for (i = 0; i < 50; i++) keybuff[i] = 0;
        for (i = 0; i < 10; i++) namebuff[i] = 0;

        // Get keyboard input for 50 characters and echo; "execute" on enter
        for (i = 0; i < 50; i++) {

            // Get the current character
            keybuff[i] = GetChar_H();

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

                // Load sectors into where the boot sector originally lived
                res = ReadSector(SPF+3, 0x7C00, (MAX_ENTRIES*0x40) / 512, 0);
                if (res < 0) {
                    ++y;
                    if (y >= TM_HEIGHT - 1) {
                        ScrollConsoleDown(0x30);
                        y = TM_HEIGHT - 2;
                    }
                    TM_PutStr("Disk failed to load, is it inserted?", 1, y, 0x34);
                    break;
                }

                // Point the entry structure template to the first FAT entry
                ent = (struct FATEntry*) mem(0x7C00);

                // Loop through every entry
                for (k = 0; k < MAX_ENTRIES && ent->name[0] != 0; k++, ent++) {
                    
                    // Skip deleted entries (starts with 0xE5)
                    if (ent->name[0] == 0xE5) continue;

                    // Reserve another line of video memory
                    ++y;
                    if (y >= TM_HEIGHT - 1) {
                        ScrollConsoleDown(0x30);
                        y = TM_HEIGHT - 2;
                    }
                    
                    // Print determination whether this is a file or a directory (located in attribute byte)
                    if ((ent->attr & 0x10) != 0) TM_PutStr("[D]", 1, y, 0x3E);
                    else TM_PutStr("[F]", 1, y, 0x3F);
                    x = 5;

                    // Determine how to print the file name based on whether it is a file or directory
                    if ((ent->attr & 0x10) != 0) { // Directory

                        // Print full 11-character file name
                        for (i = 0; i < 11 && ent->name[i] != ' '; i++) keybuff[i] = ent->name[i];
                        keybuff[i] = 0;
                        TM_PutStr(keybuff, x, y, 0x3E);

                    } else { // File

                        // Print 8 characters for the name, followed by a dot and then 3 characters for extension
                        for (i = 0; i < 8 && ent->name[i] != ' '; i++) keybuff[i] = ent->name[i];
                        keybuff[i] = '.';
                        ++i;
                        for (j = 0; j < 3; j++) keybuff[i+j] = ent->extension[j];
                        keybuff[i+j] = 0;
                        TM_PutStr(keybuff, x, y, 0x3F);

                    }
                }

                break;

            case CREATE_FILE: // Create a file command
            case DELETE_FILE: // Delete a file command

                // Ignore extra whitespace
                i = 2;
                if (ChooseCommand(keybuff) == CREATE_FILE) i = 6;          
                else if (ChooseCommand(keybuff) == DELETE_FILE) i = 2;
                while (keybuff[i] == ' ') ++i;

                // Scroll down a line and throw an error if only the command was provided
                if (keybuff[i] == 0) {
                    ++y;
                    if (y >= TM_HEIGHT - 1) {
                        ScrollConsoleDown(0x30);
                        y = TM_HEIGHT - 2;
                    }
                    TM_PutStr("File was not provided!", 1, y, 0x34);
                    break;
                }

                // Extract the name of the file to work with
                for (j = 0; j < 12 && keybuff[i] != 0 && keybuff[i] != ' '; j++,i++) namebuff[j] = keybuff[i];

                // Terminate string and create or delete the file depending on the command
                namebuff[j] = 0;  
                if (ChooseCommand(keybuff) == CREATE_FILE) res = CreateFile(namebuff, (struct FATEntry*) mem(0x7C00));              
                else if (ChooseCommand(keybuff) == DELETE_FILE) res = DeleteFile(namebuff, (struct FATEntry*) mem(0x7C00));

                // Clear the name buffer
                for (j = 0; j < 11; j++) namebuff[j] = 0;
                
                // Scroll down a line
                ++y;
                if (y >= TM_HEIGHT - 1) {
                    ScrollConsoleDown(0x30);
                    y = TM_HEIGHT - 2;
                }                        
                        
                // Print corresponding message
                if (res == -1) TM_PutStr("ERROR: Disk could not be read from.", 1, y, 0x34);
                else if (res == -2) TM_PutStr("ERROR: File could not be found.", 1, y, 0x34);
                else if (res == -3) TM_PutStr("ERROR: Disk could not be updated.", 1, y, 0x34);
                else if (res == -4) TM_PutStr("ERROR: Disk is full, cannot create file.", 1, y, 0x34);
                else if (res >= 0) TM_PutStr("Success!", 1, y, 0x3B);
                else TM_PutStr("ERROR: UNKNOWN ERROR.", 1, y, 0x34);
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
                PrintInfo(1, y-7, *disk_descriptor_loc, *memsize_loc);
                break;
                
            default: // Unknown 
            
                // Look for executable
                StrCat(keybuff, ".286");
                ent = FindFile(keybuff, (struct FATEntry*) mem(0x7C00));

                // Check if the entry exists
                if (ent) {                    

                    // Load entry
                    sectors = ent->size >> 9;
                    res = ReadSector((ent->sector)+(SPF+3+ROOT_DIRS)*SPC - 2, 0x10000, sectors, 0);
                    if (res < 0) {

                        // Scroll down to the next line
                        ++y;
                        if (y >= TM_HEIGHT - 1) {
                            ScrollConsoleDown(0x30);
                            y = TM_HEIGHT - 2;
                        }

                        // Print out a failed to load sectors command
                        TM_PutStr("Failed to load sectors to load program!", 1, y, 0x34);

                    // Otherwise, it is ready for the program to load
                    } else CallApp();
                    break;

                } else { // Entry does not exist

                    // Scroll down to the next line
                    ++y;
                    if (y >= TM_HEIGHT - 1) {
                        ScrollConsoleDown(0x30);
                        y = TM_HEIGHT - 2;
                    }

                    // Print out an unknown command
                    TM_PutStr(unknown, 1, y, 0x34);
                    break;
                }

        }   
        
        // Clear buffer
        for (i = 0; i < 50; i++) keybuff[i] = 0;
        for (i = 0; i < 10; i++) namebuff[i] = 0;

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


