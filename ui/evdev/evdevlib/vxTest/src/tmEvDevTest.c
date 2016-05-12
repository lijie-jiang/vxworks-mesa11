/* tmEvDevTest.c - Test case for evdev */

/*
 * Copyright (c) 2015-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
21jan16,zjl  clean build warnings
21dec15,zjl  clean build warnings and fix some code issues
17dec15,zjl  initialize startIx to 0 in evdevTestDevsMsg and modify 
             all the case name 
25nov15,zjl  Increase testcase timeout
19Nov15,jnl  create 
*/

/*
DESCRIPTION
This file provides a basic test cases for event device framework library.


\cs
<module>
    <component>        INCLUDE_TM_EVDEV  </component>
    <modUnderTest>     evdevTest      </modUnderTest>
    <minVxWorksVer>    7.0           </minVxWorksVer>
    <maxVxWorksVer>    .*            </maxVxWorksVer>
    <arch>             .*                     </arch>
    <cpu>              .*                      </cpu>
    <bsp>                                      </bsp>
</module>
\ce
*/

/*
DESCRIPTION
This file provides a basic test cases for event device framework library.
*/

/* includes */

#include <vxTest.h>
#include <evdevLib.h>
#include <evdevLibKbdMap.h>
#include <taskLib.h>
#include <selectLib.h>
#include <evdevLibTs.h>

/* defines */

#define EV_DEV_TEST_TASK_PRIO           205
#define EV_DEV_TEST_TASK_STACK_SIZE     (1024 * 4)

#define EV_DEV_TEST_STOP                (1 << 6)
#define EVDEV_TEST_DRIVER_NAME          "Evdev Test"
#define EVDEV_MAX_NUM                   EV_DEV_DEVICE_MAX

#define EVDEV_TEST_OK                   OK
#define EVDEV_TEST_ERROR                (0x0FFFFFFF)
#define IOERROR                         (ERROR)

#define EVDEV_TEST_OPTICAL              (1)
#define EVDEV_TEST_COMPATIBLE           (1 << 1)

#define EVDEV_TEST_KBD                  (EV_DEV_KEY)
#define EVDEV_TEST_PTR                  (EV_DEV_KEY | EV_DEV_REL)
#define EVDEV_TEST_TS                   (EV_DEV_KEY | EV_DEV_ABS)

#undef  vxTestMsg
#define vxTestMsg(level, ...)           do \
                                            { \
                                            printf(__VA_ARGS__); \
                                            printf("\n"); \
                                            } \
                                            while (0)

#define STOP_VALUE                      ((INT)('S' << 24 + 'T' << 16 + \
                                               'O' << 8 +'P'))
#define VXSIMTS_NAME                    "vxSimTs"

#define EVDEV_TEST_ERROR_OK             (0)
#define EVDEV_TEST_ERROR_NODEV          (1 << 1)
#define EVDEV_TEST_ERROR_OPEN           (1 << 2)
#define EVDEV_TEST_ERROR_CLOSE          (1 << 3)
#define EVDEV_TEST_ERROR_CLEARMSG       (1 << 4)
#define EVDEV_TEST_ERROR_MSGSIZE        (1 << 5)
#define EVDEV_TEST_ERROR_PROCMSG        (1 << 6)
#define EVDEV_TEST_ERROR_GETCAP         (1 << 7)
#define EVDEV_TEST_ERROR_WRITE          (1 << 8)
#define EVDEV_TEST_ERROR_READ           (1 << 9)
#define EVDEV_TEST_ERROR_GETVERSION     (1 << 10)
#define EVDEV_TEST_ERROR_SETOPERDEV     (1 << 11)
#define EVDEV_TEST_ERROR_GETINFO        (1 << 12)
#define EVDEV_TEST_ERROR_GETNAME        (1 << 13)

#define KBD_VK_NUM                      0x0E
#define KBD_FN_NUM                      0x0D
#define KBD_ASCII_NUM                   0x80

#define VXSIM_TS_RELEASE_TIMEOUT        (300)   /* ms */
#define VXSIM_TS_SAMPLE_TIMEOUT         (100)   /* ms */

#define ARRAY_SIZE(x)                   (sizeof (x) / sizeof (x[0]))

#define MARK(flag, bit)                 (flag |= (1 << bit))
#define UNMARK(flag, bit)               (flag &= (~(1 << bit)))
#define CHECKMARK(flag, bit)            (flag &= (1 << bit))

#define TOUCH_POINTS_MAX                5
#define TS_MOVE_MIN                     2
#define EVDEV_WHEEL_TS                  (1 << 5)

#define NO_SEM                          ((SEM_ID)0xFFFFFFFF)

#define SemMsgNoSem(idx)                {pVxSimDev[idx]->semMsg = NO_SEM;}

#define SemMsgCre(idx)    do \
                              { \
                              if (pVxSimDev[idx]->semMsg != NO_SEM) \
                                  { \
                                  pVxSimDev[idx]->semMsg = \
                                  semBCreate (SEM_Q_FIFO, SEM_EMPTY); \
                                  } \
                              else \
                                  { \
                                  pVxSimDev[idx]->semMsg = NULL; \
                                  } \
                              }while(0)

#define SemMsgDel(idx)    do \
                              { \
                              if (pVxSimDev[idx]->semMsg) \
                                  { \
                                  (void)semDelete(pVxSimDev[idx]->semMsg); \
                                  } \
                              }while(0)
#define SemMsgTake(idx)   do \
                              { \
                              if (pVxSimDev[idx]->semMsg) \
                                  { \
                                  (void)semTake(pVxSimDev[idx]->semMsg, \
                                          WAIT_FOREVER); \
                                  } \
                              }while(0)
#define SemMsgGive(idx)   do \
                              { \
                              if (pVxSimDev[idx]->semMsg) \
                                  { \
                                  (void)semGive(pVxSimDev[idx]->semMsg); \
                                  } \
                              }while(0)

#define GETPOINTNUM(x, n) {n = 0; while(x){(x) &= ((x) - 1); (n)++;}}

/* typedefs */

typedef struct evDevSem
    {
    SEM_ID              semId;          /* semaphore id */
    VXTEST_STATUS       status;         /* test status */
    VXTEST_VERBOSITY    verbosity;
    char *              pMsg;
    int                 timerStatus;    /* timer exited or not */
    int                 options;        /* semaphore options */
    SEM_B_STATE         initState;      /* initial state of semaphore */
    int                 initCount;      /* initial count */
    }evDevSem;

typedef INT32 (* pAddFunc)(void);

typedef struct evFbSize
    {
    int width;
    int height;
    } EV_FBSIZE;

typedef struct vxSimPtrDev
    {
    int                 inputDevHdr;
    UINT32              buttonState;
    UINT32              dllDataRead;
    UINT32              dllAnyData;
    UINT32              dllGetEvent;
    BOOL                isEn;
    int                 evdevFd;
    EV_DEV_DEVICE_DATA  devData;
    EV_DEV_HANDLE *     pEvdev;
    } VX_SIM_PTR_DEV;

typedef struct VxSimTsDev
    {
    int                 pioEn;
    int                 evdevFd;
    TS_DEVICE_INFO *    pDevInfo;
    TS_DEVICE           tsDev;
    } VX_SIM_TS_DEV;

typedef struct vxSimKbdDev
    {
    int                 inputDevHdr;
    UINT32              dllDataRead;
    UINT32              dllAnyData;
    UINT32              dllGetEvent;
    BOOL                isEn;
    int                 evdevFd;
    EV_DEV_DEVICE_DATA  devData;
    EV_DEV_KBD_HANDLE * pKbdHandle;
    } VX_SIM_KBD_DEV;

typedef struct vxSimDev
    {
    UINT32              devType;
    SEM_ID              semMsg;
    SEM_ID              semTask;          /* semaphore id */
    VXTEST_STATUS       status;           /* test status */
    VXTEST_VERBOSITY    verbosity;
    UINT32              msgNum;
    struct result 
        {
        UINT32          msgNum;
        UINT32          failNum;
        UINT32          errorNo;
        }opt, com;
    char *              pMsg;
    union
        {
        VX_SIM_PTR_DEV  ptr;
        VX_SIM_KBD_DEV  kbd;
        VX_SIM_TS_DEV   ts;
        }dev;
    } VX_SIM_DEV;

typedef struct MTPRESS
    {
    struct
        {
        UINT32          num;
        UINT32          pressed;
        }send;

    struct
        {
        UINT32          num;
        UINT32          id;
        UINT32          pressed;
        }check;
    }MTPRESS;

typedef struct EVDEV_RECORDMSG
    {
    EV_DEV_MSG_TYPE     msgType;
    EV_DEV_MSG          send;
    EV_DEV_MSG          receive;
    EV_DEV_MSG          expect;
    }EVDEV_RECORDMSG;

/* globals */

EV_FBSIZE       gFbInfo = {32, 20};

EVDEV_RECORDMSG gMsgRec;

/* 2 */

MTPRESS MT_2[] = 
    {
    {{1, 0x01}, {1, 0x01, 0x01}},
    {{2, 0x03}, {2, 0x03, 0x03}},
    {{2, 0x02}, {2, 0x03, 0x02}},
    {{2, 0x02}, {1, 0x02, 0x02}},
    {{2, 0x00}, {1, 0x02, 0x00}},
    };

/* 3 */

MTPRESS MT_3[] = 
    {
    {{1, 0x01}, {1, 0x01, 0x01}},
    {{2, 0x03}, {2, 0x03, 0x03}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x05}, {3, 0x07, 0x05}},
    {{3, 0x05}, {2, 0x05, 0x05}},
    {{3, 0x04}, {2, 0x05, 0x04}},
    {{3, 0x04}, {1, 0x04, 0x04}},
    {{3, 0x04}, {1, 0x04, 0x04}},
    {{3, 0x06}, {2, 0x05, 0x05}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x06}, {3, 0x07, 0x06}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x03}, {3, 0x07, 0x03}},
    {{3, 0x01}, {2, 0x03, 0x01}},
    {{3, 0x00}, {1, 0x01, 0x00}},
    };

/* 4 */

MTPRESS MT_4[] = 
    {
    {{1, 0x01}, {1, 0x01, 0x01}},
    {{2, 0x03}, {2, 0x03, 0x03}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x05}, {3, 0x07, 0x05}},
    {{3, 0x05}, {2, 0x05, 0x05}},
    {{3, 0x04}, {2, 0x05, 0x04}},
    {{3, 0x04}, {1, 0x04, 0x04}},
    {{3, 0x04}, {1, 0x04, 0x04}},
    {{3, 0x06}, {2, 0x05, 0x05}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{4, 0x0F}, {4, 0x0F, 0x0F}},
    {{4, 0x0F}, {4, 0x0F, 0x0F}},
    {{4, 0x07}, {4, 0x0F, 0x07}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{3, 0x03}, {3, 0x07, 0x03}},
    {{3, 0x01}, {2, 0x03, 0x01}},
    {{3, 0x00}, {1, 0x01, 0x00}},
    };

/* 6 */

