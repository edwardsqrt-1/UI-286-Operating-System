#ifndef PANEL_H
#define PANEL_H
#include <graphicsmode.h>
#include <time.h>
#include <widget.h>
#define MAX_WIDGETS 10

class PanelWidget : public Widget {

    protected:
        char* icon; // Panel icon used

    public:

        PanelWidget(unsigned short start, unsigned short len, unsigned short panel_height);    // Initialize panel widget
        void SetColor(unsigned char c);     // Set widget background color
        void SetBorder(unsigned char c);    // Set border color of widget
        void Draw() {
            GM_PutRect(estate.x, estate.y, estate.w, estate.h, fill_color, border_color);   // Draw the panel widget on the screen
        }

};

// Declaring a class for the Panel
class Panel {

    private:
        unsigned short panel_height;        // Height of the panel in pixels
        unsigned char fill_color;           // Main fill color of the panel
        unsigned char border;               // Border color of the panel
        unsigned char widget_count;         // Count of the number of widgets in the panel
        PanelWidget* widgets[MAX_WIDGETS];  // List of widgets in the panel

    public:

        Panel();    // Default panel initializer
        Panel(unsigned short height);   // Panel initializer with a custom height
        Panel(unsigned char fill_color, unsigned char border_color);    // Panel initializer with custom colors
        Panel(unsigned short height, unsigned char fill_color, unsigned char border_color); // Panel initializer with custom parameters

        unsigned short Height();    // Get panel height                                
        void AddWidget(PanelWidget* w); // Add widget to panel
        void Draw();        // Displays panel on screen

};

#endif
