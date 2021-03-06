### MEMORY

AGS contains 9 memory tests. The names for these tests are also in the ROM, and are:
  - CPU EXTERNAL WORK RAM (`cpu_external_work_ram()`)
  - CPU INTERNAL WORK RAM (`cpu_internal_work_ram()`)
  - PALETTE RAM (`palette_ram()`)
  - VRAM (`vram()`)
  - OAM (`oam()`)
  - CARTRIDGE TYPE FLAG (`cartridge_type_flag()`)
  - PREFETCH BUFFER (`prefetch_buffer()`)
  - WAIT STATE WAIT CONTROL (`wait_state_wait_control()`)
  - CARTRIDGE RAM WAIT CONTROL (`cartrige_ram_wait_control()`)
    
The first few tests are all very similar. They use a bunch of standard read/write tests (in different access sizes), 
DMA tests (in different access sizes) and timing tests. The last few are a bit different. All functions for the tests
return a u32 with flags. Each bit represents a test. You want all bits to be 0. The bits are LSB to MSB in order of the
tests described below:

## CPU EXTERNAL WORK RAM (first test)
Does the following:
  - 0x1:  fills up eWRAM with a constant value (`0x55`), and checks if all writes are OK (u8 size)
  - 0x2:  fills up eWRAM with incrementing values, and checks if all writes are OK (u16 size)
  - 0x4:  fills up eWRAM with a constant value (`0xaaaaaaaa`) and checks if all writes are OK (u32 size)
  - 0x8:  halfword DMAs TM0CNT_L to eWRAM and checks if all writes are okay (correct timer values!)
  - 0x10: word DMAs TM0CNT to eWRAM and checks if all writes are okay (correct timer setting _and_ values!)
  - 0x20: checks endianness by writing bytes 0x01 and 0x23 and expecting 0x2301 on a halfword read
    
## CPU INTERNAL WORK RAM (second test)
Does the following in VBlank:
   - 0x1:  fills up iWRAM with a constant value (`0x55`), and checks if all writes are OK (u8 size)
   - 0x2:  fills up iWRAM with incrementing values, and checks if all writes are OK (u16 size)
   - 0x4:  fills up iWRAM with a constant value (`0xaaaaaaaa`) and checks if all writes are OK (u32 size)
   - 0x8:  halfword DMAs TM0CNT_L to iWRAM and checks if all writes are okay (correct timer values!)
   - 0x10: word DMAs TM0CNT to iWRAM and checks if all writes are okay (correct timer setting _and_ values!)
    
## PALETTE RAM (third test)
Does the following:
  - 0x1:  fills up PRAM with a constant value (`0x55555555`), and checks if all writes are OK (u32 size)
  - 0x2:  fills up PRAM with incrementing values and checks if all writes are OK (u16 size)
  - 0x4:  fills up PRAM with a constant value (`0xaaaaaaaa`), and checks if all writes are OK (u32 size)
  - 0x8:  halfword DMAs TM0CNT_L to PRAM and checks if all writes are okay (correct timer values!)
  - 0x10: word DMAs TM0CNT to PRAM and checks if all writes are okay (correct timer setting _and_ values!)
    
## VRAM (fourth test)
There are 2 functions to test VRAM, some flags overlap.
Does the following:
  - 0x1:  fills up VRAM with a constant value (`0x55555555`), and checks if all writes are OK (u32 size)
  - 0x2:  fills up VRAM with incrementing values and checks if all writes are OK (u16 size)
  - 0x4:  fills up VRAM with a constant value (`0xaaaaaaaa`), and checks if all writes are OK (u32 size)
  - 0x8:  halfword DMAs TM0CNT_L to VRAM and checks if all writes are okay (correct timer values!)
  - 0x10: word DMAs TM0CNT to VRAM and checks if all writes are okay (correct timer setting _and_ values!)
Then the following:
  - 0x1 (again):  fills up VRAM with a constant value (`0x55555555`), and checks if all writes are OK (u32 size)
  - 0x2 (again):  fills up VRAM with incrementing values and checks if all writes are OK (u16 size)
  - 0x4 (again):  fills up VRAM with a constant value (`0xaaaaaaaa`), and checks if all writes are OK (u32 size)
  - 0x20: fills up VRAM with a constant value (`0`), and checks if all writes are OK (u32 size)
  - 0x40: fills up VRAM with a constant value (`0xffffffff`), and checks if all writes are OK (u32 size)
  - 0x10000000: fills up VRAM with a checksum values and checks if they are OK (flag `0x10000000`) (u32 size)
    
## OAM (fifth test)
Does the following:
  - 0x1:  fills up OAM with a constant value (`0x55555555`), and checks if all writes are OK (u32 size)
  - 0x2:  fills up OAM with incrementing values and checks if all writes are OK (u16 size)
  - 0x4:  fills up OAM with a constant value (`0xaaaaaaaa`), and checks if all writes are OK (u32 size)
  - 0x8:  halfword DMAs TM0CNT_L to OAM and checks if all writes are okay (correct timer values!)
  - 0x10: word DMAs TM0CNT to OAM and checks if all writes are okay (correct timer setting _and_ values!)

## CARTRIDGE TYPE FLAG (sixth test)
Does the following:
  - checks if bit 15 of WAITCNT is 0
Returns 1 if failed, 0 if passed.
    
## PREFETCH BUFFER
Does the following (with return flags):
  - 0x1: Enables the prefetch buffer, resets timer 0, does a few ROM reads, reads timer 0 and compares it
  - 0x2: Disables the prefetch buffer and does the same thing
    
## WAIT STATE WAIT CONTROL
For every waitstate setting:
    Resets timer 0, does a few ROM reads, reads timer 0 and compares it to a table.
Return flags:
1 bit for each setting, so 24 bits total. You can see the expected values for each setting in the 
`wait_control_timer_values[3][8]` table in `memory.h`.
    
## CARTRIDGE RAM WAIT CONTROL
For every waitstate setting:
    Resets timer 0, does a few SRAM reads, reads timer 0 and compares it to a table
Return flags: 1 bit for each setting, so 4 bits total. You can see the expected values for each setting in 
the `cart_wait_control_timer_values[4]` table in `memory.h`.