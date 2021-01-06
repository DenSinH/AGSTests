#ifndef AGS_MEMORY_H
#define AGS_MEMORY_H

#include "default.h"
#include "AGS.h"

#define EWRAM_START ((u8*)0x02000000)
#define EWRAM_LENGTH 0x00040000

/*
 * function at 080020e8
 * return at 080021ce
 *
 * General function used to test memory regions
 * */
enum ewram_test_flags : u32 {
    ewram_test_const8_fill       = 0x01,
    ewram_test_incrementing_fill = 0x02,
    ewram_test_const32_fill      = 0x04,
    ewram_test_dma16             = 0x08,
    ewram_test_dma32             = 0x10,
    ewram_test_endianness        = 0x20,
};

u32 eWRAM_test(u32* buffer);

/*
 * function at 0800ce6c
 * return at 0800ced6
 *
 * General function for testing reading and writing a constant u8 value.
 * Uses 2 of the below functions below to do this, and check it.
 * Writes the first invalid address to failed_address**, and the failed written value to failed_value.
 * Basically tests byte writes.
 * */
bool const8_fill_test(u8* buffer, u8 *buffer_end, u32 value, u8** failed_address, u32* failed_value);

/*
 * function at 0800ce6c
 * return at 0800ced6
 *
 * General function for testing reading and writing a incrementing values.
 * Uses 2 of the below functions below to do this, and check
 * it. Writes the first invalid address to failed_address**, and the failed written value to failed_value.
 * Basically tests halfword writes.
 * */
bool set_incrementing_test(u8* buffer, u8 *buffer_end, u8** failed_address, u32* failed_value);

/*
 * function at 0800ce6c
 * return at 0800ced6
 *
 * General function for testing reading and writing a constant u32 value.
 * Uses 2 of the below functions below to do this, and check it.
 * Writes the first invalid address to failed_address**, and the failed written value to failed_value.
 * Basically tests word writes.
 * */
bool const32_fill_test(u8* buffer, u8 *buffer_end, u32 value, u8** failed_address, u32* failed_value);

/*
 * function at 0800cf8c
 * return at 0800d114
 *
 * DMAs TM0CNT to the buffer specified in 16 bit units, then back to a local buffer and checks it.
 * Requires exact memory timings for the region being tested, and timers to be running during DMAs.
 * */
bool DMA16_test(u8* buffer, u32 access_time);

/*
 * function at 0800d118
 * return at 0800d2e2
 *
 * DMAs TM0CNT to the buffer specified in 32 bit units, then back to a local buffer and checks it.
 * Requires exact memory timings for the region being tested, and timers to be running during DMAs.
 * I don't exactly know what the parameters mean yet. The first timing factor is 2 for eWRAM, second is 0x10 for eWRAM.
 * */
bool DMA32_test(u8* buffer, u32 access_time, u32 access_time_factor);

/*
 * function at 0800f2f0
 * return at 0800f336
 *
 * General function used in eWRAM_test. Fills a region of memory up with a constant value.
 * */
enum mem_set_flags : u32 {
    mem_set_u8  = 0,
    mem_set_u16 = 1,
    mem_set_u32 = 2
};
void mem_set_const(u8* dest, u32 length, u32 value, u32 flags);

/*
 * function at 0800f1f4
 * return at 0800f25c
 *
 * General function used in eWRAM_test. Checks if the region is filled with constants, set by mem_set_const.
 * If this is not the case, writes the invalid address to failed_address**, and the written value to failed_value.
 * */
bool mem_set_const_check(u8* src, u32 length, u32 value, u32 flags, u8** failed_address, u32* failed_value);

/*
 * function at 0800f1f4
 * return at 0800f35a
 *
 * General function used in eWRAM_test. Fills destination with incrementing values (a very simple "checksum").
 * */
void mem_set_incrementing(u8* dest, u32 length);

/*
 * function at 0800f260
 * return at 0800f2a2
 *
 * General function used in eWRAM_test. Checks if the region is filled incrementing values, set by mem_set_incrementing.
 * If this is not the case, writes the invalid address to failed_address**, and the written value to failed_value.
 * */
bool mem_set_incrementing_check(u8* src, u32 length, u8** failed_address, u32* failed_value);

/*
 * test at 80021D0
 * return at 080021ea
 * */
u32 cpu_external_work_ram();

#endif //AGS_MEMORY_H
