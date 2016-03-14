/* evdevLibKbdMap.c - keyboard translation module */

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
24jun14,y_f  add evdevLib.h (US41403)
15oct13,y_f  code clean
07aug13,j_x  written
*/

/*
DESCRIPTION
This module provides some translation routines for the keyboard driver.
*/

/* includes */

#include <evdevLib.h>
#include <evdevLibKbdMap.h>

/* typedef */

typedef struct evdevKbdMapNode
    {
    NODE                listNode;
    EV_DEV_KBD_MAP *    pMap;
    } EV_DEV_KBD_MAP_NODE;

/* forward declarations */

LOCAL EV_DEV_KBD_MAP *  evdevKbdCntryCodeToMap (UINT16 countryCode);
LOCAL UINT16            evdevKbdScan2Unicode (EV_DEV_KBD_MAP * pMap, UINT8 *
                                              pFilterKeyMask, UINT8 scanCode,
                                              UINT8 modifiers);

/* local */

/* List of language mappings for transitions */

LOCAL LIST *  pEvDevKbdMapList = NULL;

/* functions */

/*******************************************************************************
*
* evdevKbdMapInit - initialize the keyboard mapping handler
*
* This routine initializes the keyboard mapping handler.
*
* RETURNS: OK if it is initialized, ERROR if it does not work.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevKbdMapInit (void)
    {
    /* If this list has already be created, just ignore & return */

    if (NULL != pEvDevKbdMapList)
        {
        return OK;
        }

    /* Create the list structure  */

    pEvDevKbdMapList = (LIST *)calloc (1, sizeof (LIST));
    if (NULL == pEvDevKbdMapList)
        {
        return ERROR;
        }

    /* and initialize it */

    lstInit (pEvDevKbdMapList);

    return OK;
    }

/*******************************************************************************
*
* evdevKbdMapAdd - add an Unicode mapper to the keyboard handler
*
* This routine adds an Unicode mapper to the keyboard handler.
*
* RETURNS: OK if it had been added, ERROR if it did not work.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevKbdMapAdd
    (
    EV_DEV_KBD_MAP *    pMap    /* Keyboard mapping to add to the list */
    )
    {
    EV_DEV_KBD_MAP_NODE *   pMapNode = NULL;

    /* This below is an error condition */

    if ((NULL == pEvDevKbdMapList) || (NULL == pMap))
        {
        return ERROR;
        }

    pMapNode = (EV_DEV_KBD_MAP_NODE *)calloc (1, sizeof (EV_DEV_KBD_MAP_NODE));
    if (NULL == pMapNode)
        {
        return ERROR;
        }

    /* Add the map to the Node */

    pMapNode->pMap = pMap;

    /* Add the map passed as parameter */

    lstAdd (pEvDevKbdMapList, (NODE *)pMapNode);

    return OK;
    }

/*******************************************************************************
*
* evdevKbdReport2Unicode - transform a set of scan codes in a report to Unicode
*
* This routine transforms a series of scan codes to Unicode. It gets a
* 'pReport' as input, and replaces the scan codes in the report with Unicode.
*
* RETURNS: the number of chars converted, 0 if nothing has been converted.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

ssize_t evdevKbdReport2Unicode
    (
    UINT16              countryCode,
    UINT8 *             pFilterKeyMask,
    EV_DEV_KBD_REPORT * pReport,    /* Report used, scancodes transformed */
    UINT16 *            pUnicode    /* Array of Unicode keys for the report */
    )
    {
    ssize_t             i = 0; /* counter through the scan codes */
    EV_DEV_KBD_MAP *    pMap;

    /* Check if the map supplied is not valid */

    if ((NULL == pFilterKeyMask) || (NULL == pReport) || (NULL == pUnicode))
        {
        return 0;
        }

    pMap = evdevKbdCntryCodeToMap (countryCode);
    if (NULL == pMap)
        {
        return 0;
        }

    /* We have maximum EV_DEV_BOOT_KEY_COUNT key presses per report */

    /* For each of the scan codes we try to convert it */

    for (i = 0; i < EV_DEV_BOOT_KEY_COUNT; i++)
        {
        if (pReport->scanCodes[i] <= EV_DEV_KBD_VALID_SCANCODE_LIMIT)
            {
            break;
            }

        pUnicode[i] = evdevKbdScan2Unicode (pMap, pFilterKeyMask,
                                            pReport->scanCodes[i],
                                            pReport->modifiers);
        }

    return i;
    }

