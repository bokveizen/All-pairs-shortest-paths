/* =============================================================================
 *  LProgressBar_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Aug 13, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/16 23:57:27 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.3 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qprogressbar.h>

#include "LProgressBar.h"
#include "LProgressBar_qt.h"
#include "LWindow_qt.h"

/*extern void *_gWidgetFocused;
extern ui2 _gWidgetFocusedId;*/

/*----------------------------
// Create ()
//--------------------------*/
LProgressBar* LProgressBar_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY)
{
	LProgressBar	*theProgressBarPtr;
	LProgressBar	theProgressBar; 

	theProgressBar.mHwnd = new QLeoProgressBar(inWindow->mHwnd, "ProgressBar");
	
	/* check for call failure */
	if (theProgressBar.mHwnd == NULL)
		Throw(LProgressBar_CANT_CREATE);
	
	theProgressBar.mHwnd->move(inX, inY); 
	theProgressBar.mHwnd->resize(inDX, inDY);
    
	/* For Windows compatibility */
	theProgressBar.mStep = 10;
	theProgressBar.mCurrentPosition = 0;
    
	theProgressBar.mID = LProgressBar_ID;
	theProgressBar.mParentWindow = inWindow;

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : ProgressBar");
#endif
	theProgressBarPtr = (LProgressBar* )LMemory_NewObject(LProgressBar, theProgressBar);

	theProgressBar.mHwnd->cLProgressBar = theProgressBarPtr;
	
	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theProgressBarPtr;
	theWindowListItemPtr->mID = LProgressBar_ID;


	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);
	
	return theProgressBarPtr;

}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LProgressBar_Destroy (LProgressBar* inBar)
{
    delete inBar->mHwnd;
    LMemory_DeleteObject (&inBar);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LProgressBar_Move (LProgressBar* inBar, i4 inX, i4 inY)
{
    inBar->mHwnd->move(inX, inY);
}

/*--------------------------------------
// SetPosition ()
//-------------------------------------*/
void LProgressBar_SetPosition (LProgressBar* inBar, ui2 inPos)
{
	inBar->mCurrentPosition = inPos;
	inBar->mHwnd->setProgress(inBar->mCurrentPosition);
}

/*--------------------------------------
// SetRange ()
//-------------------------------------*/
void LProgressBar_SetRange (LProgressBar* inBar, ui2 inMinRange, ui2 inMaxRange)
{
	inBar->mHwnd->setTotalSteps(inMaxRange - inMinRange); 
}

/*-------------------------------------
// SetStep ()
//------------------------------------*/
void LProgressBar_SetStep (LProgressBar* inBar, ui2 inStep)
{
	inBar->mStep = inStep;
}

/*-------------------------------------
// StepIt ()
//------------------------------------*/
void LProgressBar_StepIt (LProgressBar* inBar)
{
	inBar->mCurrentPosition += inBar->mStep;
	inBar->mHwnd->setProgress(inBar->mCurrentPosition);
	
}

/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LProgressBar_GetParentWindow (LProgressBar* inBar)
{
	return inBar->mParentWindow;
}


/*-------------------------------------------
// QLeoProgressBar::QLeoProgressBar ()
//-------------------------------------------*/
QLeoProgressBar::QLeoProgressBar(QWidget* inWidget, i1* inString)  : QProgressBar(inWidget, inString)
{
	setFocusPolicy(QWidget::StrongFocus);

}

/*------------------------------------------
// QLeoProgressBar::~QLeoProgressBar ()
//------------------------------------------*/
QLeoProgressBar::~QLeoProgressBar()
{

}

/*-------------------------------------
// QLeoProgressBar::focusInEvent ()
//------------------------------------*/
void QLeoProgressBar::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LProgressBar*)(((QLeoProgressBar*)this)->cLProgressBar));
	_LApplication_gGlobal.mWidgetFocusedId = ((LProgressBar*)(((QLeoProgressBar*)this)->cLProgressBar))->mID;

#ifdef PRINT_DEBUG
	qDebug( "focusInEvent 0x%x   _gWidgetFocusedId 0x%x",  _LApplication_gGlobal.mWidgetFocused, _LApplication_gGlobal.mWidgetFocusedId);
#endif



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

