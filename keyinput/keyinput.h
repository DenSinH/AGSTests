#ifndef AGS_KEYINPUT_H
#define AGS_KEYINPUT_H

#include "default.h"

/*
 * function at 0800c074
 * return at 0800c0a0
 *
 * Literally checks if no keys are pressed.
 * Returns 80000000 if any keys are pressed.
 * */
u32 keyinput_simple_test();

/*
 * function at 0800c0a4
 * return at 0800c0b6
 * */
u32 keyinput_simple();

/*
 * Disclaimer: there is a more difficult test for keyinput, but this test has been disabled.
 * This test also seemed to involve the m4a engine for some reason in it's callback, but I didn't try this.
 * */

#endif //AGS_KEYINPUT_H
