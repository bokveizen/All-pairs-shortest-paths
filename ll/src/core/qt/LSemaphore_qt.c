/* =============================================================================
 *  LSemaphore_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Dec 25, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:21:35 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.3 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "LSemaphore.h"
#include "LWindow.h"
#include "LWindow_qt.h"


static i4 gsSemaphoreKey = 0;

struct tagLSemaphore {

    int mSemaphoreHandle;
    
};

/*-----------------------------
// LSemaphore_New 
//---------------------------*/
LSemaphore* LSemaphore_New(ui4 inVal)
{
	LSemaphore 	theSemaphore;
	LSemaphore* theSemaphorePtr;
	
	theSemaphore.mSemaphoreHandle = semget(gsSemaphoreKey, 1, IPC_CREAT | 0660 );												
	
	/* check for call failure */
	if (theSemaphore.mSemaphoreHandle == -1) 
		Throw(LSemaphore_CANT_CREATE);
		
	gsSemaphoreKey++;
	
	semctl(theSemaphore.mSemaphoreHandle, 0, SETVAL, (int)inVal);
	
	theSemaphorePtr = LMemory_NewObject(LSemaphore, theSemaphore);
	
	return theSemaphorePtr;
	
}

/*-----------------------------
// LSemaphore_Signal ()
//---------------------------*/
void LSemaphore_Signal(LSemaphore* inSem)
{
	struct sembuf operation;
	
	operation.sem_num = 0;
	operation.sem_op = +1;
	operation.sem_flg = 0;
	
	semop(inSem->mSemaphoreHandle, &operation, 1);
}

/*-----------------------------
// LSemaphore_Wait ()
//---------------------------*/
void LSemaphore_Wait(LSemaphore* inSem)
{
	struct sembuf operation;
	
	operation.sem_num = 0;
	operation.sem_op = -1;
	operation.sem_flg = 0;
	
	semop(inSem->mSemaphoreHandle, &operation, 1);
}

/*-----------------------------
// LSemaphore_Delete ()
//---------------------------*/
void LSemaphore_Delete(LSemaphore* inSem)
{
	semctl(inSem->mSemaphoreHandle, 0, IPC_RMID, 1);
	LMemory_DeleteObject (&inSem);
}



#endif	/* __LSL_OS_GUI__ == __LSL_QT__ */


/* Copyright (C) 2002 Gianni Bevacqua

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
