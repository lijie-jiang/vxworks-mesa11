/* audioDrvFslSgtl5000.h - Freescale SGTL5000 audio driver header file */

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
05jun14,y_f  written (US41080)
*/

#ifndef __INCaudioDrvFslSgtl5000h
#define __INCaudioDrvFslSgtl5000h

#if __cplusplus
extern "C" {
#endif

/* defines */

#define SGTL5000_AUD_DRIVER_NAME            "fsl,sgtl5000"

#define CHIP_DIG_POWER                      (0x0002)
#define CHIP_CLK_CTRL                       (0x0004)
#define CHIP_I2S_CTRL                       (0x0006)
#define CHIP_SSS_CTRL                       (0x000A)
#define CHIP_ADCDAC_CTRL                    (0x000E)
#define CHIP_DAC_VOL                        (0x0010)
#define CHIP_ANA_ADC_CTRL                   (0x0020)
#define CHIP_ANA_HP_CTRL                    (0x0022)
#define CHIP_ANA_CTRL                       (0x0024)
#define CHIP_REF_CTRL                       (0x0028)
#define CHIP_MIC_CTRL                       (0x002A)
#define CHIP_LINE_OUT_CTRL                  (0x002C)
#define CHIP_ANA_POWER                      (0x0030)
#define CHIP_PLL_CTRL                       (0x0032)
#define CHIP_CLK_TOP_CTRL                   (0x0034)

/* CHIP_DIG_POWER register */

#define CHIP_DIG_POWER_ADC_POWERUP          6
#define CHIP_DIG_POWER_DAC_POWERUP          5
#define CHIP_DIG_POWER_DAP_POWERUP          4
#define CHIP_DIG_POWER_I2S_OUT_POWERUP      1
#define CHIP_DIG_POWER_I2S_IN_POWERUP       0

/* CHIP_CLK_CTRL register */

#define CHIP_CLK_CTRL_RATE_MODE             4
#define CHIP_CLK_CTRL_SYS_FS                2
#define CHIP_CLK_CTRL_MCLK_FREQ             0

/* CHIP_I2S_CTRL register */

#define CHIP_I2S_CTRL_SCLKFREQ              8
#define CHIP_I2S_CTRL_MS                    7
#define CHIP_I2S_CTRL_SCLK_INV              6
#define CHIP_I2S_CTRL_DLEN                  4
#define CHIP_I2S_CTRL_I2S_MODE              2
#define CHIP_I2S_CTRL_LRALIGN               1
#define CHIP_I2S_CTRL_LRPOL                 0

/* CHIP_SSS_CTRL register */

#define CHIP_SSS_CTRL_DAP_MIX_LRSWAP        14
#define CHIP_SSS_CTRL_DAP_LRSWAP            13
#define CHIP_SSS_CTRL_DAC_LRSWAP            12
#define CHIP_SSS_CTRL_I2S_LRSWAP            10
#define CHIP_SSS_CTRL_DAP_MIX_SELECT        8
#define CHIP_SSS_CTRL_DAP_SELECT            6
#define CHIP_SSS_CTRL_DAC_SELECT            4
#define CHIP_SSS_CTRL_I2S_SELECT            0

/* CHIP_ADCDAC_CTRL register */

#define CHIP_ADCDAC_CTRL_VOL_RAMP_EN        9
#define CHIP_ADCDAC_CTRL_VOL_EXPO_RAMP      8
#define CHIP_ADCDAC_CTRL_DAC_MUTE_RIGHT     3
#define CHIP_ADCDAC_CTRL_DAC_MUTE_LEFT      2
#define CHIP_ADCDAC_CTRL_ADC_HPF_FREEZE     1
#define CHIP_ADCDAC_CTRL_ADC_HPF_BYPASS     0

/* CHIP_DAC_VOL register */

#define CHIP_DAC_VOL_RIGHT                  8
#define CHIP_DAC_VOL_LEFT                   0

/* CHIP_ANA_ADC_CTRL register */

#define CHIP_ANA_ADC_CTRL_VOL_M6DB          8
#define CHIP_ANA_ADC_CTRL_VOL_RIGHT         4
#define CHIP_ANA_ADC_CTRL_VOL_LEFT          0

/* CHIP_ANA_HP_CTRL register */

#define CHIP_ANA_HP_CTRL_VOL_RIGHT          8
#define CHIP_ANA_HP_CTRL_VOL_LEFT           0

/* CHIP_ANA_CTRL register */

#define CHIP_ANA_CTRL_MUTE_LO               8
#define CHIP_ANA_CTRL_SELECT_HP             6
#define CHIP_ANA_CTRL_EN_ZCD_HP             5
#define CHIP_ANA_CTRL_MUTE_HP               4
#define CHIP_ANA_CTRL_SELECT_ADC            2
#define CHIP_ANA_CTRL_EN_ZCD_ADC            1
#define CHIP_ANA_CTRL_MUTE_ADC              0

/* CHIP_REF_CTRL register */

#define CHIP_REF_CTRL_REG_VALUE             0x012F  /* VDDA/2 (2.5V/2 = 1.25V)*/

/* CHIP_LINE_OUT_CTRL register */

#define CHIP_LINE_OUT_CTRL_REG_VALUE        0x0322  /* VDDIO/2 (3.3V/2 = 1.65V) */

/* CHIP_MIC_CTRL register */

#define CHIP_MIC_CTRL_BIAS_RESISTOR         8
#define CHIP_MIC_CTRL_BIAS_VOLT             4
#define CHIP_MIC_CTRL_GAIN                  0

/* CHIP_LINE_OUT_VOL register */

#define CHIP_LINE_OUT_VOL_RIGHT             8
#define CHIP_LINE_OUT_VOL_LEFT              0

/* CHIP_ANA_POWER register */

#define CHIP_ANA_POWER_DAC_MONO             14
#define CHIP_ANA_POWER_LINREG_SIMPLE        13
#define CHIP_ANA_POWER_STARTUP              12
#define CHIP_ANA_POWER_VDDC_CHRGPMP         11
#define CHIP_ANA_POWER_PLL                  10
#define CHIP_ANA_POWER_LINREG_D             9
#define CHIP_ANA_POWER_VCOAMP               8
#define CHIP_ANA_POWER_VAG                  7
#define CHIP_ANA_POWER_ADC_MONO             6
#define CHIP_ANA_POWER_REFTOP               5
#define CHIP_ANA_POWER_HEADPHONE            4
#define CHIP_ANA_POWER_DAC                  3
#define CHIP_ANA_POWER_CAPLESS_HEADPHONE    2
#define CHIP_ANA_POWER_ADC                  1
#define CHIP_ANA_POWER_LINEOUT              0

/* CHIP_PLL_CTRL register */

#define CHIP_PLL_CTRL_INT                   11
#define CHIP_PLL_CTRL_FRAC                  0

/* CHIP_CLK_TOP_CTRL register */

#define CHIP_CLK_TOP_CTRL_INPUT_FREQ_DIV2   3

#define MIC_BIAS_IMPEDANCE_OFF              0x0
#define MIC_BIAS_IMPEDANCE_2KOHM            0x1
#define MIC_BIAS_IMPEDANCE_4KOHM            0x2
#define MIC_BIAS_IMPEDANCE_8KOHM            0x3

#define SYS_MCLK_MIN                        8000000
#define SYS_MCLK_MEDIAN                     17000000
#define SYS_MCLK_MAX                        27000000

#define PLL_FREQ_44K                        180633600
#define PLL_FREQ_NORMAL                     196608000

#define CHIP_DAC_MAX_VOL                    0x3C    /* 0 dB */
#define CHIP_DAC_MIN_VOL                    0xF0    /* -90 dB */
#define CHIP_ANA_HP_MIN_VOL                 0x7F    /* -51.5dB */
#define CHIP_ANA_ADC_MAX_VOL                0x0F    /* +22.5dB */

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCaudioDrvFslSgtl5000h */
