### AGS tests

This repository is meant to (semi-)disassemble the AGS Aging Cartridge ROM for the GameBoy Advance. There has been
an effort to disassemble this ROM before, by Normmatt, found [here](https://github.com/Normmatt/ags_aging).

I use some of the information (mainly the test entry points) that he found. I hope he is okay with this. This project
is mainly aimed to be a source of information for emulator developers/GBA enthousiasts. 

Especially [this file](https://github.com/Normmatt/ags_aging/blob/0aa5aceebc79a26acfd974dd06451bb11f191de0/src/sub_8000AD4.c#L191) 
is of good use.

## WHAT IS THIS FOR?

It might be of help to emulator developers trying to find out why they don't pass some AGS test. You can look up what the test does here, and break on where the relevant function returns to see what the flags are that you fail for that test.

I also just found it interesting to see what the tests did exactly. The code was very neat, so it wasn't too much effort to decompile this. I'd say deocompiling the huge DMA addressing mode test was the hardest.

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
