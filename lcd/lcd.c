#include "lcd.h"
#include "AGS.h"

void wait_for_vcount_match() {
    u16 VCount = *ptr_VCOUNT + 0x10;
    if (VCount > 227) {
        VCount = 0;
    }

    // set VCount match
    *ptr_DISPSTAT = (*ptr_DISPSTAT & 0xff) | (VCount << 8);
    do { } while (*ptr_VCOUNT != VCount);
}

void wait_for_line() {
    u16 VCount = *ptr_VCOUNT;
    if (VCount > 225) {
        VCount = 0;
    }

    do { } while ((*ptr_VCOUNT) != (VCount + 1));
    do { } while ((*ptr_VCOUNT) != (VCount + 2));
}

void wait_for_frame() {
    u16 VCount = *ptr_VCOUNT;
    if (VCount > 226) {
        VCount = 0;
    }

    do { } while ((*ptr_VCOUNT) != (VCount + 1));
    do { } while ((*ptr_VCOUNT) != VCount);
    do { } while ((*ptr_VCOUNT) != (VCount + 1));
}
