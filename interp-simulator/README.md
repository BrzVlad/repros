This code showcases a simple interpreter that has many switch cases, with switch dispatch more efficiently implemented as goto to address of label. Each switch case gets all the relevant code inlined, to squeeze best performance. If the lines associated with `MINT_ADD15` are uncommented (for mint_label and the switch entry), the apple clang compiler will start sharing code between different switch cases. Additional branches are added as part of this transition. It seems that this transformation can lead to worse performance).

Example of good code for MINT_ADD14:
```
10000080c: ldrb    w9, [x0, #0x2]
100000810: ldr w9, [x1, x9, lsl #2]
100000814: add w9, w9, #0xe
100000818: mov w10, #0x3               ; =3
10000081c: ldrb    w11, [x0, #0x1]
100000820: str w9, [x1, x11, lsl #2]
100000824: add x0, x0, x10
100000828: ldrb    w9, [x0]
10000082c: ldr x9, [x8, x9, lsl #3]
100000830: br  x9
```

Example of worse code for MINT_ADD14, once MINT_ADD15 is added to the switch case set:
```
1000006e4: ldrb    w9, [x0, #0x2]
1000006e8: ldr w9, [x1, x9, lsl #2]
1000006ec: add w9, w9, #0xe
1000006f0: b   0x1000005b8 <_interp_exec_method+0x1c>

1000005b8: mov w10, #0x3               ; =3
1000005bc: ldrb    w11, [x0, #0x1]
1000005c0: str w9, [x1, x11, lsl #2]
1000005c4: add x0, x0, x10
1000005c8: ldrb    w9, [x0]
1000005cc: ldr x9, [x8, x9, lsl #3]
1000005d0: br  x9
```

Doing some debugging by adding `-mllvm -print-before-all -mllvm -print-after-all -mllvm -filter-print-funcs=interp_exec_method` to the clang command shows that, at some point in the llvm compilation, the code for switch cases is shared. Later on, when using homebrew clang, the tailduplication pass seems to inline back all switch contents, as seen by following the `IR Dump After Tail Duplication (tailduplication)` log. On apple clang this doesn't happen. Homebrew clang version 21.1.8. Apple clang version 17.0.0, Xcode 26.1.