MTPRESS MT_6[] = 
    {
    {{1, 0x01}, {1, 0x01, 0x01}},
    {{2, 0x03}, {2, 0x03, 0x03}},
    {{3, 0x07}, {3, 0x07, 0x07}},
    {{4, 0x0F}, {4, 0x0F, 0x0F}},
    {{5, 0x1F}, {5, 0x1F, 0x1F}},
    {{6, 0x3F}, {6, 0x3F, 0x3F}},
    };

/* locals */

LOCAL volatile int          evDevHd = IOERROR;
UINT32                      gRealDevsNum;
LOCAL volatile VX_SIM_DEV * pVxSimDev[EVDEV_MAX_NUM + 4] = {NULL};
EV_DEV_POINT                gRectSize                    = {0, 0};

LOCAL UINT8 vxAsciiMap[KBD_ASCII_NUM] =
    {
    0,    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x2A, 0x2B, 0x28, 0x0E, 
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,   
    0x1B, 0x1C, 0x1D, 0x29, 0x32, 0x30, 0x31, 0x33, 0x2C, 0x1E, 0x34, 0x20, 
    0x21, 0x22, 0x24, 0x34, 0x26, 0x27, 0x25, 0x2E, 0x36, 0x2D, 0x37, 0x38,
    0x27, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x33, 0x33, 
    0x36, 0x2E, 0x37, 0x38, 0x1F, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 
    0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x2F, 0x31, 0x30, 0x23, 0x2D,
    0x35, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 
    0x1B, 0x1C, 0x1D, 0x2F, 0x31, 0x30, 0x35, 0x4C
    };

LOCAL UINT8 vxVkMap[KBD_VK_NUM] =
    {
    0x4A, 0x4D, 0x49, 0x4B, 0x4E, 0x50, 0x4F, 0x52, 0x51, 0x46, 0x48, 0x39, 
    0x53, 0x47
    };

LOCAL UINT8 vxFnMap[KBD_FN_NUM] =
    {
    0,    0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44,
    0x45
    };

LOCAL TS_CALIB_DATA vxSimCalData[] =
    {
        {

        /* touch screen name */

        {VXSIMTS_NAME},       

        /* display rect */

        {0, 0, (480 - 1), (272 - 1)},   
        5,

        /* display point */

        {{0, 0}, {479, 0}, {479, 271}, {0, 271},  {240, 136}},

        /* touch screen point */

        {{0, 0}, {479, 0}, {479, 271}, {0, 271},  {240, 136}},
        }
    };

/* forward declarations */

/* functions */

INT32 addSimPtr (void);
INT32 addSimKbd (void);
INT32 evDevProcessOptMsg (EV_DEV_MSG * evdevMsg);
INT32 evDevGetKeyEvent (EV_DEV_EVENT * pEvent);
STATUS rmSimDev (UINT32 devIdx);
STATUS rmAllSimDevs (UINT32 startIdx, UINT32 endIdx);
UINT32 evDevTestOpenClose (char * evdevName);
UINT32 evDevTestReadWrite (char * evdevName, UINT32 sizeOfMsg);
UINT32 evdevGetDevCount (void);
UINT32 evdevTestBasic (char *evdevName, UINT32 devIdx, UINT32 mode);
LOCAL STATUS vxTestSimDevIoctl (VX_SIM_PTR_DEV * pDev, int request, _Vx_usr_arg_t arg);
LOCAL STATUS evdevProcessComMsg (EV_DEV_EVENT * evdevEvent, UINT32 devCap);
LOCAL STATUS evDevTestMsgProcTask (UINT32 mode, UINT32 devIdx);
LOCAL STATUS evDevTestSendPtrMsgTask (int disWidth, int disHeight, UINT32 devIdx);
LOCAL STATUS evDevTestSendKbdMsgTask (UINT32 devIdx);
VXTEST_STATUS evDevTestMessage (UINT32 mode, UINT32 devIdx);    
VXTEST_STATUS evdevTestMaxDevs (void);
VXTEST_STATUS evdevRunStandIO (UINT32 devIdx, UINT32 mode);
VXTEST_STATUS evdevTestComStandIO (void);
VXTEST_STATUS evdevTestOptStandIO (void);
VXTEST_STATUS evdevTestDevsMsg (void);

/*******************************************************************************
*
*
* ERRNO: N/A
*/

void tmEvdevTestInit (void)
    {
    return;
    }

/*******************************************************************************
*
* vxTestSimDevIoctl -ioctl for vxSim device 
*
* This routine ioctl operation for the virtual vxSim device.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS vxTestSimDevIoctl
    (
    VX_SIM_PTR_DEV * pDev,
    int              request,    /* ioctl function */
    _Vx_usr_arg_t    arg         /* function arg */
    )
    {
    STATUS  result = OK;

    /* check arguments */

    if (NULL == pDev)
        {
        errno = EFAULT;
        return ERROR;
        }

    switch (request)
        {
        case EV_DEV_IO_DEV_EN:
            pDev->isEn = TRUE;
            break;

        case EV_DEV_IO_DEV_DIS:
            pDev->isEn = FALSE;
            break;

        default:
            break;
        }

    return result;
    }

/*******************************************************************************
*
* addSimPtr - add a virtual Simulator pointer device to the evdev framework
*
* This routine add a virtual Simulator pointer device to the evdev framework.
*
* RETURNS: 
* The index of the added virtual device in the evdev, or ERROR if the virtual 
* device cannot be added
*
* ERRNO: N/A
*
*/

INT32 addSimPtr (void)
    {
    VX_SIM_DEV *     pSimDev = NULL;
    VX_SIM_PTR_DEV * pDev    = NULL;

    pSimDev = calloc (1, sizeof (VX_SIM_DEV));
    if (NULL == pSimDev)
        {
        return EVDEV_TEST_ERROR;
        }

    pSimDev->devType = EVDEV_TEST_PTR;

    pDev = &(pSimDev->dev.ptr);
    pDev->isEn = FALSE;

    /* initialize Dll */

    pDev->devData.devCap          = EV_DEV_KEY | EV_DEV_REL;
    pDev->devData.pDevName        = EVDEV_TEST_DRIVER_NAME;
    pDev->devData.ioctl           = (FUNCPTR)vxTestSimDevIoctl;
    pDev->devData.pArg            = (void *)pDev;
    pDev->devData.devInfo.bustype = EV_DEV_BUS_VIRTUAL;
    pDev->devData.devInfo.vendor  = 0;
    pDev->devData.devInfo.product = 0;
    pDev->devData.devInfo.version = 0;

    pDev->pEvdev = evdevPtrReg (&pDev->devData);
    if (NULL == pDev->pEvdev)
        {
        free (pSimDev);
        return EVDEV_TEST_ERROR;
        }

    pDev->evdevFd = IOERROR;
    pVxSimDev[pDev->pEvdev->devIdx] = pSimDev;
    printf("ptr idx: %d  pdev:0x%x\n", pDev->pEvdev->devIdx, pDev);

    return pDev->pEvdev->devIdx;
    }

LOCAL STATUS vxSimTsFunc 
    (
    void * pInst
    )
    {
    return OK;
    }

/*******************************************************************************
*
* addSimTs - add a virtual touch screen device to the evdev framework
*
* This routine add a virtual touch screen device to the evdev framework.
*
* RETURNS: 
* The index of the added virtual device in the evdev, or ERROR if the virtual 
* device cannot be added
*
* ERRNO: N/A
*
*/

INT32 addSimTs (void)
    {
    VX_SIM_DEV    * pSimDev = NULL;
    VX_SIM_TS_DEV * pDev    = NULL;

    pSimDev = calloc (1, sizeof (VX_SIM_DEV));
    if (NULL == pSimDev)
        {
        return EVDEV_TEST_ERROR;
        }

    pSimDev->devType = EVDEV_TEST_TS;

    pDev = &(pSimDev->dev.ts);
    bzero ((char *)pDev, sizeof (TS_DEVICE));

    pDev->tsDev.devAttr = TS_DEV_ATTR_INTERRUPT |
                          TS_DEV_ATTR_MULTITOUCH;
    pDev->tsDev.releasePeriod
        = (VXSIM_TS_RELEASE_TIMEOUT * sysClkRateGet()) / 1000;

    pDev->tsDev.pInst                     = (void *)1;
    pDev->tsDev.pCalData                  = vxSimCalData;
    pDev->tsDev.calDataCount              = NELEMENTS (vxSimCalData);
    pDev->tsDev.func.open                 = vxSimTsFunc;
    pDev->tsDev.func.close                = vxSimTsFunc;
    pDev->tsDev.func.ioctl                = vxSimTsFunc;
    pDev->tsDev.func.read                 = vxSimTsFunc;
    pDev->tsDev.devData.devCap            = EV_DEV_KEY | EV_DEV_ABS;
    pDev->tsDev.devData.pDevName          = VXSIMTS_NAME;
    pDev->tsDev.devData.devInfo.bustype   = EV_DEV_BUS_I2C;
    pDev->tsDev.devData.devInfo.vendor    = 0;
    pDev->tsDev.devData.devInfo.product   = 0;
    pDev->tsDev.devData.devInfo.version   = 0;

    pDev->tsDev.defDisRect.left           = vxSimCalData[0].disRect.left;
    pDev->tsDev.defDisRect.top            = vxSimCalData[0].disRect.top;
    pDev->tsDev.defDisRect.right          = vxSimCalData[0].disRect.right;
    pDev->tsDev.defDisRect.bottom         = vxSimCalData[0].disRect.bottom;

    pDev->pDevInfo = evdevTsReg (&(pDev->tsDev));

    if (NULL == pDev->pDevInfo)
        {
        free (pSimDev);
        return EVDEV_TEST_ERROR;
        }

    pDev->evdevFd = IOERROR;
    pVxSimDev[pDev->pDevInfo->pEvdev->devIdx] = pSimDev;
    printf("ts idx: %d  pdev:0x%x\n", pDev->pDevInfo->pEvdev->devIdx, pDev);

    return pDev->pDevInfo->pEvdev->devIdx;
    }

/*******************************************************************************
*
* addSimKbd - add a virtual keyboard device to the evdev framework
*
* This routine add a virtual keyboard device to the evdev framework.
*
* RETURNS: 
* The index of the added virtual device in the evdev, or ERROR if the virtual 
* device cannot be added
*
* ERRNO: N/A
*
*/

