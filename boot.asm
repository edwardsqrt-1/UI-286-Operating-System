; Boot sector actually starts at 0x7C00
org 0x7C00

_boot: ; Initializing everything and set extra segment to VGA text mode address

    ; Set extra segment as the segment for VGA text mode 80x25 by default
    mov ax, VGA_MEMORY_TEXT
    mov es, ax

    ; Clear out all values
    xor ax,ax
    xor bx,bx
    xor cx,cx
    xor dx,dx
    xor di,di

    ; Go to Entry Point
    jmp _starthere

_blankscreen: ; Function to blank the screen with the color value in al

    xor di, di

    _blankscreen_clear:
        ; Set all other characters in video memory to null
        mov [es:di], byte 0x00
        inc di

        ; Set both the foreground and background to be black
        mov [es:di], al
        inc di

        ; Determine when end of video memory has been reached
        cmp di, 0xFA0 ; (80*25)*2 for two bytes per character
        jne _blankscreen_clear

    ret

_printstr: ; Prints a string where string = bx, x = cl, y = ch, and color = dh

    ; Convert y value into multiples of 80 (80 columns) and store into di
    ; Note that each character corresponds to 2 bytes; first for character and second for attribute
    mov ax, cx
    shr ax, 8
    mov dl, 80
    mul dl
    shl ax, 1
    mov di, ax 

    ; Convert x value into the value to translate di by
    mov ax, cx
    xor ah, ah
    shl ax, 1
    add di, ax

    ; Set background color to blue
    or dh, 0x10

    ; Begin loop
    _printstr_char:

        ; Extract character and determine if null character
        mov al, [bx]
        cmp al, 0
        jne _printstr_print
        ret

        _printstr_print: ; Part of _printstr, but it skips here to print the value if not zero

            ; Place character in video memory
            mov [es:di], al
            inc di

            ; Give character a color (foreground = yellow and background = blue)
            mov [es:di], dh
            inc di

            ; Go to next character and repeat
            inc bx
            jmp _printstr_char


_starthere: ; Entry point

    ; Turn off the blinking cursor that appears by default
    mov dx, 0x3D4
    mov al, 0xA
    out dx, al
    inc dx
    mov al, 0x20
    out dx, al

    ; Make background blue
    mov al, 0x10
    call _blankscreen
    xor di, di

    ; Print out the UI-286 Construction Message
    mov ch, 12
    mov cl, 23
    mov dh, 0xE
    mov bx, ui286
    call _printstr

    ; Print out secondary message
    mov ch, 14
    mov cl, 31
    mov dh, 0xA
    mov bx, wait_str
    call _printstr

    ; Go to halting loop when done
    mov di, 158
    jmp _halt


_halt: ; Halts with a loading indicator

    ; Set the foreground color to be cyan but keep background as blue
    mov [es:di+1], byte 0x13

    ; Create loading symbol
    mov [es:di], byte '-'
    mov [es:di], byte '\'
    mov [es:di], byte '|'
    mov [es:di], byte '/'

    ; Repeat until PC is powered off
    jmp _halt

; Data variables go here
_data:

    ; String for UI-286 Construction Message
    ui286   db  "UI-286 Construction In Progress...",0
    wait_str    db  "Please stay tuned!",0

    ; Fill up to bytes 511 and 512 starting from current line
    times 510-($-$$) db 0

    ; MBR Boot signature (0x55 and 0xAA)
    dw 0xAA55

    ; Fill up rest of the floppy
    times 1228288 db 0 ; 5.25" 1.2 MB Floppy
    ; times 1474048 db 0 ; 3.5" 1.44 MB Floppy

    ; Definitions go here
    VGA_MEMORY_TEXT equ 0xB800 ; Memory address for VGA Text Mode 80x25