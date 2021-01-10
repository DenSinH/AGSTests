"""
run this to make AGS write the flags that each test outputs to 00000004
I had to inject this somewhere, so I injected it to where AGS tries to see whether you failed any test
by orr'ing each test's output
originally:
    bl run_test
    ldr r1, [r7, #total_flags]
    orr flags, r1
    str flags, [r7, #total_flags]
injected:
    bl run_test
    xor r1, r1
    str flags, [r1]
    str r7, [r7, #total_flags]
where `flags` is r0 and #total_flags is some offset for a local variable
"""

with open("AGS.gba", "rb") as f:
    AGS = list(f.read())

code = [
    b"\x04\x21",  # movs r1, #4
    b"\x08\x60",  # str r0, [r1]
    b"\xbf\x60",  # str r7, [r7, #8]
]
for i in range(3):
    AGS[0xb20 + 2 * i] = code[i][0]
    AGS[0xb20 + 2 * i + 1] = code[i][1]

with open("AGS_patched.gba", "wb+") as f:
    f.write(bytes(AGS))
