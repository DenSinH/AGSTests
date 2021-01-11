### INTERRUPTS

AGS contains 7 lcd tests. The names for these tests are also in the ROM, and are:
  - V BLANK INTR (`vblank()`)
  - H BLANK INTR (`hblank()`)
  - V COUNT INTR (`vcount()`)
  - TIMER INTR (`timer()`)
  - SIO INTR (`sio()`)
  - DMA INTR (`dma()`)
  - KEY INTR (`keypad()`)
  
All tests are very similar in this category.

## VBLANK / HBLANK / VCOUNT / SIO / KEY
Allows the respective interrupt to happen in all required registers and IE. Waits until the interrupt inevitable
has happened and checks if it has. Then does the same, but blocks it in IE. Return flags:
  - 0x1: No interrupt happened when it should have.
  - 0x2: interrupt happened when it shouldn't have been.
  
## TIMERS / DMA
Same as the above, but for each channel. So the return value is 8 bits, 2 bits per DMA channel / timer.