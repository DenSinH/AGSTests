#ifndef AGS_DMA_H
#define AGS_DMA_H

#include "default.h"

enum DMA_addr_control {
    DMA_addr_increment        = 0,
    DMA_addr_decrement        = 1,
    DMA_addr_fixed            = 2,
    DMA_addr_increment_reload = 3,
};

/*
 * function at 0800f188
 * return at 0800f1f2
 *
 * Compares 2 arrays and returns 1 if they are not equal, 0 otherwise.
 * */
u32 check_DMA_test(void* dest, size_t len, void* expected, u32 check_size, u32* failed_address, u32* failed_value);

/*
 * function at 08009308
 * return at 0800a282
 *
 * Main function for testing DMA address control. Tested for the DMA channel index passed as argument.
 * Return flags (bits):
 *
 * 0:  any test failed / any !HBlank test failed
 * 1:  any HBlank test failed
 * 2:  any SrcAddrControl == 0 (increment) test failed
 * 3:  any SrcAddrControl == 1 (decrement) test failed
 * 4:  any SrcAddrControl == 2 (fixed) test failed
 * 5:  any DestAddrControl == 0 (increment) test failed
 * 6:  any DestAddrControl == 1 (decrement) test failed
 * 7:  any DestAddrControl == 2 (fixed) test failed
 * 8:  any DestAddrControl == 3 (increment + reload) test failed
 * 9:  any !WordSized test failed
 * 10: any WordSized test failed
 * */
u32 DMA_address_control(u16 channel);

/*
 * functions at 0800a284, 0800a29c, 0800a2b4, 0800a2cc
 * returns at 0800a298, 0800a2b0, 0800a2c8, 0800a2e0
 *
 * These functions are all basically the same, except they call the DMA_address_control test for the respective channel.
 * */
u32 DMA0_address_control();
u32 DMA1_address_control();
u32 DMA2_address_control();
u32 DMA3_address_control();

/*
 * function at 0800a34c
 * return at 0800a46a
 *
 * Sets the DMA start timing to VBlank and transfers VCount to EWRAM once. Checks if the value transferred is 160.
 * return flags are 1 bit for each DMA channel, LSB = DMA0, etc.
 * */
u32 DMA_vblank_start();

/*
 * function at 0800a488
 * return at 0800a612
 *
 * Sets the DMA start timing to HBlank and transfers VCount to EWRAM for 228 HBlanks. Checks if all the values
 * transferred are incrementing values from 0 to 159 (once for each visible line), and then 0 for the remaining
 * 68 values (no HBlank DMA in VBlank).
 * return flags are 1 bit for each DMA channel, LSB = DMA0, etc.
 * */
u32 DMA_hblank_start();

/*
 * function at 0800a630
 * return at 0800a784
 *
 * Sets the DMA start timing for DMA3 to special and transfers VCont to EWRAM for 1 frame. Checks if all the values
 * transferred are incrementing values from 2 to 161 (video DMA starts in line 2 and ends in 162) and then 68 zeros\
 * (no DMA for the remaining lines).
 * return flags:
 *  1: DMA not ended in line 162
 *  2: DMA did not transfer the right values
 * */
u32 DMA_display_start();

/*
 * function at 0800a838
 * return at 0800a9d0
 *
 * Very similar to the timer/lcd interrupt flag tests. Basically, clears IF, enables 2 immediate DMAs, one with
 * IRQ and one without and checks if the IF bit is set appropriately.
 * return flags: 2 bits per channel. LSB = DMA0
 * 1: IRQ not requested when it should have been
 * 2: IRQ requested when it shouldn't have been
 * */
u32 DMA_intr_flag();

#endif //AGS_DMA_H
