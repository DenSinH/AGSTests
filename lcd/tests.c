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
    u16 failed = 0;
    s_vcount_test_values buffer[456] = {};
    u16 IME = set_IME(0);
    call_from_stack(run_vcounter_test, (u32)buffer, 456);

    // check VCount values
    u16 VCount = buffer[0].VCount;
    if (VCount < 228) {
        for (int i = 1; i < 228; i++) {
            VCount++;
            if (VCount == 228) {
                VCount = 0;
            }
            if (VCount != buffer[i].VCount) {
                failed |= 2;
                break;
            }
        }
    }
    else {
        failed |= 2;
    }

    // check timer values
    for (int i = 1; i < 228; i++) {
        if (!(0x4c1 <= buffer[i].TM0 && buffer[i].TM0 <= 0x4d1)) {
            failed |= 1;
            break;
        }
    }

    set_IME(IME);
    return failed;
}