#include "./memory.h"
#include <string.h>

u32 cpu_external_work_ram() {
    u32 buffer[8];
    return eWRAM_test(buffer);
}

u32 eWRAM_test(u32* buffer) {
    u32 flags = 0;

    if (const8_fill_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, 0x55, &buffer[0], &buffer[1])) {
        flags |= memory_test_const8_fill;
    }

    if (set_incrementing_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= memory_test_incrementing_fill;
    }

    if (const32_fill_test(EWRAM_START, EWRAM_START + EWRAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
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
        flags |= memory_test_const8_fill;
    }

    if (set_incrementing_test(IWRAM_START, IWRAM_START + IWRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= memory_test_incrementing_fill;
    }

    if (const32_fill_test(IWRAM_START, IWRAM_START + IWRAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
        flags |= memory_test_const32_fill;
    }

    if (DMA16_test(IWRAM_START, 0)) {
        flags |= memory_test_dma16;
    }

    if (DMA32_test(IWRAM_START, 0, 0x10)) {
        flags |= memory_test_dma32;
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

u32 palette_ram() {
    u32 buffer[8];
    return PRAM_test(buffer);
}

u32 PRAM_test(u32* buffer) {
    u32 flags = 0;

    u16 IME = set_IME(0);
    u16 DISPCNT = *ptr_DISPCNT;
    *ptr_DISPCNT = 0x80;  // Forced Blank

    // first DMA all of PRAM to eWRAM to not destroy data
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = PRAM_START;
    *ptr_DMA3DAD = EWRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (PRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    if (const32_fill_test(PRAM_START, PRAM_START + PRAM_LENGTH, 0x55555555, &buffer[0], &buffer[1])) {
        flags |= memory_test_const8_fill;
    }

    if (set_incrementing_test(PRAM_START, PRAM_START + PRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= memory_test_incrementing_fill;
    }

    if (const32_fill_test(PRAM_START, PRAM_START + PRAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
        flags |= memory_test_const32_fill;
    }

    if (DMA16_test(PRAM_START, 0)) {
        flags |= memory_test_dma16;
    }

    if (DMA32_test(PRAM_START, 0, 0x10)) {
        flags |= memory_test_dma32;
    }

    // restore PRAM
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = EWRAM_START;
    *ptr_DMA3DAD = PRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (PRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    *ptr_DISPCNT = DISPCNT;
    set_IME(IME);
    return flags;
}

u32 vram() {
    u32 buffer[8];
    return VRAM_test_1(buffer) | VRAM_test_2(buffer);
}

u32 VRAM_test_1(u32* buffer) {
    u32 flags = 0;

    u16 IME = set_IME(0);
    u16 DISPCNT = *ptr_DISPCNT;
    *ptr_DISPCNT = 0x80;  // Forced Blank

    // first DMA all of VRAM to eWRAM to not destroy data
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = VRAM_START;
    *ptr_DMA3DAD = EWRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (VRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    if (const32_fill_test(VRAM_START, VRAM_START + VRAM_LENGTH, 0x55555555, &buffer[0], &buffer[1])) {
        flags |= memory_test_const8_fill;
    }

    if (set_incrementing_test(VRAM_START, VRAM_START + VRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= memory_test_incrementing_fill;
    }

    if (const32_fill_test(VRAM_START, VRAM_START + VRAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
        flags |= memory_test_const32_fill;
    }

    if (DMA16_test(VRAM_START, 0)) {
        flags |= memory_test_dma16;
    }

    if (DMA32_test(VRAM_START, 0, 0x10)) {
        flags |= memory_test_dma32;
    }

    // restore VRAM
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = EWRAM_START;
    *ptr_DMA3DAD = VRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (VRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    *ptr_DISPCNT = DISPCNT;
    set_IME(IME);
    return flags;
}

u32 VRAM_test_2(u32* buffer) {
    u32 flags = 0;

    s_affine_settings settings;

    // it's not actually memcpy'd, but essentially the same thing
    memcpy(&settings, &VRAM_test_2_BG2_settings, sizeof(struct s_affine_settings));

    u16 IME = set_IME(0);
    u16 DISPCNT = *ptr_DISPCNT;
    u16 BG2PA = *ptr_BG2PA;
    u16 BG2PB = *ptr_BG2PB;
    u16 BG2PC = *ptr_BG2PC;
    u16 BG2PD = *ptr_BG2PD;
    u32 BG2X  = *ptr_BG2X;
    u32 BG2Y  = *ptr_BG2Y;
    BGAffineSet(&settings, ptr_BG2PA, 1);
    *ptr_DISPCNT = 0x0403;  // mode 3, display BG2

    // first DMA all of VRAM to eWRAM to not destroy data
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = VRAM_START;
    *ptr_DMA3DAD = EWRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (VRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    if (const32_fill_test(VRAM_START, VRAM_START + VRAM_LENGTH, 0x55555555, &buffer[0], &buffer[1])) {
        flags |= 1;
    }

    if (set_incrementing_test(VRAM_START, VRAM_START + VRAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= 2;
    }

    if (const32_fill_test(VRAM_START, VRAM_START + VRAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
        flags |= 4;
    }

    if (const32_fill_test(VRAM_START, VRAM_START + VRAM_LENGTH, 0, &buffer[6], &buffer[7])) {
        flags |= 0x20;
    }

    if (const32_fill_test(VRAM_START, VRAM_START + VRAM_LENGTH, 0xffffffff, &buffer[6], &buffer[7])) {
        flags |= 0x40;
    }

    if (mem_read_write_checksum(VRAM_START, VRAM_START + VRAM_LENGTH, 0x0a1b2c4d, &buffer[6], &buffer[7])) {
        flags |= 0x10000000;
    }

    // restore VRAM
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = EWRAM_START;
    *ptr_DMA3DAD = VRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (VRAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    *ptr_BG2PA = BG2PA;
    *ptr_BG2PB = BG2PB;
    *ptr_BG2PC = BG2PC;
    *ptr_BG2PD = BG2PD;
    *ptr_BG2X  = BG2X;
    *ptr_BG2Y  = BG2Y;
    *ptr_DISPCNT = DISPCNT;
    set_IME(IME);
    return flags;
}

u32 oam() {
    u32 buffer[8];
    return OAM_test(buffer);
}

u32 OAM_test(u32* buffer) {
    u32 flags = 0;

    u16 IME = set_IME(0);
    u16 DISPCNT = *ptr_DISPCNT;
    *ptr_DISPCNT = 0x80;  // Forced Blank

    // first DMA all of OAM to eWRAM to not destroy data
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = OAM_START;
    *ptr_DMA3DAD = EWRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (OAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    if (const32_fill_test(OAM_START, OAM_START + OAM_LENGTH, 0x55555555, &buffer[0], &buffer[1])) {
        flags |= memory_test_const8_fill;
    }

    if (set_incrementing_test(OAM_START, OAM_START + OAM_LENGTH, &buffer[2], &buffer[3])) {
        flags |= memory_test_incrementing_fill;
    }

    if (const32_fill_test(OAM_START, OAM_START + OAM_LENGTH, 0xaaaaaaaa, &buffer[4], &buffer[5])) {
        flags |= memory_test_const32_fill;
    }

    if (DMA16_test(OAM_START, 0)) {
        flags |= memory_test_dma16;
    }

    if (DMA32_test(OAM_START, 0, 0x20)) {
        flags |= memory_test_dma32;
    }

    // restore OAM
    *ptr_DMA3CNT = 0;
    *ptr_DMA3SAD = EWRAM_START;
    *ptr_DMA3DAD = OAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAWordSized) << 16) | (OAM_LENGTH >> 2);
    do { } while ((*ptr_DMA3CNT) & 0x80000000);

    *ptr_DISPCNT = DISPCNT;
    set_IME(IME);
    return flags;
}

u32 cartridge_type_flag() {
    // just checks the top bit of WAITCNT
    return (*ptr_WAITCNT & 0x8000) != 0;
}

u32 prefetch_buffer_test() {
    // reset timer 0
    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x00800000;

    // r4 = ptr_TM0CNT
    __asm__(
    "ldr r2,[r4,#0x0]\n"
    "ldr r2,[r4,#0x0]\n"
    "ldr r2,[r4,#0x0]\n"
    "ldr r2,[r4,#0x0]\n"
    "ldr r2,[r4,#0x0]\n"
    "ldr r2,[r4,#0x0]\n"
    "ldr r2,[r4,#0x0]\n"
    "ldr r2,[r4,#0x0]"
    );

    // a halfword read with r0 right after:
    return *ptr_TM0CNT;
}

u32 prefetch_buffer() {
    u32 flags = 0;

    u16 IME = set_IME(0);
    u16 WAITCNT = *ptr_WAITCNT;

    // wait state 0: First access 3 cycles (setting 1)
    // wait state 0: Second access 1 cycle (setting 1)
    // Prefetch buffer enable
    *ptr_WAITCNT = 0x4014;
    if (prefetch_buffer_test() != 0x18) {
        flags |= 1;
    }

    // wait state 0: First access 3 cycles (setting 1)
    // wait state 0: Second access 1 cycle (setting 1)
    // Prefetch buffer disable
    *ptr_WAITCNT = 0x0014;
    if (prefetch_buffer_test() != 0x33) {
        flags |= 2;
    }

    *ptr_WAITCNT = WAITCNT;
    set_IME(IME);
    return flags;
}

u32 run_waitstate_wait_control_test(u32 waitstate, u32 setting) {
    u16 WAITCNT = *ptr_WAITCNT;
    *ptr_WAITCNT = WAITCNT & 0xf803 | (setting << (waitstate * 3 + 2));
    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x00800000;  // enable, reload

    // r2 holds a pointer to 08000000 (ROM start)
    // r3 is just a trash register
    __asm__(
        "ldr r3,[r2,#0x0]\n"
        "ldr r3,[r2,#0x0]\n"
        "ldr r3,[r2,#0x0]\n"
        "ldr r3,[r2,#0x0]"
    );

    // ldrh r0, [r4, #0] (load TM0CNT_L)
    u16 TM0CNT_L = *(u16*)ptr_TM0CNT;
    *ptr_WAITCNT = WAITCNT;
    return TM0CNT_L;
}

u32 waitstate_wait_control_test() {
    u32 flags = 0;

    u32 local_timer_values[3][8] = {};

    memcpy32(local_timer_values, wait_control_timer_values, sizeof(local_timer_values));
    u16 IME = set_IME(0);
    u32 counter = 0;

    u32 timer_value;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++, counter++) {
            timer_value = call_from_stack(run_waitstate_wait_control_test, i, j);
            if (timer_value != local_timer_values[i][j]) {
                flags |= 1 << counter;
            }
        }
    }

    set_IME(IME);
    return flags;
}

u32 run_cart_wait_control_test(u32 setting, u32 _) {
    u16 WAITCNT = *ptr_WAITCNT;
    *ptr_WAITCNT = WAITCNT & 0xfffc | setting;
    *ptr_TM0CNT = 0;
    *ptr_TM0CNT = 0x00800000;  // enable, reload

    // r2 holds a pointer to 0e000000 (SRAM start)
    // r3 is just a trash register
    __asm__(
        "ldr r3,[r2,#0x0]\n"
        "ldr r3,[r2,#0x0]\n"
        "ldr r3,[r2,#0x0]\n"
        "ldr r3,[r2,#0x0]"
    );

    // ldrh r0, [r4, #0] (load TM0CNT_L)
    u16 TM0CNT_L = *(u16*)ptr_TM0CNT;
    *ptr_WAITCNT = WAITCNT;
    return TM0CNT_L;
}

u32 cartrige_ram_wait_control() {
    u32 flags = 0;

    u32 local_timer_values[4] = {};

    local_timer_values[0] = cart_wait_control_timer_values[0];
    local_timer_values[1] = cart_wait_control_timer_values[1];
    local_timer_values[2] = cart_wait_control_timer_values[2];
    local_timer_values[3] = cart_wait_control_timer_values[3];
    u16 IME = set_IME(0);
    u32 counter = 0;

    for (int i = 0; i < 4; i++, counter++) {
        u32 timer_val = call_from_stack(run_cart_wait_control_test, i, 0);
        if (timer_val != local_timer_values[i]) {
            flags |= 1 << counter;
        }
    }
    set_IME(IME);
    return flags;
}