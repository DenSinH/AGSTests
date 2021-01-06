#include "memory.h"

void mem_set_const(u8* dest, u32 length, u32 value, u32 flags) {
    /*
     * As mentioned in the readme, this is not the _exact_ way the function is internally, but this is basically it.
     * */
    if (flags == mem_set_u16) {
        for (u32 i = 0; i < length; i++) {
            *(u8*)(dest + i) = value;
        }
    }
    else if (flags == mem_set_u8) {
        for (u32 i = 0; i < length; i += 2) {
            *(u16*)(dest + i) = value;
        }
    }
    else {
        for (u32 i = 0; i < length; i += 4) {
            *(u32*)(dest + i) = value;
        }
    }
}

bool mem_set_const_check(u8* src, u32 length, u32 value, u32 flags, u8** failed_address, u32* failed_value) {
    bool failed;
    u32 current_value = 0;
    u32 i;

    if (flags == mem_set_u16) {
        for (i = 0; i < length; i++) {
            current_value = *(u8*)(src + i);
            if (current_value != value) {
                goto test_failed;
            }
        }
    }
    else if (flags == mem_set_u8) {
        for (i = 0; i < length; i += 2) {
            current_value = *(u16*)(src + i);
            if (current_value != value) {
                goto test_failed;
            }
        }
    }
    else {
        for (i = 0; i < length; i += 4) {
            current_value = *(u32*)(src + i);
            if (current_value != value) {
                goto test_failed;
            }
        }
    }

    current_value = 0;
    failed = false;
return_statement:
    *failed_address = src;
    *failed_value = current_value;
    return failed;

test_failed:  // LAB_0800f240
    failed = true;
    src += i;
    goto return_statement;
}void mem_set_incrementing(u8* dest, u32 length) {
    u32 value = 0;
    for (u32 i = 0; i < length; i += 2, value++) {
        *(u16*)(dest + i) = (value >> 16) + (u16)value;
    }
}

bool mem_set_incrementing_check(u8* src, u32 length, u8** failed_address, u32* failed_value) {
    bool failed = false;
    u32 i;
    u32 value = 0;
    u32 correct_value = 0;
    u8* current_address = 0;

    for (i = 0; i < length; i += 2, correct_value++) {
        value = *(u16*)(src + i);
        if (value != ((correct_value >> 16) + (u16)correct_value)) {
            current_address = src + i;
            failed = true;
            goto return_statement;
        }
    }
    value = 0;

    return_statement:
    *failed_address = current_address;
    *failed_value   = value;
    return failed;
}

bool set_incrementing_test(u8* buffer, u8 *buffer_end, u8** failed_address, u32* failed_value) {
    u16 IME = set_IME(0);
    mem_set_incrementing(buffer, (u32) buffer_end - (u32) buffer);
    bool failed = mem_set_incrementing_check(buffer, (u32) buffer_end - (u32) buffer, failed_address,
                                             failed_value);
    set_IME(IME);
    return failed;
}

bool const8_fill_test(u8* buffer, u8* buffer_end, u32 value, u8** failed_address, u32* failed_value) {
    u16 IME = set_IME(0);
    mem_set_const(buffer, (u32) buffer_end - (u32) buffer, value, mem_set_u8);
    bool failed = mem_set_const_check(buffer, (u32) buffer_end - (u32) buffer, value, mem_set_u8, failed_address,
                                      failed_value);
    set_IME(IME);
    return failed;
}

bool const32_fill_test(u8* buffer, u8 *buffer_end, u32 value, u8** failed_address, u32* failed_value) {
    u16 IME = set_IME(0);
    mem_set_const(buffer, (u32) buffer_end - (u32) buffer, value, mem_set_u32);
    bool failed = mem_set_const_check(buffer, (u32) buffer_end - (u32) buffer, value, mem_set_u32, failed_address,
                                      failed_value);
    set_IME(IME);
    return failed;
}

bool DMA16_test(u8* buffer, u32 access_time) {
    bool failed = false;
    u16 local_buffer[128];

    u16 IME = set_IME(0);
    *ptr_TM0CNT = 0;

    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x800000;
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = ptr_TM0CNT;
    *ptr_DMA3DAD = buffer;
    *ptr_DMA3CNT = ((DMAEnable | DMASrcFixed) << 16) | 0x0080;
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = buffer;
    *ptr_DMA3DAD = local_buffer;
    *ptr_DMA3CNT = ((DMAEnable) << 16) | 0x0080;
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    u16 diff = access_time + 2;
    u16 timer_val = local_buffer[0];
    for (u32 i = 0; i < 0x80; i++) {
        if (buffer[i] != timer_val) {
            failed = true;
            break;
        }
        timer_val += diff;
    }

    // LAB_0800d0fc
    set_IME(IME);
    return failed;
}


bool DMA32_test(u8* buffer, u32 access_time, u32 access_time_factor) {
    bool failed = false;
    u16 local_buffer[128];

    u16 IME = set_IME(0);
    *ptr_TM0CNT = 0;

    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x800000;
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = ptr_TM0CNT;
    *ptr_DMA3DAD = buffer;
    *ptr_DMA3CNT = ((DMAEnable | DMASrcFixed) << 16) | 0x0080;
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = buffer;
    *ptr_DMA3DAD = local_buffer;
    *ptr_DMA3CNT = ((DMAEnable) << 16) | 0x0080;
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    u16 factor    = 0x20 / access_time_factor;
    u16 diff      = factor * (access_time + 1) + 1;
    u16 timer_val = local_buffer[0];
    u16 TM0CNT_H  = *ptr_TM0CNT;

    for (u32 i = 0; i < 0x80; i += 2) {
        if (buffer[i] != timer_val) {
            failed = true;
            break;
        }
        if (buffer[i + 1] != TM0CNT_H) {
            failed = true;
            break;
        }

        timer_val += diff;
    }

    // LAB_0800d2ca
    set_IME(IME);
    return failed;
}

u32 eWRAM_test(u32* buffer) {
    u32 flags = 0;

    if (const8_fill_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, 0x55, &buffer[0], &buffer[1])) {
        flags |= memory_test_const8_fill;
    }

    if (set_incrementing_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= memory_test_incrementing_fill;
    }

    if (const32_fill_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, 0x55, &buffer[4], &buffer[5])) {
        flags |= memory_test_const32_fill;
    }

    if (DMA16_test(EWRAM_START, 2)) {
        flags |= memory_test_dma16;
    }

    if (DMA32_test(EWRAM_START, 2, 0x10)) {
        flags |= memory_test_dma32;
    }

    *EWRAM_START = 1;
    *(EWRAM_START + 1) = 0x23;
    if (*(u16*)EWRAM_START != 0x2301) {
        flags |= memory_test_endianness;
    }

    return flags;
}

u32 cpu_external_work_ram() {
    u32 buffer[8];
    return eWRAM_test(buffer);
}