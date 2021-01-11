### TIMER

AGS contains 3 timer tests. The names for these tests are also in the ROM, and are:
  - TIMER PRESCALER (`prescaler()`)
  - TIMER CONNECT (`timer_connect()`)
  - TIMER INTR FLAG (`timer_intr_flag()`)
  
## TIMER PRESCALER
For each timer, sets the prescaler and resets the timer. After a certain amount of cycles, compares the timer value
to the expected value. Sets the reload to 0, runs for `0x400` tight loops and reads the timer again. Expects to read
`0x1000, 0x40, 0x10, 0x1` for each prescaler setting respectively. Return flags: 4 bits per timer:
  - 1: prescaler 0 failed
  - 2: prescaler 1 failed
  - 4: prescaler 2 failed
  - 8: prescaler 3 failed

## TIMER CONNECT
Sets each timer to count-up mode, timers 0-2 with reload `0xfffe`. Runs for `0x40a` tight loops and compares the
value of TM3CNT_L to 0x200. Returns 0 if passed, 1 if failed.

## TIMER INTR FLAG
Very similar to the LCD interrupt tests. Runs a timer with IRQ enabled and checks that an IRQ is requested in IF,
then runs it without IRQ enabled and checks that no IRQ was requested. Done for each timer.
Return flags: 2 bits for each timer, LSB is timer 0:
  - 1: timer IRQ was not requested when it should have been.
  - 2: timer IRQ was requested when it shouldn't have been.