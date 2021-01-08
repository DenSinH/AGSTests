#include "lcd.h"
#include "AGS.h"

u32 run_vcounter_test(u32 _buffer, u32 number_of_tests) {
    s_vcount_test_values* buffer = (s_vcount_test_values*)_buffer;

    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x00800000;  // start timer and reload
    s_vcount_test_values* buffer_end = buffer + number_of_tests;
    u16 VCount_start = *ptr_VCOUNT;
    u16 VCount;
    do {
        do {
            VCount = *ptr_VCOUNT;
        } while (VCount_start != VCount);

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
        if (!(0x4c1 <= buffer[i].TM0 && buffer[i].TM0 <= 0x4d1)) {
            flags |= 1;
            break;
        }
    }

    set_IME(IME);
    return flags;
}

void test_vcount_intr_flag() {
    u16 VCount = *ptr_VCOUNT + 0x10;
    if (VCount > 227) {
        VCount = 0;
    }

    // set VCount match
    *ptr_DISPSTAT = (*ptr_DISPSTAT & 0xff) | (VCount << 8);
    do { } while (*ptr_VCOUNT != VCount);
}

u32 vcount_intr_flag() {
    u16 flags = 0;

    u16 IME = set_IME(0);
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = 0x20;  // V-Counter IRQ Enable
    *ptr_IF = intr_VCount; // clear VCount IRQs

    test_vcount_intr_flag();
    // at this point, a V-Counter IRQ should have been requested
    if (!((*ptr_IF) & intr_VCount)) {
        flags |= 1;
    }

    *ptr_DISPSTAT = 0;      // V-Counter IRQ Disable
    *ptr_IF = intr_VCount;  // clear VCount IRQs

    test_vcount_intr_flag();
    // at this point, V-Count match happened, but no IRQ should be requested
    if ((*ptr_IF) & intr_VCount) {
        flags |= 2;
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_IME(IME);
    return flags;
}