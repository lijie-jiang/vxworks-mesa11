/* evdevLibKbd.c - Keyboard Library */

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
13nov13,j_x  abstract kbd typematic handing to evdev kbd lib
12nov13,j_x  abstract kbd attach to shell to evdev kbd lib
31oct13,y_f  abstract kbd mapping to evdev lib
25sep13,j_x  add raw mode support
28aug13,y_f  fixed compiler warning
19jul13,j_x  create
*/

/*
DESCRIPTION
This file provides a keyboard class library that handles messages from keyboard
devices and controls the devices.
*/

/* includes */

#include <evdevLib.h>
#include <sysLib.h>
#include <tyLib.h>
#include <wdLib.h>

/* defines */

#define EV_DEV_KBD_TYPE_MATIC_RATE                      1 /* 1 second */
#define EV_DEV_KBD_TYPE_MATIC_RATE_MIN                  1 /* 1 tick */

/* typedefs */

/* structure to store the keyboard library information */

typedef struct evdevLibKbdData
    {
    TY_DEV_ID   pTyCoDev;
    UINT32      typeMaticPeriod;
    int         attachCount;
    }EV_DEV_LIB_KBD_DATA;

typedef struct evdevTycoDev
    {
    TY_DEV      tyDev;
    SIO_CHAN *  pSioChan;
    } EV_DEV_TYCO_DEV;

/* forward declarations */

LOCAL STATUS    evdevKbdIoctl (EV_DEV_KBD_HANDLE * pKbdHandle, int request,
                               _Vx_usr_arg_t arg);
LOCAL void      evdevKbdLedCheck (EV_DEV_KBD_HANDLE * pKbdHandle, UINT16 *
                                  pAscii, ssize_t nBytes);
LOCAL void      evdevKbdSendToTty (UINT16 key);
LOCAL STATUS    evdevKbdDummyCallback (void);
LOCAL void      evdevKbdShellAttach (void);
LOCAL void      evdevKbdShellDetach (void);
LOCAL STATUS    evdevKbdTypeMaticCallback (EV_DEV_KBD_HANDLE * pKbdHandle);
LOCAL STATUS    evdevKbdTypeMatic (EV_DEV_KBD_HANDLE * pKbdHandle, BOOL
                                   isTimerCb);
LOCAL STATUS    evdevKbdSendKey (EV_DEV_KBD_HANDLE * pKbdHandle, UINT16 keyCode,
                                 UINT8 state);

/* locals */

LOCAL EV_DEV_LIB_KBD_DATA * pKbdLibHd = NULL;

/* externals */

extern TY_DEV_ID    evdevKbdGetConsole (void);
extern BOOL         evdevKbdIsRedirectTty (void);

/* functions */

/*******************************************************************************
*
* evdevKbdInit - initialize the keyboard library
*
* This routine initializes the keyboard library.
*
* RETURNS: OK when successfully initialized; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevKbdInit
    (
    UINT32  typeMaticRate
    )
    {
    EV_DEV_LIB_KBD_DATA *   pKbdLib = NULL;

    pKbdLib = (EV_DEV_LIB_KBD_DATA *)calloc (1, sizeof (EV_DEV_LIB_KBD_DATA));
    if (NULL == pKbdLib)
        {
        return ERROR;
        }

    pKbdLib->typeMaticPeriod    = (sysClkRateGet () * typeMaticRate) / 1000;
    pKbdLibHd                   = pKbdLib;

    if (ERROR == evdevKbdMapInit ())
        {
        free (pKbdLib);
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* evdevKbdReg - register a keyboard device to evdev
*
* This routine registers a keyboard device to evdev.
*
* RETURNS: pointer of keyboard handle structure when the device successfully
* registered; otherwise NULL
*
* ERRNO: N/A
*
* \NOMANUAL
*/

