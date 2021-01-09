#ifndef AGS_TIMER_H
#define AGS_TIMER_H

#include "default.h"

/*
 * function at 0800925c
 * return at 08009290
 * Runs a single prescaler test given a pointer to TMxCNT and a setting
 * */
u32 run_prescaler_test(u32 tmcnt, u32 setting);

/*
 * function at 08008fb4
 * return at 08009070
 * return flags:
 *  4 bits per timer: lowest bit is for the lowest prescaler. Lowest nibble is for timer 0, highest for timer 3.
 * */
static const u32 prescalers[4] = {
    0x1000, 0x40, 0x10, 0x1
};

u32 prescaler();

/*
 * function at 08009294
 * return at 080092f4
 * Runs the test for timer count-up. Basically resets the timers and sets them to count up mode, sets the reload value
 * to the parameter passed (0xfffe) and ticks for 0x400 ticks. Then returns the value in timer 3.
 * */
u32 run_timer_connect_test(u32 reload, u32 _);

/*
 * function at 08009090
 * return at 080090ea
 * Compares the value gotten from the above function with 0x200.
 * */
u32 timer_connect();

/*
 * function at 080092fc
 * return at 08009304
 * busy loops for the amount of ticks passed.
 * */
u32 wait_ticks(u32 ticks, u32 _);

/*
 * function at 08009108
 * return at 0800923c
 * Similar to the lcd interrupt flag tests. Return flags: 2 bits per timer, bottom 2 for timer 0, then 2 for each timer.
 *  1: timer IRQ was not requested when it should have been.
 *  2: timer IRQ was requested when it shouldn't have been.
 * */
u32 timer_intr_flag();

#endif //AGS_TIMER_H
