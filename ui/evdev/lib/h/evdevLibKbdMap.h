/* evdevLibKbdMap.h - evdev keyboard mapper */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
24jun14,y_f  remove evdevLib.h (US41403)
15oct13,y_f  code clean
25sep13,j_x  add raw mode support
07aug13,j_x  written
*/

#ifndef __INCevdevLibKbdMaph
#define __INCevdevLibKbdMaph

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define EV_DEV_KBD_VALID_SCANCODE_LIMIT         (0x3)

/* key codes - used in Keycode mode */

#define EV_DEV_KBD_KEY_NULL                     0
#define EV_DEV_KBD_KEY_ESC                      1
#define EV_DEV_KBD_KEY_1                        2
#define EV_DEV_KBD_KEY_2                        3
#define EV_DEV_KBD_KEY_3                        4
#define EV_DEV_KBD_KEY_4                        5
#define EV_DEV_KBD_KEY_5                        6
#define EV_DEV_KBD_KEY_6                        7
#define EV_DEV_KBD_KEY_7                        8
#define EV_DEV_KBD_KEY_8                        9
#define EV_DEV_KBD_KEY_9                        10
#define EV_DEV_KBD_KEY_0                        11
#define EV_DEV_KBD_KEY_MINUS                    12
#define EV_DEV_KBD_KEY_EQUAL                    13
#define EV_DEV_KBD_KEY_BACKSPACE                14
#define EV_DEV_KBD_KEY_TAB                      15
#define EV_DEV_KBD_KEY_Q                        16
#define EV_DEV_KBD_KEY_W                        17
#define EV_DEV_KBD_KEY_E                        18
#define EV_DEV_KBD_KEY_R                        19
#define EV_DEV_KBD_KEY_T                        20
#define EV_DEV_KBD_KEY_Y                        21
#define EV_DEV_KBD_KEY_U                        22
#define EV_DEV_KBD_KEY_I                        23
#define EV_DEV_KBD_KEY_O                        24
#define EV_DEV_KBD_KEY_P                        25
#define EV_DEV_KBD_KEY_LEFTBRACE                26
#define EV_DEV_KBD_KEY_RIGHTBRACE               27
#define EV_DEV_KBD_KEY_ENTER                    28
#define EV_DEV_KBD_KEY_LEFTCTRL                 29
#define EV_DEV_KBD_KEY_A                        30
#define EV_DEV_KBD_KEY_S                        31
#define EV_DEV_KBD_KEY_D                        32
#define EV_DEV_KBD_KEY_F                        33
#define EV_DEV_KBD_KEY_G                        34
#define EV_DEV_KBD_KEY_H                        35
#define EV_DEV_KBD_KEY_J                        36
#define EV_DEV_KBD_KEY_K                        37
#define EV_DEV_KBD_KEY_L                        38
#define EV_DEV_KBD_KEY_SEMICOLON                39
#define EV_DEV_KBD_KEY_APOSTROPHE               40
#define EV_DEV_KBD_KEY_GRAVE                    41
#define EV_DEV_KBD_KEY_LEFTSHIFT                42
#define EV_DEV_KBD_KEY_BACKSLASH                43
#define EV_DEV_KBD_KEY_Z                        44
#define EV_DEV_KBD_KEY_X                        45
#define EV_DEV_KBD_KEY_C                        46
#define EV_DEV_KBD_KEY_V                        47
#define EV_DEV_KBD_KEY_B                        48
#define EV_DEV_KBD_KEY_N                        49
#define EV_DEV_KBD_KEY_M                        50
#define EV_DEV_KBD_KEY_COMMA                    51
#define EV_DEV_KBD_KEY_DOT                      52
#define EV_DEV_KBD_KEY_SLASH                    53
#define EV_DEV_KBD_KEY_RIGHTSHIFT               54
#define EV_DEV_KBD_KEY_KPASTERISK               55
#define EV_DEV_KBD_KEY_LEFTALT                  56
#define EV_DEV_KBD_KEY_SPACE                    57
#define EV_DEV_KBD_KEY_CAPSLOCK                 58
#define EV_DEV_KBD_KEY_F1                       59
#define EV_DEV_KBD_KEY_F2                       60
#define EV_DEV_KBD_KEY_F3                       61
#define EV_DEV_KBD_KEY_F4                       62
#define EV_DEV_KBD_KEY_F5                       63
#define EV_DEV_KBD_KEY_F6                       64
#define EV_DEV_KBD_KEY_F7                       65
#define EV_DEV_KBD_KEY_F8                       66
#define EV_DEV_KBD_KEY_F9                       67
#define EV_DEV_KBD_KEY_F10                      68
#define EV_DEV_KBD_KEY_NUMLOCK                  69
#define EV_DEV_KBD_KEY_SCROLLLOCK               70
#define EV_DEV_KBD_KEY_KP7                      71
#define EV_DEV_KBD_KEY_KP8                      72
#define EV_DEV_KBD_KEY_KP9                      73
#define EV_DEV_KBD_KEY_KPMINUS                  74
#define EV_DEV_KBD_KEY_KP4                      75
#define EV_DEV_KBD_KEY_KP5                      76
#define EV_DEV_KBD_KEY_KP6                      77
#define EV_DEV_KBD_KEY_KPPLUS                   78
#define EV_DEV_KBD_KEY_KP1                      79
#define EV_DEV_KBD_KEY_KP2                      80
#define EV_DEV_KBD_KEY_KP3                      81
#define EV_DEV_KBD_KEY_KP0                      82
#define EV_DEV_KBD_KEY_KPDOT                    83
#define EV_DEV_KBD_KEY_F11                      87
#define EV_DEV_KBD_KEY_F12                      88
#define EV_DEV_KBD_KEY_KPENTER                  96
#define EV_DEV_KBD_KEY_RIGHTCTRL                97
#define EV_DEV_KBD_KEY_KPSLASH                  98
#define EV_DEV_KBD_KEY_SYSRQ                    99
#define EV_DEV_KBD_KEY_RIGHTALT                 100
#define EV_DEV_KBD_KEY_HOME                     102
#define EV_DEV_KBD_KEY_UP                       103
#define EV_DEV_KBD_KEY_PAGEUP                   104
#define EV_DEV_KBD_KEY_LEFT                     105
#define EV_DEV_KBD_KEY_RIGHT                    106
#define EV_DEV_KBD_KEY_END                      107
#define EV_DEV_KBD_KEY_DOWN                     108
#define EV_DEV_KBD_KEY_PAGEDOWN                 109
#define EV_DEV_KBD_KEY_INSERT                   110
#define EV_DEV_KBD_KEY_DELETE                   111
#define EV_DEV_KBD_KEY_PAUSE                    119
#define EV_DEV_KBD_KEY_LEFTGUI                  125
#define EV_DEV_KBD_KEY_RIGHTGUI                 126
#define EV_DEV_KBD_KEY_MENU                     127

