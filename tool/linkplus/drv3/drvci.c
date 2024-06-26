
/* does NOT use C runtime */

#include <i86.h>
#include <conio.h>
#include <stdint.h>

#include <hw/dos/drvhead.h>
#include <hw/dos/drvreq.h>
#include <hw/dos/drvvar.h>

/* LINKPLUS LNKDOS16 can auto-define a symbol pointing to the start of this init group
 * which should be the same as the end of the first default (-1) segment group.
 * Name should be __seggrp_startn_initsect. Use _cdecl to access it which should
 * prepend the extra _ needed. */
extern unsigned char far _cdecl _seggrp_startn_initsect;

/* This time, we can declare code and data as normal,
 * then segregate this code into it's own init section using lnkdos16 -seggroup. */
static const char hello_world[] = "Hello!\r\nThis is an example device driver that acts as a basic character device.\r\nYou can read and write me through the HELLO$ character device.\r\nHave fun!\r\n\r\n";

void INIT_func(void) {
// NTS: Watcom C is missing an optimization opportunity here, so define instead
#define initreq ((struct dosdrv_request_init_t far*)dosdrv_req_ptr)

    __asm {
        push    ax
        push    si

        ; NTS: We can refer to it without reloading DS because we tied everything to DGROUP in this project
        mov     si,offset hello_world

l1:     lodsb
        or      al,al
        jz      le
        mov     ah,0x0E
        int     10h
        jmp     l1
le:

        pop     si
        pop     ax
    }

    dosdrv_req_ptr->status = dosdrv_request_status_flag_DONE;

    /* DOS then expects us to tell it the end of our resident section.
     * This is why this code has _END offsets and init sections.
     * This init code and any INIT segments will be thrown away after we return. */
    initreq->end_of_driver = &_seggrp_startn_initsect;

#undef initreq
}

