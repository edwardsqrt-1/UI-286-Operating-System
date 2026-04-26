#include <panel.h>

/* PANEL WIDGET FUNCTIONS */

// Set the fill color of the panel widget
void PanelWidget::SetColor(unsigned char c) {
    fill_color = c;
}

// Set the border of the panel widget
void PanelWidget::SetBorder(unsigned char c) {
    border_color = c;
}

// Draw the panel widget
void PanelWidget::Draw() {
    GM_PutRect(estate.x, estate.y, estate.w, estate.h, fill_color, border_color);   // Draw the panel widget on the screen
    if (icon != 0) {
        // Future icon rendering code
    }
}

/* PANEL ACTION BUTTON FUNCTIONS */

// Construct the panel action widget
PanelActionWidget::PanelActionWidget(unsigned short start, unsigned short len, unsigned short panel_height, void (*target)()) {
    
    // Set dimensions
    estate.x = start;
    estate.y = 480 - panel_height;
    estate.w = len;
    estate.h = panel_height;

    // Set colors
    fill_color = 0x1;
    border_color = 0xF;

    execute = target;

}

// On click, execute the embedded function
void PanelActionWidget::MouseDown() {
    aux_fill_color = fill_color;
    fill_color = 0xF;
    Draw();
}

// On click, execute the embedded function
void PanelActionWidget::OnClick() {
    fill_color = aux_fill_color;
    Draw();
    execute();
}

/* PANEL CLOCK FUNCTIONS */

// Construct the panel action widget
PanelClockWidget::PanelClockWidget(unsigned short start, unsigned short len, unsigned short panel_height) {
    
    // Set dimensions
    estate.x = start;
    estate.y = 480 - panel_height;
    estate.w = len;
    estate.h = panel_height;

    // Set colors
    fill_color = 0x1;
    border_color = 0xF;

    // Set initial function to null
    GetTime(&val);

}

// Update the clock and draw the new time
void PanelClockWidget::UpdateTime() {

    // Calculate the midpoint of the height of the panel
    unsigned short y = estate.y + estate.h / 2 - 5;
    unsigned short x = estate.x + estate.w / 2 - 20;

    // Store clock information at the memory address 0x10000
    GetTime(&val);

    // Print hour
    if (val.hour < 10) {
        GM_PutUInt(0, x, y, 0xF, fill_color);
        GM_PutUInt(val.hour, x+8, y, 0xF, fill_color);
    } else GM_PutUInt(val.hour, x, y, 0xF, fill_color);

    // Print the colon seperating hour and minute
    GM_PutChar(':', x+16, y, 0xF, fill_color);

    // Print the minute
    if (val.minute < 10) {
        GM_PutUInt(0, x+24, y, 0xF, fill_color);
        GM_PutUInt(val.minute, x+32, y, 0xF, fill_color);
    } else GM_PutUInt(val.minute, x+24, y, 0xF, fill_color);

}

// Do nothing when mouse is pressed on a panel clock widget
void PanelClockWidget::MouseDown() {

}

// Handle a click on the panel clock widget
void PanelClockWidget::OnClick() {
    fill_color = (fill_color + 1) & 0xF;
    Draw();
    UpdateTime();
}

/* PANEL FUNCTIONS */

// Initialize panel at a height of 32 pixels, teal as the main fill color, cyan as the auxillary, white border, and "GUI286 Panel" as default label
Panel::Panel() {
    panel_height = 32;
    fill_color = 0xB;
    border = 0xF;
    widget_count = 0;
}

// Initialize panel at a custom height; other values do not change
Panel::Panel(unsigned short height) {
    panel_height = height;
    fill_color = 0xB;
    border = 0xF;
    widget_count = 0;
}

// Return the height of the panel
unsigned short Panel::Height() {
    return panel_height;
}

// Add a widget to the panel
void Panel::AddWidget(PanelWidget* w) {
 
    // Abort if maximum widget count exists
    if (widget_count >= MAX_WIDGETS) return;

    // Otherwise continue adding widget
    widgets[widget_count] = w;
    widget_count++;

}

// Display the panel onto the screen
void Panel::Draw() {

    // Calculate the midpoint of the height of the panel
    unsigned short mid = 480 - (panel_height / 2 + 4);

    // Display panel
    GM_PutRect(0, 480-panel_height, 640, panel_height, fill_color, border);

    // Print Panel Widgets
    for (unsigned int i = 0; i < widget_count; i++) widgets[i]->Draw();

}