EV_DEV_KBD_HANDLE * evdevKbdReg
    (
    EV_DEV_DEVICE_DATA *    pDevData
    )
    {
    EV_DEV_KBD_HANDLE * pKbdHandle = NULL;

    if ((NULL == pDevData) || (NULL == pDevData->ioctl))
        {
        return NULL;
        }

    pKbdHandle = (EV_DEV_KBD_HANDLE *)calloc (1, sizeof (EV_DEV_KBD_HANDLE));
    if (NULL == pKbdHandle)
        {
        return NULL;
        }

    pKbdHandle->hWd = wdCreate ();
    if (NULL == pKbdHandle->hWd)
        {
        free (pKbdHandle);
        return NULL;
        }

    pKbdHandle->countryCode = EV_DEV_KBD_LANG_ENGLISH_US;
    pKbdHandle->keyMapMode  = EV_DEV_KBD_UNICODE_MODE;
    pKbdHandle->ioctl       = pDevData->ioctl;
    pKbdHandle->pArg        = pDevData->pArg;
    pDevData->ioctl         = evdevKbdIoctl;
    pDevData->pArg          = pKbdHandle;

    pKbdHandle->pEvdev = evdevRegDev (pDevData);
    if (NULL == pKbdHandle->pEvdev)
        {
        if (NULL != pKbdHandle->hWd)
            {
            (void)wdDelete (pKbdHandle->hWd);
            }

        pDevData->ioctl = pKbdHandle->ioctl;
        pDevData->pArg  = pKbdHandle->pArg;
        free (pKbdHandle);
        return NULL;
        }

    evdevKbdShellAttach ();

    return pKbdHandle;
    }

/*******************************************************************************
*
* evdevKbdUnreg - unregister a keyboard device from evdev
*
* This routine unregisters a keyboard device from evdev.
*
* RETURNS: OK when the device successfully unregistered; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevKbdUnreg
    (
    EV_DEV_KBD_HANDLE * pKbdHandle
    )
    {
    if ((NULL == pKbdHandle) || (NULL == pKbdHandle->pEvdev))
        {
        return ERROR;
        }

    pKbdHandle->pEvdev->pDevData->ioctl = pKbdHandle->ioctl;
    pKbdHandle->pEvdev->pDevData->pArg  = pKbdHandle->pArg;
    (void)evdevUnregDev (pKbdHandle->pEvdev);
    (void)wdCancel (pKbdHandle->hWd);
    (void)wdDelete (pKbdHandle->hWd);
    evdevKbdShellDetach ();

    free (pKbdHandle);
    return OK;
    }

/*******************************************************************************
*
* evdevKbdSendMsg - send keyboard message
*
* This routine send keyboard message to evdev event queue.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevKbdSendMsg
    (
    EV_DEV_KBD_HANDLE * pKbdHandle,
    EV_DEV_KBD_REPORT * pReport
    )
    {
    if ((NULL == pKbdHandle) || (NULL == pReport))
        {
        return ERROR;
        }

    if (NULL != pKbdHandle->hWd)
        {
        (void)wdCancel (pKbdHandle->hWd);
        }

    memcpy ((void *)&pKbdHandle->report, (const void *)pReport,
            sizeof (EV_DEV_KBD_REPORT));
    return evdevKbdTypeMatic (pKbdHandle, FALSE);
    }

/*******************************************************************************
*
* evdevKbdIoctl - keyboard library ioctl
*
* This routine handles the ioctl calls made by the upper level application. It
* provides the processing for a user level application that requires the
* services be performed at the kernel level.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS evdevKbdIoctl
    (
    EV_DEV_KBD_HANDLE * pKbdHandle,
    int                 request,    /* ioctl function */
    _Vx_usr_arg_t       arg         /* function arg */
    )
    {
    STATUS                  result  = OK;
    EV_DEV_LIB_KBD_DATA *   pKbdLib = pKbdLibHd;

    switch (request)
        {
        case EV_DEV_IO_SET_COUNTRY_CODE:
            if (NULL == (int *)arg)
                {
                result = ERROR;
                break;
                }

            pKbdHandle->countryCode = *((UINT16 *)arg);
            break;

        case EV_DEV_IO_GET_KBD_MODE:
            if (NULL == (int *)arg)
                {
                result = ERROR;
                break;
                }

            *((UINT16 *)arg) = pKbdHandle->keyMapMode;
            break;

        case EV_DEV_IO_SET_KBD_MODE:
            if (NULL == (int *)arg)
                {
                result = ERROR;
                break;
                }

            pKbdHandle->keyMapMode = *((UINT16 *)arg);
            break;

        default:
            break;
        }

    if ((EV_DEV_IO_DEV_DIS == request) && (NULL != pKbdLib->pTyCoDev))
        {
        result = OK;
        }
    else
        {
        result = pKbdHandle->ioctl (pKbdHandle->pArg, request, arg);
        }

    return result;
    }

