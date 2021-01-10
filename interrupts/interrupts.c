#include "interrupts.h"
#include "AGS.h"
#include "timer/timer.h"


void wait_for_timer_overflow(u32 timer) {
    *ptr_TMCNT(timer) = 0;
    *ptr_TMCNT(timer) = 0x00c0f000;  // enable, IRQ enable
    call_from_stack(wait_ticks, 0x40a, 0);
    *ptr_TMCNT(timer) = 0;
}

void wait_for_sio_complete() {
    *ptr_RCNT = 0;
    *ptr_SIOCNT = 0x4003;  // IRQ enable, 2MHz, internal clock
    *ptr_SIOMLT_SEND = 0;
    *ptr_SIOCNT |= 0x80;  // start
    do { } while (!((*ptr_SIOCNT) & 0x80));  // wait for transfer completion
}

void wait_for_dma_complete(u32 channel) {
    u32 src;
    u32 dest[4];
    *ptr_DMASAD(channel) = src;
    *ptr_DMADAD(channel) = dest;
    *ptr_DMACNT(channel) = ((DMAEnable | DMAIRQ | DMASrcFixed | DMADestFixed) << 16) | 1;
    do { } while (!((*ptr_DMACNT(channel)) & 0x80000000));
}

void wait_for_key_interrupt() {
    u16 KEYCNT = *ptr_KEYCNT;
    if ((~*ptr_KEYINPUT & 0x3ff) == 0) {
        // no keys pressed
        *ptr_KEYCNT = 0xc000;  // IRQ enable, IRQ AND
    }
    else {
        // a key is pressed
        *ptr_KEYCNT = 0x43ff;  // IRQ enable, IRQ OR
    }
}