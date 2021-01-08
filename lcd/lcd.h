#ifndef AGS_LCD_H
#define AGS_LCD_H

#include "default.h"

/*
 * function at 08003c88
 * return at 08003ce4
 * Runs a VCount test for a certain number of scanlines. Writes the time a frame took + the value of VCount to a buffer
 * passed as first parameter of the length of the second parameter (2 shorts per sample).
 * */
u32 run_vcounter_test(u32 buffer, u32 number_of_tests);

/*
 * function at 08003374
 * return at 08003550
 * Main test entrypoint for the vcounter test.
 *
 * return flags:
 *  1: TM0 values outside of [0x4c1..0x4d1] (inclusive)
 *  2: VCount value failed.
 * */
typedef struct s_vcount_test_value {
    u16 VCount;
    u16 TM0;
} s_vcount_test_values;
u32 vcounter();

/*
 * function at 0800362c
 * return at 080036a4
 * Runs the VCount intr flag test. Basically, sets the VCount match to 16 scanlines ahead, and waits for VCount
 * to catch up. This will freeze a system if VCount doesn't work at all.
 * */
void test_vcount_intr_flag();

/*
 * function at 08003574
 * return at 0800360e
 * Tests if the VCount interrupt flag will be set.
 *
 * return flags:
 *  1: No VCount interrupt requested in IF when one should have.
 *  2: VCount interrupt requested in IF when it shouldn't have been.
 * */
u32 vcount_intr_flag();

#endif //AGS_LCD_H
