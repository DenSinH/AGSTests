#include "dma.h"


u32 check_DMA_test(void* dest, size_t len, void* expected, u32 check_size, u32* failed_address, u32* failed_value) {
    bool failed;
    u32 current_value = 0;
    u32 i;

    if (check_size == 0) {
        for (i = 0; i < len; i++) {
            current_value = *(u8*)(dest + i);
            if (current_value != *(u8*)(expected + i)) {
                goto test_failed;
            }
        }
    }
    else if (check_size == 1) {
        for (i = 0; i < len; i += 2) {
            current_value = *(u16*)(dest + i);
            if (current_value != *(u16*)(expected + i)) {
                goto test_failed;
            }
        }
    }
    else {
        for (i = 0; i < len; i += 4) {
            current_value = *(u32*)(dest + i);
            if (current_value != *(u32*)(expected + i)) {
                goto test_failed;
            }
        }
    }

    dest = NULL;
    current_value = 0;
    failed = false;
return_statement:
    *failed_address = (u32)dest;
    *failed_value = current_value;
    return failed;

test_failed:  // LAB_0800f1d6
    failed = true;
    dest += i;
    goto return_statement;
}