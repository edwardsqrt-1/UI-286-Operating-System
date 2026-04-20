#ifndef WINDOW_H
#define WINDOW_H

#include <graphicsmode.h>
#include <rect.h>

// Forward declaration of WindowContent
class WindowContent;

// Class to describe a window
class Window {

    protected:
        Rectangle estate;                   // Rectangle that describes the window view
        Rectangle view;                     // View of the window
        char* title;                        // Window title
        unsigned char obscuring;            // Pixels behind the window are stored here
        unsigned char window_color;         // Main color of the window
        unsigned char outer_border_color;   // Color of the border lines
        unsigned char inner_border_color;   // Color of the main border within the lines
        unsigned char title_color;          // Color of the title bar
        unsigned char num_contents;         // Current number of contents in the window
        WindowContent* contents[20];        // Contents of the window

    public:
        Window();                   // Window constructor
        void Draw();                // Function to draw a simple window onto the screen
        Rectangle* GetEstate();     // Get the rectangular area the window occupies
        Rectangle* GetViewport();   // Get the viewport of the window
        void OnClick(unsigned short x, unsigned short y);   // Handle a click at a location
        void AddContent(WindowContent* c);                  // Add a widget as content of a window
        void Exit();                // Function to close out of the window
        
};

// Basic window content class to generalize all contents on the window
class WindowContent {
    protected:
        Rectangle estate;   // Area that the content will take up on the window
        Window* parent;     // Parent Window
    public:
        virtual void Draw() = 0;    // Prototype for drawing a window content onto a window
};

// Class to describe a label on the window
class Label : public WindowContent {
    
    private: 
        char* label_text;   // Label text

    public:
        Label(Window* w, char* str);                                       // Constructor with default position (0,0)
        Label(Window* w, char* str, unsigned short x, unsigned short y);   // Constructor with custom position
        void Draw();        // Draw label onto the window
        
};

// Class to describe a label on the window
class Button : public WindowContent {
    
    private: 
        char* button_text;  // Button text

    public:
        Button(Window* w, char* str, void (*target)(Button*));                                       // Constructor with default position (0, 0)
        Button(Window* w, char* str, unsigned short x, unsigned short y, void (*target)(Button*));   // Constructor with custom position
        void (*action)(Button*);    // The function that the action will call
        void OnClick();             // Execute the selected action
        void Draw();                // Draw button onto the window
};

#endif