/*******************************************************************************
*
* evdevKbdModifier2Unicode - transform modifiers to Unicode
*
* This routine transforms modifiers to Unicode.
*
* RETURNS: the number of chars converted, 0 if nothing has been converted.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

ssize_t evdevKbdModifier2Unicode
    (
    UINT8       lastModifiers,
    UINT8       newModifiers,
    UINT16 *    pUnicode    /* Array of Unicode keys for the report */
    )
    {
    ssize_t i = 0; /* counter through the scan codes */
    UINT8   modifiers;

    if (NULL == pUnicode)
        {
        return 0;
        }

    modifiers = (lastModifiers & newModifiers) ^ newModifiers;

    /* We have maximum EV_DEV_BOOT_KEY_COUNT key presses per report */

    /* For each of the modifier we try to convert it */

    while ((0 != modifiers) && (i < EV_DEV_BOOT_KEY_COUNT))
        {
        if (0 != (modifiers & EV_DEV_KBD_LEFT_CTRL))
            {
            modifiers ^= EV_DEV_KBD_LEFT_CTRL;
            pUnicode[i] = EV_DEV_KBD_LEFT_CTRL_KEY;
            }
        else if (0 != (modifiers & EV_DEV_KBD_LEFT_SHIFT))
            {
            modifiers ^= EV_DEV_KBD_LEFT_SHIFT;
            pUnicode[i] = EV_DEV_KBD_LEFT_SHIFT_KEY;
            }
        else if (0 != (modifiers & EV_DEV_KBD_LEFT_ALT))
            {
            modifiers ^= EV_DEV_KBD_LEFT_ALT;
            pUnicode[i] = EV_DEV_KBD_LEFT_ALT_KEY;
            }
        else if (0 != (modifiers & EV_DEV_KBD_LEFT_GUI))
            {
            modifiers ^= EV_DEV_KBD_LEFT_GUI;
            pUnicode[i] = EV_DEV_KBD_LEFT_GUI_KEY;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_CTRL))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_CTRL;
            pUnicode[i] = EV_DEV_KBD_RIGHT_CTRL_KEY;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_SHIFT))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_SHIFT;
            pUnicode[i] = EV_DEV_KBD_RIGHT_SHIFT_KEY;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_ALT))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_ALT;
            pUnicode[i] = EV_DEV_KBD_RIGHT_ALT_KEY;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_GUI))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_GUI;
            pUnicode[i] = EV_DEV_KBD_RIGHT_GUI_KEY;
            }
        else
            {
            modifiers = 0;
            }

        i++;
        }

    return i;
    }

