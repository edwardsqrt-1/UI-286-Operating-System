#ifndef WINDOW_H
#define WINDOW_H

#include <graphicsmode.h>
#include <rect.h>

// Class to describe a window
class Window {

    protected:
        Rectangle estate;                   // Rectangle that describes the window view
        char* title;                        // Window title
        unsigned char obscuring;            // Pixels behind the window are stored here
        unsigned char window_color;         // Main color of the window
        unsigned char outer_border_color;   // Color of the border lines
        unsigned char inner_border_color;   // Color of the main border within the lines
        unsigned char title_color;          // Color of the title bar

    public:
        Window();       // Window constructor
        void Draw();    // Function to draw a window
        Rectangle* GetEstate(); // Get the rectangular area the window occupies
        void OnClick(unsigned short x, unsigned short y);   // Handle a click at a location
        
};

// Class to describe a label on the window
class Label {
    
    private: 
        char* label_text;       // Label text
        unsigned short x_pos;   // X coordinate based on window
        unsigned short y_pos;   // Y coordinate based on window

    public:
        Label(char* str);                                       // Constructor with default position
        Label(char* str, unsigned short x, unsigned short y);   // Constructor with custom position
        
};

#endif
