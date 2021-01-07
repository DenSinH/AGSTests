#ifndef AGS_AGS_H
#define AGS_AGS_H

#include "default.h"

/*
 * A few default functions / values used in the ROM
 * */
#define ptr_DISPSTAT ((u16*)0x04000004)
#define ptr_DISPCNT ((u16*)0x04000000)
#define ptr_DMA3SAD ((u32*)0x040000d4)
#define ptr_DMA3DAD ((u32*)0x040000d8)
#define ptr_DMA3CNT ((u32*)0x040000dc)
#define ptr_TM0CNT ((u32*)0x04000100)
#define ptr_IF ((u16*)0x04000202)
#define ptr_WAITCNT ((u16*)0x04000204)

#define ptr_BG2PA ((u16*)0x4000020)
#define ptr_BG2PB ((u16*)0x4000022)
#define ptr_BG2PC ((u16*)0x4000024)
#define ptr_BG2PD ((u16*)0x4000026)
#define ptr_BG2X  ((u32*)0x4000028)
#define ptr_BG2Y ((u32*)0x400002C)

#define ptr_InterruptCheckFlag ((u32*)0x03007ff8)

/*
 * array of 16 callback functions at 0300041c in iWRAM.
 * */
void** ptr_interrupt_handler_table = (void**)0x0300041c;

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

typedef enum interrupts {
   intr_VBlank = 0x0001,
   intr_HBlank = 0x0002,
   intr_VCount = 0x0004,
   intr_Timer0 = 0x0008,
   intr_Timer1 = 0x0010,
   intr_Timer2 = 0x0020,
   intr_Timer3 = 0x0040,
   intr_SIO    = 0x0080,
   intr_DMA0   = 0x0100,
   intr_DMA1   = 0x0200,
   intr_DMA2   = 0x0400,
   intr_DMA3   = 0x0800,
   intr_Keypad = 0x1000,
   intr_Extern = 0x2000,
} interrupts;

typedef enum DISPSTAT_settings {
    DISPSTAT_VBlank        = 0x0001,
    DISPSTAT_HBLank        = 0x0002,
    DISPSTAT_VCount        = 0x0004,
    DISPSTAT_VBlankIRQ     = 0x0008,
    DISPSTAT_HBLankIRQ     = 0x0010,
    DISPSTAT_VCountIRQ     = 0x0020,
} DISPSTAT_settings;

typedef struct s_interrupt_settings {
    u16 IME;
    u16 IE;
    void* callbacks[16];
} s_interrupt_settings;

/*
 * function at 0800d790
 * return at 0800d848
 * Clears IME, sets IE to the parameter provided, stores the old values for IME and IE, and the interrupt callbacks,
 * located at 0300041c in iWRAM. Sets IF to 3fff (all interrupts enabled).
 * */
void clear_interrupt_settings(s_interrupt_settings* old, u16 IME, u16 IE);

/*
 * function at 0800d68c
 * return at 0800d6fa
 * Sets the interrupt handler for the flag passed to the intr parameter to the callback passed.
 * Returns the pointer to the old handler.
 * */
void* set_interrupt_handler(u16 intr, void* callback);

/*
 * function at 0800d854
 * return at 0800d8ae
 * Restores the interrupt settings to those in a s_interrupt_settings struct.
 * */
void set_interrupt_settings(s_interrupt_settings* new);

/*
 * function at 0800d730
 * return at 0800d75e
 * Simply sets IME and returns the old value it was. (really nothing else)
 * */
extern u16 set_IME(u16 new_value);

/*
 * function at 0800d760
 * return at 0800d78e
 * Simply sets IE and returns the old value it was. (really nothing else)
 * */
extern u16 set_IE(u16 new_value);

typedef struct s_affine_settings {
    i32 CenterX;
    i32 CenterY;
    i16 DisplayX;
    i16 DisplayY;
    i16 ScalingX;
    i16 ScalingY;
    u16 Angle;
} s_affine_settings;
/*
 * Just the BGAffineSet SWI. At 08012150.
 * */
extern void BGAffineSet(s_affine_settings* settings, void* dest, u32 number_of_calculations);

#endif //AGS_AGS_H
