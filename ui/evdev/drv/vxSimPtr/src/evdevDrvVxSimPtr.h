/* evdevDrvVxSimPtr.h - VxWorks Simulator Pointer Driver Header File */

/*
 * Copyright (c) 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
26aug13,y_f  created from udIoSimGraphicsDrv.h version 01b
*/

#ifndef __INCevdevDrvVxSimPtrh
#define __INCevdevDrvVxSimPtrh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define SIM_PTR_DRIVER_NAME         "vxSimPtr"

#define SIM_DLL_INT_RANGE_BASE      0x00f0  /* vxsim interrupts range base */
#define SIM_DLL_INT_RANGE_END       0x00ff  /* vxsim interrupts range end  */
#define SIM_DLL_GRAPHICS_INT_VEC    (SIM_DLL_INT_RANGE_BASE)
#define SIM_DLL_PTR_INT_BASE        (SIM_DLL_INT_RANGE_BASE + 10)

/* typedefs */

typedef struct vxSimDllPtrData
    {
    int             isAbsolute; /* TRUE == absolute */
    int             x;
    int             y;
    unsigned int    buttonState;/* pointer button states */
    } VX_SIM_DLL_PTR_DATA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvVxSimPtrh */
