/* evdevDrvVxSimKbd.h - VxWorks Simulator Keyboard Driver Header File */

/*
 * Copyright (c) 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
27aug13,y_f  created from udIoSimGraphicsDrv.h version 01b
*/

#ifndef __INCevdevDrvVxSimKbdh
#define __INCevdevDrvVxSimKbdh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define SIM_KBD_DRIVER_NAME         "vxSimKbd"

#define SIM_DLL_INT_RANGE_BASE      0x00f0  /* vxsim interrupts range base */
#define SIM_DLL_INT_RANGE_END       0x00ff  /* vxsim interrupts range end  */
#define SIM_DLL_GRAPHICS_INT_VEC    (SIM_DLL_INT_RANGE_BASE)
#define SIM_DLL_KBD_INT_BASE        (SIM_DLL_INT_RANGE_BASE + 5)

#define SIM_KBD_LEFT_ALT            0x00020000
#define SIM_KBD_RIGHT_ALT           0x00040000
#define SIM_KBD_LEFT_SHIFT          0x00080000
#define SIM_KBD_RIGHT_SHIFT         0x00100000
#define SIM_KBD_LEFT_CTRL           0x00200000
#define SIM_KBD_RIGHT_CTRL          0x00400000
#define SIM_KBD_NUM_PAD             0x08000000
#define SIM_KBD_FN_MASK             0x0000EF00
#define SIM_KBD_VK_MASK             0x0000E000
#define SIM_KBD_KEY_MASK            0x000000ff  /* gets key value from keyCode */

#define SIM_KBD_VK_NUM              0x0E
#define SIM_KBD_FN_NUM              0x0D
#define SIM_KBD_ASCII_NUM           0x80

#define SIM_KBD_ASTERISK            0x2A
#define SIM_KBD_PLUS_SIGN           0x2B

/* typedefs */

typedef struct vxSimDllKbdData
    {
    int             isKeyDown;      /* down transition */
    int             isExtended;     /* extended key */
    int             isKeyCode;      /* down transition */
    unsigned long   keyCode;        /* mapped key value + modifiers */
    int             packetNum;
    } VX_SIM_DLL_KBD_DATA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvVxSimKbdh */
