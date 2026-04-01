#include <mouse.h>

// Helper function to wait for the chance to write
void PS2_WaitForWrite() {

    __asm {
        wait_write:
            in al, 0x64
            test al, 0x2 // Check if second bit is zero (data does not already exist)
            jnz wait_write
    } 
    
}

// Helper function to wait for the chance to write
void PS2_WaitForRead() {

    __asm {
        wait_read:
            in al, 0x64
            test al, 0x1 // Check if first bit is one (data exists)
            jz wait_read
    } 
    
}

// Helper function to wait for an acknowledgement signal
unsigned char PS2_WaitForACK() {

    unsigned char val;

    // Get the chance to read, then get ACK or NAK
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        mov val, al
    }
    return val;
    
}

// Function to send a mouse command
unsigned char PS2_SendMouseCommand(unsigned char command) {

    char res;

    // Wait for the chance to write
    PS2_WaitForWrite();
    do { 
        __asm {

            // Specify mouse as destination for command
            mov al, 0xD4
            out 0x64, al

            // Send command
            mov al, command
            out 0x60, al
            
        }

        // Ensure the command is sent properly
        res = PS2_WaitForACK();
    } while (res == 0xFE); // Resend if the result is the resend packet (0xFE)

    // Return the value that was taken
    return res;

}

char PS2_MouseInit() {

    unsigned char res;

    // Enable the mouse on the system
    PS2_WaitForWrite();
    __asm {
        mov al, 0x20
        out 0x64, al
    }
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        //or al, 0x02
        and al, 0xDF
        push ax
    }
    PS2_WaitForWrite();
    __asm {
        mov al, 0x60
        out 0x64, al
        pop ax
        out 0x60, al
    }

    // Open the PS/2 port for the mouse
    __asm {
        mov al, 0xAE
        out 0x64, al
        mov al, 0xA8
        out 0x64, al
    }

    // Set defaults and change to "remote" mode (manually read data)
    res = PS2_SendMouseCommand(0xFF);
    res = PS2_SendMouseCommand(0xF6);
    if (res != 0xFA) return -1;
    res = PS2_SendMouseCommand(0xF0);
    if (res != 0xFA) return -2;

    // Assume basic mouse type (2 button mouse)
    return 2;

}

// Poll mouse information
char PS2_MousePoll(struct MouseInfo __far* mouse) {

    short dx, dy;
    unsigned char res;
    unsigned char bytes[5];

    // Send command for mouse packet
    res = PS2_SendMouseCommand(0xEB);
    PS2_WaitForACK();

    // Get bytes for mouse packet
    for (char i = 0; i < 3; i++) {

        // Get the current byte of the mouse packet
        PS2_WaitForRead();
        __asm {
            in al, 0x60
            mov res, al
        }
        bytes[i] = res;

    }

    // Get the values that the cursor has moved in the X and Y directions
    dx = bytes[1];
    dx |= (bytes[0] & 0x40) << 1;
    dx |= ((bytes[0] & 0x10) >> 4) << 15;
    dy = bytes[2];
    dy |= (bytes[0] & 0x80);
    dy |= ((bytes[0] & 0x20) >> 5) << 15;


    // Update X coordinate of mouse
    if (mouse->x + dx < 0) mouse->x = 0;
    else if (mouse->x + dx > 640) mouse->x = 640;
    else mouse->x += dx;

    // Update Y coordinate of mouse
    if (mouse->y + dy < 0) mouse->y = 0;
    if (mouse->y + dy > 480) mouse->y = 480;
    else mouse->y += dy;

    // Successful polling
    return 0;
}
