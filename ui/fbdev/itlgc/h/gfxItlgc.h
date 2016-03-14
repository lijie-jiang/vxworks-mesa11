/* gfxItlgc.h - Intel(R) Graphics Controller driver */

/*
 * Copyright (c) 2010-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/* 
modification history 
--------------------
24jan14,mgc  Modified for VxWorks 7 release
07oct11,m_c  Added register operations and updated mode tables
26may11,m_c  Added primary ring buffer 0 registers and some MI instructions
13jan11,m_c  Added 800x600
11dec10,m_c  Written
*/

#ifndef __INC_gfxItlgc_h
#define __INC_gfxItlgc_h

/* Supported video modes */
#define _640X480X32_60              ((uintptr_t)mode00)
#define _800X600X32_60              ((uintptr_t)mode01)

/* Bits */
#define BIT0                        0x00000001
#define BIT1                        0x00000002
#define BIT2                        0x00000004
#define BIT3                        0x00000008
#define BIT4                        0x00000010
#define BIT5                        0x00000020
#define BIT6                        0x00000040
#define BIT7                        0x00000080
#define BIT8                        0x00000100
#define BIT9                        0x00000200
#define BIT10                       0x00000400
#define BIT11                       0x00000800
#define BIT12                       0x00001000
#define BIT13                       0x00002000
#define BIT14                       0x00004000
#define BIT15                       0x00008000
#define BIT16                       0x00010000
#define BIT17                       0x00020000
#define BIT18                       0x00040000
#define BIT19                       0x00080000
#define BIT20                       0x00100000
#define BIT21                       0x00200000
#define BIT22                       0x00400000
#define BIT23                       0x00800000
#define BIT24                       0x01000000
#define BIT25                       0x02000000
#define BIT26                       0x04000000
#define BIT27                       0x08000000
#define BIT28                       0x10000000
#define BIT29                       0x20000000
#define BIT30                       0x40000000
#define BIT31                       0x80000000

/* Display pipe status mask */
#define PIPESTAT_MASK               0x80003b33

/* Memory Interface instruction */
#define MI_NOOP                     (0x00 << 23)
#define MI_FLUSH                    (0x04 << 23)
#define MI_LOAD_SCAN_LINES_EXCL     (0x13 << 23)
#define MI_LOAD_SCAN_LINES_INCL     (0x12 << 23)
#define MI_LOAD_SCAN_LINES_PIPEA    (0 << 20)
#define MI_LOAD_SCAN_LINES_PIPEB    (1 << 20)

