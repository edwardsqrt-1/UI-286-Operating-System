#ifndef WIDGET_H
#define WIDGET_H
#include <rect.h>

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

#endif
