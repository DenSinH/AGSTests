#include "AGS.h"

void clear_interrupt_handlers(s_interrupt_settings* old, u16 IME, u16 IE) {
    u16 _IME = set_IME(0);
    u16 _IE  = set_IE(IE);

    old->IME = _IME;
    old->IE  = _IE;
    for (int i = 0; i < 0x10; i++) {
        old->callbacks[i] = ptr_interrupt_handler_table[i];
        ptr_interrupt_handler_table[i] = NULL;
    }
    *ptr_IF = 0x3fff;
    set_IME(IME);
}

void* set_interrupt_handler(u16 intr, void* callback) {
    int i;
    for (i = 0; i < 0x10 && ((intr & 1) == 0); i++) {
        intr >>= 1;
    }
    void* old = ptr_interrupt_handler_table[i];
    ptr_interrupt_handler_table[i] = callback;
    return old;
}

void set_interrupt_settings(s_interrupt_settings* new) {
    set_IME(0);
    for (int i = 0; i < 0x10; i++) {
        ptr_interrupt_handler_table[i] = new->callbacks[i];
    }
    set_IE(new->IE);
    set_IME(new->IME);
}

void wait_for_interrupt(u16 interrupt) {
    *ptr_InterruptCheckFlag &= ~interrupt;
    do { } while (!((*ptr_InterruptCheckFlag) & interrupt));
}
