#include "dma.h"
#include "expected.h"
#include "AGS.h"
#include "../memory/memory.h"

u32 DMA_address_control(u16 channel) {
    u32 flags = 0;
    u32 failed[48] = {};  // array of bools for each setting that failed

    u8 incrementing_buffer[0x100];
    s_interrupt_settings old_settings = {};

    u8 buffer[0x2c] = {};
    memset(buffer, 0, sizeof(buffer));
    clear_interrupt_settings(&old_settings, 1, intr_HBlank);
    u16 DISPSTAT = *ptr_DISPSTAT;

    *ptr_DISPSTAT = DISPSTAT_HBLankIRQ;  // HBlank IRQ enable
    for (int i = 0; i < 0x100; i++) {
        incrementing_buffer[i] = i;
    }

    *ptr_DMACNT(channel) = 0;
    for (u32 HBlankDMA = 0; HBlankDMA < 2; HBlankDMA++) {
        for (u32 SrcAddrControl = 0; SrcAddrControl < 3; SrcAddrControl++) {
            for (u32 DestAddrControl = 0; DestAddrControl < 4; DestAddrControl++) {
                for (u32 WordSized = 0; WordSized < 2; WordSized++) {
                    // function commonly used in the memory tests to set a buffer to a constant
                    mem_set_const(EWRAM_START, 0x100, 0, mem_set_u32);
                    u32 DMASAD;
                    if (SrcAddrControl == DMA_addr_decrement) {
                        if (WordSized) {
                            DMASAD = (u32)incrementing_buffer - 4;
                        }
                        else {
                            DMASAD = (u32)incrementing_buffer - 2;
                        }
                    }
                    else if (SrcAddrControl == DMA_addr_increment) {
                        DMASAD = (u32)incrementing_buffer;
                    }
                    else if (SrcAddrControl == DMA_addr_fixed) {
                        DMASAD = (u32)&incrementing_buffer[0x80];
                    }

                    u32 DMADAD;
                    if (DestAddrControl == DMA_addr_decrement) {
                        if (WordSized) {
                            DMADAD = (u32)EWRAM_START + 0xfc;
                        }
                        else {
                            DMADAD = (u32)EWRAM_START + 0xfe;
                        }
                    }
                    else if (SrcAddrControl == DMA_addr_increment) {
                        DMADAD = (u32)EWRAM_START;
                    }
                    else if (SrcAddrControl == DMA_addr_fixed) {
                        DMADAD = (u32)EWRAM_START + 0x80;
                    }

                    u32 DMACNT = 0;
                    if (!HBlankDMA) {
                        // length
                        if (!WordSized) {
                            DMACNT |= 0x80;
                        }
                        else {
                            DMACNT |= 0x40;
                        }
                    }
                    else {
                        // 4 words each frame
                        if (!WordSized) {
                            DMACNT |= ((DMAStartHBlank | DMARepeat) << 16) | 8;
                        }
                        else {
                            DMACNT |= ((DMAStartHBlank | DMARepeat) << 16) | 4;
                        }
                    }

                    DMACNT |= ((WordSized << 10)
                           |   (SrcAddrControl << 7)
                           |   (DestAddrControl << 5)
                           |    DMAEnable) << 16;
                    if (HBlankDMA == 0) {
                        // just fire the DMA once
                        *ptr_DMASAD(channel) = DMASAD;
                        *ptr_DMADAD(channel) = DMADAD;
                        *ptr_DMACNT(channel) = DMACNT;
                    }
                    else if (HBlankDMA == 1) {
                        // fire for 16 frames

                        // wait for the first HBlank interrupt
                        do { } while ((*ptr_VCOUNT) > 142);
                        wait_for_interrupt(intr_HBlank);

                        *ptr_DMASAD(channel) = DMASAD;
                        *ptr_DMADAD(channel) = DMADAD;
                        *ptr_DMACNT(channel) = DMACNT;
                        for (int HBlank = 0; HBlank < 0x10; HBlank++) {
                            wait_for_interrupt(intr_HBlank);
                        }

                        // disable DMA
                        *ptr_DMACNT(channel) = 0;
                    }
                    // wait for DMA to finish
                    do { } while (((*ptr_DMACNT(channel)) & (DMAEnable << 16)));

                    u32 failed_value;
                    u32 failed_address;
                    u32 index = WordSized + DestAddrControl * 0x2 + SrcAddrControl * 0x8 + HBlankDMA * 0x18;
                    failed[index] = check_DMA_test(
                        EWRAM_START,
                        0x100,
                        expected[index],
                        0, // byte sized check
                        &failed_address,
                        &failed_value
                    );
                    if (failed[index]) {
                        flags = 1;
                    }
                }
            }
        }
    }

    /*
     * Since there are so many different tests, they try to fold the flags into only 11 bits.
     * I think the idea is that the bottom bit signifies that any test failed (see above),
     * but from the loop at the bottom, it appears that this bit is also for the non-HBlank tests...
     * The code is really convoluted in GHidra...
     * I tried to compress it down a bit as well because it was really unmanagable, and probably not written in the
     * way it was in GHidra's decompression (I hope). I suspect it was written with some inlined function or
     * whatever.
     * */
    u32 failed_compressed[12] = {};
    /*
     * failed_compressed array:
     * 0: !HBlank
     * 1: HBlank
     * 2: SrcAddrControl == 0
     * 3: SrcAddrControl == 1
     * 4: SrcAddrControl == 2
     * 5: DestAddrControl == 0
     * 6: DestAddrControl == 1
     * 7: DestAddrControl == 2
     * 8: DestAddrControl == 3
     * 9: !WordSized
     * 10: WordSized
     * */
    for (u32 HBlankDMA = 0; HBlankDMA < 2; HBlankDMA++) {
        for (u32 SrcAddrControl = 0; SrcAddrControl < 3; SrcAddrControl++) {
            for (u32 DestAddrControl = 0; DestAddrControl < 4; DestAddrControl++) {
                for (u32 WordSized = 0; WordSized < 2; WordSized++) {
                    u32 index = WordSized + DestAddrControl * 0x2 + SrcAddrControl * 0x8 + HBlankDMA * 0x18;

                    failed_compressed[HBlankDMA]           |= failed[index];
                    failed_compressed[2 + SrcAddrControl]  |= failed[index];
                    failed_compressed[5 + DestAddrControl] |= failed[index];
                    failed_compressed[10 + WordSized]      |= failed[index];
                }
            }
        }
    }

    for (int i = 0; i < 11; i++) {
        if (failed_compressed[i]) {
            flags |= 1 << i;
        }
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_interrupt_settings(&old_settings);
    return flags;
}

u32 DMA0_address_control() {
    return DMA_address_control(0);
}

u32 DMA1_address_control() {
    return DMA_address_control(1);
}

u32 DMA2_address_control() {
    return DMA_address_control(2);
}

u32 DMA3_address_control() {
    return DMA_address_control(3);
}

u32 DMA_vblank_start() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 1, intr_VBlank);
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_VBlankIRQ;  // enable VBlank IRQ
    for (int channel = 0; channel < 4; channel++) {
        // wait for the new frame
        do { } while ((*ptr_VCOUNT) != 227);
        do { } while ((*ptr_VCOUNT) == 227);
        *EWRAM_START = 0;
        *ptr_DMASAD(channel) = ptr_VCOUNT;
        *ptr_DMADAD(channel) = EWRAM_START;
        *ptr_DMACNT(channel) = ((DMAEnable | DMAStartVBlank | DMASrcFixed | DMADestFixed) << 16) | 1;
        wait_for_interrupt(intr_VBlank);
        *ptr_DMACNT(channel) = 0;
        if (*(u16*)EWRAM_START != 160) {
            flags |= 1 << channel;
        }
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 DMA_hblank_start() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 1, intr_VBlank);
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_VBlankIRQ;  // enable VBlank IRQ
    for (int channel = 0; channel < 4; channel++) {
        // clear EWRAM
        mem_set_const(EWRAM_START, 456, 0, mem_set_u32);

        // wait for the new frame
        do { } while ((*ptr_VCOUNT) != 227);
        do { } while ((*ptr_VCOUNT) == 227);
        *EWRAM_START = 0;
        *ptr_DMASAD(channel) = ptr_VCOUNT;
        *ptr_DMADAD(channel) = EWRAM_START;
        *ptr_DMACNT(channel) = ((DMAEnable | DMAStartHBlank | DMARepeat | DMASrcFixed | DMADestIncrement) << 16) | 1;

        for (int i = 0; i < 228; i++) {
            wait_for_interrupt(intr_HBlank);
        }
        *ptr_DMACNT(channel) = 0;

        int i = 0;
        u16* EWRAM = EWRAM_START;

        // visible lines should transfer VCount
        for (; i < 160; i++) {
            if (EWRAM[i] != i) {
                flags |= 1 << channel;
            }
        }

        // in VBlank no HBlank DMA happens
        for (; i < 228; i++) {
            if (EWRAM[i]) {
                flags |= 1 << channel;
            }
        }
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 DMA_display_start() {
    u32 flags = 0;

    u16 IME = set_IME(0);

    // clear EWRAM
    mem_set_const(EWRAM_START, 456, 0, mem_set_u32);

    // wait for next frame
    do { } while ((*ptr_VCOUNT) != 227);
    do { } while ((*ptr_VCOUNT) == 227);

    *ptr_DMA3SAD = ptr_VCOUNT;
    *ptr_DMA3DAD = EWRAM_START;
    *ptr_DMA3CNT = ((DMAEnable | DMAStartSpecial | DMARepeat | DMASrcFixed | DMADestIncrement) << 16) | 1;

    // wait for next frame
    do { } while ((*ptr_VCOUNT) != 227);
    do { } while ((*ptr_VCOUNT) == 227);
    // wait until video DMA should be done
    do { } while ((*ptr_VCOUNT) == 162);

    // fail if DMA is still busy
    if ((*ptr_DMA3CNT) & (DMAEnable << 16)) {
        flags |= 1;
    }
    *ptr_DMA3CNT = 0;
    u16* EWRAM = EWRAM_START;
    int i = 0;
    for (; i < 160; i++) {
        if (EWRAM[i] != i + 2) {
            flags |= 2;
        }
    }

    for (; i < 228; i++) {
        if (EWRAM[i]) {
            flags |= 2;
        }
    }

    set_IME(IME);
    return flags;
}

u32 DMA_intr_flag() {
    u32 flags = 0;

    u16 IME = set_IME(0);
    for (int channel = 0; channel < 4; channel++) {
        *ptr_IF = intr_DMA0 << channel;  // clear interrupt
        *ptr_DMASAD(channel) = ptr_VCOUNT;
        *ptr_DMADAD(channel) = EWRAM_START;
        *ptr_DMACNT(channel) = ((DMAEnable | DMAIRQ | DMAStartImmediate | DMASrcFixed | DMADestIncrement) << 16) | 1;

        // wait for DMA to finish
        do { } while ((*ptr_DMACNT(channel)) * (DMAEnable << 16));
        if (((*ptr_IF) & (intr_DMA0 << channel)) == 0) {
            flags |= 1 << (2 * channel);
        }

        *ptr_IF = intr_DMA0 << channel;  // clear interrupt
        *ptr_DMASAD(channel) = ptr_VCOUNT;
        *ptr_DMADAD(channel) = EWRAM_START;
        *ptr_DMACNT(channel) = ((DMAEnable | DMAStartImmediate | DMASrcFixed | DMADestIncrement) << 16) | 1;

        // wait for DMA to finish
        do { } while ((*ptr_DMACNT(channel)) * (DMAEnable << 16));
        if (((*ptr_IF) & (intr_DMA0 << channel)) != 0) {
            flags |= 2 << (2 * channel);
        }
    }
    set_IME(IME);
    return flags;
}
