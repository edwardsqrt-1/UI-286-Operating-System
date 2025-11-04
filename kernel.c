// Predeclare functions (needed to ensure that main286() always goes first)
void beep(short, unsigned long);

// Main kernel function (will just print off a C)
void main286() {

    // Pre-declare variables
    short i;
    
    // Beep for a specified amount of times while increasing the frequency
    for (i = 0; i < 5; i++) {
        beep(1046+i*100, 1000000);
        __asm {
            mov cx, 0x3
            mov dx, 0xD090
            mov ah, 0x86
            int 0x15
        }
    }

    // Print the letter C on the top left
    __asm {
        mov ax, 0xB800
        mov es, ax
        xor di, di 
        mov [es:di], 'C'
        mov [es:di+1], 0x1F
    }
    
    // Halt here
    while (1) {
        __asm {
            nop
            nop
        }
    }

}

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
