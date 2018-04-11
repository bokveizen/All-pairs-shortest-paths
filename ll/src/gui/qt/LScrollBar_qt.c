/* =============================================================================
 *  LScrollBar_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Aug 30, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:12:30 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.5 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qscrollbar.h>

#include "LScrollBar.h"
#include "LScrollBar_qt.h"
#include "LWindow_qt.h"

static Bool sgInsideScrollEvent;

/*----------------------------
// Create
//--------------------------*/
LScrollBar *LScrollBar_Create( LWindow *inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, ui2 inHorVer, LScrollBar_THandler inHandler )
{
	LScrollBar	*theScrollBarPtr;
	LScrollBar	theScrollBar;

	if (inHorVer == LScrollBar_HOR)
		theScrollBar.mScrollBar = new QLeoScrollBar(Qt::Horizontal, inWindow->mHwnd);
	else
		theScrollBar.mScrollBar = new QLeoScrollBar(Qt::Vertical, inWindow->mHwnd);
	
	/* check for call failure */
	if (theScrollBar.mScrollBar == NULL) 
		Throw(LScrollBar_CANT_CREATE);

	theScrollBar.mScrollBar->setGeometry(inX, inY, inDX, inDY);
	
	theScrollBar.mScrollBar->setTracking(TRUE);
	
	theScrollBar.mParentWindow = inWindow;

	theScrollBar.mID = LScrollBar_ID;

	theScrollBar.mHandler = (LScrollBar_THandler) inHandler;
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Scrollbar");
#endif
	
	theScrollBarPtr = (LScrollBar* )LMemory_NewObject(LScrollBar, theScrollBar);

	theScrollBar.mScrollBar->cLScrollBar = theScrollBarPtr;
	
	QObject::connect(theScrollBarPtr->mScrollBar, SIGNAL(sliderMoved(int )), theScrollBarPtr->mScrollBar, SLOT(slotThumbPosition(int )));
        QObject::connect(theScrollBarPtr->mScrollBar, SIGNAL(valueChanged(int )), theScrollBarPtr->mScrollBar, SLOT(slotThumbTracking(int )));

	QObject::connect(theScrollBarPtr->mScrollBar, SIGNAL(prevLine()), theScrollBarPtr->mScrollBar, SLOT(slotLineUp()));
        QObject::connect(theScrollBarPtr->mScrollBar, SIGNAL(nextLine()), theScrollBarPtr->mScrollBar, SLOT(slotLineDown()));
        QObject::connect(theScrollBarPtr->mScrollBar, SIGNAL(prevPage()), theScrollBarPtr->mScrollBar, SLOT(slotPageUp()));
        QObject::connect(theScrollBarPtr->mScrollBar, SIGNAL(nextPage()), theScrollBarPtr->mScrollBar, SLOT(slotPageDown()));


	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();


#ifdef PRINT_DEBUG
	qDebug( "\n\r Create Scrollbar List Item 0x%x",  theScrollBarPtr);

#endif
	theWindowListItemPtr->mWidgetListPtr = theScrollBarPtr;

	theWindowListItemPtr->mID = LScrollBar_ID;

	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);


	return  theScrollBarPtr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LScrollBar_Destroy (LScrollBar* inBar)
{
    delete inBar->mScrollBar;
    LMemory_DeleteObject(&inBar);
	
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LScrollBar_Move (LScrollBar* inBar, i4 inX, i4 inY)
{
     inBar->mScrollBar->move(int(inX), int(inY));
}

/*-------------------------------------
// Resize ()
//------------------------------------*/
void LScrollBar_Resize (LScrollBar* inBar, ui4 inWidth, ui4 inHeight)
{
      inBar->mScrollBar->resize(int(inWidth), int(inHeight));
}

/*--------------------------------------
// SetPosition ()
//-------------------------------------*/
void LScrollBar_SetPosition (LScrollBar* inBar, ui2 inPos)
{
	  
	//  if (sgInsideScrollEvent == FALSE)
	  	inBar->mScrollBar->setValue( int(inPos) );
}

/*-------------------------------------
// GetPosition ()
//-----------------------------------*/
ui2 LScrollBar_GetPosition (LScrollBar* inBar)
{
	return ui2((inBar->mScrollBar->value()));
}

/*--------------------------------------
// SetRange ()
//-------------------------------------*/
void LScrollBar_SetRange (LScrollBar* inBar, ui2 inMinRange, ui2 inMaxRange)
{
	inBar->mScrollBar->setMinValue(inMinRange);
	inBar->mScrollBar->setMaxValue(inMaxRange);
}

/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LScrollBar_GetParentWindow (LScrollBar* inBar)
{
	return inBar->mParentWindow;
}


/*-------------------------------------
// QLeoScrollBar::QLeoScrollBar ()
//------------------------------------*/
QLeoScrollBar::QLeoScrollBar(Orientation inOrientation, QMainWindow *inWindow) : QScrollBar( inOrientation, (QWidget*)inWindow)
{
    cLScrollBar = NULL;
    setFocusPolicy(QWidget::StrongFocus);
}

/*-------------------------------------
// QLeoScrollBar::~QLeoScrollBar ()
//------------------------------------*/
QLeoScrollBar::~QLeoScrollBar()
{

}

/*-------------------------------------
// QLeoScrollBar::focusInEvent ()
//------------------------------------*/
void QLeoScrollBar::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LScrollBar*)(((QLeoScrollBar*)this)->cLScrollBar));
	_LApplication_gGlobal.mWidgetFocusedId = ((LScrollBar*)(((QLeoScrollBar*)this)->cLScrollBar))->mID;

