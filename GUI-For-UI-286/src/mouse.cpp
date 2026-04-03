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

// Helper function to receive a byte
unsigned char PS2_GetKeyboardByte() {

    // Get the chance to read, then get ACK or NAK
    unsigned char val;
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        mov val, al
    }
    return val;
    
}

// Helper function to receive a byte
unsigned char PS2_GetMouseByte() {

    // Get the chance to read, then get ACK or NAK
    unsigned char val;
    __asm {
        wait_mouse_read:
            in al, 0x64
            test al, 0x01
            jz wait_mouse_read

            test al, 0x20
            jnz done
            in al, 0x60
            jmp wait_mouse_read

        done:
            in al, 0x60
            mov val, al
    }

    // Return the retrieved value
    return val;
    
}

// Helper function to receive a byte (Keyboard)
void PS2_SendKeyboardByte(unsigned char val) {

    // Get the chance to write, and then send command
    PS2_WaitForWrite();
    __asm {
        mov al, val
        out 0x64, al
    }
    
}

// Helper function to receive a byte (Mouse)
void PS2_SendMouseByte(unsigned char val) {

    // Tell keyboard to access the mouse
    PS2_SendKeyboardByte(0xD4);

    // Get the chance to write, then send byte to mouse
    PS2_WaitForWrite();
    __asm {
        in al, 0x60
        mov val, al
    }
    
}

// Initialize the mouse for use with the GUI
char PS2_MouseInit() {

    // Helper result variable
    unsigned char res;

    // Get the Compaq Status Byte
    PS2_WaitForWrite();
    __asm {
        mov al, 0x20
        out 0x64, al
    }

    // Make bit 5 0 (1 = disables mouse, 0 = enables mouse) and save new status byte
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        and al, 0xDF
        mov res, al
    }

    // Tell the PS/2 controller to add the new status byte
    PS2_WaitForWrite();
    __asm {
        mov al, 0x60
        out 0x64, al
    }

    // Send the new status over
    PS2_WaitForWrite();
    __asm {
        mov al, res
        out 0x60, al
    }

    // Tell the keyboard to send the following command to the mouse
    PS2_WaitForWrite();
    __asm {
        mov al, 0xD4
        out 0x64, al
    }

    // Perform a reset on the mouse
    PS2_WaitForWrite();
    __asm {
        mov al, 0xFF
        out 0x60, al
    }
    
    // Get acknowledgment byte
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        mov res, al
    }
    if (res != 0xFA) return -1;

    // Get the successful mouse reboot byte (0xAA)
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        mov res, al
    }
    if (res != 0xAA) return -1;

    // Store and return the mouse ID
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        mov res, al
    }
    return res;

}

// Poll mouse information
char PS2_MousePoll(struct MouseInfo __far* mouse) {

    // Helper variables
    short dx, dy;
    unsigned char res;
    unsigned char bytes[5];

    // Inform keyboard to send the following command to the mouse
    PS2_WaitForWrite();
    __asm {
        mov al, 0xD4
        out 0x64, al
    }

    // Write command to print out the movement data
    PS2_WaitForWrite();
    __asm {
        mov al, 0xEB
        out 0x60, al
    }
    
    // Get the acknowledgment byte and exit if failed
    PS2_WaitForRead();
    __asm {
        in al, 0x60
        mov res, al
    }
    if (res != 0xFA) return -1;

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

    // See the button responses and put them in the mouse
    if ((bytes[0] & 0x1)) mouse->left_clicked = 1;
    else mouse->left_clicked = 0;
    if ((bytes[0] & 0x2)) mouse->right_clicked = 1;
    else mouse->right_clicked = 0;

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
