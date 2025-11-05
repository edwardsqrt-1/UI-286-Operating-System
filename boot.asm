; Note: Add the following flag at the end of compilation:
;
;   -dfloppy=<identifier>
;
; The following identifiers can be:
;   1200 = 5.25" 1.2 MB
;   1440 = 3.5" 1.44 MB

; Definitions go here (are not in the program itself)
VGA_MEMORY_TEXT     equ 0xB800 ; Memory address for VGA Text Mode 80x25
STACK_ADDRESS       equ 0x699
STACK_SEG_ADDR      equ 0x500
STACK_SIZE_BYTES    equ 10
KEYBOARD_ADDRESS    equ 0x041E
K_OFF               equ 0
K_SEG               equ 0x70

; Conditional Declarations based on floppy disk choice
%if floppy = 360 ; (5.25" 360 KB floppy)
    SECTORS_PER_CLUSTER   equ 2
    TOTAL_SECTOR_AMOUNT   equ 720
    MEDIA_DESCRIPTOR      equ 0xFD
    SECTORS_PER_FAT       equ 2
    SECTORS_PER_TRACK     equ 9
    ROOT_DIRECTORIES      equ 7
    PADDING_BYTES         equ 368640

%elif floppy = 1200 ; (5.25" 1.2 MB floppy)
    SECTORS_PER_CLUSTER   equ 1
    TOTAL_SECTOR_AMOUNT   equ 2400
    MEDIA_DESCRIPTOR      equ 0xF9
    SECTORS_PER_FAT       equ 7
    SECTORS_PER_TRACK     equ 15
    ROOT_DIRECTORIES      equ 14
    PADDING_BYTES         equ 1228800

%elif floppy = 1440 ; (3.5" 1.44 MB floppy)
    SECTORS_PER_CLUSTER   equ 1
    TOTAL_SECTOR_AMOUNT   equ 2880
    MEDIA_DESCRIPTOR      equ 0xF0
    SECTORS_PER_FAT       equ 9
    SECTORS_PER_TRACK     equ 18
    ROOT_DIRECTORIES      equ 14
    PADDING_BYTES         equ 1474560

%endif

; Boot sector is loaded by the computer in the address 0x7C00 (also enforce 16-bit compatibility)
org 0x7C00
BITS 16

; Jump to code (information below is for FAT16). This tells the program to go to the byte 0x3C,
; which is the general standard for the location of the bootloader
jmp short _boot 
nop

; 8-byte Identifier for FAT16 (UI 286 Operating System)
db "UI286OPS"

; Bytes per sector (going with 512 bytes per sector)
dw 512

; Sectors per cluster (two sectors per cluster)
db SECTORS_PER_CLUSTER

; Reserved Sectors (just one for the boot sector and two more for the second stage bootloader)
dw 3

; File Allocation Tables (typically 2)
db 1

; Number of root directories (the first three nibbles actually hold the data; fourth nibble is not used)
dw ROOT_DIRECTORIES * 0x10

; Sector amount 
dw TOTAL_SECTOR_AMOUNT

; Media descriptor type
db MEDIA_DESCRIPTOR

; Sectors per File Allocation Table
dw SECTORS_PER_FAT

; Sectors per Track
dw SECTORS_PER_TRACK

; Heads on device
dw 2

; Hidden Sectors (not used)
dd 0

; Also sector amount (not used)
dd 0

; ----- Extended Boot Record -----

; Drive Number
boot_loc db 0

; Reserved 
db 0

; Signature
db 0x29

; Reserved Again
dd 0

; Volume Label
db "UI-286 BOOT"

; System Identifier String
db "FAT12   "

; Initializing everything and set extra segment to VGA text mode address
_boot:

    ; Set extra segment as the segment for VGA text mode 80x25 by default
    mov ax, VGA_MEMORY_TEXT
    mov es, ax
    mov ax, STACK_SEG_ADDR
    mov ss, ax
    mov ax, STACK_ADDRESS
    mov sp, ax


    ; Clear out all values (excluding dx to save the boot location
    mov ax,cs
    mov ds,ax
    xor ax,ax
    xor bx,bx
    xor cx,cx
    xor di,di
    xor si,si

    ; Save boot location
    mov [boot_loc], dl
    xor dx,dx

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
    
; Prints a character at the current screen location in al with a color in dh
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
    ; 80 * 2 = 160
    push bx
    mov ax, cx
    mov al, ah
    xor ah, ah
    mov bx, 160
    mul bl
    mov di, ax 

    ; Convert x value into the value to translate di by
    mov ax, cx
    xor ah, ah
    shl ax, 1
    add di, ax
    pop bx

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
    ; 80 * 2 = 160
    push bx
    mov ax, cx
    mov al, ah
    xor ah, ah
    mov bx, 160
    mul bl
    mov di, ax 

    ; Convert x value into the value to translate di by
    mov ax, cx
    xor ah, ah
    shl ax, 1
    add di, ax
    pop bx

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

    ; Clear entire screen
    mov al, 0x00
    call _blankscreen

    ; Create a top bar for the bootloader
    xor di, di
    _bootloader_topbar:
        mov [es:di], byte 0x00
        inc di
        mov [es:di], byte 0x40
        inc di
        cmp di, 160
        jb _bootloader_topbar

    ; Print out bootloader specification
    mov ch, 2
    mov cl, 1
    mov dh, 0x0C
    mov bx, bootloader_str1
    call _printstr
    add cl, 2
    mov dh, 0x0F
    mov bx, bootloader_str2
    call _printstr
    inc cl
    mov dh, 0x0B
    mov bx, bootloader_str3
    call _printstr
    add cl, 3
    mov dh, 0x0F
    mov bx, bootloader_str4
    call _printstr

    ; Retrieve/store the amount of RAM in the system (int 12 places the RAM size in KBs)
    int 0x12 ; BIOS call for RAM size; get it now while in real mode
    mov [memsize], ax
    mov ch, 3
    mov cl, 1
    mov dh, 0x0D
    mov bx, ax
    call _printnum ; Note: this is an integer, hence the need for an integer printing function

    ; Print out message to load 
    mov ch, 3
    add cl, 5
    mov dh, 0x0D
    mov bx, memory_str
    call _printstr

    ; Print out message to load 
    mov ch, 5
    mov cl, 1
    mov dh, 0x8E
    mov bx, boot_str
    call _printstr
    

    ; Load next ten sectors that stores the rest of the bootloader
    ; ah = command to read floppy
    ; al = number of sectors to read
    ; bx = offset of buffer (in this case, just the address of the buffer)
    ; ch = cylinder to use
    ; cl = sector to use
    ; dh = head to use
    ; dl = drive to read from where 0 = A:, 1 = B:, 2 = C:, and 3 = D:
    ; es = higher order bits of address. Since we are loading this right after the boot sector at the value 0x7E00 (0x7C00 + 512 bytes), this is just 0
    mov ah, 0x02
    mov al, 2
    mov bx, 0x7E00
    xor ch, ch
    mov cl, 2
    xor dx, dx
    mov es, dx
    xor dh, dh
    mov dl, [boot_loc]

    ; Execute the command to load the sectors; if successful then proceed to the post-boot sector lines, but catch any error
    int 0x13
    jc floppy_load_fail
    jmp starthere_stage2

    ; In case an error arises, go here and after printing debugging information, halt
    floppy_load_fail:

        ; Display error code in ah if failed
        mov bx, VGA_MEMORY_TEXT
        mov es, bx
        mov ch, 7
        mov cl, 1
        mov dh, 0x8C
        mov bx, err_status
        call _printstr 
        mov ch, 7
        mov cl, 25
        shr ax, 8
        mov bx, ax
        call _printnum

    ; Halt here
    _nothing:
        nop
        nop
        nop
        jmp _nothing

; Data variables go here
_bootsector_data:

    ; String for UI-286 Construction Message
    memory_str  db  "KB of RAM Available",0
    err_status  db  "Boot Failed! Status = ",0
    bootloader_str1  db  "UI",0
    bootloader_str2  db  "(",0
    bootloader_str3  db  "286",0
    bootloader_str4  db  ") Boot System v1.0a",0
    boot_str    db  "Loading the second stage into memory...",0

    ; Memory size related constants
    memsize dw  0x0000

; Fill up to bytes 511 and 512 starting from current line
times 510-($-$$) db 0

; MBR Boot signature (0x55 and 0xAA)
dw 0xAA55

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

; Function to reset floppy disk controller
_reset_floppy:
    push ax
    push dx
    xor ah, ah
    mov dl, [boot_loc]
    int 0x13
    pop dx
    pop ax
    ret

; Execution continues here after the magic number
starthere_stage2:

    ; Create a series of beep to show sound is working
    mov bx, 131
    mov si, 0x3
    mov di, 0xD090
    call _makesound
    mov bx, 262
    call _makesound
    mov bx, 523
    call _makesound
    mov bx, 1046
    call _makesound
    mov bx, 2093
    call _makesound
    mov bx, 1046
    call _makesound
    mov bx, 523
    call _makesound
    mov bx, 262
    call _makesound

    ; Reset extra segment to point to video memory
    mov ax, VGA_MEMORY_TEXT
    mov es, ax

    ; Update boot prompt to be solid green rather than blinking yellow
    mov ch, 5
    mov cl, 1
    mov dh, 0x0A
    mov bx, boot_str
    call _printstr

    ; Print out "Done!" at the end
    mov ch, 5
    mov cl, 41
    mov dh, 0x0A
    mov bx, boot_done
    call _printstr

    ; Print the intent to load the kernel
    mov ch, 6
    mov cl, 1
    mov dh, 0x0F
    mov bx, fat_load_str
    call _printstr

    ; Load the File Allocation Table
    mov ah, 0x02
    mov al, 1
    mov bx, 0x8200
    xor ch, ch
    mov cl, SECTORS_PER_FAT+4
    xor dx, dx
    mov es, dx
    mov dl, [boot_loc]
    call _reset_floppy
    int 0x13
    
    ; Carry flag tells whether loading the file allocation table worked or not
    jnc successful_FAT_load
    mov bx, kernel_fail_1
    jmp fail_kernel_load

; Proceed execution upon successfully loading the table
successful_FAT_load:

    ; Print success message
    mov bx, fat_load_successful_str
    mov ax, 0xB800
    mov es, ax
    mov ch, 8
    mov cl, 1
    mov dh, 0x0F
    call _printstr

    ; Point the destination index to the name entry (located at an offset of 0x20)
    ; Also point the source index at the name to be looked for
    mov di, 0x8220
    mov si, kernel_name

; Iterate through and determine whether the name matches
name_check:

    ; Compare characters
    mov ah, [di]
    mov al, [si]
    cmp ah, al
    je continue_name_check
    mov bx, kernel_fail_2
    jmp fail_kernel_load
    
    ; Find if the next character is the end of string character (value of 0)
    continue_name_check:
        xchg dx, si
        inc dx
        xchg dx, si
        inc di
        mov al, [si]
        cmp al, 0
        je kernel_found
        jmp name_check

; Proceed to load the kernel data
kernel_found:

    ; Go to the start sector 
    mov si, di
    add si, 15
    push si

    ; Load what the kernel start sector is (prompt)
    mov bx, kernel_start_sector_p
    mov ch, 9
    mov cl, 1
    mov dh, 0x0F
    call _printstr

    ; Load what the kernel start sector is (value)
    mov bx, [si]
    mov [kernel_start_sect], bx
    mov ch, 9
    mov cl, 15
    mov dh, 0x0F
    call _printnum

    ; Load what the size of the file is (prompt)
    mov bx, kernel_size_p
    mov ch, 10
    mov cl, 1
    mov dh, 0x0F
    call _printstr 

    ; Load what the size of the file is (value)
    pop dx
    add dx, 2
    mov si, dx
    mov bx, [si]
    mov ch, 10
    mov cl, 14
    mov dh, 0x0F
    call _printnum

    ; Load what the size of the file is (unit)
    mov bx, kernel_size_p_2
    mov ch, 10
    mov cl, 19
    mov dh, 0x0F
    call _printstr 

    ; Tell the user that the kernel is now being loaded
    mov bx, kernel_load_str
    mov ax, 0xB800
    mov es, ax
    mov ch, 11
    mov cl, 1
    mov dh, 0x0F
    call _printstr

    ; Determine how many sectors to load
    xor dx, dx
    mov cx, 512
    div cx
    inc ax
    mov [sectors_to_load], ax
    
    ; Load the kernel directly near the beginning of conventional memory (0x700)
    ; Note: Kernel Segment K_SEG = 0x70, and Kernel Offset K_OFF = 0x00; K_SEG * 0x10 + K_OFF = K_ADDR
    ; Also note the equations:
    ;   - Cylinders = (sectors_to_load+SECTORS_PER_FAT+3+ROOT_DIRECTORIES) / (2 * SECTORS_PER_TRACK)
    ;   - Heads = (sectors_to_load+SECTORS_PER_FAT+3+ROOT_DIRECTORIES) / SECTORS_PER_TRACK % 2 
    ;   - Sectors = (sectors_to_load+SECTORS_PER_FAT+3+ROOT_DIRECTORIES) % SECTORS_PER_TRACK
    
    ; Get Sectors and Heads
    mov ax, [kernel_start_sect]
    sub ax, 2
    mov bl, SECTORS_PER_CLUSTER
    mul bl
    add ax, SECTORS_PER_FAT+3+ROOT_DIRECTORIES
    push ax
    mov bl, SECTORS_PER_TRACK
    div bl
    and al, 0x1 ; Filter last bit (same as modulo 2)
    inc ah
    mov dh, al ; Start Head
    mov cl, ah ; Start Sector

    ; Get Cylinders
    mov ax, SECTORS_PER_TRACK
    mov bl, 2
    mul bl
    mov bl, al
    pop ax 
    div bl
    mov ch, al ; Start Cylinder

    ; Get other necessary parameters
    push dx
    mov ax, [sectors_to_load]
    mov ah, 0x02
    mov bx, K_OFF 
    mov dx, K_SEG
    mov es, dx 
    pop dx
    mov dl, [boot_loc]
    call _reset_floppy ; Reset just in case

    ; Run the command to the BIOS and determine whether the desired sectors have been loaded
    int 0x13
    jnc kernel_loaded
    mov bx, kernel_fail_3
    jmp fail_kernel_load

; Go to the kernel if successfully loaded (set the data segment to match; code segment is automatically set on long jump
kernel_loaded:
    mov ax, K_SEG
    mov ds, ax
    jmp K_SEG:K_OFF
    
; If the kernel fails to load, print message and then halt
fail_kernel_load:

    ; Print message
    mov ax, 0xB800
    mov ax, es
    mov ch, 21
    mov cl, 1
    mov dh, 0x04
    call _printstr

    ; Go to halting loop when done
    mov di, 398
    mov cx, 0x7
    mov dx, 0xA120
    mov ah, 0x86
    jmp _halt


; Halts with a loading indicator
_halt:

    ; Set the foreground color to be cyan but keep background as blue
    mov [es:di+1], byte 0x13

    ; Create loading symbol
    mov [es:di], byte '-'
    nop
    nop
    nop
    nop
    int 0x15
    mov [es:di], byte '\'
    nop
    nop
    nop
    nop
    int 0x15
    mov [es:di], byte '|'
    nop
    nop
    nop
    nop
    int 0x15
    mov [es:di], byte '/'
    nop
    nop
    nop
    nop
    int 0x15

    ; Repeat until PC is powered off
    jmp _halt

; Area of data for the rest of the code
_bootsector_stage2_data:

    ; Screen data
    boot_done   db  "Done!",0
    ui286   db  "UI(286) Construction in Progress...",0
    wait_str   db  "Please stay tuned for future updates!",0
    border  db "===========================================",0
    sys_memory_p    db  "Memory Size:",0
    fat_load_str db  "Now locating the UI(286) Kernel...",0
    fat_load_successful_str db  "FAT loaded successfully! Looking for KERNEL.BIN...",0
    kernel_name db  "KERNEL  BIN",0
    kernel_load_str db  "Found KERNEL.BIN! Loading UI(286)....",0
    kernel_fail_1   db  "Cannot load the FAT :(",0
    kernel_fail_2   db  "Cannot find KERNEL.BIN :(",0
    kernel_load_successful_str db  "Kernel loaded successfully, loading UI(286)!",0
    kernel_start_sector_p   db  "Start Sector:",0
    kernel_size_p   db  "Kernel Size:",0
    kernel_size_p_2 db  "Bytes",0
    kernel_fail_3   db  "Error: Cannot load UI(286) into memory :(",0
    kernel_start_sect   dw  0
    sectors_to_load     dw  0

; Fill up rest of the floppy (depends on floppy selected)
; This block of data highly depends on the floppy disk used
times PADDING_BYTES-($-$$) db 0