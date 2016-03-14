/* evdevDrvTiAm335x.h - TI AM335X Touch Screen Controller Register Definitions */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
14sep15,jnl  support querying size of the touchscreen area. (V7GFX-238)
01sep14,y_f  removed pTsCtrl (V7GFX-208)
15aug13,y_f  created
*/

/*
DESCRIPTION
This file contains register definitions for touch screen controller
on the AM335X EVM.
*/

#ifndef __INCevdevDrvTiAm335xh
#define __INCevdevDrvTiAm335xh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define AM335X_ADC_TSC_DRIVER_NAME              "ti,am3359-tscadc"

#define AM335X_ADC_TSC_IRQSTATUS_RAW            (0x24)
#define AM335X_ADC_TSC_IRQSTATUS                (0x28)
#define AM335X_ADC_TSC_IRQENABLE_SET            (0x2C)
#define AM335X_ADC_TSC_IRQENABLE_CLR            (0x30)
#define AM335X_ADC_TSC_CTRL                     (0x40)
#define AM335X_ADC_TSC_ADCSTAT                  (0x44)
#define AM335X_ADC_TSC_CLKDIV                   (0x4C)
#define AM335X_ADC_TSC_STEPENABLE               (0x54)
#define AM335X_ADC_TSC_IDLECONFIG               (0x58)
#define AM335X_ADC_TSC_CHARGE_STEPCONFIG        (0x5C)
#define AM335X_ADC_TSC_CHARGE_DELAY             (0x60)
#define AM335X_ADC_TSC_STEPCONFIG(n)            (0x64 + ((n - 1) * 8))
#define AM335X_ADC_TSC_STEPDELAY(n)             (0x68 + ((n - 1) * 8))
#define AM335X_ADC_TSC_FIFO0COUNT               (0xE4)
#define AM335X_ADC_TSC_FIFO0THRESHOLD           (0xE8)
#define AM335X_ADC_TSC_FIFO1COUNT               (0xF0)
#define AM335X_ADC_TSC_FIFO1THRESHOLD           (0xF4)
#define AM335X_ADC_TSC_FIFO0DATA                (0x100)
#define AM335X_ADC_TSC_FIFO1DATA                (0x200)

/* AM335X_ADC_TSC_CTRL register */

#define AM335X_ADC_TSC_CTRL_ENABLE              (0x1 << 0)
#define AM335X_ADC_TSC_CTRL_STEP_ID             (0x1 << 1)
#define AM335X_ADC_TSC_CTRL_WRITEPROTECT        (0x1 << 2)
#define AM335X_ADC_TSC_CTRL_POWER_DOWN          (0x1 << 4)
#define AM335X_ADC_TSC_CTRL_TOUCH_SCREEN_EN     (0x1 << 7)
#define AM335X_ADC_TSC_CTRL_4WIRE               (0x1 << 5)
#define AM335X_ADC_TSC_CTRL_5WIRE               (0x2 << 5)
#define AM335X_ADC_TSC_CTRL_8WIRE               (0x3 << 5)

/* AM335X_ADC_TSC_ADCSTAT register */

#define AM335X_ADC_TSC_ADCSTAT_STEP_IDLE        (0x10)

/* AM335X_ADC_TSC_IDLECONFIG register */

#define AM335X_ADC_TSC_IDLECONFIG_YNNSW         (0x1 << 8)
#define AM335X_ADC_TSC_IDLECONFIG_YPNSW         (0x1 << 10)
#define AM335X_ADC_TSC_IDLECONFIG_SEL_INM       (0x8 << 15)
#define AM335X_ADC_TSC_IDLECONFIG_SEL_INP       (0x0 << 19)

/* AM335X_ADC_TSC_STEPCONFIG register */

#define AM335X_ADC_TSC_STEPCONFIG_MODE          (0x2 << 0)  /* HW synchronized, one-shot */
#define AM335X_ADC_TSC_STEPCONFIG_AVERAGING     (0x4 << 2)  /* 16 samples average */
#define AM335X_ADC_TSC_STEPCONFIG_XPP           (0x1 << 5)
#define AM335X_ADC_TSC_STEPCONFIG_XNN           (0x1 << 6)
#define AM335X_ADC_TSC_STEPCONFIG_YPP           (0x1 << 7)
#define AM335X_ADC_TSC_STEPCONFIG_YNN           (0x1 << 8)
#define AM335X_ADC_TSC_STEPCONFIG_XNP           (0x1 << 9)
#define AM335X_ADC_TSC_STEPCONFIG_YPN           (0x1 << 10)
#define AM335X_ADC_TSC_STEPCONFIG_RFP           (0x1 << 12)
#define AM335X_ADC_TSC_STEPCONFIG_INM           (0x8 << 15)
#define AM335X_ADC_TSC_STEPCONFIG_CHG_INM       (0x1 << 15)
#define AM335X_ADC_TSC_STEPCONFIG_INP           (0x2 << 19)
#define AM335X_ADC_TSC_STEPCONFIG_CHG_INP       (0x1 << 19)
#define AM335X_ADC_TSC_STEPCONFIG_Z_INP         (0x3 << 19)
#define AM335X_ADC_TSC_STEPCONFIG_CHG_RFM       (0x1 << 23)
#define AM335X_ADC_TSC_STEPCONFIG_FIFO          (0x1 << 26) /* sampled data will be stored in FIFO1 */

