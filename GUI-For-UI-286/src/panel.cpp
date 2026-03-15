#include <panel.h>

// Initialize panel at a height of 32 pixels, teal as the main fill color, cyan as the auxillary, white border, and "GUI286 Panel" as default label
Panel::Panel() {
    panel_height = 32;
    fill_color = 0xB;
    aux_fill_color = 0x3;
    border = 0xF;
    label = "GUI286 Panel";
}

// Initialize panel at a custom height; other values do not change
Panel::Panel(unsigned char height) {
    panel_height = height;
    fill_color = 0xB;
    aux_fill_color = 0x3;
    border = 0xF;
    label = "GUI286 Panel";
}

// Display the panel onto the screen
void Panel::Draw() {

    // Calculate the midpoint of the height of the panel
    unsigned short mid = 480 - (panel_height / 2 + 4);

    // Display panel
    GM_PutRect(0, 480-panel_height, 640, panel_height, fill_color, border);
    GM_PutStr(label, 8, mid, 0x0, 255);
    GM_PutRect(560, 480-panel_height, 80, panel_height, aux_fill_color, border);

    // Call the update clock function
    Panel::UpdateClock();


}

// Update the clock on the panel. In the future, this will be a separate widget
void Panel::UpdateClock() {

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
}