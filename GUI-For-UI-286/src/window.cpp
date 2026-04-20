#include <window.h>

/*  WINDOW FUNCTIONS */

// Initialize window
Window::Window() {

    // Initialize window rectangle and name
    estate.x = 160;
    estate.y = 180;
    estate.w = 320;
    estate.h = 102;
    title = "GUI for UI(286)";

    // Initialize window view
    view.x = estate.x + 3;
    view.y = estate.y + 15;
    view.w = estate.w - 6;
    view.h = estate.h - 18;

    // Set default colors
    window_color = 0xD;
    title_color = 0xB;
    outer_border_color = 0x0;
    inner_border_color = 0xF;

}

// Get Window rectangle
Rectangle* Window::GetEstate() { return &estate; }

// Get Window viewport
Rectangle* Window::GetViewport() { return &view; }

// Add content to a window
void Window::AddContent(WindowContent* c) {
    contents[num_contents] = c;
    num_contents++;
}

// Draw the window to the screen
void Window::Draw() {

    // Helper variables
    unsigned short x, y;

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

            //  Draw the exit button
            else if (
                x > estate.x + estate.w - 15 && x < estate.x + estate.w - 3 && 
                y < estate.y + 14 && y > estate.y + 2
            ) GM_PutPixel(x, y, 0xC);

            // Draw the inside of the borders
            else if (
                x == estate.x + 1 || x == estate.x + estate.w - 2 || // On the left or right edges
                y == estate.y + 1 || y == estate.y + estate.h - 2  // On the top or bottom edges
            ) GM_PutPixel(x, y, inner_border_color);

            // Draw the window internals
            else if (y < estate.y + 14) GM_PutPixel(x, y, title_color); // Title bar but not window viewport
            else GM_PutPixel(x, y, window_color); // Otherwise in the viewport
        }
    }

    // Print window title and draw other contents if there are any
    GM_PutStr(title, estate.x + 4, estate.y + 4, 0x0, 0xFF);
    for (x = 0; x < num_contents; x++) contents[x]->Draw();

}

// Window click procedure 
void Window::OnClick(unsigned short x, unsigned short y) {

    // Will implement this when window dragging works

}

/* LABEL FUNCTIONS */

// Constructor for a label at default position
Label::Label(Window* w, char* str) {

    // Initialize label contents
    unsigned short length = 0;
    label_text = str;
    estate.x = 0;
    estate.y = 0;
    for (length = 0; str[length] != 0 && length < 32; length++); // Getting length of string
    estate.w = (length * 8) % (w->GetViewport()->w);
    estate.h = ((length * 8) / (w->GetViewport()->w) + 1) * 10;

    // Link the parent window with the label
    parent = w;
    w->AddContent(this);
    
}

// Constructor for a label at a custom position
Label::Label(Window* w, char* str, unsigned short x, unsigned short y) {

    // Initialize label contents
    unsigned short length = 0;
    label_text = str;
    estate.x = x;
    estate.y = y;
    for (length = 0; str[length] != 0 && length < 32; length++); // Also getting length of string
    estate.w = ((length - x) * 8) % (w->GetViewport()->w); // Use the entire width of the window          
    estate.h = (((length - x) * 8) / (w->GetViewport()->w) + 1) * 10; // Use at least the number of lines needed to print the label (wrap-around)
    
    // Link the parent window with the label
    parent = w;
    w->AddContent(this);
    
}

// Drawing function for a label
void Label::Draw() {

    // Initialize where the label is going to be printed
    char ci;
    unsigned short x = parent->GetViewport()->x + estate.x;
    unsigned short y = parent->GetViewport()->y + estate.y;

    // Draw label onto the screen character by character
    for (ci = 0; label_text[ci] != 0; ci++) {

        // If row is filled, go to next row
        if (x+9 > parent->GetViewport()->x + parent->GetViewport()->w) {
            y += 10;
            x = parent->GetViewport()->x + estate.x;
        }

        // Draw character and advance by 8 pixels
        GM_PutChar(label_text[ci], x, y, 0x0, 0xFF);
        x += 8;

    }
}

/* BUTTON FUNCTIONS */

// Constructor for the button at 0, 0
Button::Button(Window* w, char* str, void (*target)(Button*)) {
    
    // Initialize button contents
    button_text = str;
    estate.x = 0;
    estate.y = 0;
    estate.w = 50;
    estate.h = 20;

    // Link the parent window and the button, but also link the button with the action it will take
    parent = w;
    action = target;
    w->AddContent(this);
}

// Constructor for the button at a given set of coordinates
Button::Button(Window* w, char* str, unsigned short x, unsigned short y, void (*target)(Button*)) {
    
    // Initialize button contents
    button_text = str;
    estate.x = x;
    estate.y = y;
    estate.w = 50;
    estate.h = 20;

    // Link the parent window and the button, but also link the button with the action it will take
    parent = w;
    action = target;
    w->AddContent(this);
}

// Draw button onto the screen
void Button::Draw() {

    // Draw the button itself
    unsigned short x, y;
    for (x = parent->GetViewport()->x + estate.x; x < parent->GetViewport()->x + estate.x + estate.w; x++) {
        for (y = parent->GetViewport()->y + estate.y; y < parent->GetViewport()->y + estate.y + estate.h; y++) {
            if (x == parent->GetViewport()->x + estate.x || x == parent->GetViewport()->x + estate.x + estate.w - 1
                || y == parent->GetViewport()->y + estate.y || y == parent->GetViewport()->y + estate.y + estate.h - 1) 
                    GM_PutPixel(x, y, 0x0);
            else GM_PutPixel(x, y, 0xF);
        }
    }

    // Draw the label onto the button
    GM_PutStr(button_text, parent->GetViewport()->x + estate.x + 4, parent->GetViewport()->y + estate.y + (estate.h / 2 - 5), 0x0, 0xFF);
}

// Click event for the button
void Button::OnClick() { action(this); }
