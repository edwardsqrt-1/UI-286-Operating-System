#ifndef WIDGET_H
#define WIDGET_H
#include <rect.h>
#include <graphicsmode.h>

extern unsigned char default_desktop_icon[64];

// Declaring a class for a Widget
class Widget {

    protected:
        unsigned char fill_color;       // Main fill color of the panel
        unsigned char aux_fill_color;   // Auxillary fill color of the panel (the gradient color; currently unused)
        unsigned char border_color;     // Border color of the panel (255 = no border)
        Rectangle estate;               // Rectangle that the widget will take on the screen

    public:
    
        virtual void Draw() = 0;        // Template for drawing the widget

};


// Desktop icon class based on Widget
class DesktopWidget : public Widget {

    private:
        char* shortcut_name;    // Name to display (must be at most 8 characters)
        unsigned char* icon;    // Reference to the icon to use

    public:
        DesktopWidget(char* name, unsigned short x, unsigned short y);  // Constructor for the desktop widget
        void SetIcon(unsigned char* new_icon);              // Set icon
        void ResetIcon();                                   // Reset icon to default
        void Draw();                                        // Draw widget onto screen
};

#endif
