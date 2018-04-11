/* =============================================================================
 *  LThread_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Nov 27, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:04 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LThread.h"

#if __LSL_OS_CORE__ == __LSL_Win32__

struct tagLThread {
    HANDLE mThreadHandle;
    DWORD mThreadID;
};


/*----------------------------
// New ()
//--------------------------*/
LThread* LThread_New (LThread_THandler inHandler, void* inParam)
{
	LThread thethread;
	LThread* thethreadptr;
	DWORD theaux;
	
	thethread.mThreadHandle=CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) inHandler,
											(LPVOID) inParam, 0, &theaux);
	
	thethread.mThreadID=theaux;
	
	/* check for call failure */
	if (thethread.mThreadHandle==NULL) Throw(LThread_CANT_CREATE);
	
	thethreadptr=LMemory_NewObject (LThread, thethread);
	return thethreadptr;
}

/*--------------------------------------
// Delete ()
//-------------------------------------*/
void LThread_Delete (LThread* inT)
{
	TerminateThread (inT->mThreadHandle, 0);
	CloseHandle (inT->mThreadHandle);
	
	LMemory_DeleteObject (&inT);
}

/*--------------------------------------
// Suspend ()
//-------------------------------------*/
void LThread_Suspend (LThread* inT)
{
	SuspendThread (inT->mThreadHandle);
}

/*--------------------------------------
// Resume ()
//-------------------------------------*/
void LThread_Resume (LThread* inT)
{
	ResumeThread (inT->mThreadHandle);
}

/*--------------------------------------
// Wait ()
//-------------------------------------*/
void LThread_Wait (LThread* inT)
{
	WaitForSingleObject (inT->mThreadHandle, INFINITE);
}

#endif

/* Copyright (C) 2002 Andrea Ribichini

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


