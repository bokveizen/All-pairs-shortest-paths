/* =============================================================================
 *  LSemaphore_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Dec 3, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:04 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LSemaphore.h"

#if __LSL_OS_CORE__ == __LSL_Win32__

struct tagLSemaphore {
    HANDLE mSemaphoreHandle;
};


/*----------------------------
// New ()
//--------------------------*/
LSemaphore* LSemaphore_New (ui4 inVal)
{
	LSemaphore thesem;
	LSemaphore* thesemptr;
	
	thesem.mSemaphoreHandle=CreateSemaphore (NULL, (LONG) inVal, (LONG) LType_MAX_I4, NULL);												
	
	/* check for call failure */
	if (thesem.mSemaphoreHandle==NULL) Throw(LSemaphore_CANT_CREATE);
	
	thesemptr=LMemory_NewObject (LSemaphore, thesem);
	return thesemptr;
}

/*--------------------------------------
// Delete ()
//-------------------------------------*/
void LSemaphore_Delete (LSemaphore* inSem)
{
	CloseHandle (inSem->mSemaphoreHandle); /*if last handle is closed object should be removed from the system...*/
	
	LMemory_DeleteObject (&inSem);
}

/*--------------------------------------
// Signal ()
//-------------------------------------*/
void LSemaphore_Signal (LSemaphore* inSem)
{
	ReleaseSemaphore (inSem->mSemaphoreHandle, 1, NULL); 
}

/*--------------------------------------
// Wait ()
//-------------------------------------*/
void LSemaphore_Wait (LSemaphore* inSem)
{
	WaitForSingleObject (inSem->mSemaphoreHandle, INFINITE);
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

