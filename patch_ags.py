"""
Since I have done so much research into the AGS test, and I know that there are disabled tests, I thought I'd write
something to patch your AGS ROM!

AGS ROM md5: 9f74b2ad1d33e08e8a570ffe4564cbc3
"""
with open("AGS.gba", "rb") as f:
    AGS = list(f.read())

for i in range(4):
#     AGS[i + 0x4c0] = 0  # uncomment to enable the advanced KEYINPUT test
#     AGS[i + 0x4c6] = 0  # uncomment to enable test class 4 (COM)
    # ALWAYS WIN!!
    # AGS[i + 0x4d2] = 0  # call to error screen

    # SET_PHI_OUTPUT_CONTROL
    AGS[i + 0x1421b0] = [0xa9, 0x30, 0x00, 0x08][i]
    # DISPLAY test (cant click this away)
    # AGS[i + 0x1421c4] = [0xf5, 0x3d, 0x00, 0x08][i]
    # PLAY_SOUND test
    AGS[i + 0x1421d8] = [0xdd, 0x8a, 0x00, 0x08][i]
    # CARTRIDGE_INTR test
    AGS[i + 0x1421ec] = [0x3d, 0xcd, 0x00, 0x08][i]
    # EXECUTE_STOP_FUNCTION test (probably freezes it up if you implement STOP in your emulator)
    # AGS[i + 0x142200] = [0x05, 0xce, 0x00, 0x08][i]
    # EXECUTE_HALT_FUNCTION test (freezes it up)
    # AGS[i + 0x142214] = [0x3d, 0xce, 0x00, 0x08][i]

    # EXTENDED SIO
    # NORMAL SIO
    AGS[i + 0x14232c] = [0xa5, 0xac, 0x00, 0x08][i]
    # MULTI PLAY SIO
    AGS[i + 0x142340] = [0xf1, 0xae, 0x00, 0x08][i]
    # UART RECV SIO
    AGS[i + 0x142354] = [0xed, 0xb2, 0x00, 0x08][i]
    # UART SEND SIO
    AGS[i + 0x142368] = [0x0d, 0xb5, 0x00, 0x08][i]
    # DIRECT
    AGS[i + 0x14237c] = [0x25, 0xb7, 0x00, 0x08][i]
    # JOY BUS RESET
    AGS[i + 0x142390] = [0xdd, 0xb7, 0x00, 0x08][i]
    # JOY BUS STATUS
    AGS[i + 0x1423a4] = [0xd1, 0xb8, 0x00, 0x08][i]
    # JOY BUS WRITE
    AGS[i + 0x1423b8] = [0x1d, 0xb9, 0x00, 0x08][i]
    # JOY BUS READ
    AGS[i + 0x1423cc] = [0x65, 0xba, 0x00, 0x08][i]


# DISABLE ALL
# addr = 0x1421a4
# while addr < 0x14249b:
#     AGS[addr] = 0
#     addr += 0x14
# for i in range(4):
#     AGS[0x4b8 + i] = 0  # call to enable_all_tests
#     AGS[0xade + i] = 0  # call to clear_test_classes

for char in range(6):
    AGS[0x186fc + char] = ord("PATCH\0"[char])

for char in range(3):
    AGS[0x186ec + char] = ord("SIO"[char])

for char in range(14):
    AGS[0x182d8 + char] = ord("YOUR EMU SUCKS"[char])

with open("AGS_patched.gba", "wb+") as f:
    f.write(bytes(AGS))
