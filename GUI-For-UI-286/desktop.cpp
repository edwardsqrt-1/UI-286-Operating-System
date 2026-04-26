/*
 * Program: GUI for the UI(286) Operating System
 * Author: Edward Bierens
 * Date Created: Sunday April 12th, 2026
 * 
 * Description: A Command Line Interface is not enough for the modern day; as such UI(286) has an
 * optional Graphical User Interface Shell that it can run. For now, this displays a demo screen with a character 
 * map for 10 seconds, then exits. 
 * 
 */

#include <gmodes.h>
#include <graphicsmode.h>
#include <time.h>
#include <panel.h>
#include <string.h>
#include <mouse.h>
#include <window.h>
#include <widget.h>

// Externally define the cursor and background maps
extern unsigned char default_cursor[80];
extern unsigned char left_click_cursor[80];
extern unsigned char right_click_cursor[80];
extern unsigned char pattern[64];

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

// Flag to determine whether the GUI is running
char GUIRunning = 1;

// Shutdown function
void Splash() {
    
    // Helper variables
    unsigned short x;
    unsigned short y;
    unsigned char mov;

    // Print text on boot screen
    GM_PutStr("GUI for UI(286)", 260, 235, 0xF, 0xFF);
    GM_PutStr("Now loading...", 264, 260, 0xB, 0xFF);

    // The loading bar (updates every 0.25 seconds)
    for (mov = 0; mov < 16; mov++) {
        for (x = 0; x < 640; x++) {
            for (y = 0; y < 15; y++) {
                GM_PutPixel(x, y, ((x / 40) + mov) & 0xF);
            }
        }
        Delay(150000);
    }
}

// Shutdown function
void ShutdownPC() {
    
    // Print shutdown screen
    GM_BlankScreen(0x0);
    GM_PutStr("UI(286) was closed by the user.", 1, 1, 0xF, 0x0);
    GM_PutStr("You may now shut your computer down safely.", 1, 12, 0xA, 0x0);
    
    // Do no operations
    while (1) __asm {
        nop
    }
}

// Exit GUI function
void ExitGUI() { GUIRunning = 0; }

// Exit Window function
void ExitWindow(Button* sender) {
    sender->GetParent()->Exit();
}

// A catalog of all the widgets in the UI(286) Operating System
Widget* widget_list[50];
unsigned int widget_list_size = 0;
void RegisterWidget(Widget* w) {
    widget_list[widget_list_size] = w;
    widget_list_size++;
}

// A catalog of all open windows in the UI(286) Operating System
Window* window_list[5];
unsigned int window_list_size = 0;
void RegisterWindow(Window* w) {
    window_list[window_list_size] = w;
    window_list_size++;
}

