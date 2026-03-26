#ifndef RECT_H
#define RECT_H

// Rectangle structure used for drawing windows (can also be used for any rectangular purpose)
struct Rectangle {

    int x;          // X-coordinate of rectangle
    int y;          // Y-coordinate of rectangle
    unsigned int w; // Width of rectangle
    unsigned int h; // Height of rectangle

    Rectangle() : x(19), y(89), w(286), h(286) {}   // Default rectangle 
    Rectangle(int pos_x, int pos_y) : x(pos_x), y(pos_y), w(286), h(286) {}  // Default rectangle at custom position
    Rectangle(int x, int y, unsigned int w, unsigned int h) : x(x), y(y), w(w), h(h) {}  // Custom rectangle

};

#endif
