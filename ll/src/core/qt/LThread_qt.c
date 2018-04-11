/* =============================================================================
 *  LThread_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Dec 25, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:21:35 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qapplication.h>
#include <pthread.h>
#include <signal.h>

#include "LThread.h"
#include "LThread_qt.h"
#include "LWindow.h"
#include "LWindow_qt.h"

//extern QApplication	*_gApplication;

/*-----------------------------
// LThread_New ()
//---------------------------*/
LThread *LThread_New( LThread_THandler inHandler, void *inParam )
{
	LThread theThread;
	LThread *theThreadPtr;

	pthread_t theThreadId;

	pthread_create( &theThreadId, NULL/*const pthread_attr_t *attributes*/,
                /*_LThreadHandler*/inHandler,
		/*void *(*thread_function)(void *)*/  ( void * ) inParam/* NULL*//*(void*)(inParam)*//*void *arguments*/ );

	theThread.mThreadHandle = theThreadId;

	if ( theThread.mThreadHandle == NULL )
		Throw( LThread_CANT_CREATE );

	theThreadPtr = LMemory_NewObject ( LThread, theThread );

#ifdef PRINT_DEBUG
	qDebug( "\n\r LThread_New ");
#endif

	QWidgetDestroyList *theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theThreadPtr;
	theWindowListItemPtr->mID = LThread_ID;

	return theThreadPtr;
}

/*-----------------------------
// LThread_Wait ()
//---------------------------*/
void LThread_Wait( LThread *inThread )
{
	int theReturn;

	pthread_join(inThread->mThreadHandle, (void**)&theReturn);
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r pthread_join END");
#endif

}

/*-----------------------------
// LThread_Suspend ()
//---------------------------*/
void LThread_Suspend( LThread *inThread )
{
	pthread_kill( inThread->mThreadHandle, SIGSTOP );
}

/*-----------------------------
// LThread_Resume ()
//---------------------------*/
void LThread_Resume( LThread *inThread )
{
	pthread_kill( inThread->mThreadHandle, SIGCONT );
}

/*-----------------------------
// LThread_Delete ()
//---------------------------*/
void LThread_Delete(LThread* inThread)
{
	pthread_cancel( inThread->mThreadHandle );
	LMemory_DeleteObject( &inThread );
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