INT32 addSimKbd (void)
    {
    VX_SIM_DEV     * pSimDev = NULL;
    VX_SIM_KBD_DEV * pDev    = NULL;

    pSimDev = calloc (1, sizeof (VX_SIM_DEV));
    if (NULL == pSimDev)
        {
        return EVDEV_TEST_ERROR;
        }

    pSimDev->devType = EVDEV_TEST_KBD;

    pDev = &(pSimDev->dev.kbd);
    pDev->isEn = FALSE;

    /* initialize Dll */

    pDev->devData.devCap          = EV_DEV_KEY;
    pDev->devData.pDevName        = EVDEV_TEST_DRIVER_NAME;
    pDev->devData.ioctl           = (FUNCPTR)vxTestSimDevIoctl;
    pDev->devData.pArg            = (void *)pDev;
    pDev->devData.devInfo.bustype = EV_DEV_BUS_VIRTUAL;
    pDev->devData.devInfo.vendor  = 0;
    pDev->devData.devInfo.product = 0;
    pDev->devData.devInfo.version = 0;
    pDev->pKbdHandle              = evdevKbdReg (&pDev->devData);

    if (NULL == pDev->pKbdHandle)
        {
        free (pSimDev);
        return EVDEV_TEST_ERROR;
        }

    pDev->evdevFd = IOERROR;
    pVxSimDev [pDev->pKbdHandle->pEvdev->devIdx] = pSimDev;
    printf("kbd idx: %d  pdev:0x%x\n", pDev->pKbdHandle->pEvdev->devIdx, pDev);

    return pDev->pKbdHandle->pEvdev->devIdx;
    }

/*******************************************************************************
*
* rmSimDev - remove a virtual device from the evdev framework
*
* This routine remove a virtual device to the evdev framework.
*
* RETURNS: 
* OK, or ERROR if the virtual device cann't be removed
*
* ERRNO: N/A
*
*/

STATUS rmSimDev 
    (
    UINT32 devIdx
    )
    {
    if(devIdx >= ARRAY_SIZE(pVxSimDev))
        {
        return EVDEV_TEST_ERROR;
        }    

    if (NULL == pVxSimDev[devIdx])
        {
        return EVDEV_TEST_ERROR;
        }

    if(EVDEV_TEST_PTR == pVxSimDev[devIdx]->devType)
        {
        (void)evdevPtrUnreg(pVxSimDev[devIdx]->dev.ptr.pEvdev);
        }
    else if(EVDEV_TEST_KBD == pVxSimDev[devIdx]->devType)
        {
        (void)evdevKbdUnreg(pVxSimDev[devIdx]->dev.kbd.pKbdHandle);
        }
    else if(EVDEV_TEST_TS == pVxSimDev[devIdx]->devType)
        {
        (void)evdevTsUnreg(pVxSimDev[devIdx]->dev.ts.pDevInfo);
        }

    free ((VX_SIM_DEV *)pVxSimDev[devIdx]);
    pVxSimDev[devIdx] = NULL;

    return EVDEV_TEST_OK;
    }

/*******************************************************************************
*
* rmSimDev - remove all the virtual devices from the evdev framework
*
* This routine remove all the virtual devices from the evdev framework.
*
* RETURNS: 
* OK, or ERROR if the virtual devices cann't be removed
*
* ERRNO: N/A
*
*/

STATUS rmAllSimDevs 
    (
    UINT32 startIdx, 
    UINT32 endIdx
    )
    {
    STATUS result = EVDEV_TEST_OK;
    UINT32 idx;

    if(endIdx >= EVDEV_MAX_NUM)
        {
        endIdx = EVDEV_MAX_NUM -1;
        }

    if(endIdx < startIdx)
        {
        return EVDEV_TEST_ERROR;
        }
    printf("startIdx %d, endIdx = %d\n", startIdx, endIdx);
    endIdx += 1;
    for(idx = startIdx; idx < endIdx; idx++)
        {
        if(EVDEV_TEST_ERROR == rmSimDev(idx))
            {
            result = EVDEV_TEST_ERROR;
            }
        }
    return result;
    }

/*******************************************************************************
*
* evDevProcessOptMsg - process the received message by optical mode
*
* This routine process the received message by optical mode.
*
* RETURNS: OK, or ERROR if send failed
*
* ERRNO: N/A
*/

INT32 evDevProcessOptMsg
    (
    EV_DEV_MSG * evdevMsg
    )
    {
    INT32                res       = EVDEV_TEST_OK;
    EV_DEV_PTR_DATA *    pPtrData  = &(gMsgRec.send.msgData.ptrData);
    EV_DEV_PTR_MT_DATA * pMtExpect = &(gMsgRec.expect.msgData.mtData);

    switch (evdevMsg->msgType)
        {
        case EV_DEV_MSG_KBD:

            break;

        case EV_DEV_MSG_PTR:
            if(evdevMsg->msgData.ptrData.wheel)
                {
                res = memcmp(&(pPtrData->type), 
                             &(evdevMsg->msgData.ptrData.type),
                             sizeof(EV_DEV_PTR_DATA)-sizeof(pPtrData->time));
                if(res)
                    {
                    res = EVDEV_TEST_ERROR;
                    }
                }
            else
            	{
                if((pMtExpect->points[0].x != 
                    evdevMsg->msgData.ptrData.position.x) ||
                    (pMtExpect->points[0].y != 
                     evdevMsg->msgData.ptrData.position.y))
                    {
                    res = EVDEV_TEST_ERROR;
                    }
            	}

            if( EV_DEV_TEST_STOP == evdevMsg->msgData.ptrData.wheel)
                {
                res = EV_DEV_TEST_STOP;
                vxTestMsg(V_GENERAL, "OptMsg:EV_DEV_TEST_STOP");
                }
            break;

        case EV_DEV_MSG_HOT_PLUG:
            if (evdevMsg->msgData.hotPlug.isPlugin)
                {
                vxTestMsg(V_GENERAL, 
                          "Hot plug message: Plugged in. DevIdx = %d",
                          evdevMsg->msgData.hotPlug.devIdx);
                }
            else
                {
                vxTestMsg(V_GENERAL, 
                          "Hot plug message: Pulled out. DevIdx = %d",
                          evdevMsg->msgData.hotPlug.devIdx);
                }
            break;

        case EV_DEV_MSG_MT:
                
            res = memcmp(&(pMtExpect->count), &(evdevMsg->msgData.mtData.count),
                         sizeof(TS_POINT) * (pMtExpect->count) + 
                         sizeof(pMtExpect->count));
            if (res)
                {
                res = EVDEV_TEST_ERROR;
                }
            break;

        default:
            vxTestMsg(V_GENERAL, "Unknown message: Type= 0x%x\n", 
                      evdevMsg->msgType);
            res = EVDEV_TEST_ERROR;
        }

    return res;
    }

/*******************************************************************************
*
* evDevTestProcessMsg - process the received message
*
* This routine process the received message.
*
* RETURNS: OK, or ERROR if send failed
*
* ERRNO: N/A
*/

INT32 evDevGetKeyEvent
    (
    EV_DEV_EVENT * pEvent
    )
    {
    INT32             res      = EVDEV_TEST_OK;
    EV_DEV_PTR_DATA * pPtrData = &(gMsgRec.send.msgData.ptrData);

    if (0 != (pPtrData->buttonChange & EV_DEV_PTR_BTN_LEFT_BIT))
        {
#if ((_VX_CPU_FAMILY != _VX_SIMLINUX) && (_VX_CPU_FAMILY != _VX_SIMNT))
        if (EV_DEV_ABS == pPtrData->type)
            {
            pEvent->code  = EV_DEV_PTR_BTN_TOUCH;
            }
        else
#endif /* (_VX_CPU_FAMILY != _VX_SIMLINUX) && (_VX_CPU_FAMILY != _VX_SIMNT) */
            {
            pEvent->code  = EV_DEV_PTR_BTN_LEFT;
            }
        pEvent->value = pPtrData->buttonState & EV_DEV_PTR_BTN_LEFT_BIT;
        }

    if (0 != (pPtrData->buttonChange & EV_DEV_PTR_BTN_RIGHT_BIT))
        {
        pEvent->code  = EV_DEV_PTR_BTN_RIGHT;
        pEvent->value = pPtrData->buttonState & EV_DEV_PTR_BTN_RIGHT_BIT;
        }

    if (0 != (pPtrData->buttonChange & EV_DEV_PTR_BTN_MIDDLE_BIT))
        {
        pEvent->code  = EV_DEV_PTR_BTN_MIDDLE;
        pEvent->value = pPtrData->buttonState & EV_DEV_PTR_BTN_MIDDLE_BIT;
        }

    return res;
    }

/*******************************************************************************
*
* evdevProcessComMsg - process the received message by compatible mode
*
* This routine process the received message by compatible mode.
*
* RETURNS: OK, or ERROR if send failed
*
* ERRNO: N/A
*/

LOCAL STATUS evdevProcessComMsg
    (
    EV_DEV_EVENT * evdevEvent, 
    UINT32 devCap
    )
    {
    INT32                res        = EVDEV_TEST_OK;
    EV_DEV_PTR_DATA    * pPtrData   = &(gMsgRec.send.msgData.ptrData);
    EV_DEV_PTR_MT_DATA * pMtExpect  = &(gMsgRec.expect.msgData.mtData);
    EV_DEV_PTR_MT_DATA * pMtReceive = &(gMsgRec.receive.msgData.mtData);

    EV_DEV_EVENT    event;

    switch (evdevEvent->type)
        {
        case EV_DEV_SYN:
            if (EV_DEV_MSG_MT == gMsgRec.send.msgType)
                {
                if ((pMtReceive->count) || (pMtExpect->count))
                    {
                    res = memcmp(&(pMtExpect->count), &(pMtReceive->count),
                                        sizeof (TS_POINT) * (pMtExpect->count) + 
                                        sizeof (pMtExpect->count));
                    }
                    if(res)
                        {
                        res = EVDEV_TEST_ERROR;
                        }
                }
            break;

        case EV_DEV_KEY:
            if (devCap & EV_DEV_REL)
                {
                evDevGetKeyEvent (&event);
                if ((evdevEvent->code != event.code) || 
                    (evdevEvent->value != event.value))
                    {
                    res = EVDEV_TEST_ERROR;
                    }
                }
            else if (devCap & EV_DEV_ABS)
                {
                if (TRUE == pMtExpect->points[0].pressed)
                    {
                    if ((evdevEvent->code != EV_DEV_PTR_BTN_TOUCH) || 
                        (evdevEvent->value != EV_DEV_PTR_BTN_LEFT_BIT))
                        {
                        res = EVDEV_TEST_ERROR;
                        }
                    }
                }
            break;

        case EV_DEV_REL:
            {
            switch (evdevEvent->code)
                {
                case EV_DEV_PTR_REL_X:
                    if (pPtrData->position.x != evdevEvent->value)
                       {
                       res = EVDEV_TEST_ERROR;
                       }
                    break;

                case EV_DEV_PTR_REL_Y:
                    if (pPtrData->position.y != evdevEvent->value)
                        {
                        res = EVDEV_TEST_ERROR;
                    	}
                    break;

                case EV_DEV_PTR_REL_WHEEL:
                    if (pPtrData->wheel != evdevEvent->value)
                        {
                        res = EVDEV_TEST_ERROR;
                    	}
                    if (EV_DEV_TEST_STOP == evdevEvent->value)
                        {
                        res = EV_DEV_TEST_STOP;
                        vxTestMsg (V_GENERAL, "ComMsg:EV_DEV_TEST_STOP\n");
                        }
                    break;

                default:
                    vxTestMsg (V_GENERAL,"EV_DEV_REL:Unknown code = 0x%x\n", 
                               evdevEvent->code);
                }
            }
            break;

        case EV_DEV_ABS:
            {
            switch (evdevEvent->code)
                {
                case EV_DEV_PTR_ABS_X:
                    if (EV_DEV_MSG_MT == gMsgRec.send.msgType)
                        {
                        if (pMtExpect->points[0].x != evdevEvent->value)
                            {
                            res = EVDEV_TEST_ERROR;
                            }    
                        }
                    else
                        {
                        if(pPtrData->position.x != evdevEvent->value)
                            {
                            res = EVDEV_TEST_ERROR;
                            }    
                        }
                    break;

                case EV_DEV_PTR_ABS_Y:
                    if (EV_DEV_MSG_MT == gMsgRec.send.msgType)
                        {
                        if (pMtExpect->points[0].y != evdevEvent->value)
                            {
                            res = EVDEV_TEST_ERROR;
                            }    
                        }
                    else
                        {
                        if(pPtrData->position.y != evdevEvent->value)
                            {
                            res = EVDEV_TEST_ERROR;
                            }    
                        }
                    break;

                case EV_DEV_PTR_ABS_MT_SLOT:
                    pMtReceive->points[pMtReceive->count].id = 
                    evdevEvent->value;
                    break;

                case EV_DEV_PTR_ABS_MT_POSITION_X:
                    pMtReceive->points[pMtReceive->count].x = evdevEvent->value;
                    break;

                case EV_DEV_PTR_ABS_MT_POSITION_Y:
                    pMtReceive->points[pMtReceive->count].y = evdevEvent->value;
                    pMtReceive->count++;
                    break;

                case EV_DEV_PTR_ABS_MT_TRACKING_ID:
                    if (evdevEvent->value == -1)
                        {
                        pMtReceive->points[pMtReceive->count].x = 
                        pMtExpect->points[pMtReceive->count].x;
                        pMtReceive->points[pMtReceive->count].y = 
                        pMtExpect->points[pMtReceive->count].y;
                        pMtReceive->points[pMtReceive->count].pressed = FALSE;
                        pMtReceive->count++;
                        }
                    else if (evdevEvent->value == 
                             pMtReceive->points[pMtReceive->count].id)
                    	{
                        pMtReceive->points[pMtReceive->count].pressed = TRUE;
                    	}
                    else
                    	{
                        res = EVDEV_TEST_ERROR;
                    	}
                    break;

                case EV_DEV_PTR_REL_WHEEL:
                    if (pPtrData->wheel != evdevEvent->value)
                        {
                        res = EVDEV_TEST_ERROR;
                    	}
                    if (EV_DEV_TEST_STOP == evdevEvent->value)
                        {
                        res = EV_DEV_TEST_STOP;
                        }
                    break;

                default:
                    vxTestMsg(V_GENERAL,"EV_DEV_ABS:Unknown code = 0x%x\n", 
                              evdevEvent->code);
                }
            }
            break;

        default:
            vxTestMsg (V_GENERAL,"ComMsg:Unknown event = 0x%x\n", evdevEvent->type);
        }

	return res;
    }

