/* vghw.c - Wind River VG Hardware Query Functionality */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
13jul09,m_c  Written
*/

/*
DESCRIPTION
These routines provide hardware query functionality that support the OpenVG
implementation.
*/

/* includes */

#include <float.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/*******************************************************************************
 *
 *  vgHardwareQuery
 *
 */
VG_API_CALL VGHardwareQueryResult VG_API_ENTRY vgHardwareQuery
    (
    VGHardwareQueryType key,
    VGint setting
    ) VG_API_EXIT
    {
    VGHardwareQueryResult   result = 0;

    GET_GC();
    if (pGc != NULL)
        {
        switch (key)
            {
            case VG_IMAGE_FORMAT_QUERY:
                if (vgGetBitDepthWRS(setting) < 0)
                    VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                result = VG_HARDWARE_UNACCELERATED;
                break;

            case VG_PATH_DATATYPE_QUERY:
                if ((setting < VG_PATH_DATATYPE_S_8) || (setting > VG_PATH_DATATYPE_F))
                    VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                result = VG_HARDWARE_UNACCELERATED;
                break;

            default:
                VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                break;
            }
        }

zz: return (result);
    }
