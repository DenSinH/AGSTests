#include "timer.h"
#include "AGS.h"


u32 run_prescaler_test(u32 _tmcnt, u32 setting) {
    u32* tmcnt = (u32*)_tmcnt;

    *tmcnt = 0;
    *tmcnt = setting;

    // this is a very tight assembly loop
    for (int i = 0x400; i != 0; i--) { }
    u32 out_value = *(u16*)tmcnt;
    *tmcnt = 0;

    return out_value;
}

u32 prescaler() {
    u32 flags = 0;

    u32 local_prescalers[4];
    local_prescalers[0] = prescalers[0];
    local_prescalers[1] = prescalers[1];
    local_prescalers[2] = prescalers[2];
    local_prescalers[3] = prescalers[3];

    u16 IME = set_IME(0);
    u32 bit = 0;
    for (int i = 0; i < 4; i++) {
        for (int setting = 0; setting < 4; setting++) {
            u32 output = call_from_stack(run_prescaler_test, (u32)(ptr_TM0CNT + i), (setting << 16) | 0x00800000);
            if (output != local_prescalers[setting]) {
                flags |= 1 << bit;
            }
            bit++;
        }
    }
    set_IME(IME);
    return flags;
}

u32 run_timer_connect_test(u32 reload, u32 _) {
    u32* ptr_TMCNT = ptr_TM0CNT;
    // clear timers
    ptr_TMCNT[0] = 0;
    ptr_TMCNT[1] = 0;
    ptr_TMCNT[2] = 0;
    ptr_TMCNT[3] = 0;

    // set prescaling mode
    ptr_TMCNT[3] = 0x840000;
    ptr_TMCNT[2] = 0x840000 | reload;
    ptr_TMCNT[1] = 0x840000 | reload;
    ptr_TMCNT[0] = 0x840000 | reload;

    for (int i = 0x400; i != 0; i--) { }
    u16 TM3CNT = (u16)ptr_TMCNT[3];
    ptr_TMCNT[0] = 0;
    ptr_TMCNT[1] = 0;
    ptr_TMCNT[2] = 0;
    ptr_TMCNT[3] = 0;
    return TM3CNT;
}

u32 timer_connect() {
    u16 IME = set_IME(0);
    u16 output = call_from_stack(run_timer_connect_test, 0xfffe, 0);
    set_IME(IME);
    return output != 0x200;
}

u32 timer_intr_flag() {
    u32 flags = 0;

    u16 IME = set_IME(0);
    for (int i = 0; i < 4; i++) {
        ptr_TM0CNT[i] = 0;
        *ptr_IF = (8 << i);  // clear timer interrupt
        ptr_TM0CNT[i] = 0xc0f000;  // enable, IRQ enable, reload value f000
        call_from_stack(wait_ticks, 0x40a, 0);
        // by now a timer IRQ should have been requested, the IRQ flag is set, so it should be in IF as well

        if (!((*ptr_IF) & (8 << i))) {
            flags |= 1 << (2 * i);
        }

        ptr_TM0CNT[i] = 0;
        *ptr_IF = (8 << i);  // clear timer interrupt
        ptr_TM0CNT[i] = 0x80f000;  // enable, reload value f000
        call_from_stack(wait_ticks, 0x40a, 0);
        // by now a timer IRQ would have been requested, but the IRQ flag is off, so IF shouldn't hold the IRQ

        if ((*ptr_IF) & (8 << i)) {
            flags |= 2 << (2 * i);
        }
    }
    set_IME(IME);
    return flags;
}