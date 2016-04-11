/* configNet.h - Network configuration header file */

/*
 * Copyright (c) 2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,11apr11,j_z  initial creation based on itl_nehalem version 01q
*/

#ifndef INCconfigNeth
#define INCconfigNeth

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "end.h"

/* DEC 21x40 PCI (dc) driver defines */

#ifdef INCLUDE_DEC21X40_END

#define END_DC_LOAD_FUNC      sysDec21x40EndLoad
#define END_DC_BUFF_LOAN      TRUE
#define END_DC_LOAD_STRING    ""

IMPORT END_OBJ * END_DC_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_DEC21X40_END */


/* 3Com EtherLink III (elt) driver defines */

#ifdef INCLUDE_ELT_3C509_END

#define END_3C509_LOAD_FUNC   sysElt3c509EndLoad
#define END_3C509_BUFF_LOAN   TRUE
#define END_3C509_LOAD_STRING ""

IMPORT END_OBJ * END_3C509_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_ELT_3C509_END */


/* SMC Elite Ultra (ultra) driver definitions */

#ifdef INCLUDE_ULTRA_END

#define END_ULTRA_LOAD_FUNC   sysUltraEndLoad
#define END_ULTRA_BUFF_LOAN   TRUE
#define END_ULTRA_LOAD_STRING ""

IMPORT END_OBJ * END_ULTRA_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_ULTRA_END */


/* Ne2000 (ene) driver definitions */

#ifdef INCLUDE_ENE_END

#define END_ENE_LOAD_FUNC     sysNe2000EndLoad
#define END_ENE_BUFF_LOAN     TRUE
#define END_ENE_LOAD_STRING   ""

IMPORT END_OBJ * END_ENE_LOAD_FUNC (char *, void *);

#endif /* INCLUDE_ENE_END */

/* max number of END ipAttachments we can have */

#ifndef IP_MAX_UNITS
#   define IP_MAX_UNITS (NELEMENTS (endDevTbl) - 1)
#endif



/******************************************************************************
*
* END DEVICE TABLE
* ----------------
* Specifies END device instances that will be loaded to the MUX at startup.
*/

END_TBL_ENTRY endDevTbl [] =
    {

#ifdef INCLUDE_DEC21X40_END
    {0, END_DC_LOAD_FUNC, END_DC_LOAD_STRING, END_DC_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_DEC21X40_END */

#ifdef INCLUDE_ELT_3C509_END
    {0, END_3C509_LOAD_FUNC, END_3C509_LOAD_STRING, END_3C509_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_ELT_3C509_END */

#ifdef INCLUDE_ULTRA_END
    {0, END_ULTRA_LOAD_FUNC, END_ULTRA_LOAD_STRING, END_ULTRA_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_ULTRA_END */

#ifdef INCLUDE_ENE_END
    {0, END_ENE_LOAD_FUNC, END_ENE_LOAD_STRING, END_ENE_BUFF_LOAN,
    NULL, FALSE},
#endif /* INCLUDE_ENE_END */

/* Atheros AR521X WLAN Support */

#ifdef INCLUDE_AR521X_END
    {-1, END_TBL_END, NULL, 0, NULL, FALSE}, /* up to 4 Atheros NICs */
    {-1, END_TBL_END, NULL, 0, NULL, FALSE},
    {-1, END_TBL_END, NULL, 0, NULL, FALSE},
    {-1, END_TBL_END, NULL, 0, NULL, FALSE},
#endif /* INCLUDE_AR521X_END */

/* Broadcom 43XX WLAN Support */
    
#ifdef INCLUDE_BCM43XX_END
    {-1, END_TBL_END, NULL, 0, NULL, FALSE}, /* up to 4 Broadcom NICs */
    {-1, END_TBL_END, NULL, 0, NULL, FALSE},
    {-1, END_TBL_END, NULL, 0, NULL, FALSE},
    {-1, END_TBL_END, NULL, 0, NULL, FALSE},
#endif /* INCLUDE_BCM43XX_END */

    {0, END_TBL_END, NULL, 0, NULL, FALSE}
    };

#ifdef __cplusplus
}
#endif

#endif /* INCconfigNeth */

