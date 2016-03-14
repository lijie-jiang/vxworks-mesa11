/* evdevLibKbdMapUS.c - US keyboard mapping */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
24jun14,y_f  add evdevLib.h (US41403)
15oct13,y_f  add map init function
07aug13,j_x  written
*/

/*
DESCRIPTION
This module provides US keyboard mapping for the keyboard driver.
*/

/* includes */

#include <evdevLib.h>
#include <evdevLibKbdMap.h>

/* defines */

#define SOH     EV_DEV_KBD_SOH_KEY
#define STX     EV_DEV_KBD_STX_KEY
#define ETX     EV_DEV_KBD_ETX_KEY
#define EOT     EV_DEV_KBD_EOT_KEY
#define ENQ     EV_DEV_KBD_ENQ_KEY
#define ACK     EV_DEV_KBD_ACK_KEY
#define BEL     EV_DEV_KBD_BEL_KEY
#define BS      EV_DEV_KBD_BS_KEY
#define HT      EV_DEV_KBD_HT_KEY
#define LF      EV_DEV_KBD_LF_KEY
#define VT      EV_DEV_KBD_VT_KEY
#define FF      EV_DEV_KBD_FF_KEY
#define CR      EV_DEV_KBD_CR_KEY
#define SO      EV_DEV_KBD_SO_KEY
#define SI      EV_DEV_KBD_SI_KEY
#define DLE     EV_DEV_KBD_DLE_KEY
#define DC1     EV_DEV_KBD_DC1_KEY
#define DC2     EV_DEV_KBD_DC2_KEY
#define DC3     EV_DEV_KBD_DC3_KEY
#define DC4     EV_DEV_KBD_DC4_KEY
#define NAK     EV_DEV_KBD_NAK_KEY
#define SYN     EV_DEV_KBD_SYN_KEY
#define ETB     EV_DEV_KBD_ETB_KEY
#define CAN     EV_DEV_KBD_CAN_KEY
#define EM      EV_DEV_KBD_EM_KEY
#define SUB     EV_DEV_KBD_SUB_KEY
#define ESC     EV_DEV_KBD_ESC_KEY
#define FS      EV_DEV_KBD_FS_KEY
#define GS      EV_DEV_KBD_GS_KEY
#define RS      EV_DEV_KBD_RS_KEY
#define US      EV_DEV_KBD_US_KEY
#define DEL     EV_DEV_KBD_DEL_KEY
#define HOME    EV_DEV_KBD_HOME_KEY
#define END     EV_DEV_KBD_END_KEY
#define INSERT  EV_DEV_KBD_INSERT_KEY
#define PU      EV_DEV_KBD_PAGE_UP_KEY
#define PD      EV_DEV_KBD_PAGE_DOWN_KEY
#define LA      EV_DEV_KBD_LEFT_ARROW_KEY
#define RA      EV_DEV_KBD_RIGHT_ARROW_KEY
#define UA      EV_DEV_KBD_UP_ARROW_KEY
#define DA      EV_DEV_KBD_DOWN_ARROW_KEY
#define PS      EV_DEV_KBD_PRINT_SCREEN_KEY
#define PAUSE   EV_DEV_KBD_PAUSE_KEY
#define CAPS    EV_DEV_KBD_CAPS_KEY
#define NUM     EV_DEV_KBD_NUM_KEY
#define SCR     EV_DEV_KBD_SCR_KEY
#define F1      EV_DEV_KBD_F1_KEY
#define F2      EV_DEV_KBD_F2_KEY
#define F3      EV_DEV_KBD_F3_KEY
#define F4      EV_DEV_KBD_F4_KEY
#define F5      EV_DEV_KBD_F5_KEY
#define F6      EV_DEV_KBD_F6_KEY
#define F7      EV_DEV_KBD_F7_KEY
#define F8      EV_DEV_KBD_F8_KEY
#define F9      EV_DEV_KBD_F9_KEY
#define F10     EV_DEV_KBD_F10_KEY
#define F11     EV_DEV_KBD_F11_KEY
#define F12     EV_DEV_KBD_F12_KEY
#define MENU    EV_DEV_KBD_MENU_KEY

/* local */

/*
 * This is the table for normal keys with modifiers,
 * there may be max. 2 modifiers valid for each table.
 * this table may be extended if ALT, CTRL combinations,
 * or multiple combintions of several modifiers are
 * necessary (like ctrl-alt...)
 */

