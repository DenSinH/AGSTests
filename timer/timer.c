#include "timer.h"

u32 wait_ticks(u32 ticks, u32 _) {
    // tight asm loop
    for (;ticks != 0; ticks--) { }
    return 0;
}