#ifdef PRINT_DEBUG
	qDebug( "focusInEvent 0x%x   _gWidgetFocusedId 0x%x",  _LApplication_gGlobal.mWidgetFocused, _LApplication_gGlobal.mWidgetFocusedId);
#endif

}

void QLeoScrollBar::slotLineUp()
{
#ifdef PRINT_DEBUG
     qDebug( "\n\r LineUp");
#endif	
     sgInsideScrollEvent = TRUE;
     
     if (((LScrollBar *)(this->cLScrollBar))->mHandler != NULL )   {
    	((LScrollBar *)(this->cLScrollBar))->mHandler(((LScrollBar *)(this->cLScrollBar)), LScrollBar_LINEUP, ui2(0));
     }
     
     sgInsideScrollEvent = FALSE;

}

void QLeoScrollBar::slotLineDown()
{
#ifdef PRINT_DEBUG
    qDebug( "\n\r LineDown");
#endif	
	sgInsideScrollEvent = TRUE;

    (((LScrollBar *)(this->cLScrollBar)))->mHandler((LScrollBar *)(this->cLScrollBar), LScrollBar_LINEDOWN, ui2(0));

	sgInsideScrollEvent = FALSE;

}

void QLeoScrollBar::slotPageUp()
{
#ifdef PRINT_DEBUG
     qDebug( "\n\r PageUp");
#endif	
     sgInsideScrollEvent = TRUE;
     
     (((LScrollBar *)(this->cLScrollBar)))->mHandler((LScrollBar *)(this->cLScrollBar), LScrollBar_PAGEUP, ui2(0));

	sgInsideScrollEvent = FALSE;
}

void QLeoScrollBar::slotPageDown()
{

#ifdef PRINT_DEBUG
     qDebug( "\n\r PageDown");
#endif
	sgInsideScrollEvent = TRUE;
     
	(((LScrollBar *)(this->cLScrollBar)))->mHandler((LScrollBar *)(this->cLScrollBar), LScrollBar_PAGEDOWN, ui2(0));

	sgInsideScrollEvent = FALSE;
}

void QLeoScrollBar::slotThumbPosition(int inPos)
{
#ifdef PRINT_DEBUG
     qDebug( "\n\r Thumb Position");
#endif
	sgInsideScrollEvent = TRUE;
	
	(((LScrollBar *)(this->cLScrollBar)))->mHandler((LScrollBar *)(this->cLScrollBar), LScrollBar_THUMBPOSITION, ui2(inPos));

	sgInsideScrollEvent = FALSE;

}

void QLeoScrollBar::slotThumbTracking(int inPos)
{

#ifdef PRINT_DEBUG
     qDebug( "\n\r Thumb Tracking");

#endif
	sgInsideScrollEvent = TRUE;

	(((LScrollBar *)(this->cLScrollBar)))->mHandler((LScrollBar *)(this->cLScrollBar), LScrollBar_THUMBTRACK, ui2(inPos));

	sgInsideScrollEvent = FALSE;

}

#endif /* __LSL_OS_GUI__ == __LSL_QT__ */


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

