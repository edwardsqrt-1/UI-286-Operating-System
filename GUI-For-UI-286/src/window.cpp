#include <window.h>

// Initialize window
Window::Window() {

    // Initialize rectangle and name
    estate.x = 160;
    estate.y = 180;
    estate.w = 320;
    estate.h = 120;
    title = "GUI for UI(286)";

    // Set default colors
    window_color = 0xD;
    title_color = 0xB;
    outer_border_color = 0x0;
    inner_border_color = 0xF;

}

// Draw the window to the screen
void Window::Draw() {

    // Helper variables
    unsigned short x;
    unsigned short y;

    // Loop through every pixel in the range of the window
    for (x = estate.x; x < estate.x + estate.w; x++) {
        for (y = estate.y; y < estate.y + estate.h; y++) {

            // Draw the outside borders on the window should it be on the edges or between the title bar and window viewport
            if (
                x == estate.x || x == estate.x + 2 || x == estate.x + estate.w - 1 || x == estate.x + estate.w - 3 || // On the left or right edges
                y == estate.y || y == estate.y + 2 || y == estate.y + estate.h - 1 || y == estate.y + estate.h - 3 || // On the top or bottom edges
                y == estate.y + 14 || // Between the title bar and window viewport
                (x == estate.x + estate.w - 15 && y < estate.y + 14) 
            ) GM_PutPixel(x, y, outer_border_color);

            else if (
                x > estate.x + estate.w - 15 && x < estate.x + estate.w - 3 && 
                y < estate.y + 14 && y > estate.y + 2
            ) GM_PutPixel(x, y, 0xC);

            // Draw the inside of the borders
            else if (
                x == estate.x + 1 || x == estate.x + estate.w - 2 || // On the left or right edges
                y == estate.y + 1 || y == estate.y + estate.h - 2  // On the top or bottom edges
            ) GM_PutPixel(x, y, inner_border_color);

            else if (y < estate.y + 14) GM_PutPixel(x, y, title_color); // Title bar but not window viewport
            else GM_PutPixel(x, y, window_color); // Otherwise in the viewport
        }
    }

    // Print window title
    GM_PutStr(title, estate.x + 4, estate.y + 4, 0x0, 0xFF);

}

