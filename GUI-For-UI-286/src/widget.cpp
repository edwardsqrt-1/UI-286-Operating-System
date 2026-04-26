#include <widget.h>

// Default desktop icon (a piece of paper)
unsigned char default_desktop_icon[64] = {

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
    0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0xFF,
    0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00,
    0x00, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x0F, 0x00,
    0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00,
    0x00, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x0F, 0x00,
    0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

// Get the rectangle corresponding to the widget area occupied on the screen
Rectangle* Widget::GetEstate() { return &estate; }

// Initialize a desktop widget with a given name and coordinates. Also use the default icon
DesktopWidget::DesktopWidget(char* name, unsigned short x, unsigned short y) {

    unsigned char i;

    // Set widget parameters
    fill_color = 0x1;
    aux_fill_color = 0x3;
    border_color = 0xF;
    estate.x = x;
    estate.y = y;
    estate.w = 64;
    estate.h = 80;
    shortcut_name = name;

    // Set the default icon as the desktop default icon
    icon = default_desktop_icon;

}   

// Change icon of a desktop widget
void DesktopWidget::SetIcon(unsigned char* new_icon) { icon = new_icon; }

// Reset icon to the default on a desktop widget
void DesktopWidget::ResetIcon() { icon = default_desktop_icon; }

// Draws the desktop widget onto the screen
void DesktopWidget::Draw() {

    // Helper variables
    unsigned short x, y;
    unsigned char i = 0;

    // Determine where the borders, label, and icon are, and draw the frame for each element
    for (y = estate.y; y < estate.y + estate.h; y++) {
        for (x = estate.x; x < estate.x + estate.w; x++) {
            if (x == estate.x || y == estate.y || x == estate.x + estate.w - 1 || y == estate.y + estate.h - 1 || y == estate.h - 11) GM_PutPixel(x, y, border_color);
            else if (y > estate.h - 11) GM_PutPixel(x, y, aux_fill_color);
            else GM_PutPixel(x, y, fill_color);
        }
    }

    // Add label text to the shortcut
    GM_PutStr(shortcut_name, estate.x + 1, estate.y + estate.h - 12, 0xF, 0xFF);

    // Perform the drawing of the default icon if the icon was never customized
    if (icon == default_desktop_icon) {
        for (y = estate.y + 28; y < estate.y + 44; y += 2) {
            for (x = estate.x + 24; x < estate.x + 40; x += 2) {

                // Ignore if value in the bitmap is 0xFF (code for transparency)
                if (icon[i] == 0xFF) {
                    ++i;
                    continue;
                }

                // Set the current, the right adjacent, the bottom adjacent, and bottom-right adjacent pixels as the value in the bitmap
                GM_PutPixel(x, y, icon[i]);
                if (y != estate.y + 43) GM_PutPixel(x, y+1, icon[i]);
                if (x != estate.x + 39) GM_PutPixel(x+1, y, icon[i]);
                if (x != estate.x + 39 && y != estate.y + 43) GM_PutPixel(x+1, y+1, icon[i]);
                ++i;

            }
        }
    }

}   

// Click event for a desktop widget
void DesktopWidget::MouseDown() {

    // Cycle through a color as an example
    fill_color = 0xF;

    // Update widget
    Draw();

}

// Click event for a desktop widget
void DesktopWidget::OnClick() {

    // Cycle through a color as an example
    fill_color = 0x1;

    // Update widget
    Draw();

}
