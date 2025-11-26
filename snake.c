/*
 * Program: UI(286) Snake
 * Author: Edward Bierens
 * Date Created: Tuesday November 25th, 2025
 * 
 * Description: This is a simple "snake" game where you can draw a snake
 * around on the screen. W,A,S,D keys control the snake like a typical
 * video game, C changes the color of the snake, and Space pauses the game.
 * Hitting Enter will close the program and return to the CLI.
 * 
 */

#include <string.h>
#include <textmode.h>
#include <time.h>

// Possible directions that a snake may go
enum SnakeDirection {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
    SNAKE_STOPPED
};

// Structure for a snake
struct Snake {
    unsigned char length;
    unsigned char x;
    unsigned char y;
    unsigned char attr;
    enum SnakeDirection going;
};

// Entry point (do not modify, copy and paste this to the beginning of your program after your includes)
void main();
void __far entrypoint() {

    // Set the data segment
    __asm {
        mov ax, 0x1500
        mov ds, ax
    }

    // Call the program's main function
    main();

    // Exit back into the kernel
    return;
}

// Main program code
void main() {

    // Prepare variables
    int i = 0;
    char c = 0;
    struct Snake snek;

    // Initialize window
    TM_BlankScreen(0x50);
    TM_TUIInit("\xDB SNAKE! \xDB", 0x4C);

    // Initialize snake
    snek.x = 0;
    snek.y = 1;
    snek.attr = 0x1B;
    snek.going = SNAKE_STOPPED;

    // Until the enter key is pressed, draw a snake on screen
    while (c != 0x0D) {

        // Get the current character from the keyboard if one exists
        c = GetChar();

        // Determine whether to go 
        switch (c) {
            case 'w': // W = up
                snek.going = SNAKE_UP;
                break;
            case 'a': // A = left
                snek.going = SNAKE_LEFT;
                break;
            case 's': // S = down
                snek.going = SNAKE_DOWN;
                break;
            case 'd': // D = right
                snek.going = SNAKE_RIGHT;
                break;
            case ' ': // Space = pause
                c = 0;
                while (c != ' ') c = GetChar();
                break;
            case 'c': // Change color
                snek.attr = (0xF0 & snek.attr) | ((snek.attr + 1) & 0x0F);
                break;
        }

        // Go once in that direction
        switch (snek.going) {
            case SNAKE_UP:
                if (snek.y > 1) snek.y--;
                break;
            case SNAKE_DOWN:
                if (snek.y < TM_HEIGHT-2) snek.y++;
                break; 
            case SNAKE_LEFT:
                if (snek.x != 0) snek.x--;
                break;  
            case SNAKE_RIGHT:
                if (snek.x < TM_WIDTH-1) snek.x++;
                break;
        }

        // Show snake going in that direction
        TM_PutChar(0xDB, snek.x, snek.y, snek.attr);

        // Delay by a split-second
        delay(35000);

    }

}