/*******************************************************************************
*
* evdevKbdReport2Keycode - transform a set of scan codes in a report to keycode
*
* This routine transforms a series of scan codes to keycode. It gets a
* 'pReport' as input, and replaces the scan codes in the report with keycode.
*
* RETURNS: the number of chars converted, 0 if nothing has been converted.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

ssize_t evdevKbdReport2Keycode
    (
    UINT16              countryCode,
    EV_DEV_KBD_REPORT * pReport,    /* Report used, scancodes transformed */
    UINT16 *            pKeycode    /* Array of Unicode keys for the report */
    )
    {
    ssize_t             i = 0; /* counter through the scan codes */
    EV_DEV_KBD_MAP *    pMap;

    /* Check if the map supplied is not valid */

    if ((NULL == pReport) || (NULL == pKeycode))
        {
        return 0;
        }

    pMap = evdevKbdCntryCodeToMap (countryCode);
    if (NULL == pMap)
        {
        return 0;
        }

    if ((NULL == pMap->pKeycodeMap) || (0 == pMap->maxKeycodeMaps))
        {
        return 0;
        }

    /* We have maximum EV_DEV_BOOT_KEY_COUNT key presses per report */

    /* For each of the scan codes we try to convert it */

    for (i = 0; i < EV_DEV_BOOT_KEY_COUNT; i++)
        {
        if (pReport->scanCodes[i] <= EV_DEV_KBD_VALID_SCANCODE_LIMIT)
            {
            break;
            }

        pKeycode[i] = pMap->pKeycodeMap[0].pCharMap[pReport->scanCodes[i]];
        }

    return i;
    }

/*******************************************************************************
*
* evdevKbdModifier2Keycode - transform modifiers to keycode
*
* This routine transforms modifiers to keycode.
*
* RETURNS: the number of chars converted, 0 if nothing has been converted.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

ssize_t evdevKbdModifier2Keycode
    (
    UINT8       lastModifiers,
    UINT8       newModifiers,
    UINT16 *    pKeycode    /* Array of Unicode keys for the report */
    )
    {
    ssize_t i = 0; /* counter through the scan codes */
    UINT8   modifiers;

    if (NULL == pKeycode)
        {
        return 0;
        }

    modifiers = (lastModifiers & newModifiers) ^ newModifiers;

    /* We have maximum EV_DEV_BOOT_KEY_COUNT key presses per report */

    /* For each of the modifier we try to convert it */

    while ((0 != modifiers) && (i < EV_DEV_BOOT_KEY_COUNT))
        {
        if (0 != (modifiers & EV_DEV_KBD_LEFT_CTRL))
            {
            modifiers ^= EV_DEV_KBD_LEFT_CTRL;
            pKeycode[i] = EV_DEV_KBD_KEY_LEFTCTRL;
            }
        else if (0 != (modifiers & EV_DEV_KBD_LEFT_SHIFT))
            {
            modifiers ^= EV_DEV_KBD_LEFT_SHIFT;
            pKeycode[i] = EV_DEV_KBD_KEY_LEFTSHIFT;
            }
        else if (0 != (modifiers & EV_DEV_KBD_LEFT_ALT))
            {
            modifiers ^= EV_DEV_KBD_LEFT_ALT;
            pKeycode[i] = EV_DEV_KBD_KEY_LEFTALT;
            }
        else if (0 != (modifiers & EV_DEV_KBD_LEFT_GUI))
            {
            modifiers ^= EV_DEV_KBD_LEFT_GUI;
            pKeycode[i] = EV_DEV_KBD_KEY_LEFTGUI;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_CTRL))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_CTRL;
            pKeycode[i] = EV_DEV_KBD_KEY_RIGHTCTRL;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_SHIFT))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_SHIFT;
            pKeycode[i] = EV_DEV_KBD_KEY_RIGHTSHIFT;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_ALT))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_ALT;
            pKeycode[i] = EV_DEV_KBD_KEY_RIGHTALT;
            }
        else if (0 != (modifiers & EV_DEV_KBD_RIGHT_GUI))
            {
            modifiers ^= EV_DEV_KBD_RIGHT_GUI;
            pKeycode[i] = EV_DEV_KBD_KEY_RIGHTGUI;
            }
        else
            {
            modifiers = 0;
            }

        i++;
        }

    return i;
    }

/*******************************************************************************
*
* evdevKbdCntryCodeToMap - get the keyboard map corresponding to a country code
*
* This routine is used by the keyboard driver to get the keyboard mapping
* corresponding to the country code 

* RETURNS: pointer the keyboard map in EV_DEV_KBD_MAP, or NULL.
*
* ERRNO: N/A
*/