// Driver program for the GUI
void guiroot() { 

    // Initialize position and color indices to be 0
    unsigned int i = 0;
    unsigned int y = 324;
    unsigned int x = 280;
    char c, m, n;
    unsigned char color = 0;
    struct MouseInfo* mouse_state;
    unsigned short clicked_x = 0xFFFF;
    unsigned short clicked_y = 0xFFFF;
    unsigned char mouse_down = 0;
    Rectangle* win_area;

    // Initialize Panel
    Panel p(28);

    // Test widget 0 on the left
    PanelActionWidget label(0, 150, p.Height(), 0);
    RegisterWidget(&label);
    label.SetColor(0x7);
    p.AddWidget(&label);

    // Test widget 1 on the 3rd furthest from the right
    PanelActionWidget shutdown(440, 50, p.Height(), ShutdownPC);
    RegisterWidget(&shutdown);
    shutdown.SetColor(0x4);
    p.AddWidget(&shutdown);

    // Test widget 2 on the 2nd furthest from the right
    PanelActionWidget ret(490, 50, p.Height(), ExitGUI);
    RegisterWidget(&ret);
    ret.SetColor(0x5);
    p.AddWidget(&ret);

    // Test widget 3 on the right side of the panel
    PanelClockWidget panel_clock(540, 100, p.Height());
    RegisterWidget(&panel_clock);
    panel_clock.SetColor(0x3);
    p.AddWidget(&panel_clock);

    // Set the screen mode to be Mode 0x12 and point a buffer at memory
    SetGraphicsMode(MODE_640x480x16);

    // Start boot splash
    Splash();

    // Clear the desktop with blue
    GM_BlankScreen(0x1);

    // Draw the desktop background
    for (y = 0; y < 480 - p.Height(); y++) {
        for (x = 0; x < 640; x++) {
            GM_PutPixel(x, y, pattern[(y & 0x7) * 8 + (x & 0x7)]);
        }
    }

    // Draw panel and widgets
    p.Draw();

    // Initialize Welcome window
    Window hello_world;
    Label prompt(&hello_world, "Welcome to the GUI for the UI(286) Operating System! This is a work in progress, however I hope you find it interesting!", 5, 5);
    Button ok(&hello_world, "Cool!", 130, 60, ExitWindow);
    RegisterWindow(&hello_world);
    hello_world.Draw();

    // Initialize the mouse
    Cursor cur(320, 240, default_cursor);
    m = PS2_MouseInit();
    if (m >= 0) cur.SetType(m);

    // Test out desktop widgets
    DesktopWidget dw1("Test #1", 4, 4);
    RegisterWidget(&dw1);
    DesktopWidget dw2("Test #2", 78, 4);
    RegisterWidget(&dw2);
    DesktopWidget dw3("Test #3", 152, 4);
    RegisterWidget(&dw3);
    DesktopWidget dw4("Test #4", 226, 4);
    RegisterWidget(&dw4);

    // Draw said desktop widgets
    dw1.Draw();
    dw2.Draw();
    dw3.Draw();
    dw4.Draw();

    // Wait for the X key stroke, and then go back to the CLI
    c = 0;
    while (GUIRunning) {

        // Get character if one exists in the buffer and get time
        c = GetChar();
        if (c == 'x' || c == 'X') {
            GUIRunning = 0;
            break;
        }
        panel_clock.UpdateTime();

        // Poll the mouse
        mouse_state = cur.AccessState();
        m = PS2_MousePoll(mouse_state);

        // Move the cursor if a new poll was successfully retreived
        if (m != -1) {
            
            // Log the coordinates originally clicked
            clicked_x = mouse_state->x;
            clicked_y = mouse_state->y;

            // Address mouse not clicked (anymore)
            if (!mouse_state->left_clicked && !mouse_state->right_clicked) {

                // If the mouse is no longer pressed from a click, send an event
                if (mouse_down == 1) {
                    
                    // Tell the OS the mouse is no longer pressed
                    mouse_down = 0;

                    for (i = 0; i < window_list_size; i++) {
                        win_area = window_list[i]->GetEstate();
                        if (clicked_x >= win_area->x && clicked_x < win_area->x + win_area->w
                            && clicked_y >= win_area->y && clicked_y < win_area->y + win_area->h) {
                                window_list[i]->OnClick(clicked_x, clicked_y);
                                i = 0xFFFF;
                                break;
                            }
                    }

                    // Get widget dimensions and check if a widget was clicked unless a window was already clicked
                    if (i != 0xFFFF) for (i = 0; i < widget_list_size; i++) {
                        win_area = widget_list[i]->GetEstate();
                        if (clicked_x >= win_area->x && clicked_x < win_area->x + win_area->w
                            && clicked_y >= win_area->y && clicked_y < win_area->y + win_area->h) {
                                widget_list[i]->OnClick();
                                break;
                            }
                    }
                }

                // Change cursor icon to default
                cur.ChangeIcon(default_cursor);
                cur.PlaceCursor();
                

            } else { // The mouse has a button down

                // Change icon depending on what button was clicked
                if (mouse_state->left_clicked) cur.ChangeIcon(left_click_cursor);
                else if (mouse_state->right_clicked) cur.ChangeIcon(right_click_cursor);

                // Trigger a mouse down event if the mouse was just pressed
                if (mouse_down == 0) {
                    
                    // Tell the OS the mouse is pressed down
                    mouse_down = 1;

                    // Get window dimensions and check if anywhere in the window was pressed
                    for (i = 0; i < window_list_size; i++) {
                        win_area = window_list[i]->GetEstate();
                        if (clicked_x >= win_area->x && clicked_x < win_area->x + win_area->w
                            && clicked_y >= win_area->y && clicked_y < win_area->y + win_area->h) {
                                window_list[i]->MouseDown(clicked_x, clicked_y);
                                i = 0xFFFF;
                                break;
                            }
                    }

                    // Get widget dimensions and check if a widget was clicked unless a window was already clicked
                    if (i != 0xFFFF) for (i = 0; i < widget_list_size; i++) {
                        win_area = widget_list[i]->GetEstate();
                        if (clicked_x >= win_area->x && clicked_x < win_area->x + win_area->w
                            && clicked_y >= win_area->y && clicked_y < win_area->y + win_area->h) {
                                widget_list[i]->MouseDown();
                                break;
                            }
                    }
                    
                } else {

                    // Get window dimensions and check if anywhere in the window was pressed
                    for (i = 0; i < window_list_size; i++) {
                        win_area = window_list[i]->GetEstate();
                        if (clicked_x >= win_area->x && clicked_x < win_area->x + win_area->w
                            && clicked_y >= win_area->y && clicked_y < win_area->y + win_area->h) {
                                window_list[i]->MouseDown(clicked_x, clicked_y);
                                i = 0xFFFF;
                                break;
                            }
                    }

                }

            }
        }

        // Place cursor just in case 
        cur.PlaceCursor();
    }

    // Before exiting entirely, set the graphics mode back to text
    ResetGraphicsMode();

}

// Default mouse cursor icon for the GUI
unsigned char default_cursor[80] = {
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x0E, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x0E, 0x0E, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x0E, 0x0E, 0x0C, 0x04, 0x01, 0xFF, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x01, 0x01, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// Left-click mouse cursor icon for the GUI
unsigned char left_click_cursor[80] = {
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x03, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x03, 0x03, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x03, 0x03, 0x0C, 0x04, 0x01, 0xFF, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x01, 0x01, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// Right-click mouse cursor icon for the GUI
unsigned char right_click_cursor[80] = {
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x0A, 0x0A, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x0A, 0x0A, 0x0C, 0x04, 0x01, 0xFF, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x01,
        0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x01, 0xFF,
        0x00, 0x0C, 0x0C, 0x01, 0x01, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// Background pattern
unsigned char pattern[64] = {
    0x5,0x5,0x1,0x1,0x1,0x1,0x3,0x3,
    0x5,0x1,0x1,0x9,0x9,0x1,0x1,0x3,
    0x1,0x1,0x9,0x9,0x9,0x9,0x1,0x1,
    0x1,0x9,0x9,0x9,0x9,0x9,0x9,0x1,
    0x1,0x9,0x9,0x9,0x9,0x9,0x9,0x1,
    0x1,0x1,0x9,0x9,0x9,0x9,0x1,0x1,
    0x3,0x1,0x1,0x9,0x9,0x1,0x1,0x5,
    0x3,0x3,0x1,0x1,0x1,0x1,0x5,0x5
};