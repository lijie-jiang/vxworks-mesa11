/* audioDrvWm8962.h - Wolfson Microelectronics 8962 audio driver header file */

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
17feb14,y_f  written
*/

#ifndef __INCaudioDrvWm8962h
#define __INCaudioDrvWm8962h

#if __cplusplus
extern "C" {
#endif

/* defines */

#define WM8962_AUD_DRIVER_NAME                  "wlf,wm8962"

#define WM8962_CHIP_ID                          0x6243

#define WM8962_LEFT_INPUT_VOLUME                0x00
#define WM8962_RIGHT_INPUT_VOLUME               0x01
#define WM8962_HPOUTL_VOLUME                    0x02
#define WM8962_HPOUTR_VOLUME                    0x03
#define WM8962_CLOCKING1                        0x04
#define WM8962_ADC_DAC_CONTROL_1                0x05
#define WM8962_AUDIO_INTERFACE_0                0x07
#define WM8962_CLOCKING2                        0x08
#define WM8962_AUDIO_INTERFACE_2                0x0E
#define WM8962_SOFTWARE_RESET                   0x0F
#define WM8962_LEFT_ADC_VOLUME                  0x15
#define WM8962_RIGHT_ADC_VOLUME                 0x16
#define WM8962_PWR_MGMT_1                       0x19
#define WM8962_PWR_MGMT_2                       0x1A
#define WM8962_ADDITIONAL_CONTROL_3             0x1B
#define WM8962_ANTI_POP                         0x1C
#define WM8962_INPUT_MIXER_CONTROL_1            0x1F
#define WM8962_LEFT_INPUT_MIXER_VOLUME          0x20
#define WM8962_RIGHT_INPUT_MIXER_VOLUME         0x21
#define WM8962_INPUT_MIXER_CONTROL_2            0x22
#define WM8962_LEFT_INPUT_PGA_CONTROL           0x25
#define WM8962_RIGHT_INPUT_PGA_CONTROL          0x26
#define WM8962_DC_SERVO_0                       0x3C
#define WM8962_DC_SERVO_1                       0x3D
#define WM8962_ANALOGUE_HP_0                    0x45
#define WM8962_CHARGE_PUMP_1                    0x48
#define WM8962_PLL_SOFTWARE_RESET               0x7F
#define WM8962_PLL2                             0x81
#define WM8962_THREED1                          0x10C

/* WM8962_RIGHT_INPUT_VOLUME */

#define WM8962_RIGHT_INPUT_VOLUME_IN_VU         8

/* WM8962_RIGHT_ADC_VOLUME */

#define WM8962_RIGHT_ADC_VOLUME_ADC_VU          8

/* WM8962_CLOCKING1 */

#define WM8962_CLOCKING1_DSPCLK_DIV             9

/* WM8962_AUDIO_INTERFACE_0 */

#define WM8962_AUDIO_INTERFACE_0_MSTR           6
#define WM8962_AUDIO_INTERFACE_0_WL             2

/* WM8962_HPOUTR_VOLUME */

#define WM8962_HPOUTR_VOLUME_HPOUT_VU           8

/* WM8962_ADC_DAC_CONTROL_1 */

#define WM8962_ADC_DAC_CONTROL_1_DAC_MUTE       3

/* WM8962_CLOCKING2 */

#define WM8962_CLOCKING2_SYSCLK_ENA             5

/* WM8962_PWR_MGMT_1 */

#define WM8962_PWR_MGMT_1_VMID_SEL              7
#define WM8962_PWR_MGMT_1_BIAS_ENA              6
#define WM8962_PWR_MGMT_1_INL_ENA               5
#define WM8962_PWR_MGMT_1_INR_ENA               4
#define WM8962_PWR_MGMT_1_ADCL_ENA              3
#define WM8962_PWR_MGMT_1_ADCR_ENA              2
#define WM8962_PWR_MGMT_1_MICBIAS_ENA           1

/* WM8962_ADDITIONAL_CONTROL_3 */

#define WM8962_ADDITIONAL_CONTROL_3_MODE        4

/* WM8962_DC_SERVO_0 */

#define WM8962_DC_SERVO_0_INL_DCS_ENA           7
#define WM8962_DC_SERVO_0_INL_DCS_STARTUP       6
#define WM8962_DC_SERVO_0_INR_DCS_ENA           3
#define WM8962_DC_SERVO_0_INR_DCS_STARTUP       2

/* WM8962_THREED1 */

#define WM8962_THREED1_ADC_MONOMIX              6

#define WM8962_CLOCKING2_RESET_VALUE            (0x09C4)

#define WM8962_AIF_RATE_MAX                     (2047)
#define WM8962_BITCLK_RATE_MAX                  (12288000)
#define WM8962_HP_MIN_VOL                       0x35    /* -68 dB */
#define WM8962_HP_MAX_VOL                       0x7F    /* +6 dB */
#define WM8962_ADC_MAX_VOL                      0xFF    /* +23.625 dB */
#define WM8962_PGA_MAX_VOL                      0x3F    /* +24.00 dB */

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCaudioDrvWm8962h */
