; Boot sector actually starts at 0x7C00
org 0x7C00
_boot:

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

_printstr: ; Prints a string pointed to by the register BX

    ; Extract character and determine if null character
    mov al, [bx]
    cmp al, 0
    jne _printstr_print
    ret

_printstr_print: ; Part of _printstr, but it skips here to print the value if not zero
    ; Place character in video memory
    mov [es:di], byte al
    inc di

    ; Give character a color (foreground = cyan and background = black)
    mov [es:di], byte 0x3
    inc di

    ; Go to next character and repeat
    inc bx
    jmp _printstr


; Entry point
_starthere:

    ; Turn off the blinking cursor that appears by default
    mov dx, 0x3D4
    mov al, 0xA
    out dx, al
    inc dx
    mov al, 0x20
    out dx, al

    ; Print out the UI-286 Construction Message
    mov bx, ui286
    call _printstr

    ; Go to halting loop when done
    jmp _done

; Finishing point 
_done:

    ; Set all other characters in video memory to null
    mov [es:di], byte 0x0
    inc di

    ; Set both the foreground and background to be black
    mov [es:di], byte 0x0
    inc di

    ; Determine when end of video memory has been reached
    cmp di, 0xFA0 ; (80*25)*2 for two bytes per character
    je _halt
    jmp _done

; Halts with a loading indicator
_halt:

    ; Choose end of first line
    mov di, 158

    ; Set the foreground color to be red but keep background as black
    mov [es:di+1], byte 0x4

    ; Create loading symbol
    mov [es:di], byte '-'
    mov [es:di], byte '\'
    mov [es:di], byte '|'
    mov [es:di], byte '/'

    ; Repeat
    jmp _halt

; Data variables go here
_data:

    ; String for UI-286 Construction Message
    ui286   db  "UI-286 Construction In Progress...",0

    ; Fill up to bytes 511 and 512
    times 510-($-$$) db 0

    ; MBR Boot signature (0x55 and 0xAA)
    dw 0xAA55

    ; Fill up rest of the floppy
    times 1228288 db 0 ; 5.25" 1.2 MB Floppy
    ; times 1474048 db 0 ; 3.5" 1.44 MB Floppy

    ; Definitions go here
    VGA_MEMORY_TEXT equ 0xB800