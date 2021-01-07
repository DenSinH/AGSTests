#include "./memory.h"

u32 cpu_external_work_ram() {
    u32 buffer[8];
    return eWRAM_test(buffer);
}

u32 eWRAM_test(u32* buffer) {
    u32 flags = 0;

    if (const8_fill_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, 0x55, &buffer[0], &buffer[1])) {
        flags |= ewram_test_const8_fill;
    }

    if (set_incrementing_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= ewram_test_incrementing_fill;
    }

    if (const32_fill_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
        flags |= ewram_test_const32_fill;
    }

    if (DMA16_test(EWRAM_START, 2)) {
        flags |= ewram_test_dma16;
    }

    if (DMA32_test(EWRAM_START, 2, 0x10)) {
        flags |= ewram_test_dma32;
    }

    *EWRAM_START = 1;
    *(EWRAM_START + 1) = 0x23;
    if (*(u16*)EWRAM_START != 0x2301) {
        flags |= ewram_test_endianness;
    }

    return flags;
}

u32 cpu_internal_work_ram() {
    u32 buffer[8];
    return iWRAM_test(buffer);
}

u32 iWRAM_test(u32* buffer) {
    s_interrupt_settings old_interrupt_settings = {};
    u16 DISPSTAT;

    // setup interrupt handler to the iWRAM test
    clear_interrupt_settings(&old_interrupt_settings, 0, 0);
    set_interrupt_handler(intr_VBlank, &iWRAM_test_async);
    *ptr_InterruptCheckFlag = 0;

    DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_VBlankIRQ;

    set_IE(intr_VBlank);
    set_IME(1);

    do { } while ((*ptr_InterruptCheckFlag & 1) == 0);

    // restore settings
    *ptr_DISPSTAT = DISPSTAT;
    set_interrupt_settings(&old_interrupt_settings);

    for (int i = 0; i < 6; i++) {
        buffer[i] = ptr_iWRAM_results->fail_data[i];
    }
    return ptr_iWRAM_results->flags;
}

void iWRAM_test_async() {
    u32 flags = 0;
    u32 buffer[6] = {};

    // first DMA all of iWRAM to eWRAM to not destroy data
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = IWRAM_START;
    *ptr_DMA3DAD = EWRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (IWRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    if (const8_fill_test(IWRAM_START, IWRAM_START + IWRAM_LENGTH, 0x55, &buffer[0], &buffer[1])) {
        flags |= iwram_test_const8_fill;
    }

    if (set_incrementing_test(IWRAM_START, IWRAM_START + IWRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= iwram_test_incrementing_fill;
    }

    if (const32_fill_test(IWRAM_START, IWRAM_START + IWRAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
        flags |= iwram_test_const32_fill;
    }

    if (DMA16_test(IWRAM_START, 2)) {
        flags |= iwram_test_dma16;
    }

    if (DMA32_test(EWRAM_START, 2, 0x10)) {
        flags |= iwram_test_dma32;
    }

    // restore iWRAM
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = EWRAM_START;
    *ptr_DMA3DAD = IWRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (IWRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    // copy settings
    for (int i = 0; i < 6; i++) {
        ptr_iWRAM_results->fail_data[i] = buffer[i];
    }
    ptr_iWRAM_results->flags = flags;
}