/* North display registers */
/* [01000 .. 04fff] Gunit Memory Interface (MI) */
#define FENCE__0                    0x02000 /* Fence 0 */
#define FENCE__1                    0x02004 /* Fence 1 */
#define FENCE__2                    0x02008 /* Fence 2 */
#define FENCE__3                    0x0200c /* Fence 3 */
#define FENCE__4                    0x02010 /* Fence 4 */
#define FENCE__5                    0x02014 /* Fence 5 */
#define FENCE__6                    0x02018 /* Fence 6 */
#define FENCE__7                    0x0201c /* Fence 7 */
#define PGTBL_CTL                   0x02020 /* Page Table Control */
#define   PGTBL_CTL_PTE             0x00000001
#define PGTBL_ER                    0x02024 /* Page Table Error */
#define PRB0_TAIL                   0x02030 /* Primary Ring Buffer 0 Tail */
#define PRB0_HEAD                   0x02034 /* Primary Ring Buffer 0 Head */
#define PRB0_START                  0x02038 /* Primary Ring Buffer 0 Start */
#define PRB0_CTL                    0x0203c /* Primary Ring Buffer 0 Control */
#define HWS_PGA                     0x02080 /* Hardware Status Page Address */
#define HWSTAM                      0x02098 /* Hardware Status Mask */
#define SCPD0                       0x0209c /* Scratch Pad 0 */
#define IER                         0x020a0 /* Interrupt Enable Register */
#define IIR                         0x020a4 /* Interrupt Identity Register */
#define IMR                         0x020a8 /* Interrupt Mask Register */
#define ISR                         0x020ac /* Interrupt Status Register */
#define EIR                         0x020b0 /* Error Identity Register */
#define EMR                         0x020b4 /* Error Mask Register */
#define ESR                         0x020b8 /* Error Status Register */
#define INSTPM                      0x020c0 /* Instruction Parser Mode */
#define FW_BLC1                     0x020d8 /* Display FIFO Watermark and Burst Length Control 1 */
#define FW_BLC2                     0x020dc /* Display FIFO Watermark and Burst Length Control 2 */
#define FW_BLC_SELF                 0x020e0 /* Display FIFO Watermark */
#define MI_ARB_STATE                0x020e4 /* Memory Interface Arbitration State */
#define FW_BLC3                     0x020ec /* Display FIFO Watermark and Burst Length Control 3 */
#define G_DEBUG                     0x020fc /* Gunit Debug */
#define FENCE__8                    0x03000 /* Fence 8 */
#define FENCE__9                    0x03004 /* Fence 9 */
#define FENCE__10                   0x03008 /* Fence 10 */
#define FENCE__11                   0x0300c /* Fence 11 */
#define FENCE__12                   0x03010 /* Fence 12 */
#define FENCE__13                   0x03014 /* Fence 13 */
#define FENCE__14                   0x03018 /* Fence 14 */
#define FENCE__15                   0x0301c /* Fence 15 */
/* [05000 .. 05fff] GMBUS and I/O Control registers */
#define GMBUS0                      0x05100 /* GMBUS Clock/Port Select */
#define GMBUS1                      0x05104 /* GMBUS Command/Status */
#define GMBUS2                      0x05108 /* GMBUS Status */
#define GMBUS3                      0x0510c /* GMBUS Data Buffer */
#define GMBUS4                      0x05110 /* GMBUS Interrupt Mask */
#define GMBUS5                      0x05120 /* 2 Byte Index */
#define GMBUS6                      0x05124 /* cdclk divider */
/* [06000 .. 06fff] Display Clock Control registers */
#define VGA0_DIV                    0x06000 /* VGA 0 Divisor (25.175 MHz) */
#define VGA1_DIV                    0x06004 /* VGA 1 Divisor (28.322 MHz) */
#define VGA_PD                      0x06010 /* VGA Post Divisor Values */
#define DPLL_A_CR                   0x06014 /* DPLL A Control Register */
#define DPLL_B_CR                   0x06018 /* DPLL B Control Register */
#define FPA0                        0x06040 /* DPLL A Divisor 0 */
#define FPA1                        0x06044 /* DPLL A Divisor 1 */
#define FPB0                        0x06048 /* DPLL B Divisor 0 */
#define FPB1                        0x0604c /* DPLL B Divisor 1 */
#define DPLL_TEST                   0x0606c /* DPLL A and DPLL B Test */
#define TWO_D_CG_DIS                0x06200 /* Clock Gating Disable for Display */
#define THREE_D_CG_DIS              0x06204 /* Clock Gating Disable for 3D (PowerVR Thaila) and Video (PowerVR MSVDX) */
/* [0a000 .. 0afff] Display Palette registers */
#define DPALETTE_A                  0x0a000 /* Pipe A Display Palette */
#define DPALETTE_B                  0x0a800 /* Pipe B Display Palette */
/* [30000 .. 3ffff] Overlay registers */
#define OVADD                       0x30000 /* Overlay Register Update Address */
#define OTEST                       0x30004 /* Overlay Test */
#define DOVSTA                      0x30008 /* Display/Overlay Status */
#define DOVSTAEX                    0x3000c /* Display/Overlay Extended Status */
#define GAMC5                       0x30010 /* Overlay Dedicated Gamma Correction registers */
#define GAMC4                       0x30014
#define GAMC3                       0x30018
#define GAMC2                       0x3001c
#define GAMC1                       0x30020
#define GAMC0                       0x30024
#define SYNCPH0                     0x30058 /* Overlay Flip Sync Lock Phase registers */
#define SYNCPH1                     0x3005c
#define SYNCPH2                     0x30060
#define SYNCPH3                     0x30064
#define OBUF_0Y                     0x30100 /* Overlay Buffer 0 Start Address Y Pointer */
#define OBUF_1Y                     0x30104 /* Overlay Buffer 1 Start Address Y Pointer */
#define OBUF_0U                     0x30108 /* Overlay Buffer 0 Start Address U Pointer */
#define OBUF_0V                     0x3010c /* Overlay Buffer 0 V Pointer */
#define OBUF_1U                     0x30110 /* Overlay Buffer 1 Start Address U Pointer */
#define OBUF_1V                     0x30114 /* Overlay Buffer 1 Start Address V Pointer */
#define OSTRIDE                     0x30118 /* Overlay Stride */
#define YRGB_VPH                    0x3011c /* Y/RGB Vertical Phase */
#define UV_VPH                      0x30120 /* UV Vertical Phase */
#define HORZ_PH                     0x30124 /* Horizontal Phase */
#define INIT_PHS                    0x30128 /* Initial Phase Shift */
#define DWINPOS                     0x3012c /* Destination Window Position */
#define DWINSZ                      0x30130 /* Destination Window Size */
#define SWIDTH                      0x30134 /* Source Width */
#define SWIDTHSW                    0x30138 /* Source SWORD Width */
#define SHEIGHT                     0x3013c /* Source Height */ 
#define YRGBSCALE                   0x30140 /* Y/RGB Scale Factor */
#define UVSCALE                     0x30144 /* U V Scale Factor */
#define OCLRC0                      0x30148 /* Overlay Color Correction 0 */
#define OCLRC1                      0x3014c /* Overlay Color Correction 1 */
#define DCLRKV                      0x30150 /* Destination Color Key Value */
#define DCLRKM                      0x30154 /* Destination Color Key Mask */
#define SCHRKVH                     0x30158 /* Source Chroma Key Value High */
#define SCHRKVL                     0x3015c /* Source Chroma Key Value Low */
#define SCHRKEN                     0x30160 /* Source Chroma Key Enable */
#define OCONFIG                     0x30164 /* Overlay Configuration */
#define OCOMD                       0x30168 /* Overlay Command */
#define UVSCALEV                    0x301a4 /* UV Vertical Downscale Integer */
#define UV_HCOEFS                   0x30700 /* 32 x 64 - Overlay UV Horizontal Filter Coefficients */
/* [60000 .. 610ff] Display Pipeline */
#define HTOTAL_A                    0x60000 /* Pipe A Horizontal Total */
#define HBLANK_A                    0x60004 /* Pipe A Horizontal Blank */
#define HSYNC_A                     0x60008 /* Pipe A Horizontal Sync */
#define VTOTAL_A                    0x6000c /* Pipe A Vertical Total */
#define VBLANK_A                    0x60010 /* Pipe A Vertical Blank */
#define VSYNC_A                     0x60014 /* Pipe A Vertical Sync */
#define PIPEASRC                    0x6001c /* Pipe A Source Image Size */
#define BCLRPAT_A                   0x60020 /* Pipe A Border Color Pattern */
#define VSYNCSHIFT_A                0x60028 /* Vertical Sync Shift (Poulsbo) */
#define CRCCTRLCOLORA_R             0x60050 /* Pipe A CRC Color Channel Control registers */
#define CRCCTRLCOLORA_G             0x60054
#define CRCCTRLCOLORA_B             0x60058
#define CRCCTRLCOLORA_A             0x6005c
#define CRCRESCOLORA_R              0x60060 /* Pipe A CRC Color Channel Result registers */
#define CRCRESCOLORA_G              0x60064
#define CRCRESCOLORA_B              0x60068
#define CRCRESCOLORA_A              0x6006c
#define HTOTAL_B                    0x61000 /* Pipe B Horizontal Total */
#define HBLANK_B                    0x61004 /* Pipe B Horizontal Blank */
#define HSYNC_B                     0x61008 /* Pipe B Horizontal Sync */
#define VTOTAL_B                    0x6100c /* Pipe B Vertical Total */
#define VBLANK_B                    0x61010 /* Pipe B Vertical Blank */
#define VSYNC_B                     0x61014 /* Pipe B Vertical Sync */
#define PIPEBSRC                    0x6101c /* Pipe B Source Image Size */
#define BCLRPAT_B                   0x61020 /* Pipe B Border Color Pattern */
#define VSYNCSHIFT_B                0x61028 /* Vertical Sync Shift Register(Poulsbo) */
#define CRCCTRLCOLORB_R             0x61050 /* Pipe B CRC Color Channel Control registers */
#define CRCCTRLCOLORB_G             0x61054
#define CRCCTRLCOLORB_B             0x61058
#define CRCCTRLCOLORB_A             0x6105c
#define CRCRESCOLORB_R              0x61060 /* Pipe B CRC Color Channel Result registers */
#define CRCRESCOLORB_G              0x61064
#define CRCRESCOLORB_B              0x61068
#define CRCRESCOLORB_A              0x6106c
/* [61100 .. 611ff] Display Port Control */
#define ADPA_PORT_CTRL              0x61100 /* Analog Display Port Register */
#define PORT_HOTPLUG_EN             0x61110 /* Port Hot Plug Enable */
#define PORT_HOTPLUG_STAT           0x61114 /* Port Hot Plug Status */
#define SDVOB_PORT_CTRL             0x61140 /* Digital Display Port B Control */
#define SDVO_DFT                    0x61150 /* Digital Display Port DFT */
#define DFT                         0x61154 /* Generic-DFT */
#define SDVOC_PORT_CTRL             0x61160 /* Digital Display Port B Control */
#define SDVOB_CB                    0x61170 /* SDVO Buffer control bit */
#define LVDS_CB                     0x61174 /* LVDS Buffer control bit */
#define LVDS_PORT_CTRL              0x61180 /* LVDS Digital Display Port Control */
/* [61200 .. 612ff] Panel Fitting/LVDS */
#define PP_STATUS                   0x61200 /* Panel Power Status */
#define PP_CONTROL                  0x61204 /* Panel Power Control */
#define PP_ON_DELAYS                0x61208 /* Panel Power-On Sequencing Delays */
#define PP_OFF_DELAYS               0x6120c /* Panel Power-Off Sequencing Delays */
#define PP_DIVISOR                  0x61210 /* Panel Power Cycle Delay and Reference Divisor */
#define PFIT_CONTROL                0x61230 /* Panel Fitting Controls */
#define PFIT_PGM_RATIOS             0x61234 /* Programmed Panel Fitting Ratios */
#define PFIT_AUTO_RATIOS            0x61238 /* Auto Scaling Ratios Readback */
#define PFIT_INIT_PHASE             0x6123c /* Scaling Initial Phase */
#define BLC_PWM_CTL                 0x61254 /* Backlight PWM Control */
#define BLM_HIST_CTL                0x61260 /* Image BLM  Histogram Control */
#define BLM_THRESH_01               0x61270 /* BLM Threshold 0/1 */
#define BLM_THRESH_23               0x61274 /* BLM Threshold 2/3 */
#define BLM_THRESH_45               0x61278 /* BLM Threshold 4/5 */
#define BLM_THRESH_67               0x6127c /* BLM Threshold 6/7 */
#define BLM_THRESH_89               0x61280 /* BLM Threshold 8/9 */
#define BLM_THRESH_1011             0x61284 /* BLM Threshold 10/11 */
#define BLM_STATUS_01               0x61290 /* BLM Segment Status 0/1 */
#define BLM_STATUS_23               0x61294 /* BLM Segment Status 2/3 */
#define BLM_STATUS_45               0x61298 /* BLM Segment Status 4/5 */
#define BLM_STATUS_67               0x6129c /* BLM Segment Status 6/7 */
#define BLM_STATUS_89               0x612a0 /* BLM Segment Status 8/9 */
#define BLM_STATUS_1011             0x612a4 /* BLM Segment Status 10/11 */
#define BLM_GUARDBAND               0x612a8 /* BLM Threshold Guardband */
/* [70000 .. 7ffff] Display and Cursor Registers */
#define PIPEA_DSL                   0x70000 /* Pipe A Display Scan Line */
#define PIPEA_SLC                   0x70004 /* Pipe A Display Scan Line Count Range Compare */
#define PIPEACONF                   0x70008 /* Pipe A Configuration Register */
#define PIPEASTAT                   0x70024 /* Pipe A Display Status */
#define DSPARB                      0x70030 /* Display Arbitration Control */
#define PIPEAFRAMEH                 0x70040 /* Pipe A Frame Count High */
#define PIPEAFRAMEPIX               0x70044 /* Pipe A Frame Count Low and Pixel Count */
#define CURACNTR                    0x70080 /* Cursor A Control */
#define CURABASE                    0x70084 /* Cursor A Base Address */
#define CURAPOS                     0x70088 /* Cursor A Position */
#define CURAPALET0                  0x70090 /* Cursor A Palette 0 */
#define CURAPALET1                  0x70094 /* Cursor A Palette 1 */
#define CURAPALET2                  0x70098 /* Cursor A Palette 2 */
#define CURAPALET3                  0x7009c /* Cursor A Palette 3 */
#define CURBCNTR                    0x700c0 /* Cursor B Control */
#define CURBBASE                    0x700c4 /* Cursor B Base Address */
#define CURBPOS                     0x700c8 /* Cursor B Position */
#define CURBPALET0                  0x700d0 /* Cursor B Palette 0 */
#define CURBPALET1                  0x700d4 /* Cursor B Palette 1 */
#define CURBPALET2                  0x700d8 /* Cursor B Palette 2 */
#define CURBPALET3                  0x700dc /* Cursor B Palette 3 */
#define DSPAFLIPADDR                0x7017c /* Display A Async Flip Start Address */
#define DSPACNTR                    0x70180 /* Display A Plane Control */
#define DSPAADDR                    0x70184 /* Display A Start Address */
#define DSPASTRIDE                  0x70188 /* Display A Stride */
#define DSPASIZE                    0x70190 /* Display A Source Height and Width */
#define DSPAKEYVAL                  0x70194 /* Display A Sprite Color Key Value */
#define DSPAKEYMSK                  0x70198 /* Display A Sprite Color Key Mask */
#define CHICKEN_BIT                 0x70400 /* Chicken Bit */
#define SWF00                       0x70410 /* Software Flag 00 */
#define SWF01                       0x70414 /* Software Flag 01 */
#define SWF02                       0x70418 /* Software Flag 02 */
#define SWF03                       0x7041c /* Software Flag 03 */
#define SWF04                       0x70420 /* Software Flag 04 */
#define SWF05                       0x70424 /* Software Flag 05 */
#define SWF06                       0x70428 /* Software Flag 06 */
#define SWF07                       0x7042c /* Software Flag 07 */
#define SWF08                       0x70430 /* Software Flag 08 */
#define SWF09                       0x70434 /* Software Flag 09 */
#define SWF0A                       0x70438 /* Software Flag 0A */
#define SWF0B                       0x7043c /* Software Flag 0B */
#define SWF0C                       0x70440 /* Software Flag 0C */
#define SWF0D                       0x70444 /* Software Flag 0D */
#define SWF0E                       0x70448 /* Software Flag 0E */
#define SWF0F                       0x7044c /* Software Flag 0F */
#define PIPEB_DSL                   0x71000 /* Pipe B Display Scan Line */
#define PIPEB_SLC                   0x71004 /* Pipe B Display Scan Line Count Range Compare */
#define PIPEBCONF                   0x71008 /* Pipe B Configuration Register */
#define PIPEBSTAT                   0x71024 /* Pipe B Display Status */
#define PIPEBFRAMEH                 0x71040 /* Pipe B Frame Count High */
#define PIPEBFRAMEPIX               0x71044 /* Pipe B Frame Count Low and Pixel Count */
#define DSPBFLIPADDR                0x7117c /* Display B Async flip Start Address */
#define DSPBCNTR                    0x71180 /* Display B Sprite Plane Control */
#define DSPBADDR                    0x71184 /* Display B Sprite Start Address */
#define DSPBSTRIDE                  0x71188 /* Display B Sprite Stride */
#define DSPBPOS                     0x7118c /* Display B Sprite Position */
#define DSPBSIZE                    0x71190 /* Display B Sprite Height and Width */
#define DSPBKEYVAL                  0x71194 /* Display B Sprite Color Key Value */
#define DSPBKEYMSK                  0x71198 /* Display B Sprite Color Key Mask */
#define VGACNTRL                    0x71400 /* VGA Display Plane Control */
#define SWF10                       0x71410 /* Software Flag 10 */
#define   SWF10_FW_ID               0xe1df0000
#define   SWF10_ST_BIT              0x00000004
#define SWF11                       0x71414 /* Software Flag 11 */
#define SWF12                       0x71418 /* Software Flag 12 */
#define SWF13                       0x7141c /* Software Flag 13 */
#define SWF14                       0x71420 /* Software Flag 14 */
#define SWF15                       0x71424 /* Software Flag 15 */
#define SWF16                       0x71428 /* Software Flag 16 */
#define SWF17                       0x7142c /* Software Flag 17 */
#define SWF18                       0x71430 /* Software Flag 18 */
#define SWF19                       0x71434 /* Software Flag 19 */
#define SWF1A                       0x71438 /* Software Flag 1A */
#define SWF1B                       0x7143c /* Software Flag 1B */
#define SWF1C                       0x71440 /* Software Flag 1C */
#define SWF1D                       0x71444 /* Software Flag 1D */
#define SWF1E                       0x71448 /* Software Flag 1E */
#define SWF1F                       0x7144c /* Software Flag 1F */
#define DSPCCNTR                    0x72180 /* Display C Sprite Control */
#define DSPCADDR                    0x72184 /* Display C Sprite Start Address */
#define DSPCSTRIDE                  0x72188 /* Display C Sprite Stride */
#define DSPCPOS                     0x7218c /* Display C Sprite Position */
#define DSPCSIZE                    0x72190 /* Display C Sprite Height and Width */
#define DSPCKEYMINVAL               0x72194 /* Display C Sprite Color Key Min Value */
#define DSPCKEYMSK                  0x72198 /* Display C Sprite Color Key Mask */
#define DSPCKEYMAXVAL               0x721a0 /* Display C Sprite Color Key Max Value */
#define DCLRC0                      0x721d0 /* Display C Color Correction 0 */
#define DCLRC1                      0x721d4 /* Display C Color Correction 1 */
#define DCGAMC5                     0x721e0 /* Display C Gamma Correction 5 */
#define DCGAMC4                     0x721e4 /* Display C Gamma Correction 4 */
#define DCGAMC3                     0x721e8 /* Display C Gamma Correction 3 */
#define DCGAMC2                     0x721ec /* Display C Gamma Correction 2 */
#define DCGAMC1                     0x721f0 /* Display C Gamma Correction 1 */
#define DCGAMC0                     0x721f4 /* Display C Gamma Correction 0 */
#define SWF30                       0x72414 /* Software Flag 30 */
#define SWF31                       0x72418 /* Software Flag 31 */
#define SWF32                       0x7241c /* Software Flag 32 */

