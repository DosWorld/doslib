; auto-generated code, do not edit
bits 16          ; 16-bit real mode

section _TEXT class=CODE

extern dosdrv_interrupt_near_           ; <- watcall

global _dosdrv_header
global _dosdrv_req_ptr

; DOS device driver header (must come FIRST)
_dosdrv_header:
        dw  0xFFFF,0xFFFF               ; next device ptr
        dw  0xA800                      ; bit 15: 1=character device
                                        ; bit 14: 1=supports IOCTL control strings
                                        ; bit 13: 1=supports output until busy
                                        ; bit 11: 1=understands open/close
                                        ; bit  6: 0=does not understand 3.2 functions
                                        ; bit 3:0: 0=not NUL, STDIN, STDOUT, CLOCK, etc.
        dw  _dosdrv_strategy            ; offset of strategy routine
        dw  dosdrv_interrupt_stub_      ; offset of interrupt routine
        db  'HELLO$  '                  ; device name (8 chars)
        db  0

; =================== Strategy routine ==========
_dosdrv_strategy:
        mov     [cs:_dosdrv_req_ptr+0],bx ; BYTE ES:BX+0x00 = length of request header
        mov     [cs:_dosdrv_req_ptr+2],es ; BYTE ES:BX+0x01 = unit number
        retf                              ; BYTE ES:BX+0x02 = command code
                                          ; WORD ES:BX+0x03 = driver return status word
                                          ;      ES:BX+0x05 = reserved??

; =================== Interrupt routine =========
dosdrv_interrupt_stub_:
        push    ax
        push    ds
        mov     ax,cs
        mov     ds,ax
        call    dosdrv_interrupt_near_
        pop     ds
        pop     ax
        retf

section _DATA class=DATA

; var storage
_dosdrv_req_ptr:
        dd  0

; Watcom's _END symbol acts as expected in C when you access it, but then acts funny
; when you try to take the address of it. So we have our own.
section _END class=END
global _dosdrv_end
_dosdrv_end:

; begin of init section i.e. the cutoff point once initialization is finished.
section _BEGIN class=INITBEGIN
global _dosdrv_initbegin
_dosdrv_initbegin:

group DGROUP _END _BEGIN _TEXT _DATA