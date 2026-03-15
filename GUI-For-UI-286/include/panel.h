#ifndef PANEL_H
#define PANEL_H
#include <graphicsmode.h>
#include <time.h>
#define MAX_WIDGETS 10

// Declaring a class for the Panel
class Panel {
    private:
        unsigned short panel_height;    // Height of the panel in pixels
        unsigned char fill_color;       // Main fill color of the panel
        unsigned char aux_fill_color;   // Auxillary fill color of the panel
        unsigned char border;           // Border color of the panel
        char* label;                    // Name of the label to print on the panel

    public:
        Panel();                        // Default panel initializer
        Panel(unsigned char height);    // Panel initializer with a custom height
        void Draw();                    // Displays panel on screen
        void UpdateClock();             // Updates the clock (intended to be a standalone widget)
};

#endif
