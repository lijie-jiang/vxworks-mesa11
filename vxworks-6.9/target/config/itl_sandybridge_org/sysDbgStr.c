/* sysDbgStr.c - Debug Store mechanism BSP library */

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
01a,11apr11,j_z  initial creation based on itl_nehalem version 01a
*/

/*
DESCRIPTION
This library contains routines to manipulate the Debug Store mechanism.

SEE ALSO: src/arch/i86/dbgStrLib.c
*/

#include "vxWorks.h"
#include "sysLib.h"
#include "arch/i86/dbgStrLib.h"


/* externals */

IMPORT DS_CONFIG dbgStrCfg;		/* system DS config */
IMPORT DS_CONFIG * dbgStrCurrent;	/* current DS config */


/* globals */

UINT32	sysPmcIntCnt		= 0;	/* PMC int count */
UINT32	sysBtsIntCnt		= 0;	/* BTS int count */
UINT32	sysPebsIntCnt		= 0;	/* PEBS int count */


/* locals */

LOCAL FUNCPTR sysBtsRoutine	= NULL;	/* rtn to call on BTS interrupt */
LOCAL INT32   sysBtsArg		= 0;	/* its argument */
LOCAL BOOL    sysBtsConnected	= FALSE;
LOCAL FUNCPTR sysPebsRoutine	= NULL;	/* rtn to call on PEBS interrupt */
LOCAL INT32   sysPebsArg	= 0;	/* its argument */
LOCAL BOOL    sysPebsConnected	= FALSE;


/* forward declarations */

void sysPmcInt (void);		/* BTS/PEBS/PMC interrupt handler */


/*******************************************************************************
*
* sysDbgStrInit - initialize the BTS, PEBS and PMC in the BSP
*
* This routine initializes the BTS/PEBS/PMC in the BSP.  It is attached to the
* BTS/PEBS/PMC interrupt vector by the routine sysBtsConnect(), 
* sysPebsConnect() and sysPmcConnect() respectively.
*/

void sysDbgStrInit (void)
    {
    LL_INT resetValue = PEBS_RESET;

    /* set up the BTS/PEBS/PMC interrupt handler */

    (void)intConnect (INUM_TO_IVEC (INT_NUM_LOAPIC_PMC), sysPmcInt, 0);
    *(int *)(loApicBase + LOAPIC_PMC) = LOAPIC_EDGE | LOAPIC_FIXED | 
					INT_NUM_LOAPIC_PMC;

    /* set up the system BTS/PEBS buffer : 16KB, 16KB, task mode */

    if (dbgStrLibInit (0, 0, DS_SYS_MODE) != OK)
	return;

    /* set the system BTS/PEBS configuration parameters */

    dbgStrConfig ((WIND_TCB *)NONE, BTS_ENABLED, PEBS_ENABLED,
		  BTS_INT_MODE, BTS_BUF_MODE,
		  PEBS_EVENT, PEBS_METRIC, PEBS_OS, &resetValue);

    /* start/stop (enable/disable) the BTS/PEBS with the above parameters */

    dbgStrStart ((WIND_TCB *)NONE);

    /* link in the show routine */

#ifdef INCLUDE_SHOW_ROUTINES
    dbgStrShowInit ();
#endif /* INCLUDE_SHOW_ROUTINES */
    }

/*******************************************************************************
*
* sysPmcInt - interrupt level processing for BTS, PEBS and PMC 
*
* This routine handles the BTS/PEBS/PMC interrupt.  It is attached to the
* BTS/PEBS/PMC interrupt vector by the routine sysBtsConnect(), 
* sysPebsConnect() and sysPmcConnect() respectively.
*/

void sysPmcInt (void)
    {
    INT32 oldEnable;		/* old lock value */
    DS_BUF_HEADER * pH;		/* DS header */

    /* acknowledge PMC interrupt */

    {
    sysPmcIntCnt++;		/* XXX */
    }

    /* get the Debug Store Buffer Header */

    if ((dbgStrCurrent == NULL) || (dbgStrCurrent->pH == NULL))
	{
        /* clear the MASK bit in the PMC entry in LVT */

        *(int *)(loApicBase + LOAPIC_PMC) &= ~LOAPIC_MASK;

	return;
	}

    pH = dbgStrCurrent->pH;

    /* acknowledge BTS interrupt */

    if ((dbgStrCurrent->btsIntMode) &&
	((UINT32)pH->btsIndex >= pH->btsThreshold))
	{
	oldEnable = dbgStrBtsEnable (FALSE);	/* disable BTS */
	sysBtsIntCnt++;
        if (sysBtsRoutine != NULL)
	    (* sysBtsRoutine) (sysBtsArg);
	pH->btsIndex = pH->btsBase;		/* reset */
	if (oldEnable || dbgStrCurrent->btsEnabled)
	    dbgStrBtsEnable (TRUE);		/* enable BTS if it was */
	}
    
    /* acknowledge PEBS interrupt */

    if ((UINT32)pH->pebsIndex >= pH->pebsThreshold)
	{
	oldEnable = dbgStrPebsEnable (FALSE);	/* disable PEBS */
	sysPebsIntCnt++;
        if (sysPebsRoutine != NULL)
	    (* sysPebsRoutine) (sysPebsArg);
	pH->pebsIndex = pH->pebsBase;		/* reset */
	if (oldEnable || dbgStrCurrent->pebsEnabled)
	    dbgStrPebsEnable (TRUE);		/* enable PEBS if it was */
	}

    /* clear the MASK bit in the PMC entry in LVT */

    *(int *)(loApicBase + LOAPIC_PMC) &= ~LOAPIC_MASK;
    }

/*******************************************************************************
*
* sysBtsConnect - connect a routine to the BTS interrupt
*
* This routine specifies the interrupt service routine to be called at each
* BTS interrupt.  Normally, it is called from sysHwInit2() in sysLib.c to 
* connect usrBts() to the BTS interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: usrBts(), sysBtsInt()
*/

STATUS sysBtsConnect
    (
    FUNCPTR routine,	/* routine to be called at each BTS interrupt */
    int arg		/* argument with which to call routine */
    )
    {

    sysBtsRoutine   = routine;
    sysBtsArg	    = arg;
    sysBtsConnected = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* sysPebsConnect - connect a routine to the PEBS interrupt
*
* This routine specifies the interrupt service routine to be called at each
* PEBS interrupt.  Normally, it is called from sysHwInit2() in sysLib.c to 
* connect usrPebs() to the PEBS interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: usrPebs(), sysPebsInt()
*/

STATUS sysPebsConnect
    (
    FUNCPTR routine,	/* routine to be called at each PEBS interrupt */
    int arg		/* argument with which to call routine */
    )
    {

    sysPebsRoutine   = routine;
    sysPebsArg	     = arg;
    sysPebsConnected = TRUE;

    return (OK);
    }

