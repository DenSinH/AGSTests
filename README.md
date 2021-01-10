### AGS tests

This repository is meant to (semi-)disassemble the AGS Aging Cartridge ROM for the GameBoy Advance. There has been
an effort to disassemble this ROM before, by Normmatt, found [here](https://github.com/Normmatt/ags_aging).

I use some of the information (mainly the test entry points) that he found. I hope he is okay with this. This project
is mainly aimed to be a source of information for emulator developers/GBA enthousiasts. 

Especially [this file](https://github.com/Normmatt/ags_aging/blob/0aa5aceebc79a26acfd974dd06451bb11f191de0/src/sub_8000AD4.c#L191) 
is of good use.

## CONTENTS
Each folder contains the functions related to the tests of whichever category the folder is named after. They all contain a `.h` file with the functions, and info on them. I documented most of them like this:
```
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
```
So that you can break at the start of the function, on the return, or compare your output (by either breaking on the return and checking `r0`, or by patching AGS with the `output_test_results_patch.py` script and logging it to your terminal. Every test in the suite returns flags of some sort. Some more than others. Some just 1, but either way, you can see what they mean by looking in the `.md` file, the source in the `tests.c` file and most of them they are also documented in the `.h` file.

## WHAT IS THIS FOR?

It might be of help to emulator developers trying to find out why they don't pass some AGS test. You can look up what the test does here, and break on where the relevant function returns to see what the flags are that you fail for that test.

I also just found it interesting to see what the tests did exactly. The code was very neat, so it wasn't too much effort to decompile this. I'd say deocompiling the huge DMA addressing mode test was the hardest.

## OUTPUT PATCH

If you run `output_test_results_patch.py` with `AGS.gba` in your working directory (with md5 sum `9f74b2ad1d33e08e8a570ffe4564cbc3`), it will patch the ROM so that it will output the result flags for each test to address `0x0004`. You can edit the first byte of the `code` list in that file to adjust the address. I chose 4 because `0` might occur
randomly when games mess up and write to `null`, and this won't overflow your emulator/get flooded out that way. Might be very useful for emulator developers. 

#### NOTE: doing this causes the ROM to always say you failed, even if you passed every test.

I chose to output it this way cause I was limited to 3 thumb mode instructions to output it. I could maybe have made it so it outputs to some other address, but I was fine with this.

## PATCHER

I also made a patcher (`patch_ags.py`) to use some of the unused tests. Some freeze up (my) emulators, so you'll have to uncomment those to try them out yourself.
The tests that are unused in the AGS ROM with md5 sum `9f74b2ad1d33e08e8a570ffe4564cbc3` are:
  - advanced KEYINPUT test (shows the buttons and requires you to press them, this got annoying so I commented it out)
  - COM test class (requires you to press start when connected to another AGS system, this got annoying so I commented it out)
  - SET_PHI_OUTPUT_CONTROL
  - DISPLAY (this is the display sequence when all tests are passed)
  - PLAY_SOUND (this is also the sound that plays when all tests are passed)
  - CARTRIDGE_INTR
  - EXECUTE_STOP_FUNCTION (commented out because it likely freezes up a system, my emulator doesn't because I haven't implemented STOP)
  - EXECUTE_HALT_FUNCTION (freezes up (my) emulators)

Extended SIO tests (instead of the DMA tests):
  - NORMAL SIO
  - MULTI PLAY SIO
  - UART RECV SIO
  - UART SEND SIO
  - DIRECT
  - JOY BUS RESET
  - JOY BUS STATUS
  - JOY BUS WRITE
  - JOY BUS READ
  
I also added something that allows you to always "win" the ROM. So if you uncomment the line after `ALWAYS WIN!!`. You can also disable all tests and win that way (uncomment the block somewhere near the bottom).