/*******************************************************************************
*
* evDevTestMsgProcTask - used to process message
*
* This routine used to process message.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evDevTestMsgProcTask
    (
    UINT32 mode, 
    UINT32 devIdx
    )
    {
    int            msgCount;
    EV_DEV_MSG     evDevMsg;
    char           evdevName[EV_DEV_NAME_LEN + 1];
    int            evDevFd  = IOERROR;
    UINT32         msgSize  = 0;
    int            result   = EVDEV_TEST_OK;
    UINT32         devCap   = 0;
    EV_DEV_EVENT * pEvent   = (EV_DEV_EVENT *)(&evDevMsg);
    UINT32 *       pMsgNum  = NULL;
    UINT32 *       pFailNum = NULL;
    UINT32 *       pErrorNo = NULL;

    volatile VX_SIM_DEV * pVxDev = pVxSimDev[devIdx];

    if (pVxDev == NULL)
        {
        goto error;
        }

    bzero (evdevName, sizeof (evdevName));
    if (EVDEV_TEST_COMPATIBLE == mode)
        {
        (void) (void) snprintf ((char *)evdevName, EV_DEV_NAME_LEN, "%s%d", 
                  EV_DEV_NAME_PREFIX, devIdx);
        msgSize = sizeof (EV_DEV_EVENT);
        pMsgNum = (UINT32 *)&(pVxDev->com.msgNum);
        pFailNum = (UINT32 *)&(pVxDev->com.failNum);
        pErrorNo = (UINT32 *)&(pVxDev->com.errorNo);
        *pMsgNum = 1;
        }
    else
        {
        (void) (void) snprintf ((char *)evdevName, EV_DEV_NAME_LEN, "%s", EV_DEV_NAME);
        msgSize = sizeof (EV_DEV_MSG);
        pMsgNum = (UINT32 *)&(pVxDev->opt.msgNum);
        pFailNum = (UINT32 *)&(pVxDev->opt.failNum);
        pErrorNo = (UINT32 *)&(pVxDev->opt.errorNo);
        *pMsgNum = 0;
        }
    *pFailNum = 0;
    *pErrorNo |= EVDEV_TEST_ERROR_OK;

    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        vxTestMsg (V_FAIL, "Open event device failed!");
        *pErrorNo |= EVDEV_TEST_ERROR_OPEN;
        goto error;
        }

    if (EVDEV_TEST_COMPATIBLE == mode)
        {
        if (ERROR == ioctl (evDevFd, EV_DEV_IO_GET_CAP, (char *)&devCap))
            {
            *pErrorNo |= EVDEV_TEST_ERROR_GETCAP;
            goto error;
            }
        }

    SemMsgGive(devIdx);
    vxTestMsg(V_GENERAL, "---Begin receiving messages from event device.---");

    while (1)
        {
        msgCount = (int)read (evDevFd, (char *)&evDevMsg, msgSize);
        if (msgSize == msgCount)
            {
            if (EVDEV_TEST_OPTICAL == mode)
                {
#ifndef _WRS_CONFIG_EVDEV_SINGLE_TOUCH_MODE
                if ((evDevMsg.msgType == EV_DEV_MSG_PTR) &&
                    (EVDEV_TEST_TS == pVxDev->devType) &&
                    (evDevMsg.msgData.ptrData.wheel != EV_DEV_TEST_STOP))
                    {
                    *pErrorNo |= EVDEV_TEST_ERROR_PROCMSG;
                    (*pFailNum)++;
                    goto error;
                    }
#endif
                result = evDevProcessOptMsg(&evDevMsg);
                (*pMsgNum)++;
                SemMsgGive(devIdx);
                }
            else
                {
                result = evdevProcessComMsg((EV_DEV_EVENT*)(&evDevMsg), devCap);
                if(EV_DEV_SYN == pEvent->type)
                    {
                    (*pMsgNum)++;
                    SemMsgGive(devIdx);
                    }
                }

            if (result == EVDEV_TEST_ERROR)
                {
                *pErrorNo |= EVDEV_TEST_ERROR_PROCMSG;
                (*pFailNum)++;
                goto error;
                }
            else if (result == EV_DEV_TEST_STOP)
                {
                break;
                }
            }
        else
            {
            *pErrorNo |= EVDEV_TEST_ERROR_MSGSIZE;
            vxTestMsg (V_FAIL, "The size doesn't equal to EV_DEV_MSG!");
            goto error;
            }

        }
    vxTestMsg(V_GENERAL, "---Stop receiving message from event device.---");

    if (IOERROR == close (evDevFd))
        {
        *pErrorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close event device failed!");
        goto error;
        }

    (void)semGive(pVxDev->semTask);

    return EVDEV_TEST_OK;

error:
    vxTestMsg (V_GENERAL, "---Error:Stop receiving message.---");
    if (IOERROR != evDevFd)
        {
        (void)close (evDevFd);
        }
    if(pVxDev)
        {
        (void)semGive (pVxDev->semTask);
    	}
    return EVDEV_TEST_ERROR;
    }

/*******************************************************************************
*
* evDevTestSendPtrMsgTask - used to send pointer position message
* 
* This routine send pointer position message.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evDevTestSendPtrMsgTask
    (
    int disWidth, 
    int disHeight, 
    UINT32  
    devIdx
    )
    {
    EV_DEV_RECT       rect;
    EV_DEV_HANDLE *   pEvdevHandle   = pVxSimDev[devIdx]->dev.ptr.pEvdev;
    EV_DEV_PTR_DATA * pPtrData       = &(gMsgRec.send.msgData.ptrData);
    UINT32            msgNum         = 0;
    int               i              = 0;
    int               j              = 0;
    int               k              = 0;

    UINT32            buttonState[3] = {
                                       EV_DEV_PTR_BTN_LEFT_BIT, 
                                       EV_DEV_PTR_BTN_RIGHT_BIT,
                                       EV_DEV_PTR_BTN_MIDDLE_BIT};

    rect.left                        = 0;
    rect.top                         = 0;
    rect.right                       = disWidth - 1;
    rect.bottom                      = disHeight - 1;

    memset (pPtrData, 0, sizeof(EV_DEV_PTR_DATA));

    for (k = 0; k < sizeof (buttonState) / sizeof (buttonState[0]); k++)
        {
        for(i = rect.left; i < rect.right; i += 1)
            {
            for(j = rect.top; j < rect.bottom; j += 1)
                {
                SemMsgTake(devIdx);
                pPtrData->id           = 0;
                pPtrData->type         = EV_DEV_ABS;
                pPtrData->buttonState  = buttonState[k];
                pPtrData->buttonChange = 0;
                pPtrData->position.x   = i;
                pPtrData->position.y   = j;
                pPtrData->wheel        = 1;
                (void)evdevPtrSendMsg (pEvdevHandle, pPtrData);
                msgNum++;
                }
            }
    
        for(i = rect.left; i < rect.right; i += 1)
            {
            for(j = rect.top; j < rect.bottom; j += 1)
                {
                SemMsgTake(devIdx);
                pPtrData->id           = 0;
                pPtrData->type         = EV_DEV_REL;
                pPtrData->buttonState  = buttonState[k];
                pPtrData->buttonChange = 0;
                pPtrData->position.x   = i;
                pPtrData->position.y   = j;
                pPtrData->wheel        = 1;
                (void)evdevPtrSendMsg (pEvdevHandle, pPtrData);
                msgNum++;
                }
            }

        }

    SemMsgTake (devIdx);
    pVxSimDev[devIdx]->msgNum = msgNum + 1;
    memset (pPtrData, 0, sizeof (EV_DEV_PTR_DATA));
    pPtrData->type  = EV_DEV_ABS;
    pPtrData->wheel = EV_DEV_TEST_STOP;
    (void)evdevPtrSendMsg (pEvdevHandle, pPtrData);

    return EVDEV_TEST_OK;
}

/*******************************************************************************
*
* evSendPtrStopMsg - stop to send pointer position message
* 
* This routine stop to send pointer position message.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evSendPtrStopMsg 
    (
    UINT32 devIdx
    )
    {
    EV_DEV_PTR_DATA * pPtrData = &(gMsgRec.send.msgData.ptrData);
    TS_DEVICE_INFO  * pDevInfo = pVxSimDev[devIdx]->dev.ts.pDevInfo;

    memset (pPtrData, 0, sizeof(EV_DEV_PTR_DATA));
    pPtrData->type  = EV_DEV_ABS;
    pPtrData->wheel = EV_DEV_TEST_STOP;
    (void)evdevPtrSendMsg(pDevInfo->pEvdev, pPtrData);

    return EVDEV_TEST_OK;
    }

/*******************************************************************************
*
* evDevTestSendTsMtMsgTask - send multipoint touch screen position message
* 
* This routine stop to send multipoint touch screen position message.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evDevTestSendTsMtMsgTask
    (
    int    disWidth, 
    int    disHeight, 
    UINT32 devIdx
    )
    {
    int                  k          = 0;
    UINT32               p          = 0;
    UINT32               pos        = 0;
    UINT32               w          = 0;
    int                  sp         = 0;
    int                  ep         = 1;
    EV_DEV_RECT          rect;
    TS_DEVICE_INFO     * pDevInfo   = pVxSimDev[devIdx]->dev.ts.pDevInfo;

    EV_DEV_PTR_MT_DATA * pMtSend    = &(gMsgRec.send.msgData.mtData);
    EV_DEV_PTR_MT_DATA * pMtExpect  = &(gMsgRec.expect.msgData.mtData);
    EV_DEV_PTR_MT_DATA * pMtReceive = &(gMsgRec.receive.msgData.mtData);
    UINT32               press      = 0; 
    UINT32               lastPress  = 0;
    UINT32               msgPoints  = 0;
    UINT32               temp       = 0;
    UINT32               num        = 0;
    UINT32               msgNum     = 0;

    gMsgRec.send.msgType = EV_DEV_MSG_MT;
    rect.left            = vxSimCalData[0].disRect.left;
    rect.top             = vxSimCalData[0].disRect.top;
    rect.right           = vxSimCalData[0].disRect.right;
    rect.bottom          = vxSimCalData[0].disRect.bottom;

    w = rect.right - rect.left;

    memset (pMtSend, 0, sizeof (EV_DEV_PTR_MT_DATA));
    pMtSend->count = TS_DEV_MULTITOUCH_MAX;

    for (sp = 0; sp < TS_DEV_MULTITOUCH_MAX; sp++)
        {
        for (ep = sp; ep < TS_DEV_MULTITOUCH_MAX; ep++)
            {
            SemMsgTake(devIdx);
            bzero((char *)pMtExpect, sizeof (EV_DEV_PTR_MT_DATA));
            bzero((char *)pMtReceive, sizeof (EV_DEV_PTR_MT_DATA));

            press = 0;
            for (p = sp; p <= ep; p++)
                {
                press |= (1 << p);
            	}

            if ((sp == TS_DEV_MULTITOUCH_MAX - 1) && 
                (ep == TS_DEV_MULTITOUCH_MAX - 1))
                {
                press = 0;    
                }
            msgPoints = (lastPress | press);
            lastPress = press;

            for (k = 0; k < TS_DEV_MULTITOUCH_MAX; k++)
                {
                pMtSend->points[k].pressed = !!(press & (1 << k));
                if (pMtSend->points[k].pressed)
                    {
                    pMtSend->points[k].id = k;
                    pMtSend->points[k].x = pos/w + rect.left;
                    pMtSend->points[k].y = pos%w + rect.top;
                    pos += TS_MOVE_MIN;
                    }
                }

            k = 0;
            temp = msgPoints;
            GETPOINTNUM (temp, num);
            pMtExpect->count = num;
            for (p = 0; p < num; p++)
                {
                while (k < TS_DEV_MULTITOUCH_MAX)
                    {
                    if (msgPoints & (1 << k))
                        {
                        pMtExpect->points[p].id = k;
                        pMtExpect->points[p].pressed = pMtSend->points[k].pressed;
                        pMtExpect->points[p].x = pMtSend->points[k].x;
                        pMtExpect->points[p].y = pMtSend->points[k].y;                
                        k++;                
                        break;
                        }
                    k++;
                    }
            	 }
            if (pVxSimDev[devIdx]->dev.ts.tsDev.devAttr & TS_DEV_ATTR_MULTITOUCH)
                {
                (void)evdevTsSendMtMsg (pDevInfo, pMtSend);
                }
            else
                {
                (void)evdevTsSendMsg(pDevInfo, &(pMtSend->points[0]));
                }
                msgNum+=2;

            }
        }    

    SemMsgTake (devIdx);
    evSendPtrStopMsg (devIdx);
    pVxSimDev[devIdx]->msgNum = msgNum + 1;

    return EVDEV_TEST_OK;
    } 

/*******************************************************************************
*
* evDevTestSendKbdMsgTask - send keyboard message
* 
* This routine send keyboard message.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evDevTestSendKbdMsgTask
    (
    UINT32 devIdx
    )
    {
    UINT32               i           = 0; 
    UINT32               j           = 0;
    UINT32               msgNum      = 0;
    EV_DEV_KBD_REPORT    report;
    EV_DEV_KBD_HANDLE  * pKbdHandle  = pVxSimDev[devIdx]->dev.kbd.pKbdHandle;
    STATUS               status      = OK;

    UINT32               modifiers[] = {
                                       EV_DEV_KBD_NO_MODIFIER, 
                                       EV_DEV_KBD_LEFT_ALT, 
                                       EV_DEV_KBD_RIGHT_ALT,
                                       EV_DEV_KBD_LEFT_SHIFT, 
                                       EV_DEV_KBD_RIGHT_SHIFT,
                                       EV_DEV_KBD_LEFT_CTRL,
                                       EV_DEV_KBD_RIGHT_CTRL,
                                       EV_DEV_KBD_LEFT_GUI,
                                       EV_DEV_KBD_RIGHT_GUI
                                       };
    UINT32               num         = sizeof(modifiers)/sizeof(modifiers[0]);

    bzero ((char *)&report, sizeof (EV_DEV_KBD_REPORT));

    SemMsgTake(devIdx);

    for (i = 0; i < num; i++)
        {
        report.modifiers = (UINT8) modifiers[i];
        for (j = 0; j < KBD_ASCII_NUM; j++)
            {
            report.scanCodes[0] = vxAsciiMap[j];
            status = evdevKbdSendMsg(pKbdHandle, &report);
            if (status != ERROR)
                {
                msgNum++;
                SemMsgTake (devIdx);
                }
            }
        for (j = 0; j < KBD_VK_NUM; j++)
            {
            report.scanCodes[0] = vxVkMap[j];
            status = evdevKbdSendMsg (pKbdHandle, &report);
            if (status != ERROR)
                {
                msgNum++;
                SemMsgTake (devIdx);
                }
            }
        for (j = 0; j < KBD_FN_NUM; j++)
            {
            report.scanCodes[0] = vxFnMap[j];
            status = evdevKbdSendMsg (pKbdHandle, &report);
            if(status != ERROR)
                {
                msgNum++;
                SemMsgTake (devIdx);
                }
            }
        }
    vxTestMsg (V_IMPERATIVE, "Send keyboard message finished");

    pVxSimDev[devIdx]->msgNum = msgNum + 1;
    memset (&(gMsgRec.send.msgData.ptrData), 0, sizeof (EV_DEV_PTR_DATA));
    gMsgRec.send.msgData.ptrData.type  = EV_DEV_ABS;
    gMsgRec.send.msgData.ptrData.wheel = EV_DEV_TEST_STOP;
    (void)evdevPtrSendMsg(pKbdHandle->pEvdev, &(gMsgRec.send.msgData.ptrData));

    return EVDEV_TEST_OK;
    }  

/******************************************************************************
*
* evDevTestMessage - test test routine for evdev message
*
* This routine start 2 or 3 tasks to run test case for evdev pointer or 
* keyboard message
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
* successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS evDevTestMessage
    (
    UINT32 mode,
    UINT32 devIdx
    )
    {
    TASK_ID taskIdP1  = TASK_ID_ERROR;
    TASK_ID taskIdP2  = TASK_ID_ERROR;
    TASK_ID taskId2   = TASK_ID_ERROR;    
    int disWidth  = gFbInfo.width;
    int disHeight = gFbInfo.height;

    volatile VX_SIM_DEV * pVxDev = NULL;

    if (devIdx >= EVDEV_MAX_NUM)
    	{
        return VXTEST_FAIL;
    	}
#ifndef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    mode &= (~EVDEV_TEST_OPTICAL);
#endif
#ifndef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    mode &= (~ EVDEV_TEST_COMPATIBLE);
#endif
    if (!((mode & EVDEV_TEST_OPTICAL) || 
          (mode&EVDEV_TEST_COMPATIBLE)))
    	{
        return VXTEST_FAIL;
    	}        

    if (pVxSimDev[devIdx])
        {
        pVxDev = pVxSimDev[devIdx];
        pVxDev->semTask = semCCreate (SEM_Q_FIFO, 0);
        SemMsgCre(devIdx);
        pVxDev->pMsg = NULL;
        }
    else
        {
        return VXTEST_FAIL; 
        }

    bzero ((char *)&gMsgRec, sizeof (gMsgRec));

    if (mode & EVDEV_TEST_OPTICAL)
    	{
        taskIdP1 = taskSpawn ("OpticalMsgProc", EV_DEV_TEST_TASK_PRIO + 1, 0,
                                                EV_DEV_TEST_TASK_STACK_SIZE,
                                                (FUNCPTR)evDevTestMsgProcTask,
                                                EVDEV_TEST_OPTICAL, devIdx,
                                                0, 0, 0, 0, 0, 0, 0, 0);
        if (TASK_ID_ERROR == taskIdP1)
            {
            vxTestMsg (V_FAIL, "taskSpawn evDevTestMsgProcTask failed");
            return VXTEST_FAIL;
            }
        }

    if (mode & EVDEV_TEST_COMPATIBLE)
    	{
        taskIdP2 = taskSpawn ("ComMsgProc", EV_DEV_TEST_TASK_PRIO + 1, 0,
                                                EV_DEV_TEST_TASK_STACK_SIZE,
                                                (FUNCPTR)evDevTestMsgProcTask,
                                                EVDEV_TEST_COMPATIBLE, devIdx,
                                                0, 0, 0, 0, 0, 0, 0, 0);
        if (TASK_ID_ERROR == taskIdP2)
            {
            vxTestMsg (V_FAIL, "taskSpawn evDevTestMsgProcTask failed");
            return VXTEST_FAIL;
            }
        }

    if (EVDEV_TEST_PTR == pVxDev->devType)
        {
        taskId2 = taskSpawn ("evDevTestSendPtrMsg", EV_DEV_TEST_TASK_PRIO, 0,
                                                EV_DEV_TEST_TASK_STACK_SIZE,
                                                (FUNCPTR)evDevTestSendPtrMsgTask,
                                                (_Vx_usr_arg_t)disWidth,
                                                (_Vx_usr_arg_t)disHeight,
                                                devIdx, 0, 0, 0, 0, 0, 0, 0);
        }
    else if (EVDEV_TEST_KBD == pVxDev->devType)
        {
        taskId2 = taskSpawn ("evDevTestSendKbdMsg", EV_DEV_TEST_TASK_PRIO, 0,
                                                EV_DEV_TEST_TASK_STACK_SIZE,
                                                (FUNCPTR)evDevTestSendKbdMsgTask,
                                                devIdx, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        }
    else if (EVDEV_TEST_TS == pVxDev->devType)
        {
        taskId2 = taskSpawn ("evDevTestSendTsMsg", EV_DEV_TEST_TASK_PRIO, 0,
                                                EV_DEV_TEST_TASK_STACK_SIZE,
                                                (FUNCPTR)evDevTestSendTsMtMsgTask,
                                                (_Vx_usr_arg_t)disWidth,
                                                (_Vx_usr_arg_t)disHeight,
                                                devIdx, 0, 0, 0, 0, 0, 0, 0);
        }

    if (TASK_ID_ERROR == taskId2)
        {
        if (TASK_ID_ERROR != taskIdP1)
            {
            (void)taskDelete(taskIdP1);
            }
        if (TASK_ID_ERROR != taskIdP2)
            {
            (void)taskDelete(taskIdP2);
            }
        (void)semDelete (pVxDev->semTask);
        SemMsgDel (devIdx);
        vxTestMsg (V_FAIL, "taskSpawn evDevTestSendPtrPosTask failed");
        return VXTEST_FAIL;
        }

    if (mode&EVDEV_TEST_OPTICAL)
        {
        (void)semTake (pVxDev->semTask, WAIT_FOREVER);
        }
    if (mode&EVDEV_TEST_COMPATIBLE)
        {
        (void)semTake (pVxDev->semTask, WAIT_FOREVER);
        }

    (void)semDelete (pVxDev->semTask);
    SemMsgDelSemMsgDel (devIdx);

    if (TASK_ID_ERROR != taskIdP1)
        {
        (void)taskDelete(taskIdP1);
        }
    if (TASK_ID_ERROR != taskIdP2)
        {
        (void)taskDelete(taskIdP2);
        }

    (void)taskDelete(taskId2);

    if (pVxDev->pMsg)
        {
        vxTestMsg (pVxDev->verbosity, pVxDev->pMsg);
        }

    vxTestMsg (V_GENERAL, "MsgNum = %d", pVxDev->msgNum);
    vxTestMsg (V_GENERAL, "opt.msgNum = %d", pVxDev->opt.msgNum);
    vxTestMsg (V_GENERAL, "opt.failNum = %d", pVxDev->opt.failNum);
    vxTestMsg (V_GENERAL, "opt.errorNo = %d", pVxDev->opt.errorNo);
    vxTestMsg (V_GENERAL, "com.msgNum = %d", pVxDev->com.msgNum);
    vxTestMsg (V_GENERAL, "com.failNum = %d", pVxDev->com.failNum);
    vxTestMsg (V_GENERAL, "com.errorNo = %d", pVxDev->com.errorNo);

    if ((pVxDev->opt.failNum) || (pVxDev->com.failNum) || 
        (pVxDev->opt.errorNo) || (pVxDev->com.errorNo))
        {
        pVxDev->status = VXTEST_FAIL;
        }

    return pVxDev->status;
    }

/*******************************************************************************
*
* evDevTestOpenClose - open and close evdev
*
* This routine open and close evdev.
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
* successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

UINT32 evDevTestOpenClose
    (
    char * evdevName
    )
    {
    int    num      = 0;
    int    evDevFd  = IOERROR;
    int    evDevFd2 = IOERROR;
    UINT32 errorNo  = EVDEV_TEST_ERROR_OK;

    /* 1 */

    if (evdevName == NULL)
        {
        evdevName = EV_DEV_NAME;
        }

    num++;
    evDevFd = open(evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg(V_FAIL, "Open %s failed!", evdevName);
        goto error_oc;
        }

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg(V_FAIL, "Close %s failed!", evdevName);
        goto error_oc;
        }

    /* 2 */

    num++;
    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg (V_FAIL, "Open %s failed!", evdevName);
        goto error_oc;
        }

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s failed!", evdevName);
        goto error_oc;
        }

    /* 3 */

    num++;
    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg (V_FAIL, "Open %s failed!", evdevName);
        goto error_oc;
        }

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s failed!", evdevName);
        goto error_oc;
        }

    /* 4 */

    num++;
    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg (V_FAIL, "Open %s failed!", evdevName);
        goto error_oc;
        }

    evDevFd2 = open (evdevName, 0, 0);
    if (IOERROR != evDevFd2)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg (V_FAIL, "Reopen %s success!", evdevName);
        (void)close (evDevFd2);
        evDevFd2 = IOERROR;
        goto error_oc;
        }

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s failed!", evdevName);
        goto error_oc;
        }

    /* 5 */

    num++;
    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg (V_FAIL, "Open %s failed!", evdevName);
        goto error_oc;
        }

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s failed!", evdevName);
        goto error_oc;
        }

    if (EVDEV_TEST_OK == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Reclose %s success!", evdevName);
        goto error_oc;
        }

    return errorNo;
