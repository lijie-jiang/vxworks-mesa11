/* evdevDrvVirtualKbd.h - VxWorks Virtual Keyboard Driver Header File */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
16may14,yat  Add virtual keyboard driver. (US24741)
*/

#ifndef __INCevdevDrvVirtualKbdh
#define __INCevdevDrvVirtualKbdh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define VIRTUAL_KBD_DRIVER_NAME         "virtualKbd"

#define VIRTUAL_KBD_FN_MASK             0x0000EF00
#define VIRTUAL_KBD_VK_MASK             0x0000E000
#define VIRTUAL_KBD_KEY_MASK            0x000000ff  /* gets key value from keyCode */

#define VIRTUAL_KBD_VK_NUM              0x0E
#define VIRTUAL_KBD_FN_NUM              0x0D
#define VIRTUAL_KBD_ASCII_NUM           0x80

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvVirtualKbdh */
