/* audioDrvTiAic3106.h - TI TLV320AIC3106 audio codec driver header file */

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
21mar14,y_f  written (US34862)
*/

#ifndef __INCaudioDrvTiAic3106h
#define __INCaudioDrvTiAic3106h

#if __cplusplus
extern "C" {
#endif

/* defines */

#define AIC3106_AUD_DRIVER_NAME             "ti,aic3106"

/* audio codec registers: page0 */

#define AIC3106_PAGE_SELECT                 (0)
#define AIC3106_SOFTWARE_RESET              (1)
#define AIC3106_SAMPLE_RATE                 (2)
#define AIC3106_PLL_PRG_A                   (3)
#define AIC3106_PLL_PRG_B                   (4)
#define AIC3106_PLL_PRG_C                   (5)
#define AIC3106_PLL_PRG_D                   (6)
#define AIC3106_DATA_PATH                   (7)
#define AIC3106_IF_CTRL_A                   (8)
#define AIC3106_IF_CTRL_B                   (9)
#define AIC3106_IF_CTRL_C                   (10)
#define AIC3106_OVERFLOW_FLG                (11)
#define AIC3106_DIGITAL_FILTER              (12)
#define AIC3106_LADC_PGA_CTRL               (15)
#define AIC3106_RADC_PGA_CTRL               (16)
#define AIC3106_MIC3LL_CTRL                 (17)
#define AIC3106_MIC3LR_CTRL                 (18)
#define AIC3106_LINE1_LADC_CTRL             (19)
#define AIC3106_LINE1_RADC_CTRL             (22)
#define AIC3106_MICBASIC_CTRL               (25)
#define AIC3106_ADC_FLAG                    (36)
#define AIC3106_DAC_PWR_AND_DRIVER_CTRL     (37)
#define AIC3106_HIG_PWR_AND_DRIVER_CTRL     (38)
#define AIC3106_DAC_OUT_SWITCH_CTRL         (41)
#define AIC3106_POP_REDUCTION_CTRL          (42)
#define AIC3106_LDAC_VOL_CTRL               (43)
#define AIC3106_RDAC_VOL_CTRL               (44)
#define AIC3106_LPGA_HPLOUT_VOL_CTRL        (46)
#define AIC3106_LINE2L_HPLOUT_VOL_CTRL      (45)
#define AIC3106_DACL1_HPLOUT_VOL_CTRL       (47)
#define AIC3106_HPLOUT_LVL_CTRL             (51)
#define AIC3106_LINE2L_HPLCOM_VOL_CTRL      (52)
#define AIC3106_LPGA_HPLCOM_VOL_CTRL        (53)
#define AIC3106_DACL1_HPLCOM_VOL_CTRL       (54)
#define AIC3106_HPLCOM_LVL_CTRL             (58)
#define AIC3106_LINE2R_HPROUT_VOL_CTRL      (62)
#define AIC3106_RPGA_HPROUT_VOL_CTRL        (63)
#define AIC3106_DACR1_HPLOUT_VOL_CTRL       (64)
#define AIC3106_HPROUT_LVL_CTRL             (65)
#define AIC3106_LINE2R_HPRCOM_VOL_CTRL      (69)
#define AIC3106_RPGA_HPRCOM_VOL_CTRL        (70)
#define AIC3106_DACR1_HPRCOM_VOL_CTRL       (71)
#define AIC3106_HPRCOM_LVL_CTRL             (72)
#define AIC3106_LINE2L_MONOLOPM_VOL_CTRL    (73)
#define AIC3106_PGAL_MONOLOPM_VOL_CTRL      (74)
#define AIC3106_DACL1_MONOLOPM_VOL_CTRL     (75)
#define AIC3106_LINE2R_MONOLOPM_VOL_CTRL    (76)
#define AIC3106_PGAR_MONOLOPM_VOL_CTRL      (77)
#define AIC3106_DACR1_MONOLOPM_VOL_CTRL     (78)
#define AIC3106_MONO_LOPM_LVL_CTRL          (79)
#define AIC3106_LINE2L_LLOPM_VOL_CTRL       (80)
#define AIC3106_LPGA_LLOPM_VOL_CTRL         (81)
#define AIC3106_DACL1_LLOPM_VOL_CTRL        (82)
#define AIC3106_LINE2R_LLOPM_VOL_CTRL       (83)
#define AIC3106_RPGA_LLOPM_VOL_CTRL         (84)
#define AIC3106_DACR1_LLOPM_VOL_CTRL        (85)
#define AIC3106_LLOPM_LVL_CTRL              (86)
#define AIC3106_LINE2L_RLOPM_VOL_CTRL       (87)
#define AIC3106_LPGA_RLOPM_VOL_CTRL         (88)
#define AIC3106_DACL1_RLOPM_VOL_CTRL        (89)
#define AIC3106_LINE2R_RLOPM_VOL_CTRL       (90)
#define AIC3106_RPGA_RLOPM_VOL_CTRL         (91)
#define AIC3106_DACR1_RLOPM_VOL_CTRL        (92)
#define AIC3106_RLOPM_LVL_CTRL              (93)
#define AIC3106_MP_STATUS                   (94)
#define AIC3106_GPIO1_CTRL                  (98)
#define AIC3106_GPIO2_CTRL                  (99)
#define AIC3106_GPIO_CTRL_A                 (100)
#define AIC3106_GPIO_CTRL_B                 (101)

/* AIC3106_IF_CTRL_A macro */

#define BIT_CLOCK                           (1 << 7)
#define WORD_CLOCK                          (1 << 6)

/* AIC3106_IF_CTRL_B macro */

#define TRANS_MODE(n)                       (n << 6)
#define DATA_WORD_LEN(n)                    (n << 4)
#define I2S_MODE                            (0x0)
#define DSP_MODE                            (0x1)
#define RJUST_MODE                          (0x2)
#define LJUST_MODE                          (0x3)
#define DATA_16                             (0x0)
#define DATA_20                             (0x1)
#define DATA_24                             (0x2)
#define DATA_32                             (0x3)

/* AIC3106_PLL_PRG_A macro */

#define PLL_ENABLE                          (1 << 7)
#define PLL_Q(n)                            (n << 3)
#define PLL_P(n)                            (n)

/* AIC3106_DATA_PATH macro */

#define FS_HLZ(n)                           (n << 7)
#define STEREO_DATA                         (0x1)
#define MONO_DATA                           (0x3)
#define LDAC_DPATH(n)                       (n << 3)
#define RDAC_DPATH(n)                       (n << 1)
#define LRDAC_DPATH_MSK                     (0x1E)
#define FS_44_1                             (1 << 7)
#define FS_48                               (0 << 7)
#define ADC_DUALR                           (1 << 6)
#define DAC_DUALR                           (1 << 5)

/* AIC3106_SAMPLE_RATE macro */

#define ADC_SAMPLE(n)                       (n << 4)
#define DAC_SAMPLE(n)                       (n)
#define FS_DIV_1                            (0x0)
#define FS_DIV_1_5                          (0x1)
#define FS_DIV_2                            (0x2)
#define FS_DIV_2_5                          (0x3)
#define FS_DIV_3                            (0x4)
#define FS_DIV_3_5                          (0x5)
#define FS_DIV_4                            (0x6)
#define FS_DIV_4_5                          (0x7)
#define FS_DIV_5                            (0x8)
#define FS_DIV_5_5                          (0x9)
#define FS_DIV_6                            (0xa)

/* AIC3106_OVERFLOW_FLG macro */

#define PLL_R(n)                            (n)

/* AIC3106_PLL_PRG_B macro */

#define PLL_J(n)                            (n << 2)

/* AIC3106_PLL_PRG_C macro */

#define PLL_D_LW(n)                         (n)

/* AIC3106_PLL_PRG_D macro */

#define PLL_D_HI(n)                         (n << 2)

/* AIC3106_DAC_PWR_AND_DRIVER_CTRL macro */

#define LDAC_PW_ON                          (1 << 7)
#define RDAC_PW_ON                          (1 << 6)
#define SINGLEEND(n)                        (n << 4)

/* AIC3106_DACL1_HPLOUT_VOL_CTRL and AIC3106_DACR1_HPLOUT_VOL_CTRL macro */

#define HPOUT_DEF_VOL                       (0x20)
#define DAC_L1_TO_HPOUT                     (1 << 7)

/*
 * AIC3106_HPLOUT_LVL_CTRL, AIC3106_HPROUT_LVL_CTRL, AIC3106_HPLCOM_LVL_CTRL
 * and AIC3106_HPRCOM_LVL_CTRL macro
 */

#define HP_MUTE_DISABLE                     (1 << 3)
#define HP_PWDN_DRV                         (1 << 2)
#define HP_FULL_PW                          (1)

/* AIC3106_LLOPM_LVL_CTRL and AIC3106_LLOPM_LVL_CTRL macro */

#define LOPM_OUTPUT_LEVEL                   (0x9 << 4)
#define LOPM_MUTE_DISABLE                   (1 << 3)
#define LOPM_FULL_PW                        (1)

/* AIC3106_LDAC_VOL_CTRL and AIC3106_RDAC_VOL_CTRL macro */

#define DAC_MUTE_ENABLE                     (1 << 7)

/* AIC3106_PAGE_SELECT macro */

#define PAGE_0                              (0x0)
#define PAGE_1                              (0x1)

/* AIC3106_SOFTWARE_RESET macro */

#define SOFT_RESET                          (0x80)

/* AIC3106_LADC_PGA_CTRL and AIC3106_RADC_PGA_CTRL macro */

#define PGA_MUTE_ENABLE                     (1 << 7)

/* AIC3106_MICBASIC_CTRL register */

#define MIC_OUT_20V                         (0x1 << 6)
#define MIC_OUT_25V                         (0x2 << 6)
#define MIC_OUT_OFF                         (0x0 << 6)
#define MIC_OUT_AVDD                        (0x3 << 6)

/* AIC3106_LINE1_LADC_CTRL register */

#define ADC_POWER_ON                        (0x1 << 2) 
#define LINE1_INPUT_LVL(n)                  (n << 3)

/* AIC3106_DIGITAL_FILTER register */

#define LEFT_DAC_EFFECTS                    (1 << 3)
#define LEFT_DAC_DEEMPHASIS                 (1 << 2)
#define RIGHT_DAC_EFFECTS                   (1 << 1)
#define RIGHT_DAC_DEEMPHASIS                (1 << 0)

/* AIC3106_POP_REDUCTION_CTRL macro */

#define POWER_ON_1MS                        (0x3 << 4)
#define POWER_ON_50MS                       (0x5 << 4)
#define POWER_ON_100MS                      (0x6 << 4)
#define POWER_ON_200MS                      (0x7 << 4)
#define POWER_ON_400MS                      (0x8 << 4)
#define POWER_ON_800MS                      (0x9 << 4)

#define AIC3106_BASE_CLK_RATE               (12000000)
#define AIC3106_DAC_MIN_VOL                 0x7F    /* -63.5 dB */
#define AIC3106_ADC_MAX_VOL                 0x77    /* +59.5 dB */

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCaudioDrvTiAic3106h */