error_oc:
    errorNo += (num<<16);
    vxTestMsg (V_FAIL, "errorNo = 0x%x", errorNo);

    if (IOERROR != evDevFd)
        {
        (void)close (evDevFd);
        }

    return errorNo;
    }

/******************************************************************************
*
* evDevTestReadWrite - test routine for evdev read and write
*
* This routine read and write of evdev
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

UINT32 evDevTestReadWrite
    (
    char * evdevName, 
    UINT32 sizeOfMsg
    )
    {
    int        evDevFd  = IOERROR;
    EV_DEV_MSG evDevMsg;
    char *     pEvDevMsg;
    ssize_t    msgCount;
    int        i        = 0;
    int        j        = 0;
    int        failSize = -1;
    UINT32     errorNo  = EVDEV_TEST_ERROR_OK;
    int        num      = 0;

    /* 1 */

    if (evdevName == NULL)
        {
        evdevName = EV_DEV_NAME;
        }

    num++;
    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg(V_FAIL, "Open %s failed!", evdevName);
        goto error_rw;
        }

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s failed!", evdevName);
        goto error_rw;
        }

    msgCount = write (evDevFd, (char *)&evDevMsg, (size_t) sizeOfMsg);
    if (IOERROR != msgCount )
        {
        errorNo |= EVDEV_TEST_ERROR_WRITE;
        vxTestMsg (V_GENERAL, "msgCount = %d", msgCount);
        vxTestMsg (V_FAIL, "When evDev closed, write message success!");
        }
    msgCount = read (evDevFd, (char *)&evDevMsg, sizeOfMsg);
    if (IOERROR != msgCount)
        {
        errorNo |= EVDEV_TEST_ERROR_READ;
        vxTestMsg (V_GENERAL, "msgCount = %d",msgCount);
        vxTestMsg (V_FAIL, "When evDev closed, read message success!");
        }

    /* 2 */

    num++;
    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg (V_FAIL, "Open %s failed!", evdevName);
        goto error_rw;
        }

    msgCount = write (evDevFd, NULL, (size_t) sizeOfMsg);
    if (failSize != msgCount)
        {
        errorNo |= EVDEV_TEST_ERROR_WRITE;
        vxTestMsg (V_GENERAL, "msgCount = %d",msgCount);
        vxTestMsg (V_FAIL, "Error:write buffer = NULL success!");
        }

    msgCount = read (evDevFd, NULL, sizeOfMsg);
    if (failSize != msgCount)
        {
        errorNo |= EVDEV_TEST_ERROR_READ;
        vxTestMsg (V_GENERAL, "msgCount = %d", msgCount);
        vxTestMsg (V_FAIL, "Error:read buffer = NULL success!");
        }

    /* 3 */

    num++;
    msgCount = write (evDevFd, (char *)&evDevMsg, 0);
    if (0 != msgCount)
        {
        errorNo |= EVDEV_TEST_ERROR_WRITE;
        vxTestMsg (V_GENERAL, "msgCount = %d", msgCount);
        vxTestMsg (V_FAIL, "Error:write size=0 success!");
        }

    msgCount = read (evDevFd, (char *)&evDevMsg, 0);
    if (0 != msgCount)
        {
        errorNo |= EVDEV_TEST_ERROR_READ;
        vxTestMsg (V_GENERAL, "msgCount = %d", msgCount);
        vxTestMsg (V_FAIL, "Error:read size=0 success!");
        }

    /* 4 */

    num++;
    msgCount = write (evDevFd, 0, 0);
    if (failSize != msgCount)
        {
        errorNo |= EVDEV_TEST_ERROR_WRITE;
        vxTestMsg (V_GENERAL, "msgCount = %d", msgCount);
        vxTestMsg (V_FAIL, "Error:write buffer=0 size=0 success!");
        }

    msgCount = read (evDevFd, 0, 0);
    if (0 != msgCount)
        {
        errorNo |= EVDEV_TEST_ERROR_READ;
        vxTestMsg (V_GENERAL, "msgCount = %d", msgCount);
        vxTestMsg (V_FAIL, "Error:read buffer=0 size=0 success!");
        }

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s failed!", evdevName);
        goto error_rw;
        }

    /* 5 */

    num++;
    evDevFd = open (evdevName, 0, 0);
    if (IOERROR == evDevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg(V_FAIL, "Open %s failed!", evdevName);
        goto error_rw;
        }

    for(i = 1; i <= EV_DEV_MIN_MSG_NUM; i++)
        {
        for(j = 0; j < i; j++)
            {
            msgCount = write (evDevFd, (char *)&evDevMsg, sizeOfMsg);
            if (sizeOfMsg != msgCount)
                {
                vxTestMsg (V_FAIL, "Error:write i=%d j=%d", i, j);
                errorNo |= EVDEV_TEST_ERROR_WRITE;
                }
            }

        for (j = 0; j < i; j++)
            {
            msgCount = read (evDevFd, (char *)&evDevMsg, sizeOfMsg);
            if (sizeOfMsg != msgCount)
                {
                errorNo |= EVDEV_TEST_ERROR_READ;
                vxTestMsg (V_FAIL, "Error:read i=%d j=%d", i, j);
                }
            }
        }

    pEvDevMsg = (char *)malloc (sizeOfMsg * EV_DEV_MIN_MSG_NUM);
    if (NULL == pEvDevMsg)
        {
        vxTestMsg (V_FAIL, "malloc pEvDevMsg buffer Error!");
        }
    else
        {
        int msgSize = 0;

        for (i = 1; i <= EV_DEV_MIN_MSG_NUM; i++)
            {
            msgSize = sizeOfMsg * i;
            msgCount = write (evDevFd, (char *)pEvDevMsg, msgSize);
            if (msgSize != msgCount)
                {
                errorNo |= EVDEV_TEST_ERROR_WRITE;
                vxTestMsg (V_FAIL, "Error:write i=%d",i);
                }

            msgCount = read (evDevFd, (char *)pEvDevMsg, msgSize);
            if (msgSize != msgCount)
                {
                errorNo |= EVDEV_TEST_ERROR_READ;
                vxTestMsg (V_FAIL, "Error:read i=%d", i);
                }
            }
        }
    free (pEvDevMsg);

    if (IOERROR == close (evDevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s failed!", evdevName);
        goto error_rw;
        }

    return errorNo;

error_rw:

    errorNo += (num << 16);
    vxTestMsg(V_FAIL, "errorNo = 0x%x", errorNo);

    if (IOERROR != evDevFd)
        {
        (void)close (evDevFd);
        }

    return VXTEST_FAIL;
    }

