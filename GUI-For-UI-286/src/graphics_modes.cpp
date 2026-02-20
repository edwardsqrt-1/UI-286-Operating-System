#include <graphics_modes.h>

// Sets a particular graphics mode on the screen
void SetGraphicsMode(unsigned char mode) {

    __asm {
        xor ah, ah
        mov al, mode
        int 0x10
    }

}

// Sets a particular graphics mode on the screen
void ResetGraphicsMode() {

    __asm {
        xor ah, ah
        mov al, MODE_80x25x16
        int 0x10
    }
    
}
