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

// Construct the panel widget
PanelWidget::PanelWidget(unsigned short start, unsigned short len, unsigned short panel_height) {
    
    // Set dimensions
    estate.x = start;
    estate.y = 480 - panel_height;
    estate.w = len;
    estate.h = panel_height;

    // Set colors
    fill_color = 0x1;
    border_color = 0xF;

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

// Update the clock on the panel. In the future, this will be a separate widget
/*void Panel::UpdateClock() {

    // Calculate the midpoint of the height of the panel
    unsigned short mid = 480 - (panel_height / 2 + 4);

    // Store clock information at the memory address 0x10000
    struct rtc_time __far* clock = (struct rtc_time __far *) 0x10000000L;
    GetTime(clock);

    // Print hour
    if (clock->hour < 10) {
        GM_PutUInt(0, 584, mid, 0xF, aux_fill_color);
        GM_PutUInt(clock->hour, 592, mid, 0xF, aux_fill_color);
    } else GM_PutUInt(clock->hour, 584, mid, 0xF, aux_fill_color);

    // Print the colon seperating hour and minute
    GM_PutChar(':', 600, mid, 0xF, aux_fill_color);

    // Print the minute
    if (clock->minute < 10) {
        GM_PutUInt(0, 608, mid, 0xF, aux_fill_color);
        GM_PutUInt(clock->minute, 616, mid, 0xF, aux_fill_color);
    } else GM_PutUInt(clock->minute, 608, mid, 0xF, aux_fill_color);
}*/