/*******************************************************************************
*
* evdevGetDevCount - get the number of real devices
*
* This routine opens the event device and reads the messages from the device by
* using optimized API.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

UINT32 evdevGetDevCount (void)
    {
    int    evdevCoreFd = IOERROR;
    UINT32 devCount    = EVDEV_TEST_ERROR;
    UINT32 devCount2   = EVDEV_TEST_ERROR;

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    evdevCoreFd = open (EV_DEV_NAME, 0, 0);
    if (ERROR == evdevCoreFd)
        {
        vxTestMsg (V_FAIL, "Open %s fail\n", EV_DEV_NAME);
        return EVDEV_TEST_ERROR;
        }

    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_DEV_COUNT,
                        (char *)&devCount))
        {
        vxTestMsg (V_FAIL, "EV_DEV_IO_GET_DEV_COUNT fail\n");
        (void)close (evdevCoreFd);
        return EVDEV_TEST_ERROR;
        }

    if (ERROR == close (evdevCoreFd))
        {
        vxTestMsg (V_FAIL, "Close %s fail\n", EV_DEV_NAME);
        evdevCoreFd = ERROR;
        }
#endif

    devCount2 = addSimKbd();
    if (EVDEV_TEST_ERROR == devCount2)
        {
        return EVDEV_TEST_ERROR;
        }
    if (EVDEV_TEST_ERROR == rmSimDev (devCount2))
        {
        return EVDEV_TEST_ERROR;
        }

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    if (devCount != devCount2)
        {
        vxTestMsg(V_FAIL, "Device count1 = %d ,count2 =%d\n", devCount, 
                  devCount2);
        return EVDEV_TEST_ERROR;
        }
#endif

    if (devCount2 > EVDEV_MAX_NUM)
        {
        return EVDEV_TEST_ERROR;
        }
    printf ("Device count = %d\n", devCount2);

    return devCount2;
    }

/*******************************************************************************
*
* evdevTestBasic - test the ioctl of the evdev
*
* This routine opens the event device and set and get the parameter with ioctl
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

UINT32 evdevTestBasic 
    (
    char * evdevName, 
    UINT32 devIdx, 
    UINT32 mode
    )
    {
    int                evdevFd  = IOERROR;
    char               devName[EV_DEV_NAME_LEN + 1];
    UINT32             evdevVer = 0;
    EV_DEV_DEVICE_INFO devInfo;
    UINT32             devCap   = 0;
    UINT32             errorNo  = EVDEV_TEST_ERROR_OK;

    evdevFd = open (evdevName, 0, 0);
    if (IOERROR == evdevFd)
        {
        errorNo |= EVDEV_TEST_ERROR_OPEN;
        vxTestMsg (V_FAIL, "Open %s failed!", evdevName);
        return errorNo;
        }

    /* 1 */

    if (IOERROR == ioctl (evdevFd, EV_DEV_IO_GET_VERSION, (char *)&evdevVer))
        {
        errorNo |= EVDEV_TEST_ERROR_GETVERSION;
        vxTestMsg (V_FAIL, "EV_DEV_IO_GET_VERSION failed!\n");
        }
    if (EV_DEV_VERSION != evdevVer)
        {
        errorNo |= EVDEV_TEST_ERROR_GETVERSION;
        vxTestMsg (V_FAIL, "EV_DEV_IO_GET_VERSION error!\n");
        }

    /* 2 */

    if (IOERROR == ioctl (evdevFd, EV_DEV_IO_SET_OPERATE_DEV, (char *)&devIdx))
        {
        errorNo |= EVDEV_TEST_ERROR_SETOPERDEV;
        vxTestMsg (V_FAIL, "EV_DEV_IO_SET_OPERATE_DEV failed! devIdx = %d\n", 
                   devIdx);
        }

    bzero ((char *)&devInfo, sizeof (EV_DEV_DEVICE_INFO));
    if (IOERROR == ioctl (evdevFd, EV_DEV_IO_GET_INFO, (char *)&devInfo))
        {
        errorNo |= EVDEV_TEST_ERROR_GETINFO;
        vxTestMsg (V_FAIL, "EV_DEV_IO_GET_INFO failed!\n");
        }

    /* 3 */

    bzero (devName, sizeof (devName));
    if (IOERROR == ioctl (evdevFd, EV_DEV_IO_GET_NAME, (char *)devName))
        {
        errorNo |= EVDEV_TEST_ERROR_GETNAME;
        vxTestMsg (V_FAIL, "EV_DEV_IO_GET_NAME failed!\n");
        }

    /* 4 */

    if (IOERROR == ioctl (evdevFd, EV_DEV_IO_GET_CAP, (char *)&devCap))
        {
        errorNo |= EVDEV_TEST_ERROR_GETCAP;
        vxTestMsg (V_FAIL, "EV_DEV_IO_GET_CAP failed!\n");
        }
    if ((devCap != EVDEV_TEST_KBD) && (devCap!=EVDEV_TEST_PTR) && 
        (devCap != EVDEV_TEST_TS))
        {
        errorNo |= EVDEV_TEST_ERROR_GETCAP;
        vxTestMsg (V_FAIL, "Error device capabilities = 0x%x\n", devCap);
        }

    if (IOERROR == close (evdevFd))
        {
        errorNo |= EVDEV_TEST_ERROR_CLOSE;
        vxTestMsg (V_FAIL, "Close %s  Success!", evdevName);
        }

    return errorNo;
    }