/* Register operations */
enum
{
    OP_END = 128,   /* end script */
    OP_WR_IMM,      /* write immediate (offset, value) */  
    OP_WAIT,        /* wait (delay in microseconds) */
    OP_UPD,         /* update (offset, mask, or-value) */
    OP_CMP_EQ,      /* compare equality (offset, mask, value) */
    OP_CMP_NE,      /* compare inequality (offset, mask, value) */
    OP_WR           /* write (offset) */
};

/* South display registers */
#define PCH_DAC_CTL                 0xe1100 /* Analog Port CRT DAC Control */
#define PCH_HDMI_CTL                0xe1140 /* HDMI Port Control */
#define PCH_LVDS                    0xe1180 /* LVDS Port Control Register */

/* 640 x 480 x 32 @ 60 Hz */
LOCAL  const unsigned int   mode00[] = {GFX_DISP_PORT_SDVO_B,
                                        OP_WR_IMM, FENCE__0, 0x00000000,
                                        OP_WR_IMM, FENCE__1, 0x00000000,
                                        OP_WR_IMM, FENCE__2, 0x00000000,
                                        OP_WR_IMM, FENCE__3, 0x00000000,
                                        OP_WR_IMM, FENCE__4, 0x00000000,
                                        OP_WR_IMM, FENCE__5, 0x00000000,
                                        OP_WR_IMM, FENCE__6, 0x00000000,
                                        OP_WR_IMM, FENCE__7, 0x00000000,
                                        OP_WR_IMM, FENCE__8, 0x00000000,
                                        OP_WR_IMM, FENCE__9, 0x00000000,
                                        OP_WR_IMM, FENCE__10, 0x00000000,
                                        OP_WR_IMM, FENCE__11, 0x00000000,
                                        OP_WR_IMM, FENCE__12, 0x00000000,
                                        OP_WR_IMM, FENCE__13, 0x00000000,
                                        OP_WR_IMM, FENCE__14, 0x00000000,
                                        OP_WR_IMM, FENCE__15, 0x00000000,
                                        OP_WR_IMM, FW_BLC1, 0x490a010a,
                                        OP_WR_IMM, FW_BLC2, 0x14100d0a,
                                        OP_WR_IMM, FW_BLC_SELF, 0x0b0c9812,
                                        OP_WR_IMM, MI_ARB_STATE, 0x00000000,
                                        OP_WR_IMM, FW_BLC3, 0x00007770,
                                        OP_WR_IMM, G_DEBUG, 0x1000020f,
                                        OP_WR_IMM, VGA0_DIV, 0x00031108,
                                        OP_WR_IMM, VGA1_DIV, 0x00031406,
                                        OP_WR_IMM, VGA_PD, 0x00020002,
                                        OP_WR_IMM, FPA0, 0x00031108,
                                        OP_WR_IMM, FPA1, 0x00031108,
                                        OP_WR_IMM, FPB0, 0x00031108,
                                        OP_WR_IMM, FPB1, 0x00031108,
                                        OP_WR_IMM, TWO_D_CG_DIS, 0x00001000,
                                        OP_WR_IMM, THREE_D_CG_DIS, 0x00000000,
                                        OP_WR_IMM, DPLL_A_CR, 0xd4020c33,
                                        OP_WR_IMM, DPLL_B_CR, 0x04801203,
                                        OP_WR_IMM, SDVOC_PORT_CTRL, 0x00000000,
                                        OP_WR_IMM, SDVOB_PORT_CTRL, 0x80000080,
                                        OP_WR_IMM, PP_CONTROL, 0x00000000,
                                        OP_WR_IMM, PP_ON_DELAYS, 0x00000000,
                                        OP_WR_IMM, PP_OFF_DELAYS, 0x00000000,
                                        OP_WR_IMM, PP_DIVISOR, 0x00270f04,
                                        OP_WR_IMM, PFIT_CONTROL, 0x00000000,
                                        OP_WR_IMM, PFIT_PGM_RATIOS, 0x00000000,
                                        OP_WR_IMM, PFIT_AUTO_RATIOS, 0x00000000,
                                        OP_WR_IMM, PFIT_INIT_PHASE, 0x01000100,
                                        OP_WR_IMM, BLC_PWM_CTL, 0x00000000,
                                        OP_WR_IMM, BLM_HIST_CTL, 0x00000000,
                                        OP_WR_IMM, LVDS_PORT_CTRL, 0x40000000,
                                        OP_WR_IMM, PIPEASTAT, 0x01003bf3,
                                        OP_WR_IMM, DSPARB, 0x00001d9c,
                                        OP_WR_IMM, HTOTAL_A, 0x031f027f,
                                        OP_WR_IMM, HBLANK_A, 0x031f027f,
                                        OP_WR_IMM, HSYNC_A, 0x02ef028f,
                                        OP_WR_IMM, VTOTAL_A, 0x020c01df,
                                        OP_WR_IMM, VBLANK_A, 0x020c01df,
                                        OP_WR_IMM, VSYNC_A, 0x01eb01e9,
                                        OP_WR_IMM, PIPEASRC, 0x027f01df,
                                        OP_WR_IMM, BCLRPAT_A, 0x00000000,
                                        OP_WR_IMM, PIPEBSTAT, 0x00003bf3,
                                        OP_WR_IMM, HTOTAL_B, 0x031f027f,
                                        OP_WR_IMM, HBLANK_B, 0x03170287,
                                        OP_WR_IMM, HSYNC_B, 0x02ef028f,
                                        OP_WR_IMM, VTOTAL_B, 0x020c01df,
                                        OP_WR_IMM, VBLANK_B, 0x020401e7,
                                        OP_WR_IMM, VSYNC_B, 0x01eb01e9,
                                        OP_WR_IMM, PIPEBSRC, 0x027f01df,
                                        OP_WR_IMM, BCLRPAT_B, 0x00000000,
                                        OP_WR_IMM, PIPEACONF, 0x00000000,
                                        OP_WR_IMM, PIPEBCONF, 0x00000000,
                                        OP_WR_IMM, DSPASTRIDE, 0x00000a00,
                                        OP_WR_IMM, DSPAKEYVAL, 0x00000000,
                                        OP_WR_IMM, DSPAKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPBSTRIDE, 0x00000000,
                                        OP_WR_IMM, DSPBPOS, 0x00000000,
                                        OP_WR_IMM, DSPBSIZE, 0x00000000,
                                        OP_WR_IMM, DSPBKEYVAL, 0x00000000,
                                        OP_WR_IMM, DSPBKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPCSTRIDE, 0x00000000,
                                        OP_WR_IMM, DSPCPOS, 0x00000000,
                                        OP_WR_IMM, DSPCSIZE, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMINVAL, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMAXVAL, 0x00000000,
                                        OP_WR_IMM, DCLRC0, 0x01000000,
                                        OP_WR_IMM, DCLRC1, 0x00000080,
                                        OP_WR_IMM, DCGAMC5, 0x00c0c0c0,
                                        OP_WR_IMM, DCGAMC4, 0x00808080,
                                        OP_WR_IMM, DCGAMC3, 0x00404040,
                                        OP_WR_IMM, DCGAMC2, 0x00202020,
                                        OP_WR_IMM, DCGAMC1, 0x00101010,
                                        OP_WR_IMM, DCGAMC0, 0x00080808,
                                        OP_WR_IMM, DSPCCNTR, 0x00000000,
                                        OP_WR_IMM, DSPCADDR, 0x00000000,
                                        OP_WR_IMM, DSPBCNTR, 0x01000000,
                                        OP_WR_IMM, DSPBADDR, 0x00000000,
                                        OP_WR_IMM, DSPACNTR, 0x98000000,
                                        OP_WR_IMM, DSPAADDR, 0x00000000,
                                        OP_WR_IMM, VGACNTRL, 0x8020008e,
                                        OP_WR_IMM, CURABASE, 0x00000000,
                                        OP_WR_IMM, CURAPOS, 0x00000000,
                                        OP_WR_IMM, CURAPALET0, 0x00000000,
                                        OP_WR_IMM, CURAPALET1, 0x00000000,
                                        OP_WR_IMM, CURAPALET2, 0x00000000,
                                        OP_WR_IMM, CURAPALET3, 0x00000000,
                                        OP_WR_IMM, CURBBASE, 0x00000000,
                                        OP_WR_IMM, CURBPOS, 0x00000000,
                                        OP_WR_IMM, CURBPALET0, 0x00000000,
                                        OP_WR_IMM, CURBPALET1, 0x00000000,
                                        OP_WR_IMM, CURBPALET2, 0x00000000,
                                        OP_WR_IMM, CURBPALET3, 0x00000000,
                                        OP_WR_IMM, HWS_PGA, 0x1ffff000,
                                        OP_WR_IMM, SCPD0, 0x00000000,
                                        OP_END,
                                        /**/
                                        GFX_DISP_PORT_LVDS,
                                        OP_WR_IMM, FENCE__0, 0x00000000,
                                        OP_WR_IMM, FENCE__1, 0x00000000,
                                        OP_WR_IMM, FENCE__2, 0x00000000,
                                        OP_WR_IMM, FENCE__3, 0x00000000,
                                        OP_WR_IMM, FENCE__4, 0x00000000,
                                        OP_WR_IMM, FENCE__5, 0x00000000,
                                        OP_WR_IMM, FENCE__6, 0x00000000,
                                        OP_WR_IMM, FENCE__7, 0x00000000,
                                        OP_WR_IMM, FENCE__8, 0x00000000,
                                        OP_WR_IMM, FENCE__9, 0x00000000,
                                        OP_WR_IMM, FENCE__10, 0x00000000,
                                        OP_WR_IMM, FENCE__11, 0x00000000,
                                        OP_WR_IMM, FENCE__12, 0x00000000,
                                        OP_WR_IMM, FENCE__13, 0x00000000,
                                        OP_WR_IMM, FENCE__14, 0x00000000,
                                        OP_WR_IMM, FENCE__15, 0x00000000,
                                        OP_WR_IMM, FW_BLC1, 0x490a010a,
                                        OP_WR_IMM, FW_BLC2, 0x14100d0a,
                                        OP_WR_IMM, FW_BLC_SELF, 0x0b0c9812,
                                        OP_WR_IMM, MI_ARB_STATE, 0x00000000,
                                        OP_WR_IMM, FW_BLC3, 0x00007770,
                                        OP_WR_IMM, G_DEBUG, 0x1000020f,
                                        OP_WR_IMM, VGA0_DIV, 0x00031108,
                                        OP_WR_IMM, VGA1_DIV, 0x00031406,
                                        OP_WR_IMM, VGA_PD, 0x00020002,
                                        OP_WR_IMM, FPA0, 0x00031108,
                                        OP_WR_IMM, FPA1, 0x00031108,
                                        OP_WR_IMM, FPB0, 0x00030e09,
                                        OP_WR_IMM, FPB1, 0x00031108,
                                        OP_WR_IMM, TWO_D_CG_DIS, 0x00001000,
                                        OP_WR_IMM, THREE_D_CG_DIS, 0x00000000,
                                        OP_WR_IMM, DPLL_A_CR, 0x04800c03,
                                        OP_WR_IMM, DPLL_B_CR, 0xd8027203,
                                        OP_WR_IMM, SDVOC_PORT_CTRL, 0x00000000,
                                        OP_WR_IMM, SDVOB_PORT_CTRL, 0x00480000,
                                        OP_WR_IMM, PP_CONTROL, 0x00000001,
                                        OP_WR_IMM, PP_ON_DELAYS, 0x025907d1,
                                        OP_WR_IMM, PP_OFF_DELAYS, 0x01f507d1,
                                        OP_WR_IMM, PP_DIVISOR, 0x00270f05,
                                        OP_WR_IMM, PFIT_CONTROL, 0x80002668,
                                        OP_WR_IMM, PFIT_PGM_RATIOS, 0x00000000,
                                        OP_WR_IMM, PFIT_AUTO_RATIOS, 0x9fe09fe0,
                                        OP_WR_IMM, PFIT_INIT_PHASE, 0x01000100,
                                        OP_WR_IMM, BLC_PWM_CTL, 0x7a137a12,
                                        OP_WR_IMM, BLM_HIST_CTL, 0x00000000,
                                        OP_WR_IMM, LVDS_PORT_CTRL, 0xc0300300,
                                        OP_WR_IMM, PIPEASTAT, 0x00003bf3,
                                        OP_WR_IMM, DSPARB, 0x00001d9c,
                                        OP_WR_IMM, HTOTAL_A, 0x031f027f,
                                        OP_WR_IMM, HBLANK_A, 0x03170287,
                                        OP_WR_IMM, HSYNC_A, 0x02ef028f,
                                        OP_WR_IMM, VTOTAL_A, 0x020c01df,
                                        OP_WR_IMM, VBLANK_A, 0x020401e7,
                                        OP_WR_IMM, VSYNC_A, 0x01eb01e9,
                                        OP_WR_IMM, PIPEASRC, 0x027f01df,
                                        OP_WR_IMM, BCLRPAT_A, 0x00000000,
                                        OP_WR_IMM, PIPEBSTAT, 0x01003bf3,
                                        OP_WR_IMM, HTOTAL_B, 0x053f03ff,
                                        OP_WR_IMM, HBLANK_B, 0x053f03ff,
                                        OP_WR_IMM, HSYNC_B, 0x049f0417,
                                        OP_WR_IMM, VTOTAL_B, 0x032502ff,
                                        OP_WR_IMM, VBLANK_B, 0x032502ff,
                                        OP_WR_IMM, VSYNC_B, 0x03080302,
                                        OP_WR_IMM, PIPEBSRC, 0x027f01df,
                                        OP_WR_IMM, BCLRPAT_B, 0x00000000,
                                        OP_WR_IMM, PIPEACONF, 0x00000000,
                                        OP_WR_IMM, PIPEBCONF, 0x00000000,
                                        OP_WR_IMM, DSPASTRIDE, 0x00000000,
                                        OP_WR_IMM, DSPAKEYVAL, 0x00000000,
                                        OP_WR_IMM, DSPAKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPBSTRIDE, 0x00000a00,
                                        OP_WR_IMM, DSPBPOS, 0x00000000,
                                        OP_WR_IMM, DSPBSIZE, 0x01df027f,
                                        OP_WR_IMM, DSPBKEYVAL, 0x00000000,
                                        OP_WR_IMM, DSPBKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPCSTRIDE, 0x00000000,
                                        OP_WR_IMM, DSPCPOS, 0x00000000,
                                        OP_WR_IMM, DSPCSIZE, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMINVAL, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMAXVAL, 0x00000000,
                                        OP_WR_IMM, DCLRC0, 0x01000000,
                                        OP_WR_IMM, DCLRC1, 0x00000080,
                                        OP_WR_IMM, DCGAMC5, 0x00c0c0c0,
                                        OP_WR_IMM, DCGAMC4, 0x00808080,
                                        OP_WR_IMM, DCGAMC3, 0x00404040,
                                        OP_WR_IMM, DCGAMC2, 0x00202020,
                                        OP_WR_IMM, DCGAMC1, 0x00101010,
                                        OP_WR_IMM, DCGAMC0, 0x00080808,
                                        OP_WR_IMM, DSPCCNTR, 0x00000000,
                                        OP_WR_IMM, DSPCADDR, 0x00000000,
                                        OP_WR_IMM, DSPBCNTR, 0x99000000,
                                        OP_WR_IMM, DSPBADDR, 0x00000000,
                                        OP_WR_IMM, DSPACNTR, 0x00000000,
                                        OP_WR_IMM, DSPAADDR, 0x00000000,
                                        OP_WR_IMM, VGACNTRL, 0xa2c4008e,
                                        OP_WR_IMM, CURABASE, 0x00000000,
                                        OP_WR_IMM, CURAPOS, 0x00000000,
                                        OP_WR_IMM, CURAPALET0, 0x00000000,
                                        OP_WR_IMM, CURAPALET1, 0x00000000,
                                        OP_WR_IMM, CURAPALET2, 0x00000000,
                                        OP_WR_IMM, CURAPALET3, 0x00000000,
                                        OP_WR_IMM, CURBBASE, 0x00000000,
                                        OP_WR_IMM, CURBPOS, 0x00000000,
                                        OP_WR_IMM, CURBPALET0, 0x00000000,
                                        OP_WR_IMM, CURBPALET1, 0x00000000,
                                        OP_WR_IMM, CURBPALET2, 0x00000000,
                                        OP_WR_IMM, CURBPALET3, 0x00000000,
                                        OP_WR_IMM, HWS_PGA, 0x1ffff000,
                                        OP_WR_IMM, SCPD0, 0x00000000,
                                        OP_END,
                                        /**/
                                        0xffffffff};

