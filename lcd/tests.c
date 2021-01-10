#include "lcd.h"
#include "AGS.h"

u32 run_vcounter_test(u32 _buffer, u32 number_of_tests) {
    s_vcount_test_values* buffer = (s_vcount_test_values*)_buffer;

    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x00800000;  // start timer and reload
    s_vcount_test_values* buffer_end = buffer + number_of_tests;
    u16 VCount_start = *ptr_VCOUNT;
    u16 VCount;
    bool VCount_match;
    do {
        do {
            VCount = *ptr_VCOUNT;
            VCount_match = VCount == VCount_start;
            VCount_start = VCount;
        } while (VCount_match);

        // read timer value and reset
        u16 TM0CNT = *ptr_TM0CNT;
        *ptr_TM0CNT = 0;
        *ptr_TM0CNT = 0x00800000;
        buffer->VCount = VCount;
        buffer->TM0    = TM0CNT;
        buffer++;
    } while (buffer != buffer_end);
    return 0;
}

u32 vcounter() {
    u16 flags = 0;
    s_vcount_test_values buffer[456] = {};
    u16 IME = set_IME(0);
    call_from_stack(run_vcounter_test, (u32) buffer, 456);

    // check VCount values
    u16 VCount = buffer[0].VCount;
    if (VCount < 228) {
        for (int i = 1; i < 228; i++) {
            VCount++;
            if (VCount == 228) {
                VCount = 0;
            }
            if (VCount != buffer[i].VCount) {
                flags |= 2;
                break;
            }
        }
    } else {
        flags |= 2;
    }

    // check timer values
    for (int i = 1; i < 228; i++) {
        if (!(1217 <= buffer[i].TM0 && buffer[i].TM0 <= 1233)) {
            flags |= 1;
            break;
        }
    }

    set_IME(IME);
    return flags;
}

