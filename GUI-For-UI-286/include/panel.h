#ifndef PANEL_H
#define PANEL_H
#include <graphicsmode.h>
#include <time.h>
#include <widget.h>
#define MAX_WIDGETS 10

// Base class for a panel widget
class PanelWidget : public Widget {

    protected:
        char* icon; // Panel icon used

    public:

        void SetColor(unsigned char c);     // Set widget background color
        void SetBorder(unsigned char c);    // Set border color of widget
        void Draw();                        // Draw widget on the screen
};

// Panel clock widget that tells you the time
class PanelClockWidget : public PanelWidget {

    private:
        struct rtc_time val;    // Time structure

    public:

        PanelClockWidget(unsigned short start, unsigned short len, unsigned short panel_height); // Construct the clock widget on the panel
        void UpdateTime();  // Update time value and print it on screen
        void MouseDown();   // Handle a mouse down event
        void OnClick();     // Handle a click to a button
    
};

// Panel Widget that is meant to perform an action when clicked
class PanelActionWidget : public PanelWidget {

    public:
        PanelActionWidget(unsigned short start, unsigned short len, unsigned short panel_height, void (*target)()); // Construct the action widget for the panel
        void (*execute)(); // The actual pointer to said function
        void MouseDown();  // Handle a mouse down event
        void OnClick();    // Handle a click to a button

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