/*******************************************************************************
*
* evdevKbdLedCheck - find any led value in the report and switch it
*
* This routine finds any led value in the report and switch it.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void evdevKbdLedCheck
    (
    EV_DEV_KBD_HANDLE * pKbdHandle,
    UINT16 *            pAscii,
    ssize_t             keyCnt
    )
    {
    ssize_t i;
    UINT16  ledKeyCode;

    for (i = 0; i < keyCnt; i++)
        {
        switch (pAscii[i])
            {
            case EV_DEV_KBD_NUM_KEY:
            case EV_DEV_KBD_CAPS_KEY:
            case EV_DEV_KBD_SCR_KEY:
                ledKeyCode = pAscii[i];
                (void)pKbdHandle->ioctl (pKbdHandle->pArg, EV_DEV_IO_SET_LED,
                                         &ledKeyCode);
                break;

            default:
                break;
            }
        }
    }

/*******************************************************************************
*
* evdevKbdSendToTty - send ascii value to Tty
*
* This routine sends ascii value to Tty. It is used for console device.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void evdevKbdSendToTty
    (
    UINT16  key
    )
    {
    EV_DEV_LIB_KBD_DATA *   pKbdLib = pKbdLibHd;

    if (NULL == pKbdLib->pTyCoDev)
        {
        return;
        }

    if ((key <= EV_DEV_KBD_DEL_KEY) && (EV_DEV_KBD_SO_KEY != key))
        {
        (void)tyIRd ((TY_DEV_ID)pKbdLib->pTyCoDev, (char)key);
        }
    }

/*******************************************************************************
*
* evdevKbdDummyCallback - dummy function for SIO device
*
* This routine is dummy function for SIO device to redirect SIO received data
*
* RETURNS: always ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS evdevKbdDummyCallback (void)
    {
    return ERROR;
    }

/*******************************************************************************
*
* evdevKbdShellAttach - attach the keyboard to shell if it is configured
*
* This routine attaches the keyboard to the target shell or console.
* It redirects the IO and restarts the shell.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void evdevKbdShellAttach (void)
    {
    EV_DEV_LIB_KBD_DATA *   pKbdLib         = pKbdLibHd;
    EV_DEV_TYCO_DEV *       pTyCoDevAttach  = NULL;

    pTyCoDevAttach = (EV_DEV_TYCO_DEV *)evdevKbdGetConsole ();

    pKbdLib->pTyCoDev = (TY_DEV_ID)pTyCoDevAttach;

    if (NULL != pTyCoDevAttach)
        {
        if (0 == pKbdLib->attachCount)
            {
            if (evdevKbdIsRedirectTty ())
                {
                sioCallbackInstall (pTyCoDevAttach->pSioChan,
                    SIO_CALLBACK_PUT_RCV_CHAR,
                    (STATUS (*) (void *, ...))evdevKbdDummyCallback,
                    (void *)pTyCoDevAttach);
                }
            }
        pKbdLib->attachCount++;
        }
    }

/*******************************************************************************
*
* evdevKbdShellDetach - detach the keyboard from shell
*
* This routine detaches the keyboard from the target shell or console.
* It redirects the IO to the original console.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void evdevKbdShellDetach (void)
    {
    EV_DEV_LIB_KBD_DATA *   pKbdLib         = pKbdLibHd;
    EV_DEV_TYCO_DEV *       pTyCoDevAttach  = NULL;

    pTyCoDevAttach = (EV_DEV_TYCO_DEV *) pKbdLib->pTyCoDev;

    if (NULL == pTyCoDevAttach)
        {
        return;
        }

    pKbdLib->attachCount--;

    if (pKbdLib->attachCount <= 0)
        {
        if (evdevKbdIsRedirectTty ())
            {
            sioCallbackInstall (pTyCoDevAttach->pSioChan,
                                SIO_CALLBACK_PUT_RCV_CHAR,
                                (STATUS (*) (void *, ...))tyIRd,
                                (void *)pTyCoDevAttach);
            }
        }
    }

/*******************************************************************************
*
* evdevKbdTypeMaticCallback - trigger the tymatic handling for a device
*
* This routine triggers the tymatic handling for a device.
*
* RETURNS: always OK
*
* ERRNO: N/A
*/

