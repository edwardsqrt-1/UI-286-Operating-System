#include <time.h>

// Defining the function to delay by a given amount of milliseconds
void delay(unsigned long u_sec) {
    
    // Split into higher 16 bits and lower 16 bits
    unsigned short h_u_sec = u_sec >> 16;
    unsigned short l_u_sec = u_sec & 0xFFFF;
    
    // Assign portions into respective registers (cx = highest 16 bits, dx = 16 lowest bits) and do a BIOS interrupt
    __asm {
        mov cx, h_u_sec
        mov dx, l_u_sec
        mov ah, 0x86
        int 0x15
    }
}