LOCAL EV_DEV_KBD_MAP * evdevKbdCntryCodeToMap
    (
    UINT16  countryCode             /* language Id to get the Map */
    )
    {
    EV_DEV_KBD_MAP_NODE *   pMapNode = NULL;

    /* This below is an error condition */

    if (NULL == pEvDevKbdMapList)
        {
        return NULL;
        }

    /* Add the map passed as parameter */

    pMapNode = (EV_DEV_KBD_MAP_NODE *) lstFirst (pEvDevKbdMapList);

    while ((NULL != pMapNode) &&
           (NULL != pMapNode->pMap) &&
           (pMapNode->pMap->countryCode != countryCode))
        {
        pMapNode = (EV_DEV_KBD_MAP_NODE *) lstNext (&pMapNode->listNode);
        }

    /* This can be NULL (if nothing found) or the real map */

    if(NULL != pMapNode)
        {
        return pMapNode->pMap;
        }
    else
        {
        return NULL;
        }
    }

/*******************************************************************************
*
* evdevKbdScan2Unicode - transform a scan code into Unicode based on the map
*
* This routine transforms a scan code into Unicode. It gets a scan code as
* input, returns the converted Unicode.
*
* RETURNS: the Unicode code converted from the scan code, or 0.
*
* ERRNO: N/A
*/

LOCAL UINT16 evdevKbdScan2Unicode
    (
    EV_DEV_KBD_MAP *    pMap,
    UINT8 *             pFilterKeyMask,
    UINT8               scanCode,      /* scancode to transform */
    UINT8               modifiers      /* modifiers used        */
    )
    {
    int     i                   = 0;
    int     j                   = 0;
    UINT16  keyCode             = 0;
    UINT8   filterKeyMask       = 0;
    UINT8   modifierOffIndex    = 0;

    /* Is this scan code a valid scan code ? */

    if (scanCode >= EV_DEV_KBD_MAX_SCANCODES_PER_MAP)
        {
        return 0;
        }

    /*
     * Check if the key is a filter key. If it is a filter key such as
     * caps-lock or num-lock, then it has a consequence on all following
     * key presses; So remember the filter, and return the associated
     * keycode
     */

    for (i = 0; i < (int)pMap->maxFilters; i++)
        {
        if (scanCode == pMap->pFilterMap[i].filter.filterScanCode)
            {
            *pFilterKeyMask = (UINT8)(*pFilterKeyMask ^ (i + 1));
            return pMap->pFilterMap[i].filter.filterKeyCode;
            }
        }

    /*
     * OK the key pressed is not a filter key.
     * However, if it has been pressed earlier,
     * we have to act and ignore the associated key
     */

    filterKeyMask = *pFilterKeyMask;

    for (i = 0; (filterKeyMask != 0) && (i < (int)pMap->maxKeyMaps); i++)
        {
        if ((filterKeyMask & 1) &&
            (pMap->pFilterMap[i].pCharMap[scanCode]))
            {
            if (!((UINT16)modifiers &
                   pMap->pFilterMap[i].filter.modifierOffMask))
                {
                return pMap->pFilterMap[i].pCharMap[scanCode];
                }
            else
                {
                modifierOffIndex = pMap->pFilterMap[i].filter.modifierOffIndex;
                return pMap->pModifierMap[modifierOffIndex].pCharMap[scanCode];
                }
            }

        filterKeyMask = filterKeyMask >> 1;
        }

    for (i = 0; i < (int)pMap->maxKeyMaps; i++)
        {
        for (j = 0; j < EV_DEV_KBD_MAX_MODIFIERS_PER_MAP; j++)
            {
            if ((UINT16)modifiers == pMap->pModifierMap[i].pModifiers[j])
                {
                keyCode = pMap->pModifierMap[i].pCharMap[scanCode];
                return keyCode;
                }
            }
        }

    return 0;
    }