/******************************************************************************
*
* evdevTestMaxDevs - testing the max number devices
*
* \cs
* <testCase>
*     <timeout>      300000    </timeout>
*     <reentrant>    TRUE    </reentrant>
*     <memCheck>     TRUE     </memCheck>
*     <destructive>  FALSE </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS evdevTestMaxDevs(void)
    {
    UINT32        startIdx;
    UINT32        i         = 0;
    UINT32        endIdx    = EVDEV_MAX_NUM;
    INT32         idx       = 0;
    VXTEST_STATUS vxTestRes = VXTEST_PASS;
    STATUS        result    = EVDEV_TEST_OK;

    startIdx = evdevGetDevCount();
    if (EVDEV_TEST_ERROR == startIdx)
        {
        return VXTEST_FAIL;
        }

    /* 1 */

    for (i = startIdx; i < endIdx; i++)
        {
        idx = addSimKbd();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            vxTestMsg (V_FAIL, "Case 1: i = %d  idx = %d", i, idx);
            }
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM-1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestMsg(V_FAIL, "Case 1: rmAllSimDevs fail");
        }

    /* 2 */

    for(i = startIdx; i < endIdx; i++)
        {
        idx = addSimPtr();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            vxTestMsg (V_FAIL, "Case 2: i = %d  idx = %d", i, idx);
            }
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM-1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestMsg (V_FAIL, "Case 2: rmAllSimDevs fail");
        }

    /* 3 */

    for(i = startIdx; i < endIdx; i++)
        {
        idx = addSimTs();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            vxTestMsg (V_FAIL, "Case 3: i = %d  idx = %d", i, idx);
            }
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM-1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestMsg(V_FAIL, "Case 3: rmAllSimDevs fail");
        }

    /* 3 */

    i = startIdx;
    while (i < endIdx)
        {        
        idx = addSimKbd();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            vxTestMsg(V_FAIL, "Case 4: i = %d  idx = %d", i, idx);
            }
        i++;

        if (i < endIdx)
            {
            idx = addSimPtr();
            if(i != idx)
                {
                vxTestRes = VXTEST_FAIL;
                vxTestMsg (V_FAIL, "Case 4: i = %d  idx = %d", i, idx);
                }
            i++;
            }

        if (i < endIdx)
            {
            idx = addSimTs();
            if(i != idx)
                {
                vxTestRes = VXTEST_FAIL;
                vxTestMsg (V_FAIL, "Case 4: i = %d  idx = %d", i, idx);
                }
            i++;
            }
        }

    for (i = 0; i < 4; i++)
        {
        idx = addSimKbd();
        if (EVDEV_TEST_ERROR != idx)
            {
            vxTestRes = VXTEST_FAIL;
            vxTestMsg (V_FAIL, "Case 5: idx = %d", idx);
            }
        idx = addSimPtr();
        if (EVDEV_TEST_ERROR != idx)
            {
            vxTestRes = VXTEST_FAIL;
            vxTestMsg (V_FAIL, "Case 5: idx = %d", idx);
            }
        }

    result = rmAllSimDevs(startIdx, EVDEV_MAX_NUM-1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestMsg(V_FAIL, "Case 1: rmAllSimDevs fail");
        }

    return vxTestRes;
    }

/******************************************************************************
*
* evdevTestQuicklyAddRmDevs - test routine for quickly add and remove devices
*
* \cs
* <testCase>
*     <timeout>      300000    </timeout>
*     <reentrant>    TRUE    </reentrant>
*     <memCheck>     TRUE     </memCheck>
*     <destructive>  FALSE </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS evdevTestQuicklyAddRmDevs(void)
    {
    UINT32        startIdx;
    UINT32        i         = 0;
    UINT32        endIdx    = EVDEV_MAX_NUM;
    INT32         idx       = 0;
    VXTEST_STATUS vxTestRes = VXTEST_PASS;
    STATUS        result    = EVDEV_TEST_OK;

    startIdx = evdevGetDevCount();
    if (EVDEV_TEST_ERROR == startIdx)
        {
        return VXTEST_FAIL;
        }

    /* 1 KeyBoard */

    for (i = startIdx; i < endIdx; i++)
        {
        idx = addSimKbd();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if(idx>=0)
            {
            if(EVDEV_TEST_ERROR == rmSimDev ((UINT32)idx))
                {
                vxTestRes = VXTEST_FAIL;
                }
            }
        idx = addSimKbd();
        if (i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        }
    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    /* 2 Mouse */

    for (i = startIdx; i < endIdx; i++)
        {
        idx = addSimPtr();
        if (i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (idx >= 0)
            {
            if(EVDEV_TEST_ERROR == rmSimDev ((UINT32)idx))
                {
                vxTestRes = VXTEST_FAIL;
                }
            }
        idx = addSimPtr();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        }
    result = rmAllSimDevs(startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    /* 3 TS */

    for (i = startIdx; i < endIdx; i++)
        {
        idx = addSimTs();
        if (i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (idx>=0)
            {
            if (EVDEV_TEST_ERROR == rmSimDev ((UINT32)idx))
                {
                vxTestRes = VXTEST_FAIL;
                }
            }
        idx = addSimTs();
        if (i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        }
    result = rmAllSimDevs(startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    /* 4 Mouse KeyBoard */

    for (i = startIdx; i < endIdx; i++)
        {
        idx = addSimPtr();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if(idx>=0)
            {
            if(EVDEV_TEST_ERROR == rmSimDev ((UINT32)idx))
                {
                vxTestRes = VXTEST_FAIL;
                }
            }
        idx = addSimKbd();
        if(i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        }
    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM-1);
    if(EVDEV_TEST_ERROR == result)
    {
        vxTestRes = VXTEST_FAIL;
    }

    /* 5 KeyBoard Mouse */

    for (i = startIdx; i < endIdx; i++)
        {
        idx = addSimKbd();
        if (i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (idx >= 0)
            {
            if (EVDEV_TEST_ERROR == rmSimDev ((UINT32)idx))
                {
                vxTestRes = VXTEST_FAIL;
                }
            }
        idx = addSimPtr();
        if (i != idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        }
    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    return vxTestRes;
    }

/******************************************************************************
*
* evdevRunStandIO - test routine for evdev stand io
*
* This routine used to testing the evdev stand io
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

 VXTEST_STATUS evdevRunStandIO
    (
    UINT32 devIdx, 
    UINT32 mode
    )
    {
    char          evdevName[EV_DEV_NAME_LEN + 1];
    UINT32        i         = 0;
    VXTEST_STATUS vxTestRes = VXTEST_PASS;
    UINT32        sizeOfMsg = 0;

    bzero (evdevName, sizeof (evdevName));
    if (EVDEV_TEST_COMPATIBLE == mode)
        {
        (void) snprintf ((char *)evdevName, EV_DEV_NAME_LEN, "%s%d", 
                  EV_DEV_NAME_PREFIX, devIdx);
        sizeOfMsg = sizeof (EV_DEV_EVENT);
        }
    else
        {
        (void) snprintf ((char *)evdevName, EV_DEV_NAME_LEN, "%s", EV_DEV_NAME);
        sizeOfMsg = sizeof (EV_DEV_MSG);
        }

    if (EVDEV_TEST_ERROR_OK != evDevTestOpenClose (evdevName))
        {
        vxTestRes = VXTEST_FAIL;
        vxTestMsg (V_FAIL, "evDevTestOpenClose %s Fail!", evdevName);
        }
    else
        {
        vxTestMsg (V_GENERAL, "evDevTestOpenClose %s Pass!", evdevName);
        }

    if (EVDEV_TEST_ERROR_OK != evDevTestReadWrite (evdevName, sizeOfMsg))
        {
        vxTestRes = VXTEST_FAIL;
        vxTestMsg (V_FAIL, "evDevTestReadWrite %s Fail!", evdevName);
        }
    else
        {
        vxTestMsg (V_GENERAL, "evDevTestReadWrite %s Pass!", evdevName);
        }

    if (EVDEV_TEST_ERROR_OK != evdevTestBasic (evdevName, i, EVDEV_TEST_OPTICAL))
        {
        vxTestRes = VXTEST_FAIL;
        vxTestMsg (V_FAIL, "evdevTestBasic %s Fail!", evdevName);
        }
    else
        {
        vxTestMsg (V_GENERAL, "evdevTestBasic %s Pass!", evdevName);
        }

    return vxTestRes;
    }

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE

/******************************************************************************
*
* evdevTestComStandIO - test routine for evdev Compatible mode
*
* This
*
* \cs
* <testCase>
*     <timeout>       300000    </timeout>
*     <reentrant>     TRUE    </reentrant>
*     <memCheck>      TRUE     </memCheck>
*     <destructive>   FALSE </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS evdevTestComStandIO(void)
    {
    UINT32        i         = 0;
    UINT32        startIdx  = 2;
    UINT32        endIdx    = EVDEV_MAX_NUM;
    VXTEST_STATUS vxTestRes = VXTEST_PASS;
    STATUS        result    = EVDEV_TEST_OK;

    startIdx = evdevGetDevCount();
    if (EVDEV_TEST_ERROR == startIdx)
        {
        return VXTEST_FAIL;
        }

    for (i = 0; i < startIdx; i++)
        {
        vxTestRes = evdevRunStandIO (i, EVDEV_TEST_COMPATIBLE);
        }

    /* 1 */

    for (i = startIdx; i < endIdx; i++)
        {
        (void)addSimKbd();
        vxTestRes = evdevRunStandIO (i, EVDEV_TEST_COMPATIBLE);
        }

    for (i = 0; i < startIdx; i++)
        {
        vxTestRes = evdevRunStandIO (i, EVDEV_TEST_COMPATIBLE);
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    /* 2 */

    for (i = startIdx; i < endIdx; i++)
        {
        (void)addSimPtr();
        vxTestRes = evdevRunStandIO (i, EVDEV_TEST_COMPATIBLE);
        }

    for (i = 0; i < startIdx; i++)
        {
        vxTestRes = evdevRunStandIO (i, EVDEV_TEST_COMPATIBLE);
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    return vxTestRes;
    }
#endif

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE

/******************************************************************************
*
* evdevTestOptStandIO - test routine for evdev optiacl mode
*
* \cs
* <testCase>
*     <timeout>       300000       </timeout>
*     <reentrant>     TRUE       </reentrant>
*     <memCheck>      TRUE        </memCheck>
*     <destructive>   FALSE    </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS evdevTestOptStandIO(void)
    {
    UINT32        startIdx;
    UINT32        i         = 0;
    UINT32        endIdx    = EVDEV_MAX_NUM;
    VXTEST_STATUS vxTestRes = VXTEST_PASS;
    STATUS        result    = EVDEV_TEST_OK;

    startIdx = evdevGetDevCount();
    if (EVDEV_TEST_ERROR == startIdx)
        {
        vxTestRes = VXTEST_FAIL;
        startIdx = 2;
        }

    for (i = 0; i < startIdx; i++)
        {
        vxTestRes = evdevRunStandIO(i, EVDEV_TEST_OPTICAL);
        }

    /* 1 */

    for (i = startIdx; i < endIdx; i++)
        {
        (void)addSimKbd();
        vxTestRes = evdevRunStandIO(i, EVDEV_TEST_OPTICAL);
        }

    for (i = 0; i < startIdx; i++)
        {
        vxTestRes = evdevRunStandIO(i, EVDEV_TEST_OPTICAL);
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1);
    if(EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    /* 2 */

    for (i = startIdx; i < endIdx; i++)
        {
        (void)addSimPtr ();
        vxTestRes = evdevRunStandIO(i, EVDEV_TEST_OPTICAL);
        }

    for (i = 0; i < startIdx; i++)
        {
        vxTestRes = evdevRunStandIO (i, EVDEV_TEST_OPTICAL);
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }

    /* 3 */

    for (i = startIdx; i < endIdx; i++)
        {
        (void)addSimTs ();
        vxTestRes = evdevRunStandIO(i, EVDEV_TEST_OPTICAL);
        }

    for (i = 0; i < startIdx; i++)
        {
        vxTestRes = evdevRunStandIO (i, EVDEV_TEST_OPTICAL);
        }

    result = rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1);
    if (EVDEV_TEST_ERROR == result)
        {
        vxTestRes = VXTEST_FAIL;
        }


    return vxTestRes;
    }
#endif

/******************************************************************************
*
* evdevTestDevsMsg - test routine for evdev Compatible mode send and receive 
*                      message
*
* \cs
* <testCase>
*     <timeout>      300000    </timeout>
*     <reentrant>    TRUE    </reentrant>
*     <memCheck>     TRUE     </memCheck>
*     <destructive>  FALSE </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS evdevTestDevsMsg(void)
    {
    VXTEST_STATUS vxTestRes = VXTEST_PASS;
    UINT32        i         = 0;
    UINT32        startIdx  = 2;
    UINT32        endIdx    = EVDEV_MAX_NUM;
    INT32         idx       = 0;

    startIdx = evdevGetDevCount();
    if (EVDEV_TEST_ERROR == startIdx)
        {
        vxTestRes = VXTEST_FAIL;
        startIdx = 2;
        }

    for (i = startIdx; i < endIdx; i += 3)
        {
        idx = addSimPtr();
        if (EVDEV_TEST_ERROR == idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (VXTEST_PASS != evDevTestMessage (EVDEV_TEST_OPTICAL, idx))
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (idx >= endIdx - 1)
            {
            break;
            }

        idx = addSimKbd();
        if (EVDEV_TEST_ERROR == idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (VXTEST_PASS != evDevTestMessage (EVDEV_TEST_OPTICAL, idx))
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (idx >= endIdx - 1)
            {
            break;
            }

        idx = addSimTs();
        if (EVDEV_TEST_ERROR == idx)
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (VXTEST_PASS != evDevTestMessage (EVDEV_TEST_OPTICAL, idx))
            {
            vxTestRes = VXTEST_FAIL;
            }
        if (idx >= endIdx - 1)
            {
            break;
            }
        }

    if (EVDEV_TEST_OK != rmAllSimDevs (startIdx, EVDEV_MAX_NUM - 1))
        {
        vxTestRes = VXTEST_FAIL;
        }

    return vxTestRes;
    }

LOCAL VXTEST_ENTRY vxTestTbl_tmEvDevTest[] = 
    {
    {"evdevTestComStandIO", (FUNCPTR)evdevTestComStandIO, 0, 0, 0, 15000, VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0,"test functionality of evdev device driver."},
    {"evdevTestOptStandIO", (FUNCPTR)evdevTestOptStandIO, 0, 0, 0, 15000, VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0, "test functionality of evdev device driver."},
    {"evdevTestDevsMsg", (FUNCPTR)evdevTestDevsMsg, 0, 0, 0, 100000, VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0, "test functionality of evdev device driver."},
    {"evdevTestQuicklyAddRmDevs", (FUNCPTR)evdevTestQuicklyAddRmDevs,  0, 0, 0, 50000,  VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0, "test functionality of evdev device driver."},
    {NULL, (FUNCPTR)"tmEvDevTest", 0, 0, 0, 600000, 0, 0, 0}
    };
 
/**************************************************************************
*
* tmEvDevTestExec - Exec test module
*
* This routine should be called to initialize the test module.
*
* RETURNS: N/A
*
* NOMANUAL
*/

#ifdef _WRS_KERNEL
 
STATUS tmEvDevTestExec
    (
    char *          testCaseName,
    VXTEST_RESULT * pTestResult
    )
    {
    return vxTestRun((VXTEST_ENTRY**)&vxTestTbl_tmEvDevTest, testCaseName, 
                     pTestResult);
    } 

#else 
    
STATUS tmEvDevTestExec
    (
    char *          testCaseName,
    VXTEST_RESULT * pTestResult,
    int             argc,
    char *          argv[]
    )
    {
    return vxTestRun((VXTEST_ENTRY**)&vxTestTbl_tmEvDevTest, testCaseName, 
                     pTestResult, argc, argv);
    }

/**************************************************************************
* main - User application entry function
*
* This routine is the entry point for user application. A real time process
* is created with first task starting at this entry point. 
*
*/

int main
    (
    int    argc,    /* number of arguments */
    char * argv[]   /* array of arguments */
    )
    {
    return tmEvDevTestExec(NULL, NULL, argc, argv);
    }
#endif