u32 vcount_intr_flag() {
    u16 flags = 0;

    u16 IME = set_IME(0);
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_VCountIRQ;  // V-Counter IRQ Enable
    *ptr_IF = intr_VCount; // clear VCount IRQs

    wait_for_vcount_match();
    // at this point, a V-Counter IRQ should have been requested
    if (!((*ptr_IF) & intr_VCount)) {
        flags |= 1;
    }

    *ptr_DISPSTAT = 0;      // V-Counter IRQ Disable
    *ptr_IF = intr_VCount;  // clear VCount IRQs

    wait_for_vcount_match();
    // at this point, V-Count match happened, but no IRQ should be requested
    if ((*ptr_IF) & intr_VCount) {
        flags |= 2;
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_IME(IME);
    return flags;
}

u32 hblank_intr_flag() {
    u32 flags = 0;

    u16 IME = set_IME(0);
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_HBLankIRQ;   // enable HBlank IRQ
    *ptr_IF = intr_HBlank;  // clear interrupt

    wait_for_line();
    // at this point, HBlank IRQ should have been requested
    if (!(*ptr_IF & intr_HBlank)) {
        flags |= 1;
    }

    *ptr_DISPSTAT = 0;  // disable HBlank IRQ
    *ptr_IF = intr_HBlank;  // clear interrupt
    wait_for_line();
    // at this point, HBlank should have happened, but no IRQ should have fired

    if (*ptr_IF & intr_HBlank) {
        flags |= 2;
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_IME(IME);

    return flags;
}

u32 vblank_intr_flag() {
    u32 flags = 0;

    u16 IME = set_IME(0);
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_VBlankIRQ;      // enable VBlank IRQ
    *ptr_IF = intr_VBlank;  // clear interrupt

    wait_for_frame();
    // at this point, VBlank IRQ should have been requested
    if (!(*ptr_IF & intr_VBlank)) {
        flags |= 1;
    }

    *ptr_DISPSTAT = 0;  // disable VBlank IRQ
    *ptr_IF = intr_VBlank;  // clear interrupt
    wait_for_frame();
    // at this point, VBlank should have happened, but no IRQ should have fired

    if (*ptr_IF & intr_VBlank) {
        flags |= 2;
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_IME(IME);

    return flags;
}

u32 run_vcount_status_test(u32 _buffer, u32 length) {
    s_vcount_status_test_value* buffer = (s_vcount_status_test_value*)_buffer;

    u16 VCount_status = (*ptr_DISPSTAT) & DISPSTAT_VCount;
    u16 new_VCount_status;
    bool VCount_status_change;
    do {
        // wait until VCount match flag set
        do {
            new_VCount_status = (*ptr_DISPSTAT) & DISPSTAT_VCount;  // <---- (1)
            VCount_status_change = new_VCount_status != VCount_status;
            VCount_status = new_VCount_status;
        } while (!VCount_status_change);

        // done in a single store, using the value loaded at (1) (the full value, not the & 4'ed value)
        buffer->DISPSTAT = *ptr_DISPSTAT;
        buffer->VCOUNT   = *ptr_VCOUNT;
        buffer++;
    } while ((u32)buffer != _buffer + length);
    return 0;
}

u32 vcount_status() {
    u16 flags = 0;

    s_vcount_status_test_value buffer[4] = {};
    u16 IME = set_IME(0);
    u16 DISPSTAT = *ptr_DISPSTAT;
    u16 VCount_match = (*ptr_VCOUNT) + 10;
    if (VCount_match > 226) {
        VCount_match = 0;
    }
    *ptr_DISPSTAT = (VCount_match << 8);  // set VCount match to be in 10 lines

    for (int i = 0; i < 4; i++) {
        // check if flag is set in DISPSTAT
        if (!(buffer[i].DISPSTAT & DISPSTAT_VCount)) {
            flags |= 1;
            break;
        }
        // check if it happened at the right VCount value
        else if (buffer[i].VCOUNT != VCount_match) {
            flags |= 2;
            break;
        }
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_IME(IME);
    return flags;
}

u32 run_hblank_status_test(u32 _buffer, u32 length) {
    s_hblank_status_test_value* buffer = (s_hblank_status_test_value*)_buffer;

    // reset timer
    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x00800000;
    u16 HBlank_status = (*ptr_DISPSTAT) & DISPSTAT_HBLank;
    bool HBlank_status_change;
    u16 DISPSTAT;
    do {
        // wait until HBlank status change
        do {
            DISPSTAT = *ptr_DISPSTAT;
            u16 new_HBlank_status = DISPSTAT & DISPSTAT_HBLank;
            HBlank_status_change = HBlank_status != new_HBlank_status;
            HBlank_status = new_HBlank_status;
        } while (!HBlank_status_change);

        u16 TM0CNT = *ptr_TM0CNT;
        // reset timer
        *ptr_TM0CNT = 0;
        *ptr_TM0CNT = 0x00800000;

        // done in a single store, using the value loaded at (1) (the full value, not the & 4'ed value)
        buffer->DISPSTAT = DISPSTAT;
        buffer->TM0CNT   = TM0CNT;
        buffer++;
    } while ((u32)buffer != _buffer + length);
    *ptr_TM0CNT = 0;
    return 0;
}
u32 hblank_status() {
    u16 flags = 0;

    s_hblank_status_test_value buffer[456] = {};

    u16 IME = set_IME(0);
    call_from_stack(run_hblank_status_test, (u32)buffer, 0);
    for (int i = 0; i < 456; i++) {
        if (!(buffer[i].DISPSTAT & DISPSTAT_HBLank)) {
            if (!((209 <= buffer[i].TM0CNT) && (buffer[i].TM0CNT <= 225))) {
                flags |= 1;
                break;
            }
        }
        else {
            if (!((990 <= buffer[i].TM0CNT) && (buffer[i].TM0CNT <= 1009))) {
                flags |= 2;
                break;
            }
        }
    }
    set_IME(IME);
    return flags;
}

u32 run_vblank_status_test(u32 _buffer, u32 length) {
    s_vcount_status_test_value* buffer = (s_vcount_status_test_value*)_buffer;

    u32 VBlank_status = (*ptr_DISPSTAT) & DISPSTAT_VBlank;
    bool VBlank_status_change;
    do {
        // wait until HBlank status change
        do {
            u16 new_VBlank_status = (*ptr_DISPSTAT) & DISPSTAT_VBlank;  // <--- 32 bit load (1)
            VBlank_status_change = VBlank_status != new_VBlank_status;
            VBlank_status = new_VBlank_status;
        } while (!VBlank_status_change);

        // these loads come from the same load as (1)
        buffer->DISPSTAT = *ptr_DISPSTAT;
        buffer->VCOUNT   = *ptr_VCOUNT;
        buffer++;
    } while ((u32)buffer != (_buffer + length));
    return 0;
}

u32 vblank_status() {
    u32 flags = 0;

    s_vcount_status_test_value buffer[4] = {};

    u16 IME = set_IME(0);
    call_from_stack(run_hblank_status_test, buffer, 4);
    for (int i = 0; i < 4; i++) {
        if (!(buffer[i].DISPSTAT & DISPSTAT_VBlank)) {
            if (buffer[i].VCOUNT != 227) {
                flags |= 1;
                break;
            }
        }
        else {
            if (buffer[i].VCOUNT != 160) {
                // I think this is a bug...
                flags |= 1;
                break;
            }
        }
    }
    set_IME(IME);
    return flags;
}