/* AM335X_ADC_TSC_STEPDELAY register */

#define AM335X_ADC_TSC_STEPDELAY_OPENDLY        (0x98 << 0)
#define AM335X_ADC_TSC_STEPDELAY_SAMPLEDLY      (0x98 << 24)

/* AM335X_ADC_TSC_CHARGE_STEPCONFIG register */

#define AM335X_ADC_TSC_CHARGE_STEPCONFIG_INM    (0x1 << 15)
#define AM335X_ADC_TSC_CHARGE_STEPCONFIG_INP    (0x1 << 19)
#define AM335X_ADC_TSC_CHARGE_STEPCONFIG_RFM    (0x1 << 23)

/* AM335X_ADC_TSC_CHARGE_DELAY register */

#define AM335X_ADC_TSC_CHARGE_DELAY_VAL         (0x200)

/* AM335X_ADC_TSC_STEPENABLE register */

#define AM335X_ADC_TSC_STEPENABLE_STEPEN        (0x1FF)

/* AM335X_ADC_TSC_IRQENABLE_SET register */

#define AM335X_ADC_TSC_IRQ_HW_PEN_ASYNC         (0x1 << 0)
#define AM335X_ADC_TSC_IRQ_FIFO1THRES           (0x1 << 5)
#define AM335X_ADC_TSC_IRQ_PEN_UP               (0x1 << 9)
#define AM335X_ADC_TSC_IRQ_HW_PEN_SYNC          (0x1 << 10)

/* AM335X_ADC_TSC_IRQSTATUS_RAW register */

#define AM335X_ADC_TSC_IRQSTATUS_RAW_PEN_UP     (0x1 << 9)

#define AM335X_READ32(pCtrl,offset)                 \
    vxbRead32 (pCtrl->vxbHandle,                    \
              (UINT32 *)((ULONG)(pCtrl)->baseAddr + offset))

#define AM335X_WRITE32(pCtrl,offset,value)              \
    vxbWrite32 (pCtrl->vxbHandle,                   \
               (UINT32 *)((ULONG)(pCtrl)->baseAddr + offset), value)

#define ADC_TSC_SRC_CLK                         (24000000)
#define ADC_TSC_CLK                             (3000000)
#define AM335X_RELEASE_TIMEOUT                  (200) /* 200 ms */

#define X_MAX_NUM                               (3)
#define Y_MAX_NUM                               (3)
#define PRESSURE_MAX_NUM                        (1)
#define SAMPLE_MAX_NUM                          (X_MAX_NUM + Y_MAX_NUM +    \
                                                 PRESSURE_MAX_NUM * 2)

#define SAMPLE_MAX_DIFF                         (15)
#define SAMPLE_MAX_MOVE                         (100)

#define AM335X_ADC_TSC_DEFAULT_DIS_WIDTH        800
#define AM335X_ADC_TSC_DEFAULT_DIS_HEIGHT       480

#define AM335X_ADC_TSC_X_MAX                    4095
#define AM335X_ADC_TSC_Y_MAX                    4095

/* typedefs */

/* structure to store the touch screen module information */

typedef struct am335xTsData
    {
    VXB_DEV_ID          pInst;
    void *              baseAddr;   /* base address of touch screen module */
    void *              vxbHandle;  /* vxBus handle */
    VXB_RESOURCE *      intRes;     /* interrupt resources */
    TS_DEVICE_INFO *    pDevInfo;
    TS_CALIB_DATA *     pCalData;
    TS_POINT            point;
    UINT32              disWidth;
    UINT32              disHeight;
    BOOL                isFirstOpen;
    UINT8               errorCount;
    int                 axisXMax;
    int                 axisYMax;
    }AM335X_TS_DATA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvTiAm335xh */