/* Unicode key codes - used in Unicode mode */

#define EV_DEV_KBD_SOH_KEY                      0x0001
#define EV_DEV_KBD_STX_KEY                      0x0002
#define EV_DEV_KBD_ETX_KEY                      0x0003
#define EV_DEV_KBD_EOT_KEY                      0x0004
#define EV_DEV_KBD_ENQ_KEY                      0x0005
#define EV_DEV_KBD_ACK_KEY                      0x0006
#define EV_DEV_KBD_BEL_KEY                      0x0007
#define EV_DEV_KBD_BS_KEY                       0x0008
#define EV_DEV_KBD_HT_KEY                       0x0009
#define EV_DEV_KBD_LF_KEY                       0x000A
#define EV_DEV_KBD_VT_KEY                       0x000B
#define EV_DEV_KBD_FF_KEY                       0x000C
#define EV_DEV_KBD_CR_KEY                       0x000D
#define EV_DEV_KBD_SO_KEY                       0x000E
#define EV_DEV_KBD_SI_KEY                       0x000F
#define EV_DEV_KBD_DLE_KEY                      0x0010
#define EV_DEV_KBD_DC1_KEY                      0x0011
#define EV_DEV_KBD_DC2_KEY                      0x0012
#define EV_DEV_KBD_DC3_KEY                      0x0013
#define EV_DEV_KBD_DC4_KEY                      0x0014
#define EV_DEV_KBD_NAK_KEY                      0x0015
#define EV_DEV_KBD_SYN_KEY                      0x0016
#define EV_DEV_KBD_ETB_KEY                      0x0017
#define EV_DEV_KBD_CAN_KEY                      0x0018
#define EV_DEV_KBD_EM_KEY                       0x0019
#define EV_DEV_KBD_SUB_KEY                      0x001A
#define EV_DEV_KBD_ESC_KEY                      0x001B
#define EV_DEV_KBD_FS_KEY                       0x001C
#define EV_DEV_KBD_GS_KEY                       0x001D
#define EV_DEV_KBD_RS_KEY                       0x001E
#define EV_DEV_KBD_US_KEY                       0x001F
#define EV_DEV_KBD_DEL_KEY                      0x007F

