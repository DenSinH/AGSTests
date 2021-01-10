#include "keyinput.h"
#include "AGS.h"


u32 keyinput_simple_test() {
    u32 flags;
    if (!(*ptr_KEYINPUT & 0x3ff)) {
        flags = 0;
    }
    else {
        flags = 0x80000000;
    }
    return flags;
}

u32 keyinput_simple() {
    return keyinput_simple_test();
}