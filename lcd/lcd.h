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
void wait_for_vcount_match();

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

/*
 * function at 08003760
 * return at 080037b8
 * Reads VCount, waits for (at least) 1 full line (HBlank interrupt should have happened now) and returns.
 * */
void wait_for_line();

/*
 * function at 080036a8
 * return at 08003742
 * return flags:
 *  1: No HBlank interrupt requested in IF when one should have.
 *  2: HBlank interrupt requested in IF when it shouldn't have been.
 * */
u32 hblank_intr_flag();

/*
 * function at 08003874
 * return at 08003876
 * Reads VCount, waits for (at least) 1 full frame (VBlank interrupt should have happened now) and returns.
 * */
void wait_for_frame();

/*
 * function at 080037bc
 * return at 08003856
 * return flags:
 *  1: No VBlank interrupt requested in IF when one should have.
 *  2: VBlank interrupt requested in IF when it shouldn't have been.
 * */
u32 vblank_intr_flag();

/*
 * function at 08003cf4
 * return at 08003d30
 *
 * Runs the vcount status tests. Waits until the VCount status changes, then stores the value of DISPSTAT and of
 * VCOUNT to the buffer passed as a parameter.
 * */
typedef struct s_vcount_status_test_value {
    u16 DISPSTAT;
    u16 VCOUNT;
} s_vcount_status_test_value;
u32 run_vcount_status_test(u32 _buffer, u32 length);

/*
 * function at 080038e8
 * return at 080039fe
 *
 * Runs the above function and compares the result values.
 * return flags:
 *  1: VCount match flag not set in DISPSTAT
 *  2: VCount value on match is not the same as the value in DISPSTAT
 * */
u32 vcount_status();

/*
 * function at 08003d38
 * return at 08003da0
 *
 * Runs the HBlank status tests. Waits until the HBlank status changes, then stores the value of DISPSTAT and of
 * TM0CNT_L to the buffer passed as a parameter.
 * */
typedef struct s_hblank_status_test_value {
    u16 DISPSTAT;
    u16 TM0CNT;
} s_hblank_status_test_value;
u32 run_hblank_status_test(u32 buffer, u32 length);

/*
 * function at 08003a1c
 * return at 08003b98
 *
 * Runs the above function and compares the result values.
 * return flags:
 *   1: time in HBlank was not between 209 and 225 cycles
 *   2: time in HDraw was not between 990 and 1009 cycles
 * */
u32 hblank_status();

/*
 * function at 08003dac
 * return at 08003dec
 *
 * Runs the VBlank status tests. Waits until the VBlank status changes, then stores the value of DISPSTAT and of
 * VCount to the buffer passed as a parameter.
 * */
u32 run_vblank_status_test(u32 buffer, u32 length);

/*
 * function at 08003bbc
 * return at 08003c6a
 *
 * Runs the above function and compares the result values.
 * return flags:
 *  1: VBlank flag not set in line 160 OR VBlank flag not cleared in line 227
 * I think the intent was to have 2 flags, but this seems to be a bug in the software.
 * */
u32 vblank_status();

#endif //AGS_LCD_H