#define EV_DEV_KBD_VK                           0xE000

#define EV_DEV_KBD_HOME_KEY                     (EV_DEV_KBD_VK + 0)
#define EV_DEV_KBD_END_KEY                      (EV_DEV_KBD_VK + 1)
#define EV_DEV_KBD_INSERT_KEY                   (EV_DEV_KBD_VK + 2)
#define EV_DEV_KBD_PAGE_UP_KEY                  (EV_DEV_KBD_VK + 3)
#define EV_DEV_KBD_PAGE_DOWN_KEY                (EV_DEV_KBD_VK + 4)
#define EV_DEV_KBD_LEFT_ARROW_KEY               (EV_DEV_KBD_VK + 5)
#define EV_DEV_KBD_RIGHT_ARROW_KEY              (EV_DEV_KBD_VK + 6)
#define EV_DEV_KBD_UP_ARROW_KEY                 (EV_DEV_KBD_VK + 7)
#define EV_DEV_KBD_DOWN_ARROW_KEY               (EV_DEV_KBD_VK + 8)
#define EV_DEV_KBD_PRINT_SCREEN_KEY             (EV_DEV_KBD_VK + 9)
#define EV_DEV_KBD_PAUSE_KEY                    (EV_DEV_KBD_VK + 10)
#define EV_DEV_KBD_CAPS_KEY                     (EV_DEV_KBD_VK + 11)
#define EV_DEV_KBD_NUM_KEY                      (EV_DEV_KBD_VK + 12)
#define EV_DEV_KBD_SCR_KEY                      (EV_DEV_KBD_VK + 13)
#define EV_DEV_KBD_LEFT_SHIFT_KEY               (EV_DEV_KBD_VK + 14)
#define EV_DEV_KBD_RIGHT_SHIFT_KEY              (EV_DEV_KBD_VK + 15)
#define EV_DEV_KBD_LEFT_CTRL_KEY                (EV_DEV_KBD_VK + 16)
#define EV_DEV_KBD_RIGHT_CTRL_KEY               (EV_DEV_KBD_VK + 17)
#define EV_DEV_KBD_LEFT_ALT_KEY                 (EV_DEV_KBD_VK + 18)
#define EV_DEV_KBD_RIGHT_ALT_KEY                (EV_DEV_KBD_VK + 19)
#define EV_DEV_KBD_LEFT_GUI_KEY                 (EV_DEV_KBD_VK + 20)
#define EV_DEV_KBD_RIGHT_GUI_KEY                (EV_DEV_KBD_VK + 21)
#define EV_DEV_KBD_MENU_KEY                     (EV_DEV_KBD_VK + 22)

#define EV_DEV_KBD_FUNC_KEY                     (EV_DEV_KBD_VK + 0x0f00)

#define EV_DEV_KBD_F1_KEY                       (EV_DEV_KBD_FUNC_KEY + 1)
#define EV_DEV_KBD_F2_KEY                       (EV_DEV_KBD_FUNC_KEY + 2)
#define EV_DEV_KBD_F3_KEY                       (EV_DEV_KBD_FUNC_KEY + 3)
#define EV_DEV_KBD_F4_KEY                       (EV_DEV_KBD_FUNC_KEY + 4)
#define EV_DEV_KBD_F5_KEY                       (EV_DEV_KBD_FUNC_KEY + 5)
#define EV_DEV_KBD_F6_KEY                       (EV_DEV_KBD_FUNC_KEY + 6)
#define EV_DEV_KBD_F7_KEY                       (EV_DEV_KBD_FUNC_KEY + 7)
#define EV_DEV_KBD_F8_KEY                       (EV_DEV_KBD_FUNC_KEY + 8)
#define EV_DEV_KBD_F9_KEY                       (EV_DEV_KBD_FUNC_KEY + 9)
#define EV_DEV_KBD_F10_KEY                      (EV_DEV_KBD_FUNC_KEY + 10)
#define EV_DEV_KBD_F11_KEY                      (EV_DEV_KBD_FUNC_KEY + 11)
#define EV_DEV_KBD_F12_KEY                      (EV_DEV_KBD_FUNC_KEY + 12)