LOCAL STATUS evdevKbdTypeMaticCallback
    (
    EV_DEV_KBD_HANDLE * pKbdHandle
    )
    {
    (void)excJobAdd ((VOIDFUNCPTR)evdevKbdTypeMatic, (_Vx_usr_arg_t)pKbdHandle,
                     (_Vx_usr_arg_t)TRUE, 0, 0, 0, 0);
    return OK;
    }

/*******************************************************************************
*
* evdevKbdTypeMatic - do the actual typematic handing
*
* This routine checks for a device whose key is currently pressed, and posts
* the associated message to evdev core.
*
* RETURNS: OK, or ERROR if there is anything wrong.
*
* ERRNO: N/A
*/

LOCAL STATUS evdevKbdTypeMatic
    (
    EV_DEV_KBD_HANDLE * pKbdHandle,
    BOOL                isTimerCb
    )
    {
    UINT16                  pUnicode[EV_DEV_BOOT_KEY_COUNT];
    ssize_t                 keyCnt          = 0;
    EV_DEV_LIB_KBD_DATA *   pKbdLib         = pKbdLibHd;
    UINT32                  typeMaticPeriod = EV_DEV_KBD_TYPE_MATIC_RATE_MIN;
    UINT16                  keyCode         = 0;
    UINT16                  unicode         = 0;
    UINT8                   state           = EV_DEV_KBD_KEY_UNPRESS;
    STATUS                  status          = OK;

    if (NULL == pKbdHandle)
        {
        return ERROR;
        }

    if (isTimerCb)
        {
        state   = EV_DEV_KBD_KEY_PRESS;
        unicode = pKbdHandle->lastUnicode;
        keyCode = pKbdHandle->lastKeycode;
        }
    else
        {
        if (pKbdHandle->isMapped)
            {
            if (0 != pKbdHandle->report.scanCodes[0])
                {
                state   = EV_DEV_KBD_KEY_PRESS;
                unicode = (UINT16)pKbdHandle->report.scanCodes[0];
                }
            else
                {
                state   = EV_DEV_KBD_KEY_UNPRESS;
                unicode = pKbdHandle->lastUnicode;
                }
            }
        else
            {
            bzero ((char *)pUnicode, sizeof (UINT16) * EV_DEV_BOOT_KEY_COUNT);
            keyCnt = evdevKbdReport2Unicode (pKbdHandle->countryCode,
                                             (UINT8 *)&pKbdHandle->filterKeyMask,
                                             &(pKbdHandle->report), pUnicode);
            if (0 != keyCnt)
                {
                evdevKbdLedCheck (pKbdHandle, pUnicode, keyCnt);
                }
            else
                {
                keyCnt = evdevKbdModifier2Unicode (pKbdHandle->lastModifiers,
                                                   pKbdHandle->report.modifiers,
                                                   pUnicode);
                }

            if (0 != keyCnt)
                {
                state   = EV_DEV_KBD_KEY_PRESS;
                unicode = pUnicode[0];
                }
            else
                {
                state   = EV_DEV_KBD_KEY_UNPRESS;
                unicode = pKbdHandle->lastUnicode;
                }

            if (EV_DEV_KBD_KEYCODE_MODE == pKbdHandle->keyMapMode)
                {
                bzero ((char *)pUnicode,
                       sizeof (UINT16) * EV_DEV_BOOT_KEY_COUNT);
                keyCnt = evdevKbdReport2Keycode (pKbdHandle->countryCode,
                                                 &(pKbdHandle->report),
                                                 pUnicode);
                if (0 == keyCnt)
                    {
                    keyCnt =
                        evdevKbdModifier2Keycode (pKbdHandle->lastModifiers,
                                                  pKbdHandle->report.modifiers,
                                                  pUnicode);
                    }

                if (0 != keyCnt)
                    {
                    keyCode = pUnicode[0];
                    }
                else
                    {
                    keyCode = pKbdHandle->lastKeycode;
                    }
                }
            }
        }

    if (0 != unicode)
        {
        if (EV_DEV_KBD_KEY_UNPRESS != state)
            {
            evdevKbdSendToTty (unicode);
            }

        if (EV_DEV_KBD_KEYCODE_MODE == pKbdHandle->keyMapMode)
            {
            status |= evdevKbdSendKey (pKbdHandle, keyCode, state);
            }
        else
            {
            status |= evdevKbdSendKey (pKbdHandle, unicode, state);
            }

        if ((NULL != pKbdHandle->hWd) && (pKbdHandle->chkTypeMatic) &&
            (EV_DEV_KBD_KEY_UNPRESS != state))
            {
            if (isTimerCb)
                {
                typeMaticPeriod = pKbdLib->typeMaticPeriod;
                }
            else
                {
                typeMaticPeriod = EV_DEV_KBD_TYPE_MATIC_RATE * sysClkRateGet ();
                }

            if (0 == typeMaticPeriod)
                {
                typeMaticPeriod = EV_DEV_KBD_TYPE_MATIC_RATE_MIN;
                }

            status |= wdStart (pKbdHandle->hWd, typeMaticPeriod,
                               (FUNCPTR)evdevKbdTypeMaticCallback,
                               (_Vx_usr_arg_t)pKbdHandle);
            }
        }
    else
        {
        status = ERROR;
        }

    if (EV_DEV_KBD_KEY_UNPRESS == state)
        {
        pKbdHandle->lastUnicode = 0;
        pKbdHandle->lastKeycode = 0;
        }
    else
        {
        pKbdHandle->lastUnicode = unicode;
        pKbdHandle->lastKeycode = keyCode;
        }

    pKbdHandle->lastModifiers = pKbdHandle->report.modifiers;

    return status;
    }

