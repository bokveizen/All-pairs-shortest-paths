/* =============================================================================
 *  LStatusBar_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Aug 13, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:12:30 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qstatusbar.h>
#include <qlabel.h>

#include "LStatusBar.h"
#include "LStatusBar_qt.h"
#include "LWindow_qt.h"

static QLabel	*sgLabel[MAX_PART_NUMBER];
static ui1	 	sgLastPart;

/*----------------------------
// Create
//--------------------------*/
LStatusBar *LStatusBar_Create( LWindow *inWindow, ui1 inNumParts )
{
	LStatusBar	*theStatusBarPtr;
	LStatusBar	theStatusBar;
	ui4			theWidth, theHeight;

	sgLastPart = inNumParts - 1;

	theStatusBar.mStatusBar = new QLeoStatusBar((QWidget*)inWindow->mHwnd, "StatusBar");

	/* check for call failure */
	if (theStatusBar.mStatusBar == NULL)
		Throw(LStatusBar_CANT_CREATE);

	inWindow->mStatusBar = TRUE;
	LWindow_GetClientAreaSize(inWindow, &theWidth, &theHeight);

	theStatusBar.mStatusBar->setSizeGripEnabled(TRUE/*false*/);

	theStatusBar.mStatusBar->resize(theWidth, STATUS_BAR_HEIGHT);
	theStatusBar.mStatusBar->move(0, theHeight - STATUS_BAR_HEIGHT);

	theStatusBar.mID = LStatusBar_ID;
	theStatusBar.mParentWindow = inWindow;

	QFont theFont;
	theFont.setFamily("Helvetica");
	theStatusBar.mStatusBar->setFont(theFont);


#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Status Bar");
#endif
	theStatusBarPtr = (LStatusBar* )LMemory_NewObject(LStatusBar, theStatusBar);
	
	theStatusBar.mStatusBar->cLStatusBar = theStatusBarPtr;
	
	for (i1 i=0; i < inNumParts; i++) {

		QLabel* theLabelPtr = new QLabel(NULL, (QWidget*)theStatusBarPtr->mStatusBar, NULL, 0 );

#ifdef PRINT_DEBUG
	qDebug( "\n\r NewStatusBar theLabelPtr : 0x%x",theLabelPtr);
#endif

		theStatusBarPtr->mStatusBar->addWidget(theLabelPtr, 10);

		sgLabel[i]  =  theLabelPtr;

	}


	
	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theStatusBarPtr;
	theWindowListItemPtr->mID = LStatusBar_ID;

	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);
	
	return theStatusBarPtr;

}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LStatusBar_Destroy (LStatusBar* inBar)
{
	for (int i = 0; i < MAX_PART_NUMBER; i++)
		if (sgLabel[i] != NULL)
			delete sgLabel[i];

	delete inBar->mStatusBar;
	LMemory_DeleteObject(&inBar);
}

/*--------------------------------------
// SetPart ()
//-------------------------------------*/
void LStatusBar_SetPart (LStatusBar* inBar, ui1 inPartIndex, ui4 inPartRightMargin)
{
	
}

/*--------------------------------------
// SetText ()
//-------------------------------------*/
void LStatusBar_SetText (LStatusBar* inBar, ui1 inPart, i1* inText)
{
#ifdef PRINT_DEBUG
	qDebug( "\n\r SetText sgLabel[inPart] : 0x%x",sgLabel[inPart]);
#endif
	
	(sgLabel[inPart])->setText(inText);

}

/*------------------------------------
// GetParentWindow ()
//----------------------------------*/
LWindow* LStatusBar_GetParentWindow (LStatusBar* inBar)
{
	return inBar->mParentWindow;
}

/*------------------------------------
// Refresh ()
//----------------------------------*/
void LStatusBar_Refresh (LStatusBar* inBar)
{

}


/*-------------------------------------------
// QLeoStatusBar::QLeoStatusBar ()
//-------------------------------------------*/
QLeoStatusBar::QLeoStatusBar(QWidget* inWidget, i1* inString)  : QStatusBar(inWidget, inString)
{
	setFocusPolicy(QWidget::StrongFocus);

}

/*------------------------------------------
// QLeoStatusBar::~QLeoStatusBar ()
//------------------------------------------*/
QLeoStatusBar::~QLeoStatusBar()
{

}

/*-------------------------------------
// QLeoStatusBar::focusInEvent ()
//------------------------------------*/
void QLeoStatusBar::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LStatusBar*)(((QLeoStatusBar*)this)->cLStatusBar));
	_LApplication_gGlobal.mWidgetFocusedId = ((LStatusBar*)(((QLeoStatusBar*)this)->cLStatusBar))->mID;

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


