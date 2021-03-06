#ifndef AGS_AGS_H
#define AGS_AGS_H

#include "default.h"

/*
 * A few default functions / values used in the ROM
 * */
#define ptr_DISPCNT ((u16*)0x04000000)
#define ptr_DISPSTAT ((u16*)0x04000004)
#define ptr_VCOUNT ((u16*)0x04000006)
#define ptr_DMASAD(_channel) ((u32*)(0x040000b0 + 0xc * (_channel)))
#define ptr_DMADAD(_channel) ((u32*)(0x040000b4 + 0xc * (_channel)))
#define ptr_DMACNT(_channel) ((u32*)(0x040000b8 + 0xc * (_channel)))
#define ptr_DMA0SAD ((u32*)0x040000b0)
#define ptr_DMA0DAD ((u32*)0x040000b4)
#define ptr_DMA0CNT ((u32*)0x040000b8)
#define ptr_DMA3SAD ((u32*)0x040000d4)
#define ptr_DMA3DAD ((u32*)0x040000d8)
#define ptr_DMA3CNT ((u32*)0x040000dc)
#define ptr_TM0CNT ((u32*)0x04000100)
#define ptr_TMCNT(_timer) ((u32*)(0x04000100 + 4 * (_timer)))
#define ptr_SIOCNT ((u16*)0x04000128)
#define ptr_RCNT ((u16*)0x04000134)
#define ptr_SIOMLT_SEND ((u16*)0x0400012a)
#define ptr_KEYINPUT ((u16*)0x04000300)
#define ptr_KEYCNT ((u16*)0x04000302)
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
    DISPSTAT_HBlankIRQ     = 0x0010,
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
 * function at 0800d640
 * return at 0800d688
 * Clears the interrupt flag corresponding to the interrupt passed and waits for it to happen again.
 * */
void wait_for_interrupt(u16 interrupt);

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

/*
 * a memcpy routine using ldmia/stmia without BIOS overhead. At 080127e8.
 * */
extern void memcpy32(void* dest, const void* src, size_t len);

/*
 * A function taking 4 params:
 *  - a pointer to a function
 *  - a pointer to the end of a function
 *  - an argument for the function to be called
 *  - another argument for the function to be called
 *
 * For clarity, I will make this 3 arguments. Actual function is at 0800f150.
 * */
extern u32 call_from_stack(u32 (*func)(u32, u32), u32 arg1, u32 arg2);

/*
 * basis memcpy and memset functions.
 * */
extern void memcpy(void* dest, void* src, size_t len);
extern void memset(void* dest, char value, size_t len);

#endif //AGS_AGS_H
