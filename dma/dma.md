### DMA

AGS contains 9 lcd tests. The names for these tests are also in the ROM, and are:
  - DMA0 ADDRESS CONTROL (`DMA0_address_control()`)
  - DMA1 ADDRESS CONTROL (`DMA1_address_control()`)
  - DMA2 ADDRESS CONTROL (`DMA2_address_control()`)
  - DMA3 ADDRESS CONTROL (`DMA3_address_control()`)
  - DMA V BLANK START (`DMA_vblank_start()`)
  - DMA H BLANK START (`DMA_hblank_start()`)
  - DMA DISPLAY START (`DMA_display_start()`)
  - DMA INTR FLAG (`DMA_intr_flag()`)
  - DMA PRIORITY (`DMA_priority()`)
  
The tests do the following:

## ADDRESS CONTROL
This test is the same for DMA0/1/2/3, except they use a different channel. 
The test tests out all different possible configurations and DMAs an array of 256 incrementing bytes to
EWRAM. Then it compares it against a huge table of expected values (`expected.h`).
The return flags are as follows:
 - 0:  any test failed / any !HBlank test failed
 - 1:  any HBlank test failed
 - 2:  any SrcAddrControl == 0 (increment) test failed
 - 3:  any SrcAddrControl == 1 (decrement) test failed
 - 4:  any SrcAddrControl == 2 (fixed) test failed
 - 5:  any DestAddrControl == 0 (increment) test failed
 - 6:  any DestAddrControl == 1 (decrement) test failed
 - 7:  any DestAddrControl == 2 (fixed) test failed
 - 8:  any DestAddrControl == 3 (increment + reload) test failed
 - 9:  any !WordSized test failed
 - 10: any WordSized test failed
I suspect bit 0 was not meant to have this "double purpose", but it does.

## V BLANK START
Sets the DMA start timing to VBlank and transfers VCount to EWRAM once. Checks if the value transferred is 160.
return flags are 1 bit for each DMA channel, LSB = DMA0, etc.

## H BLANK START
Sets the DMA start timing to HBlank and transfers VCount to EWRAM for 228 HBlanks. Checks if all the values
transferred are incrementing values from 0 to 159 (once for each visible line), and then 0 for the remaining
68 values (no HBlank DMA in VBlank).
return flags are 1 bit for each DMA channel, LSB = DMA0, etc.

## DISPLAY START
Only for DMA 3. Sets the DMA start timing for DMA3 to special and transfers VCont to EWRAM for 1 frame. Checks if all the values
transferred are incrementing values from 2 to 161 (video DMA starts in line 2 and ends in 162) and then 68 zeros\
(no DMA for the remaining lines).
return flags:
 1: DMA not ended in line 162
 2: DMA did not transfer the right values
 
## INTR FLAG
Very similar to the timer/lcd intr flag tests. Basically, clears IF, enables 2 immediate DMAs, one with
IRQ and one without and checks if the IF bit is set appropriately.
return flags: 2 bits per channel. LSB = DMA0
1: IRQ not requested when it should have been
2: IRQ requested when it shouldn't have been

## PRIORITY
For each channel DMA 0, 1, 2: starts that channel in HBlank, repeating, and starts channel + 1 with a longer DMA
immediately. Waits for the longer DMA to finish (lower priority). Both channels DMA TM0CNT to a separate part of
EWRAM. Not strict at all timing wise for when the DMAs take over.
Fails if any timer value is incorrect or if the higher priority one does not
fire while the lower priority is running.
return flags: 1 for each DMA 0, 1 and 2. LSB = DMA0
 1: any timer value is incorrect or if the higher priority DMA one does not
    fire while the lower priority is running.