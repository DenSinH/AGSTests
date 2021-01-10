#ifndef AGS_INTERRUPTS_H
#define AGS_INTERRUPTS_H

#include "default.h"

/*
 * function at 0800c4f0
 * return at 0800c5be
 *
 * Checks that VBlank interrupts occur when IE allows them, and don't when IE masks them out.
 * return flags:
 *  1: No VBlank interrupt happened when it should have.
 *  2: VBlank interrupt happened when it shouldn't have been.
 * */
u32 vblank();

/*
 * function at 0800c5dc
 * return at 0800c6aa
 *
 * Checks that HBlank interrupts occur when IE allows them, and don't when IE masks them out.
 * return flags:
 *  1: No HBlank interrupt happened when it should have.
 *  2: HBlank interrupt happened when it shouldn't have been.
 * */
u32 hblank();

/*
 * function at 0800c6c8
 * return at 0800c796
 *
 * Checks that VCount interrupts occur when IE allows them, and don't when IE masks them out.
 * return flags:
 *  1: No VCount interrupt happened when it should have.
 *  2: VCount interrupt happened when it shouldn't have been.
 * */
u32 vcount();

/*
 * function at 0800c8c8
 * return at 0800c90c
 *
 * Waits for a timer overflow to happen by busy waiting
 * */
void wait_for_timer_overflow(u32 timer);

/*
 * function at 0800c7b4
 * return at 0800c8a8
 *
 * Checks that timer interrupts occur when IE allows them for each timer, and don't when IE masks them out.
 * return flags: 2 bits per timer. LSB = TM0
 *  1: No timer interrupt happened when it should have.
 *  2: timer interrupt happened when it shouldn't have been.
 * */
u32 timer();

/*
 * function at 0800c9f0
 * return at 0800ca46
 *
 * Starts a serial transfer with interrupt at the end and waits for it to complete.
 * */
void wait_for_sio_complete();

/*
 * function at 0800c924
 * return at 0800c9d2
 *
 * Checks that SIO interrupts occur when IE allows them, and don't when IE masks them out.
 * return flags:
 *  1: No SIO interrupt happened when it should have.
 *  2: SIO interrupt happened when it shouldn't have been.
 * */
u32 sio();

/*
 * function at 0800cb6c
 * return at 0800cbec
 *
 * Starts a DMA transfer with interrupt at the end and waits for it to complete.
 * */
void wait_for_dma_complete(u32 channel);

/*
 * function at 0800ca48
 * return at 0800cb4c
 *
 * Checks that DMA interrupts occur when IE allows them for each timer, and don't when IE masks them out.
 * return flags: 2 bits per DMA channel. LSB = DMA0
 *  1: No DMA interrupt happened when it should have.
 *  2: DMA interrupt happened when it shouldn't have been.
 * */
u32 dma();

/*
 * function at 0800ccd4
 * return at 0800cd2a
 *
 * Depending on the current state of KEYINPUT, sets KEYCNT so that an interrupt inevitably happens.
 * */
void wait_for_key_interrupt();

/*
 * function at 0800cbf0
 * return at 0800ccb6
 *
 * Checks that keypad interrupts occur when IE allows them, and don't when IE masks them out.
 * return flags:
 *  1: No keypad interrupt happened when it should have.
 *  2: keypad interrupt happened when it shouldn't have been.
 * */
u32 keypad();

#endif //AGS_INTERRUPTS_H
