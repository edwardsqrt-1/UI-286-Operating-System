#include "time.h"

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

// Getting the time directly from the Real Time Clock on the PC
void gettime(struct rtc_time* time_struct) {

    // Get a temporary character value
    unsigned char temp = 0x80;
    unsigned char isBinary = 0;
    unsigned char is24Hour = 0;


    // Wait for the RTC to be ready to read from (seek Update In Progress bit in Status Register A at CMOS register 10 (0xA))
    while (temp != 0x0) {
        __asm {
            mov al, 0x0A
            out 0x70, al
            nop
            nop
            nop
            nop
            nop
            in al, 0x71
            and al, 0x80
            mov temp, al
        }
    }

    // Look at the binary and 24 hour flags (Status Register B at CMOS register 11 (0xB))
    __asm {
        mov al, 0x0B
        out 0x70, al
        nop
        nop
        nop
        nop
        nop
        in al, 0x71
        mov temp, al
    }
    isBinary = temp & 0x4;
    is24Hour = temp & 0x2;

    // Retreive seconds value from RTC and convert to binary integer (located at CMOS register 0)
    __asm {
        xor al, al
        out 0x70, al
        nop
        nop
        nop
        nop
        nop
        in al, 0x71
        mov temp, al
    }
    if (isBinary) time_struct->second = temp;
    else time_struct->second = ((temp & 0xF0) >> 4) * 10 + (temp & 0xF);

    // Repeat but with the minute value (located in CMOS register 2)
    __asm {
        mov al, 2
        out 0x70, al
        nop
        nop
        nop
        nop
        nop
        in al, 0x71
        mov temp, al
    }
    if (isBinary) time_struct->minute = temp;
    else time_struct->minute = ((temp & 0xF0) >> 4) * 10 + (temp & 0xF);

    // Repeat but with the hour value (located in CMOS register 4)
    __asm {
        mov al, 4
        out 0x70, al
        nop
        nop
        nop
        nop
        nop
        in al, 0x71
        mov temp, al
    }
    if (isBinary) time_struct->hour = temp & 0x1F;
    time_struct->hour = ((temp & 0xF0) >> 4) * 10 + (temp & 0xF);

    // Repeat but with the day of month value (located in CMOS register 7)
    __asm {
        mov al, 7
        out 0x70, al
        nop
        nop
        nop
        nop
        nop
        in al, 0x71
        mov temp, al
    }
    if (isBinary) time_struct->day = temp & 0x1F;
    time_struct->day = ((temp & 0xF0) >> 4) * 10 + (temp & 0xF);

    // Repeat but with the month value (located in CMOS register 8)
    __asm {
        mov al, 8
        out 0x70, al
        nop
        nop
        nop
        nop
        nop
        in al, 0x71
        mov temp, al
    }
    if (isBinary) time_struct->month = temp & 0x1F;
    time_struct->month = ((temp & 0xF0) >> 4) * 10 + (temp & 0xF);

}
