#include "interrupts.h"
#include "AGS.h"
#include "lcd/lcd.h"


u32 vblank() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 0, 0);

    // allow VBlank interrupt
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_VBlankIRQ;
    *ptr_InterruptCheckFlag = 0;
    *ptr_IF = intr_VBlank;
    set_IE(intr_VBlank);
    set_IME(1);

    wait_for_frame();

    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_VBlank) == 0) {
        flags |= 1;
    }

    // same, but disable VBlank in IE
    *ptr_IF = intr_VBlank;
    set_IE(0);
    set_IME(1);

    wait_for_frame();

    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_VBlank) != 0) {
        flags |= 2;
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 hblank() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 0, 0);

    // allow HBlank interrupt
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_HBlankIRQ;
    *ptr_InterruptCheckFlag = 0;
    *ptr_IF = intr_HBlank;
    set_IE(intr_HBlank);
    set_IME(1);

    wait_for_line();

    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_HBlank) == 0) {
        flags |= 1;
    }

    // same, but disable HBlank in IE
    *ptr_IF = intr_HBlank;
    set_IE(0);
    set_IME(1);

    wait_for_line();

    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_HBlank) != 0) {
        flags |= 2;
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 vcount() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 0, 0);

    // allow VCount interrupt
    u16 DISPSTAT = *ptr_DISPSTAT;
    *ptr_DISPSTAT = DISPSTAT_VCountIRQ;
    *ptr_InterruptCheckFlag = 0;
    *ptr_IF = intr_VCount;
    set_IE(intr_VCount);
    set_IME(1);

    wait_for_vcount_match();

    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_VCount) == 0) {
        flags |= 1;
    }

    // same, but disable VCount in IE
    *ptr_IF = intr_VCount;
    set_IE(0);
    set_IME(1);

    wait_for_vcount_match();

    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_VCount) != 0) {
        flags |= 2;
    }
    *ptr_DISPSTAT = DISPSTAT;
    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 timer() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 0, 0);

    for (int timer = 0; timer < 4; timer++) {
        // allow timer interrupt
        *ptr_InterruptCheckFlag = 0;
        *ptr_IF = intr_Timer0 << timer;
        set_IE(intr_VCount);
        set_IME(1);

        wait_for_timer_overflow(timer);

        set_IME(0);

        // check result
        if (((*ptr_InterruptCheckFlag) & (intr_Timer0 << timer)) == 0) {
            flags |= 1 << (2 * timer);
        }

        // same, but disable timer irq in IE
        *ptr_IF = intr_Timer0 << timer;
        set_IE(0);
        set_IME(1);

        wait_for_timer_overflow(timer);

        set_IME(0);

        // check result
        if (*ptr_InterruptCheckFlag != 0) {
            // ^ this is actually how it's checked, it probably doesn't make a difference, but it isn't right
            flags |= 2 << (2 * timer);
        }
    }

    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 sio() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 0, 0);

    // allow SIO interrupt
    *ptr_InterruptCheckFlag = 0;
    *ptr_IF = intr_SIO;
    set_IE(intr_SIO);
    set_IME(1);

    wait_for_sio_complete();
    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_SIO) == 0) {
        flags |= 1;
    }

    // same, but disable SIO in IE
    *ptr_IF = intr_SIO;
    set_IE(0);
    set_IME(1);

    wait_for_sio_complete();
    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_SIO) != 0) {
        flags |= 2;
    }
    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 dma() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 0, 0);

    for (int channel = 0; channel < 4; channel++) {
        // allow DMA interrupt
        *ptr_InterruptCheckFlag = 0;
        *ptr_IF = intr_DMA0 << channel;
        set_IE(intr_VCount);
        set_IME(1);

        wait_for_dma_complete(channel);

        set_IME(0);

        // check result
        if (((*ptr_InterruptCheckFlag) & (intr_DMA0 << channel)) == 0) {
            flags |= 1 << (2 * channel);
        }

        // same, but disable timer irq in IE
        *ptr_IF = intr_DMA0 << channel;
        set_IE(0);
        set_IME(1);

        wait_for_dma_complete(channel);

        set_IME(0);

        // check result
        if (((*ptr_InterruptCheckFlag) & (intr_DMA0 << channel)) != 0) {
            flags |= 2 << (2 * channel);
        }
    }

    set_interrupt_settings(&interrupt_settings);
    return flags;
}

u32 keypad() {
    u32 flags = 0;

    s_interrupt_settings interrupt_settings = {};

    clear_interrupt_settings(&interrupt_settings, 0, 0);

    // allow SIO interrupt
    *ptr_InterruptCheckFlag = 0;
    *ptr_IF = intr_Keypad;
    set_IE(intr_Keypad);
    set_IME(1);

    wait_for_key_interrupt();
    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_Keypad) == 0) {
        flags |= 1;
    }

    // same, but disable keypad in IE
    *ptr_IF = intr_Keypad;
    set_IE(0);
    set_IME(1);

    wait_for_key_interrupt();
    set_IME(0);

    // check result
    if (((*ptr_InterruptCheckFlag) & intr_Keypad) != 0) {
        flags |= 2;
    }
    set_interrupt_settings(&interrupt_settings);
    return flags;
}