/*******************************************************************************
*
* evdevKbdSendKey - send a key code to evdev
*
* This routine sends a key code to evdev.
*
* RETURNS: OK, or ERROR if there is anything wrong.
*
* ERRNO: N/A
*/

LOCAL STATUS evdevKbdSendKey
    (
    EV_DEV_KBD_HANDLE * pKbdHandle,
    UINT16              keyCode,
    UINT8               state
    )
    {
#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    EV_DEV_MSG              msg;
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */
#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    EV_DEV_EVENT            event;
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */
    struct timeval          time;
    STATUS                  status  = OK;

    time.tv_sec     = tickGet () / sysClkRateGet ();
    time.tv_usec    = (tickGet () * 1000000) / sysClkRateGet () -
                      time.tv_sec * 1000000;

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    event.time.tv_sec   = time.tv_sec;
    event.time.tv_usec  = time.tv_usec;
    event.type          = EV_DEV_KEY;
    event.code          = keyCode;
    event.value         = (INT32)state;

    status = evdevSendMsg (pKbdHandle->pEvdev, (char *)&event,
                           sizeof (EV_DEV_EVENT));

    event.type  = EV_DEV_SYN;
    event.code  = 0;
    event.value = 0;
    status |= evdevSendMsg (pKbdHandle->pEvdev, (char *)&event,
                            sizeof (EV_DEV_EVENT));
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    msg.msgType                         = EV_DEV_MSG_KBD;
    msg.msgData.kbdData.value           = keyCode;
    msg.msgData.kbdData.state           = state;
    msg.msgData.kbdData.time.tv_sec     = time.tv_sec;
    msg.msgData.kbdData.time.tv_usec    = time.tv_usec;
    status |= evdevSendMsg (pKbdHandle->pEvdev, (char *)&msg,
                            sizeof (EV_DEV_MSG));
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

    return status;
    }
