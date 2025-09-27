; Boot sector actually starts at 0x7C00
org 0x7C00

; Initializing everything and set extra segment to VGA text mode address
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
    mov ds,dx

    ; Go to Entry Point
    jmp _starthere

; Function to blank the screen with the color value in al
_blankscreen: 

    ; Set the screen pointing value to the first pixel and begin
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

; Creates sound on PC Speaker with frequency at bx
_makesound:

    ; Extract corresponding value to send to the timer chip from the frequency; save in cx
    ; 1193180 / frequency
    mov ax, 0x34DC
    mov dx, 0x12
    div bx
    mov cx, ax

    ; Set the following options on the timer chip's register at 0x43:
    ; - Set bit 0 to 0 such that binary is used
    ; - Set bits 3, 2, and 1 to 011 such that the square wave generator is selected
    ; - Set bits 5 and 4 to 11 such that both bytes are used (the frequency takes 2 bytes)
    ; - Set bits 7 and 6 to 10 to select Channel 2
    mov al, 10110110b
    out 0x43, al
    
    ; Deploy frequency value to the timer chip's channel 2 port at 0x42 (little endian)
    mov ax, cx
    out 0x42, al
    mov al, ah
    out 0x42, al

    ; Set bits 0 and 1 of the Keyboard Controller to 1 such that sound can play
    in  al, 0x61
    or  al, 00000011b
    out 0x61, al

    ; Use the built-in BIOS interrupt to delay by the specified amount of time
    mov cx, si
    mov dx, di
    mov ah, 0x86
    int 0x15

    ; Set bits 0 and 1 of the Keyboard Controller back to 0 to turn off sound
    in al, 0x61
    and al, 11111100b
    out 0x61, al

    ; Exit
    ret
    
; Prints a character in al with a color in dh
_printchar:

    ; Place character in video memory
    mov [es:di], al
    inc di

    ; Give character a color (foreground = yellow and background = blue)
    mov [es:di], dh
    inc di

    ; Exit 
    ret

; Prints a number where number = bx, x = cl, y = ch, and color = dh
_printnum: 

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

    ; Add blue background color
    or dh, 0x90

    ; Free up dx register to hold the remainder (it contains the higher digits of integer), and move number to ax. Set bx to 10, and set cx to 0
    mov si, dx
    xor dx, dx
    mov ax, bx
    mov bx, 10
    xor cx, cx

    ; Begin the process of dividing and storing the remainder character values
    _printnum_div:

        ; Get the registers such that ax = result, dx = one-digit remainder
        div bx 
        xor dh, dh 

        ; Get the character value for said integer; store into stack
        add dl, '0' 
        push dx
        xor dl, dl

        ; Keep track of the number of divisions done (equls the amount of digits); finish when ax = 0
        inc cx 
        cmp ax, 0 
        jne _printnum_div


    ; Begin printing integers; restore color contents to dx
    mov dx, si
    _printnum_print:

        ; Get digit from stack
        pop ax

        ; Print character and decrement cx
        call _printchar
        dec cx

        ; If cx is not zero, go again
        jcxz _printnum_done
        jmp _printnum_print


    ; Exit the integer printing function
    _printnum_done:
        ret

; Prints a string where string = bx, x = cl, y = ch, and color = dh
_printstr:

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

        ; Should there be a non-null character, print it to the screen
        _printstr_print: 

            ; Print character
            call _printchar

            ; Go to next character and repeat
            inc bx
            jmp _printstr_char


; Boot Process
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

    ; Print out upper border
    mov ch, 8
    mov cl, 18
    mov dh, 0xE
    mov bx, border
    call _printstr

    ; Print out lower border
    mov ch, 18
    mov cl, 18
    mov dh, 0xE
    mov bx, border
    call _printstr

    ; Print out memory message prompt
    mov ch, 22
    mov cl, 30
    mov dh, 0xB
    mov bx, sys_memory_p
    call _printstr

    ; Retreive/store the amount of RAM in the system (int 12 places the RAM size in KBs)
    int 0x12 ; BIOS call for RAM size; get it now while in real mode
    inc ax ; Add one to include the 512 byte boot sector memory + 1 extra sector
    mov [memsize], ax
    mov ch, 22
    mov cl, 43
    mov bx, ax
    call _printnum ; Note: this is an integer, hence the need for an integer printing function

    ; Add "KB" at the end of the number
    add di, 2
    mov al, 'K'
    call _printchar
    mov al, 'B'
    call _printchar

    ; Create a running rise of notes (note value in bx) as a speaker test
    mov bx, 131
    mov si, 0x3
    mov di, 0xD090
    call _makesound
    mov bx, 262
    call _makesound
    mov bx, 523
    call _makesound
    mov bx, 1047
    call _makesound
    mov bx, 2093
    call _makesound
    mov bx, 3951
    call _makesound
    mov bx, 5643
    call _makesound

    ; Go to halting loop when done
    mov di, 398
    jmp _halt

; Halts with a loading indicator
_halt:

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
    border  db  "============================================",0

    ; Memory size related constants
    sys_memory_p    db  "Memory Size: ",0
    memsize dw  0x0000

    ; (Preview) This is what the input prompt will look like
    sys_key_p    db  "Input > ",0

    ; Fill up to bytes 511 and 512 starting from current line
    times 510-($-$$) db 0

    ; MBR Boot signature (0x55 and 0xAA)
    dw 0xAA55

    ; Fill up rest of the floppy
    times 1228288 db 0 ; 5.25" 1.2 MB Floppy
    ;times 1474048 db 0 ; 3.5" 1.44 MB Floppy

    ; Definitions go here (are not in the program itself)
    VGA_MEMORY_TEXT     equ 0xB800 ; Memory address for VGA Text Mode 80x25
    STACK_ADDRESS       equ 0x7BFF
    STACK_SIZE_BYTES    equ 10
    KEYBOARD_ADDRESS    equ 0x041E