#ifndef GRAPHICS_MODES_H
#define GRAPHICS_MODES_H

// Defined graphics mode
#define MODE_80x25x16 0x3
#define MODE_640x480x16 0x12
#define ADDR_VGA_GRAPHICS (char*) 0xA0000000L
#define MODE_320x200x256 0x13

// Function to set to a particular graphics mode
void SetGraphicsMode(unsigned char mode);

// Resets to the default text mode
void ResetGraphicsMode();

#endif