LOCAL EV_DEV_KBD_MODIFIER_MAP   evdevUSKeyMapTable[] =
/*              0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F   */
    {
        { { EV_DEV_KBD_NO_MODIFIER , EV_DEV_KBD_NO_MODIFIER },

/* 0x0_ */  {   0,      0,      0,      0,      'a',    'b',    'c',    'd',    'e',    'f',    'g',    'h',    'i',    'j',    'k',    'l',
/* 0x1_ */      'm',    'n',    'o',    'p',    'q',    'r',    's',    't',    'u',    'v',    'w',    'x',    'y',    'z',    '1',    '2',
/* 0x2_ */      '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    CR,     ESC,    BS,     HT,     ' ',    '-',    '=',    '[',
/* 0x3_ */      ']',    '\\',   0,      ';',    '\'',   '`',    ',',    '.',    '/',    CAPS,   F1,     F2,     F3,     F4,     F5,     F6,
/* 0x4_ */      F7,     F8,     F9,     F10,    F11,    F12,    PS,     SCR,    PAUSE,  INSERT, HOME,   PU,     DEL,    END,    PD,     RA,
/* 0x5_ */      LA,     DA,     UA,     NUM,    '/',    '*',    '-',    '+',    CR,     END,    DA,     PD,     LA,     0,      RA,     HOME,
/* 0x6_ */      UA,     PU,     INSERT, DEL,    0,      MENU
            }
        }
        ,
        { { EV_DEV_KBD_LEFT_SHIFT,  EV_DEV_KBD_RIGHT_SHIFT },

/* 0x0_ */  {   0,      0,      0,      0,      'A',    'B',    'C',    'D',    'E',    'F',    'G',    'H',    'I',    'J',    'K',    'L',
/* 0x1_ */      'M',    'N',    'O',    'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',    'X',    'Y',    'Z',    '!',    '@',
/* 0x2_ */      '#',    '$',    '%',    '^',    '&',    '*',    '(',     ')',   CR,     ESC,    BS,     HT,     ' ',    '_',    '+',    '{',
/* 0x3_ */      '}',    '|',    '~',    ':',    '\"',   '~',    '<',    '>',    '?',    CAPS,   F1,     F2,     F3,     F4,     F5,     F6,
/* 0x4_ */      F7,     F8,     F9,     F10,    F11,    F12,    PS,     SCR,    PAUSE,  INSERT, HOME,   PU,     DEL,    END,    PD,     RA,
/* 0x5_ */      LA,     DA,     UA,     NUM,    '/',    '*',    '-',    '+',    CR,     END,    DA,     PD,     LA,     0,      RA,     HOME,
/* 0x6_ */      UA,     PU,     INSERT, DEL,    0,      MENU
            }
        }
        ,
        { { EV_DEV_KBD_LEFT_CTRL,  EV_DEV_KBD_RIGHT_CTRL },

/* 0x0_ */  {   0,      0,      0,      0,      SOH,    STX,    ETX,    EOT,    ENQ,    ACK,    BEL,    BS,     HT,     LF,     VT,     FF,
/* 0x1_ */      CR,     SO,     SI,     DLE,    DC1,    DC2,    DC3,    DC4,    NAK,    SYN,    ETB,    CAN,    EM,     SUB,    0,      0,
/* 0x2_ */      0,      0,      0,      0,      0,      0,      0,      0,      CR,     ESC,    0,      HT,     0,      0,      0,      ESC,
/* 0x3_ */      GS,     RS,     FS,     US,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x4_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x5_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x6_ */      0,      0,      0,      0,      0,      0
            }
        }
        ,
        { { EV_DEV_KBD_LEFT_CTRL | EV_DEV_KBD_LEFT_SHIFT, EV_DEV_KBD_RIGHT_CTRL | EV_DEV_KBD_LEFT_SHIFT },

/* 0x0_ */  {   0,      0,      0,      0,      SOH,    STX,    ETX,    EOT,    ENQ,    ACK,    BEL,    BS,     HT,     LF,     VT,     FF,
/* 0x1_ */      CR,     SO,     SI,     DLE,    DC1,    DC2,    DC3,    DC4,    NAK,    SYN,    ETB,    CAN,    EM,     SUB,    0,      0,
/* 0x2_ */      0,      0,      0,      0,      0,      0,      0,      0,      CR,     ESC,    0,      HT,     0,      0,      0,      ESC,
/* 0x3_ */      GS,     RS,     FS,     US,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x4_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x5_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x6_ */      0,      0,      0,      0,      0,      0
            }
        }
        ,
        { { EV_DEV_KBD_LEFT_CTRL | EV_DEV_KBD_RIGHT_SHIFT, EV_DEV_KBD_RIGHT_CTRL | EV_DEV_KBD_RIGHT_SHIFT },

/* 0x0_ */  {   0,      0,      0,      0,      SOH,    STX,    ETX,    EOT,    ENQ,    ACK,    BEL,    BS,     HT,     LF,     VT,     FF,
/* 0x1_ */      CR,     SO,     SI,     DLE,    DC1,    DC2,    DC3,    DC4,    NAK,    SYN,    ETB,    CAN,    EM,     SUB,    0,      0,
/* 0x2_ */      0,      0,      0,      0,      0,      0,      0,      0,      CR,     ESC,    0,      HT,     0,      0,      0,      ESC,
/* 0x3_ */      GS,     RS,     FS,     US,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x4_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x5_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x6_ */      0,      0,      0,      0,      0,      0
            }
        }
    };

/*
 * This is the table for normal filtered keys
 * (like for example num-lock / caps-lock).
 * It only contains the key which are changing
 * from the std. array.
 * It is selected by the filter key scan code.
 */

LOCAL EV_DEV_KBD_FILTER_MAP evdevUSFilteredKeyMapTable[] =
    {

        { { EV_DEV_KBD_NUM_LOCK_SCANCODE,  0, 0, EV_DEV_KBD_NUM_KEY },

/* 0x0_ */  {   0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x1_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x2_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x3_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x4_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x5_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      '1',    '2',    '3',    '4',    '5',    '6',    '7',
/* 0x6_ */      '8',    '9',    '0',    '.',    0,      0
            }
        },

        { { EV_DEV_KBD_CAPS_LOCK_SCANCODE,  EV_DEV_KBD_LEFT_SHIFT |  EV_DEV_KBD_RIGHT_SHIFT, 0 ,  EV_DEV_KBD_CAPS_KEY },

/* 0x0_ */  {   0,      0,      0,      0,      'A',    'B',    'C',    'D',    'E',    'F',   'G',     'H',    'I',    'J',    'K',    'L',
/* 0x1_ */      'M',    'N',    'O',    'P',    'Q',    'R',    'S',    'T',    'U',    'V',   'W',     'X',    'Y',    'Z',    0,      0,
/* 0x3_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x4_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x5_ */      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
/* 0x6_ */      0,      0,      0,      0,      0,      0
            }
        }
    };

/*
 * This is the table for keycode mapping.
 * It is used in keycode mode.
 */

LOCAL EV_DEV_KBD_KEYCODE_MAP evdevUSKeycodeMapTable[] =
    {
        {
            {
/* 0x0_ */  0,
            0,
            0,
            0,
            EV_DEV_KBD_KEY_A,
            EV_DEV_KBD_KEY_B,
            EV_DEV_KBD_KEY_C,
            EV_DEV_KBD_KEY_D,
            EV_DEV_KBD_KEY_E,
            EV_DEV_KBD_KEY_F,
            EV_DEV_KBD_KEY_G,
            EV_DEV_KBD_KEY_H,
            EV_DEV_KBD_KEY_I,
            EV_DEV_KBD_KEY_J,
            EV_DEV_KBD_KEY_K,
            EV_DEV_KBD_KEY_L,
/* 0x1_ */  EV_DEV_KBD_KEY_M,
            EV_DEV_KBD_KEY_N,
            EV_DEV_KBD_KEY_O,
            EV_DEV_KBD_KEY_P,
            EV_DEV_KBD_KEY_Q,
            EV_DEV_KBD_KEY_R,
            EV_DEV_KBD_KEY_S,
            EV_DEV_KBD_KEY_T,
            EV_DEV_KBD_KEY_U,
            EV_DEV_KBD_KEY_V,
            EV_DEV_KBD_KEY_W,
            EV_DEV_KBD_KEY_X,
            EV_DEV_KBD_KEY_Y,
            EV_DEV_KBD_KEY_Z,
            EV_DEV_KBD_KEY_1,
            EV_DEV_KBD_KEY_2,
/* 0x2_ */  EV_DEV_KBD_KEY_3,
            EV_DEV_KBD_KEY_4,
            EV_DEV_KBD_KEY_5,
            EV_DEV_KBD_KEY_6,
            EV_DEV_KBD_KEY_7,
            EV_DEV_KBD_KEY_8,
            EV_DEV_KBD_KEY_9,
            EV_DEV_KBD_KEY_0,
            EV_DEV_KBD_KEY_ENTER,
            EV_DEV_KBD_KEY_ESC,
            EV_DEV_KBD_KEY_BACKSPACE,
            EV_DEV_KBD_KEY_TAB,
            EV_DEV_KBD_KEY_SPACE,
            EV_DEV_KBD_KEY_MINUS,
            EV_DEV_KBD_KEY_EQUAL,
            EV_DEV_KBD_KEY_LEFTBRACE,
/* 0x3_ */  EV_DEV_KBD_KEY_RIGHTBRACE,
            EV_DEV_KBD_KEY_BACKSLASH,
            0,
            EV_DEV_KBD_KEY_SEMICOLON,
            EV_DEV_KBD_KEY_APOSTROPHE,
            EV_DEV_KBD_KEY_GRAVE,
            EV_DEV_KBD_KEY_COMMA,
            EV_DEV_KBD_KEY_DOT,
            EV_DEV_KBD_KEY_SLASH,
            EV_DEV_KBD_KEY_CAPSLOCK,
            EV_DEV_KBD_KEY_F1,
            EV_DEV_KBD_KEY_F2,
            EV_DEV_KBD_KEY_F3,
            EV_DEV_KBD_KEY_F4,
            EV_DEV_KBD_KEY_F5,
            EV_DEV_KBD_KEY_F6,
/* 0x4_ */  EV_DEV_KBD_KEY_F7,
            EV_DEV_KBD_KEY_F8,
            EV_DEV_KBD_KEY_F9,
            EV_DEV_KBD_KEY_F10,
            EV_DEV_KBD_KEY_F11,
            EV_DEV_KBD_KEY_F12,
            EV_DEV_KBD_KEY_SYSRQ,
            EV_DEV_KBD_KEY_SCROLLLOCK,
            EV_DEV_KBD_KEY_PAUSE,
            EV_DEV_KBD_KEY_INSERT,
            EV_DEV_KBD_KEY_HOME,
            EV_DEV_KBD_KEY_PAGEUP,
            EV_DEV_KBD_KEY_DELETE,
            EV_DEV_KBD_KEY_END,
            EV_DEV_KBD_KEY_PAGEDOWN,
            EV_DEV_KBD_KEY_RIGHT,
/* 0x5_ */  EV_DEV_KBD_KEY_LEFT,
            EV_DEV_KBD_KEY_DOWN,
            EV_DEV_KBD_KEY_UP,
            EV_DEV_KBD_KEY_NUMLOCK,
            EV_DEV_KBD_KEY_KPSLASH,
            EV_DEV_KBD_KEY_KPASTERISK,
            EV_DEV_KBD_KEY_KPMINUS,
            EV_DEV_KBD_KEY_KPPLUS,
            EV_DEV_KBD_KEY_KPENTER,
            EV_DEV_KBD_KEY_KP1,
            EV_DEV_KBD_KEY_KP2,
            EV_DEV_KBD_KEY_KP3,
            EV_DEV_KBD_KEY_KP4,
            EV_DEV_KBD_KEY_KP5,
            EV_DEV_KBD_KEY_KP6,
            EV_DEV_KBD_KEY_KP7,
/* 0x6_ */  EV_DEV_KBD_KEY_KP8,
            EV_DEV_KBD_KEY_KP9,
            EV_DEV_KBD_KEY_KP0,
            EV_DEV_KBD_KEY_KPDOT,
            0,
            EV_DEV_KBD_KEY_MENU
            }
        },
    };

LOCAL EV_DEV_KBD_MAP    evdevKbdMapUS =
    {
    evdevUSKeyMapTable,                 /* Map table */
    evdevUSFilteredKeyMapTable,         /* filter keys */
    evdevUSKeycodeMapTable,             /* keycode map */
    5,                                  /* Number of Mappings */
    2,                                  /* Number of filter keys */
    1,                                  /* Number of keycode maps */
    EV_DEV_KBD_LANG_ENGLISH_US,         /* Language identifier */
    };

/* functions */

/*******************************************************************************
*
* evdevKbdMapUSInit - register the US keyboard mapping to the keyboard library
*
* This routine registers the US keyboard mapping to the keyboard library.
*
* RETURNS: OK, or ERROR if the mapping cannot be registered
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevKbdMapUSInit (void)
    {
    return evdevKbdMapAdd (&evdevKbdMapUS);
    }