/* These are the different modifier keys possibles as of HID spec */

#define EV_DEV_KBD_NO_MODIFIER                  0x0000
#define EV_DEV_KBD_LEFT_CTRL                    0x0001
#define EV_DEV_KBD_LEFT_SHIFT                   0x0002
#define EV_DEV_KBD_LEFT_ALT                     0x0004
#define EV_DEV_KBD_LEFT_GUI                     0x0008
#define EV_DEV_KBD_RIGHT_CTRL                   0x0010
#define EV_DEV_KBD_RIGHT_SHIFT                  0x0020
#define EV_DEV_KBD_RIGHT_ALT                    0x0040
#define EV_DEV_KBD_RIGHT_GUI                    0x0080

#define EV_DEV_KBD_MAX_MODIFIERS_PER_MAP        2
#define EV_DEV_KBD_MAX_SCANCODES_PER_MAP        0x66

/* keyboard mapping */

#define EV_DEV_KBD_NUM_LOCK_SCANCODE            0x53
#define EV_DEV_KBD_CAPS_LOCK_SCANCODE           0x39
#define EV_DEV_KBD_SCR_LOCK_SCANCODE            0x47

#define EV_DEV_KBD_KEY_UNPRESS                  0
#define EV_DEV_KBD_KEY_PRESS                    1

/* typedefs */

typedef struct evdevKbdModifierMap
    {
    UINT16  pModifiers[EV_DEV_KBD_MAX_MODIFIERS_PER_MAP]; /* 0 terminated array */
    UINT16  pCharMap[EV_DEV_KBD_MAX_SCANCODES_PER_MAP];   /* Char array */
    } EV_DEV_KBD_MODIFIER_MAP;

typedef struct evdevKbdKeycodeMap
    {
    UINT16  pCharMap[EV_DEV_KBD_MAX_SCANCODES_PER_MAP];   /* Char array */
    } EV_DEV_KBD_KEYCODE_MAP;

typedef struct evdevKbdFilter
    {
    UINT8   filterScanCode;     /* 0 terminated array */
    UINT16  modifierOffMask;
    UINT8   modifierOffIndex;
    UINT16  filterKeyCode;
    } EV_DEV_KBD_FILTER;

typedef struct evdevKbdFilterMap
    {
    EV_DEV_KBD_FILTER   filter;
    UINT16              pCharMap[EV_DEV_KBD_MAX_SCANCODES_PER_MAP];  /* Char array */
    } EV_DEV_KBD_FILTER_MAP;

typedef struct evdevKbdMap
    {
    EV_DEV_KBD_MODIFIER_MAP *   pModifierMap;
    EV_DEV_KBD_FILTER_MAP *     pFilterMap;
    EV_DEV_KBD_KEYCODE_MAP *    pKeycodeMap;
    UINT32                      maxKeyMaps;
    UINT32                      maxFilters;
    UINT32                      maxKeycodeMaps;
    UINT16                      countryCode;
    } EV_DEV_KBD_MAP;

/* prototypes */

#ifdef _WRS_KERNEL
extern STATUS   evdevKbdMapInit (void);
extern STATUS   evdevKbdMapAdd (EV_DEV_KBD_MAP * pMap);
extern ssize_t  evdevKbdReport2Unicode (UINT16 countryCode, UINT8 *
                                        pFilterKeyMask, EV_DEV_KBD_REPORT *
                                        pReport, UINT16 * pUnicode);
extern ssize_t  evdevKbdReport2Keycode (UINT16 countryCode, EV_DEV_KBD_REPORT *
                                        pReport, UINT16 * pKeycode);
extern ssize_t  evdevKbdModifier2Unicode (UINT8 lastModifiers, UINT8
                                          newModifiers, UINT16 * pUnicode);
extern ssize_t  evdevKbdModifier2Keycode (UINT8 lastModifiers, UINT8
                                          newModifiers, UINT16 * pKeycode);
#endif /* _WRS_KERNEL */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevLibKbdMaph */
