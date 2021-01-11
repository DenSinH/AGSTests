### LCD

AGS contains 7 lcd tests. The names for these tests are also in the ROM, and are:
  - V COUNTER (`vcounter()`)
  - V COUNT INTR FLAG (`vcount_intr_flag()`)
  - H BLANK INTR FLAG (`hblank_intr_flag()`)
  - V BLANK INTR FLAG (`vblank_intr_flag()`)
  - V COUNT STATUS (`vcount_status()`)
  - H BLANK STATUS (`hblank_status()`)
  - V BLANK STATUS (`vblank_status()`)
  
Tests 2, 3, 4 are very similar, and so are tests 5, 6, 7.
## V COUNTER
Does the following:
A number of times, it waits for VCount to change, then stores the VCount value and the the time it took with TM0CNT.
The VCount values should increment by 1 every time (and modulo 228), and the timer values should all be between
1217 and 1233 cycles.
Return flags:
  - 0x1: TM0 value not in range `0x4c1..0x4d1`.
  - 0x2: VCount values not incrementing by 1 every time.

## X INTR FLAG
Does the following:
Sets the bit in DISPSTAT corresponding to the `X` interrupt. Waits for an amount of time in which this interrupt should
surely have happened and checks if the bit is set in IF.
Does the same, but disables the interrupt in DISPSTAT and makes sure the interrupt is _not_ set in IF after.
Return flags:
  - 0x1: No interrupt requested in IF when one should have.
  - 0x2: Interrupt requested in IF when it shouldn't have been.

## V COUNT STATUS
Checks if VCount matches get signaled in DISPSTAT and if they happen at the right VCount values.
Return flags:
  - 0x1: VCount match flag not set in DISPSTAT
  - 0x2: VCount value on match is not the same as the value in DISPSTAT

## X BLANK STATUS
Checks if the `X` blank flag gets set and cleared at the right interval:
  - HBlank flag: set for 209 - 225 cycles, clear for 990 - 1009 cycles
  - VBlank flag: set in line 160, cleared in line 227 (!)
  
Return flags:
#### for H BLANK
  - 0x1: time in HBlank was not between 209 and 225 cycles
  - 0x2: time in HDraw was not between 990 and 1009 cycles
#### for V BLANK
I think something went wrong when this test was made, as there is only one flag, while the intent was probably
to have 2.
  - 0x1: VBlank flag not set in line 160 OR VBlank flag not cleared in line 227
