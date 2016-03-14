    .text
    .func   gfxNeonCopy
    .global gfxNeonCopy
gfxNeonCopy:
    PLD     [r0, #0xC0]
    VLDM r0!,{d0-d7}
    VSTM r1!,{d0-d7}
    subs    r2,r2,#64
    bgt     gfxNeonCopy
    bx      lr
    .endfunc
