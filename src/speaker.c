#include "speaker.h"

// Beep noise
void beep(short freq, unsigned long delay) {

    // Split the least significant 32-bits into high part of the delay and low part respectively
    short h_delay = delay >> 16;
    short l_delay = (short) delay & 0xFFFF;

    // Duplicate of the bootloader code; explained a little but see the bootloader code for more information
    __asm {

        // Create modified frequency value
        mov ax, 0x34DC
        mov dx, 0x12
        mov bx, freq
        div bx

        // Initialize PIT and Send frequency value
        mov cx, ax
        mov al, 0xB6
        out 0x43, al
        mov ax, cx
        out 0x42, al
        mov al, ah
        out 0x42, al

        // Turn on sound
        in  al, 0x61
        or  al, 0x3
        out 0x61, al

        // Delay
        mov cx, h_delay
        mov dx, l_delay
        mov ah, 0x86
        int 0x15

        // Turn off sound
        in al, 0x61
        and al, 0xFC
        out 0x61, al
    }

    // Exit beep function
    return;

}
