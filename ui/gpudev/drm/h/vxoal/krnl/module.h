/* module.h - module functionality header file*/

/*
 * Copyright (c) 1999-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
15jul15,yat  Clean up vxoal (US60452)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux module operations.

NOMANUAL
*/

#ifndef _VXOAL_MODULES_H
#define _VXOAL_MODULES_H

#define MODULE_PARM_DESC(_parm, desc)
#define MODULE_AUTHOR(author)
#define MODULE_DESCRIPTION(descript)
#define MODULE_LICENSE(license)
#define MODULE_SUPPORTED_DEVICE(name)
#define module_init(init)
#define module_exit(exit)
#define module_param_array_named(name, param, type, num, prot)
#define module_param_named(name, param, type, prot)
#define module_param_named_unsafe(name, param, type, prot)
#define module_param_unsafe(param, type, prot)

#define KBUILD_MODNAME (0)
#define THIS_MODULE (0)

#define MODULE_DEVICE_TABLE(type, name)
#define MODULE_FIRMWARE(name)

#endif /* _VXOAL_MODULES_H */
