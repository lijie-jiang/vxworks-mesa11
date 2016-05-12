/* evdevLibKbd.h - Keyboard Library Header */

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
25sep13,j_x  add raw mode support
28aug13,y_f  fixed compiler warning
16aug13,j_x  add kbd control type
12jul13,j_x  add country code
04jul13,y_f  create
*/

#ifndef __INCevdevLibKbdh
#define __INCevdevLibKbdh

#if __cplusplus
extern "C" {
#endif

/* defines */

#define EV_DEV_BOOT_KEY_COUNT              (0x06)    /* size of scan codes */

/*
 * Language Identifiers
 * Defined in Universal Serial Bus Language Identifiers (LANGIDs)
 */

#define EV_DEV_KBD_LANG_ENGLISH_US          0x0409
#define EV_DEV_KBD_LANG_VENDOR_DEFINED_1    0xf0ff
#define EV_DEV_KBD_LANG_VENDOR_DEFINED_2    0xf4ff
#define EV_DEV_KBD_LANG_VENDOR_DEFINED_3    0xf8ff
#define EV_DEV_KBD_LANG_VENDOR_DEFINED_4    0xfcff

#define EV_DEV_KBD_UNICODE_MODE             0
#define EV_DEV_KBD_KEYCODE_MODE             1

/* typedefs */

typedef struct evdevKbdData
    {
    struct timeval  time;
    UINT16          value;          /* key value */
    UINT8           state;
    } EV_DEV_KBD_DATA;

#ifdef _WRS_KERNEL
typedef struct evdevKbdReport
    {
    UINT8   modifiers;                          /* modifier keys */
    UINT8   reserved;                           /* reserved */
    UINT8   scanCodes [EV_DEV_BOOT_KEY_COUNT];  /* scan codes */
    } __attribute__ ((packed)) EV_DEV_KBD_REPORT;

typedef struct evdevKbdDevHandle
    {
    WDOG_ID             hWd;                    /* Watchdog for typematic */
    EV_DEV_KBD_REPORT   report;
    UINT8               lastModifiers;
    UINT16              lastKeycode;
    UINT16              lastUnicode;
    UINT16              countryCode;
    UINT16              keyMapMode;
    UINT8               filterKeyMask;
    BOOL                chkTypeMatic;
    BOOL                isMapped;
    EV_DEV_HANDLE *     pEvdev;
    FUNCPTR             ioctl;
    void *              pArg;
    } EV_DEV_KBD_HANDLE;
#endif /* _WRS_KERNEL */

/* prototypes */

#ifdef _WRS_KERNEL
extern EV_DEV_KBD_HANDLE *  evdevKbdReg (EV_DEV_DEVICE_DATA * pDevData);
extern STATUS               evdevKbdUnreg (EV_DEV_KBD_HANDLE * pKbdHandle);
extern STATUS               evdevKbdSendMsg (EV_DEV_KBD_HANDLE * pKbdHandle,
                                             EV_DEV_KBD_REPORT * pReport);
#endif /* _WRS_KERNEL */

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevLibKbdh */
