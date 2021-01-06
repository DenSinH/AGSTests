#ifndef AGS_AGS_H
#define AGS_AGS_H

#include "default.h"

/*
 * A few default functions / values used in the ROM
 * */
#define ptr_TM0CNT ((u32*)0x04000100)
#define ptr_DMA3SAD ((u32*)0x040000d4)
#define ptr_DMA3DAD ((u32*)0x040000d8)
#define ptr_DMA3CNT ((u32*)0x040000dc)

typedef enum dma_settings {
    DMADestIncrement       = 0x0000,
    DMADestDecrement       = 0x0020,
    DMADestFixed           = 0x0040,
    DMADestIncrementReload = 0x0060,

    DMASrcAddrControl      = 0x0180,
    DMASrcIncrement        = 0x0000,
    DMASrcDecrement        = 0x0080,
    DMASrcFixed            = 0x0100,

    DMARepeat              = 0x0200,
    DMAWordSized           = 0x0400,

    DMAStartTiming         = 0x3000,
    DMAStartImmediate      = 0x0000,
    DMAStartVBlank         = 0x1000,
    DMAStartHBlank         = 0x2000,
    DMAStartSpecial        = 0x3000,

    DMAIRQ                 = 0x4000,
    DMAEnable              = 0x8000,
} dma_settings;

/*
 * function at 0800d730
 * return at 0800d75e
 * Simply sets IME and returns the old value it was.
 * */
extern u16 set_IME(u16 new_value);

#endif //AGS_AGS_H
