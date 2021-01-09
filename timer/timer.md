### TIMER

AGS contains 3 timer tests. The names for these tests are also in the ROM, and are:
  - TIMER PRESCALER (`prescaler()`)
  - TIMER CONNECT (`timer_connect()`)
  - TIMER INTR FLAG (`timer_intr_flag()`)
  
## TIMER PRESCALER
For each timer, sets the prescaler and resets the timer. After a certain amount of cycles, compares the timer value
to the expected value.

## TIMER CONNECT
Sets each timer to count-up mode, timers 0-2 with reload `0xfffe`. Runs for `0x40a` cycles and compares the
value of TM3CNT_L to 0x200.

## TIMER INTR FLAG
Very similar to the LCD interrupt tests. Runs a timer with IRQ enabled and checks that an IRQ is requested in IF,
then runs it without IRQ enabled and checks that no IRQ was requested. Done for each timer.