/* 800 x 600 x 32 @ 60 Hz */
LOCAL  const unsigned int   mode01[] = {GFX_DISP_PORT_SDVO_B,
                                        OP_WR_IMM, FENCE__0, 0x00000000,
                                        OP_WR_IMM, FENCE__1, 0x00000000,
                                        OP_WR_IMM, FENCE__2, 0x00000000,
                                        OP_WR_IMM, FENCE__3, 0x00000000,
                                        OP_WR_IMM, FENCE__4, 0x00000000,
                                        OP_WR_IMM, FENCE__5, 0x00000000,
                                        OP_WR_IMM, FENCE__6, 0x00000000,
                                        OP_WR_IMM, FENCE__7, 0x00000000,
                                        OP_WR_IMM, FENCE__8, 0x00000000,
                                        OP_WR_IMM, FENCE__9, 0x00000000,
                                        OP_WR_IMM, FENCE__10, 0x00000000,
                                        OP_WR_IMM, FENCE__11, 0x00000000,
                                        OP_WR_IMM, FENCE__12, 0x00000000,
                                        OP_WR_IMM, FENCE__13, 0x00000000,
                                        OP_WR_IMM, FENCE__14, 0x00000000,
                                        OP_WR_IMM, FENCE__15, 0x00000000,
                                        OP_WR_IMM, FW_BLC1, 0x490a010a,
                                        OP_WR_IMM, FW_BLC2, 0x14100d0a,
                                        OP_WR_IMM, FW_BLC_SELF, 0x0b0c9812,
                                        OP_WR_IMM, MI_ARB_STATE, 0x00000000,
                                        OP_WR_IMM, FW_BLC3, 0x00007770,
                                        OP_WR_IMM, G_DEBUG, 0x1000020f,
                                        OP_WR_IMM, VGA0_DIV, 0x00031108,
                                        OP_WR_IMM, VGA1_DIV, 0x00031406,
                                        OP_WR_IMM, VGA_PD, 0x00020002,
                                        OP_WR_IMM, FPA0, 0x00041008,
                                        OP_WR_IMM, FPA1, 0x00031108,
                                        OP_WR_IMM, FPB0, 0x00031108,
                                        OP_WR_IMM, FPB1, 0x00031108,
                                        OP_WR_IMM, TWO_D_CG_DIS, 0x00001000,
                                        OP_WR_IMM, THREE_D_CG_DIS, 0x00000000,
                                        OP_WR_IMM, DPLL_A_CR, 0xd4010c33,
                                        OP_WR_IMM, DPLL_B_CR, 0x04801203,
                                        OP_WR_IMM, SDVOC_PORT_CTRL, 0x00000000,
                                        OP_WR_IMM, SDVOB_PORT_CTRL, 0x80000080,
                                        OP_WR_IMM, PP_CONTROL, 0x00000000,
                                        OP_WR_IMM, PP_ON_DELAYS, 0x00000000,
                                        OP_WR_IMM, PP_OFF_DELAYS, 0x00000000,
                                        OP_WR_IMM, PP_DIVISOR, 0x00270f04,
                                        OP_WR_IMM, PFIT_CONTROL, 0x00000000,
                                        OP_WR_IMM, PFIT_PGM_RATIOS, 0x00000000,
                                        OP_WR_IMM, PFIT_AUTO_RATIOS, 0x00000000,
                                        OP_WR_IMM, PFIT_INIT_PHASE, 0x01000100,
                                        OP_WR_IMM, BLC_PWM_CTL, 0x00000000,
                                        OP_WR_IMM, BLM_HIST_CTL, 0x00000000,
                                        OP_WR_IMM, LVDS_PORT_CTRL, 0x40000000,
                                        OP_WR_IMM, PIPEASTAT, 0x01003bf3,
                                        OP_WR_IMM, DSPARB, 0x00001d9c,
                                        OP_WR_IMM, HTOTAL_A, 0x041f031f,
                                        OP_WR_IMM, HBLANK_A, 0x041f031f,
                                        OP_WR_IMM, HSYNC_A, 0x03c70347,
                                        OP_WR_IMM, VTOTAL_A, 0x02730257,
                                        OP_WR_IMM, VBLANK_A, 0x02730257,
                                        OP_WR_IMM, VSYNC_A, 0x025c0258,
                                        OP_WR_IMM, PIPEASRC, 0x031f0257,
                                        OP_WR_IMM, BCLRPAT_A, 0x00000000,
                                        OP_WR_IMM, PIPEBSTAT, 0x00003bf3,
                                        OP_WR_IMM, HTOTAL_B, 0x031f027f,
                                        OP_WR_IMM, HBLANK_B, 0x03170287,
                                        OP_WR_IMM, HSYNC_B, 0x02ef028f,
                                        OP_WR_IMM, VTOTAL_B, 0x020c01df,
                                        OP_WR_IMM, VBLANK_B, 0x020401e7,
                                        OP_WR_IMM, VSYNC_B, 0x01eb01e9,
                                        OP_WR_IMM, PIPEBSRC, 0x027f01df,
                                        OP_WR_IMM, BCLRPAT_B, 0x00000000,
                                        OP_WR_IMM, PIPEACONF, 0x00000000,
                                        OP_WR_IMM, PIPEBCONF, 0x00000000,
                                        OP_WR_IMM, DSPASTRIDE, 0x00000c80, 
                                        OP_WR_IMM, DSPAKEYVAL, 0x00000000, 
                                        OP_WR_IMM, DSPAKEYMSK, 0x00000000, 
                                        OP_WR_IMM, DSPBSTRIDE, 0x00000000, 
                                        OP_WR_IMM, DSPBPOS, 0x00000000, 
                                        OP_WR_IMM, DSPBSIZE, 0x00000000, 
                                        OP_WR_IMM, DSPBKEYVAL, 0x00000000, 
                                        OP_WR_IMM, DSPBKEYMSK, 0x00000000, 
                                        OP_WR_IMM, DSPCSTRIDE, 0x00000000, 
                                        OP_WR_IMM, DSPCPOS, 0x00000000, 
                                        OP_WR_IMM, DSPCSIZE, 0x00000000, 
                                        OP_WR_IMM, DSPCKEYMSK, 0x00000000, 
                                        OP_WR_IMM, DSPCKEYMINVAL, 0x00000000, 
                                        OP_WR_IMM, DSPCKEYMAXVAL, 0x00000000, 
                                        OP_WR_IMM, DCLRC0, 0x01000000, 
                                        OP_WR_IMM, DCLRC1, 0x00000080, 
                                        OP_WR_IMM, DCGAMC5, 0x00c0c0c0, 
                                        OP_WR_IMM, DCGAMC4, 0x00808080, 
                                        OP_WR_IMM, DCGAMC3, 0x00404040, 
                                        OP_WR_IMM, DCGAMC2, 0x00202020, 
                                        OP_WR_IMM, DCGAMC1, 0x00101010, 
                                        OP_WR_IMM, DCGAMC0, 0x00080808, 
                                        OP_WR_IMM, DSPCCNTR, 0x00000000, 
                                        OP_WR_IMM, DSPCADDR, 0x00000000, 
                                        OP_WR_IMM, DSPBCNTR, 0x01000000, 
                                        OP_WR_IMM, DSPBADDR, 0x00000000, 
                                        OP_WR_IMM, DSPACNTR, 0x98000000, 
                                        OP_WR_IMM, DSPAADDR, 0x00000000, 
                                        OP_WR_IMM, VGACNTRL, 0x8020008e, 
                                        OP_WR_IMM, CURABASE, 0x00000000, 
                                        OP_WR_IMM, CURAPOS, 0x00000000, 
                                        OP_WR_IMM, CURAPALET0, 0x00000000, 
                                        OP_WR_IMM, CURAPALET1, 0x00000000, 
                                        OP_WR_IMM, CURAPALET2, 0x00000000, 
                                        OP_WR_IMM, CURAPALET3, 0x00000000, 
                                        OP_WR_IMM, CURBBASE, 0x00000000, 
                                        OP_WR_IMM, CURBPOS, 0x00000000, 
                                        OP_WR_IMM, CURBPALET0, 0x00000000, 
                                        OP_WR_IMM, CURBPALET1, 0x00000000, 
                                        OP_WR_IMM, CURBPALET2, 0x00000000, 
                                        OP_WR_IMM, CURBPALET3, 0x00000000, 
                                        OP_WR_IMM, HWS_PGA, 0x1ffff000, 
                                        OP_WR_IMM, SCPD0, 0x00000000, 
                                        OP_END,
                                        /**/
                                        GFX_DISP_PORT_LVDS,
                                        OP_WR_IMM, FENCE__0, 0x00000000,
                                        OP_WR_IMM, FENCE__1, 0x00000000,
                                        OP_WR_IMM, FENCE__2, 0x00000000,
                                        OP_WR_IMM, FENCE__3, 0x00000000,
                                        OP_WR_IMM, FENCE__4, 0x00000000,
                                        OP_WR_IMM, FENCE__5, 0x00000000,
                                        OP_WR_IMM, FENCE__6, 0x00000000,
                                        OP_WR_IMM, FENCE__7, 0x00000000,
                                        OP_WR_IMM, FENCE__8, 0x00000000,
                                        OP_WR_IMM, FENCE__9, 0x00000000,
                                        OP_WR_IMM, FENCE__10, 0x00000000,
                                        OP_WR_IMM, FENCE__11, 0x00000000,
                                        OP_WR_IMM, FENCE__12, 0x00000000,
                                        OP_WR_IMM, FENCE__13, 0x00000000,
                                        OP_WR_IMM, FENCE__14, 0x00000000,
                                        OP_WR_IMM, FENCE__15, 0x00000000,
                                        OP_WR_IMM, FW_BLC1, 0x490a010a,
                                        OP_WR_IMM, FW_BLC2, 0x14100d0a,
                                        OP_WR_IMM, FW_BLC_SELF, 0x0b0c9812,
                                        OP_WR_IMM, MI_ARB_STATE, 0x00000000,
                                        OP_WR_IMM, FW_BLC3, 0x00007770,
                                        OP_WR_IMM, G_DEBUG, 0x1000020f,
                                        OP_WR_IMM, VGA0_DIV, 0x00031108,
                                        OP_WR_IMM, VGA1_DIV, 0x00031406,
                                        OP_WR_IMM, VGA_PD, 0x00020002,
                                        OP_WR_IMM, FPA0, 0x00031108,
                                        OP_WR_IMM, FPA1, 0x00031108,
                                        OP_WR_IMM, FPB0, 0x00030e09,
                                        OP_WR_IMM, FPB1, 0x00031108,
                                        OP_WR_IMM, TWO_D_CG_DIS, 0x00001000,
                                        OP_WR_IMM, THREE_D_CG_DIS, 0x00000000,
                                        OP_WR_IMM, DPLL_A_CR, 0x04800c03,
                                        OP_WR_IMM, DPLL_B_CR, 0xd8027203,
                                        OP_WR_IMM, SDVOC_PORT_CTRL, 0x00000000,
                                        OP_WR_IMM, SDVOB_PORT_CTRL, 0x00480000,
                                        OP_WR_IMM, PP_CONTROL, 0x00000001,
                                        OP_WR_IMM, PP_ON_DELAYS, 0x025907d1,
                                        OP_WR_IMM, PP_OFF_DELAYS, 0x01f507d1,
                                        OP_WR_IMM, PP_DIVISOR, 0x00270f05,
                                        OP_WR_IMM, PFIT_CONTROL, 0x80002668,
                                        OP_WR_IMM, PFIT_PGM_RATIOS, 0x00000000,
                                        OP_WR_IMM, PFIT_AUTO_RATIOS, 0xc7f0c7f0,
                                        OP_WR_IMM, PFIT_INIT_PHASE, 0x01000100,
                                        OP_WR_IMM, BLC_PWM_CTL, 0x7a137a12,
                                        OP_WR_IMM, BLM_HIST_CTL, 0x00000000,
                                        OP_WR_IMM, LVDS_PORT_CTRL, 0xc0300300,
                                        OP_WR_IMM, PIPEASTAT, 0x00003bf3,
                                        OP_WR_IMM, DSPARB, 0x00001d9c,
                                        OP_WR_IMM, HTOTAL_A, 0x031f027f,
                                        OP_WR_IMM, HBLANK_A, 0x03170287,
                                        OP_WR_IMM, HSYNC_A, 0x02ef028f,
                                        OP_WR_IMM, VTOTAL_A, 0x020c01df,
                                        OP_WR_IMM, VBLANK_A, 0x020401e7,
                                        OP_WR_IMM, VSYNC_A, 0x01eb01e9,
                                        OP_WR_IMM, PIPEASRC, 0x027f01df,
                                        OP_WR_IMM, BCLRPAT_A, 0x00000000,
                                        OP_WR_IMM, PIPEBSTAT, 0x01003bf3,
                                        OP_WR_IMM, HTOTAL_B, 0x053f03ff,
                                        OP_WR_IMM, HBLANK_B, 0x053f03ff,
                                        OP_WR_IMM, HSYNC_B, 0x049f0417,
                                        OP_WR_IMM, VTOTAL_B, 0x032502ff,
                                        OP_WR_IMM, VBLANK_B, 0x032502ff,
                                        OP_WR_IMM, VSYNC_B, 0x03080302,
                                        OP_WR_IMM, PIPEBSRC, 0x031f0257,
                                        OP_WR_IMM, BCLRPAT_B, 0x00000000,
                                        OP_WR_IMM, PIPEACONF, 0x00000000,
                                        OP_WR_IMM, PIPEBCONF, 0x00000000,
                                        OP_WR_IMM, DSPASTRIDE, 0x00000000,
                                        OP_WR_IMM, DSPAKEYVAL, 0x00000000,
                                        OP_WR_IMM, DSPAKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPBSTRIDE, 0x00000c80,
                                        OP_WR_IMM, DSPBPOS, 0x00000000,
                                        OP_WR_IMM, DSPBSIZE, 0x0257031f,
                                        OP_WR_IMM, DSPBKEYVAL, 0x00000000,
                                        OP_WR_IMM, DSPBKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPCSTRIDE, 0x00000000,
                                        OP_WR_IMM, DSPCPOS, 0x00000000,
                                        OP_WR_IMM, DSPCSIZE, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMSK, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMINVAL, 0x00000000,
                                        OP_WR_IMM, DSPCKEYMAXVAL, 0x00000000,
                                        OP_WR_IMM, DCLRC0, 0x01000000,
                                        OP_WR_IMM, DCLRC1, 0x00000080,
                                        OP_WR_IMM, DCGAMC5, 0x00c0c0c0,
                                        OP_WR_IMM, DCGAMC4, 0x00808080,
                                        OP_WR_IMM, DCGAMC3, 0x00404040,
                                        OP_WR_IMM, DCGAMC2, 0x00202020,
                                        OP_WR_IMM, DCGAMC1, 0x00101010,
                                        OP_WR_IMM, DCGAMC0, 0x00080808,
                                        OP_WR_IMM, DSPCCNTR, 0x00000000,
                                        OP_WR_IMM, DSPCADDR, 0x00000000,
                                        OP_WR_IMM, DSPBCNTR, 0x99000000,
                                        OP_WR_IMM, DSPBADDR, 0x00000000,
                                        OP_WR_IMM, DSPACNTR, 0x00000000,
                                        OP_WR_IMM, DSPAADDR, 0x00000000,
                                        OP_WR_IMM, VGACNTRL, 0xa2c4008e,
                                        OP_WR_IMM, CURABASE, 0x00000000,
                                        OP_WR_IMM, CURAPOS, 0x00000000,
                                        OP_WR_IMM, CURAPALET0, 0x00000000,
                                        OP_WR_IMM, CURAPALET1, 0x00000000,
                                        OP_WR_IMM, CURAPALET2, 0x00000000,
                                        OP_WR_IMM, CURAPALET3, 0x00000000,
                                        OP_WR_IMM, CURBBASE, 0x00000000,
                                        OP_WR_IMM, CURBPOS, 0x00000000,
                                        OP_WR_IMM, CURBPALET0, 0x00000000,
                                        OP_WR_IMM, CURBPALET1, 0x00000000,
                                        OP_WR_IMM, CURBPALET2, 0x00000000,
                                        OP_WR_IMM, CURBPALET3, 0x00000000,
                                        OP_WR_IMM, HWS_PGA, 0x1ffff000,
                                        OP_WR_IMM, SCPD0, 0x00000000,
                                        OP_END,
                                        /**/
                                        0xffffffff};

#endif  /* __INC_gfxItlgcDrv_h */
