#ifndef AGS_MEMORY_H
#define AGS_MEMORY_H

#include "default.h"
#include "AGS.h"

#define EWRAM_START ((u8*)0x02000000)
#define EWRAM_LENGTH 0x00040000
#define IWRAM_START ((u8*)0x03000000)
// we don't want to touch the stack
#define IWRAM_LENGTH 0x00007d00
#define PRAM_START ((u8*)0x05000000)
#define PRAM_LENGTH 0x400
#define VRAM_START ((u8*)0x06000000)
#define VRAM_LENGTH 0x18000
#define OAM_START ((u8*)0x07000000)
#define OAM_LENGTH 0x400

enum memory_test_flags : u32 {
    memory_test_const8_fill       = 0x01,
    memory_test_incrementing_fill = 0x02,
    memory_test_const32_fill      = 0x04,
    memory_test_dma16             = 0x08,
    memory_test_dma32             = 0x10,
    memory_test_endianness        = 0x20,
};

/*
 * function at 080020e8
 * return at 080021ce
 *
 * Function used to test eWRAM memory region
 * */
u32 eWRAM_test(u32* buffer);

/*
 * function at 0800220c
 * return at 080022c6
 *
 * Function used to test iWRAM memory region. The tests are very similar to the eWRAM tests, except they all
 * happen in VBlank now.
 * */
u32 iWRAM_test(u32* buffer);

/*
 * function at 08002304
 * return at 08002480
 *
 * Actual tests for the iWRAM region, these write the results to a global result struct that is then copied by the
 * synchronous handler.
 * */
typedef struct s_iWRAM_test_results {
    u32 flags;
    u32 _;
    u32 fail_data[6];  // same data as for the eWRAM test, pointers to bad data / bad data value
} s_iWRAM_test_results;

s_iWRAM_test_results* ptr_iWRAM_results = (s_iWRAM_test_results*)0x03001080;

void iWRAM_test_async();

/*
 * function at 08002488
 * return at 0800260a
 *
 * Actual tests for the Palette RAM region. Basically the same as the eWRAM tests.
 * Note: PRAM 8 bit writes are messed up. The const8_fill test actually fills with words
 * */
u32 PRAM_test(u32* buffer);

/*
 * function at 08002648
 * return at 080027ca
 *
 * Actual tests for the VRAM region. Basically the same as the PRAM tests
 * */
u32 VRAM_test_1(u32* buffer);

// at 080190d8
static const s_affine_settings VRAM_test_2_BG2_settings = {
    .CenterX  = 0x7800,
    .CenterY  = 0x5000,
    .DisplayX = 0x0078,
    .DisplayY = 0x0050,
    .ScalingX = 0x0208,
    .ScalingY = 0x0208,
    .Angle    = 0xe000,
};

/*
 * function at 080027cc
 * return at 08002a8a
 *
 * More tests for VRAM.
 * */
u32 VRAM_test_2(u32* buffer);

/*
 * function at 08002aec
 * return at 08002c6e
 *
 * OAM tests. Basically the same as PRAM tests.
 * */
u32 OAM_test(u32* buffer);

/*
 * function at 0800326c
 * return at 08003290
 * Reads TM0CNT and checks returns the value after some reads to check the prefetch buffer length.
 * */
u32 prefetch_buffer_test();

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
 * function at 0800d52c
 * return at 0800d5f0
 *
 * General function used in VRAM_test_2. Fills region with "checksummed" values, then checks them again.
 * */
bool mem_read_write_checksum(u8* dest, const u8* dest_end, u32 start_val, u8** failed_address, u32* failed_value);

/*
 * test at 80021D0
 * return at 080021ea
 * */
u32 cpu_external_work_ram();

/*
 * test at 080022c8
 * return at 080022e2
 * */
u32 cpu_internal_work_ram();

/*
 * test at 0800260c
 * return at 08002626
 * */
u32 palette_ram();

/*
 * test at 08002a8c
 * return at 08002ab2
 * */
u32 vram();

/*
 * test at 08002c70
 * return at 08002c8a
 * */
u32 oam();

/*
 * test at 08002fb8
 * return at 08002ff2
 *
 * Literally just checks if the top bit of WAITCNT is 0.
 * */
u32 cartridge_type_flag();

/*
 * test at 08003010
 * return at 0800308a
 * */
u32 prefetch_buffer();

#endif //AGS_MEMORY_H
