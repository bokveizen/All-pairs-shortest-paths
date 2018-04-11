/* =============================================================================
 *  LPushButton_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Jun 26, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/16 23:57:27 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qpixmap.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include <qpushbutton.h>

#include "LPushButton.h"
#include "LPushButton_qt.h"
#include "LWindow_qt.h"
#include "LBitmap_qt.h"

/*extern void *_gWidgetFocused;
extern ui2 _gWidgetFocusedId;*/
/*----------------------------
// Create 
//--------------------------*/
LPushButton* LPushButton_Create(LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1* inText, LBitmap* inBitmap, LPushButton_THandler inHandler)
{

	LPushButton	*thePushButtonPtr;
	LPushButton	thePushButton;
	
	thePushButton.mPushButton = new QLeoPushButton(inText, (QWidget*)inWindow->mHwnd);
	
	/* check for call failure */
	if (thePushButton.mPushButton == NULL) 
		Throw(LPushButton_CANT_CREATE);

	thePushButton.mPushButton->setGeometry(inX, inY, inDX, inDY);

	thePushButton.mParentWindow = inWindow;
	
	thePushButton.mID = LPushButton_ID;

	thePushButton.mHandler = (LPushButton_THandler)inHandler;
	
	if (inBitmap != NULL)
		thePushButton.mPushButton->setPixmap(*(inBitmap->mHbitmap));
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Pushbutton");

#endif
	

	
	thePushButtonPtr = (LPushButton* )LMemory_NewObject(LPushButton,thePushButton);
	
	thePushButton.mPushButton->cLPushButton = thePushButtonPtr;
	

	//LPushButton_SetFont (thePushButtonPtr, LPushButton_SYSTEMVAR);

	QObject::connect(thePushButtonPtr->mPushButton, SIGNAL(clicked()), thePushButtonPtr->mPushButton, SLOT(slotPushButtonClicked()));


	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	
#ifdef PRINT_DEBUG
	qDebug( "\n\r Create PushButton List Item 0x%x",  thePushButtonPtr);

#endif
	theWindowListItemPtr->mWidgetListPtr = thePushButtonPtr;

	theWindowListItemPtr->mID = LPushButton_ID;

	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);

	return  thePushButtonPtr;

}

/*--------------------------------------
// Destroy 
//-------------------------------------*/
void LPushButton_Destroy(LPushButton* inButton)
{
	delete inButton->mPushButton;
	LMemory_DeleteObject(&inButton);
	
}

/*--------------------------------------
// Move 
//-------------------------------------*/
void LPushButton_Move(LPushButton* inButton, i4 inX, i4 inY)
{
	inButton->mPushButton->move(int(inX), int(inY)); 
}

/*--------------------------------------
// Resize 
//-------------------------------------*/
void LPushButton_Resize(LPushButton* inButton, ui4 inWidth, ui4 inHeight)
{
	inButton->mPushButton->resize(int(inWidth), int(inHeight)); 	
}

/*-------------------------------------
// SetFocus 
//------------------------------------*/
void LPushButton_SetFocus(LPushButton* inButton)
{
	inButton->mPushButton->setFocus(); 
}

/*------------------------------------
// GetParentWindow ()
//-----------------------------------*/
LWindow* LPushButton_GetParentWindow (LPushButton* inButton)
{
    return inButton->mParentWindow;
}

/*-----------------------------------
// SetFont ()
//----------------------------------*/
void LPushButton_SetFont (LPushButton* inButton, LPushButton_TFont inFont)
{
	QFont theFont;
	
	switch (inFont) {

		case LPushButton_SYSTEMFIXED:
		    theFont.setFamily( "Courier");
		    theFont.setBold(TRUE);
		    theFont.setPixelSize(11);
		break;
		
		case LPushButton_SYSTEMVAR:
			//theFont.setFamily( "Times");
			theFont.setFamily( "Helvetica");
			theFont.setBold(TRUE);
			theFont.setPixelSize(11);
		break;

		case LPushButton_ANSIFIXED :
			theFont.setFamily( "Courier");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;

		case LPushButton_ANSIVAR :
			//theFont.setFamily( "Helvetica");
			theFont.setFamily( "Helvetica");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;
	}


	inButton->mPushButton->setFont(theFont);
}

/*-----------------------------------
// Enable ()
//----------------------------------*/
void LPushButton_Enable (LPushButton* inButton, Bool inEnabled)
{
	inButton->mPushButton->setEnabled(inEnabled);
}


/*-------------------------------------------
// QLeoPushButton::QLeoPushButton ()
//-------------------------------------------*/
QLeoPushButton::QLeoPushButton(i1 *inString, QWidget* inWidget)  : QPushButton(inString, inWidget, NULL)
{
	/* inserito per testare FocusInEvent */
	
	//QFont theFont;
	
	//theFont.setFamily( "Helvetica");

	setFocusPolicy(QWidget::StrongFocus);

	/*theFont.setPixelSize(6);
	setFont(theFont);*/


}

/*------------------------------------------
// QLeoPushButton::~QLeoPushButton ()
//------------------------------------------*/
QLeoPushButton::~QLeoPushButton()
{

}

/*-----------------------------------------
// QLeoPushButton::slotPushButtonClicked ()
//-----------------------------------------*/
void QLeoPushButton::slotPushButtonClicked()
{
	((LPushButton *)(this->cLPushButton))->mHandler((LPushButton*)this->cLPushButton);
}

/*-------------------------------------
// QLeoPushButton::focusInEvent ()
//------------------------------------*/
void QLeoPushButton::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LPushButton*)(((QLeoPushButton*)this)->cLPushButton));
	_LApplication_gGlobal.mWidgetFocusedId = ((LPushButton*)(((QLeoPushButton*)this)->cLPushButton))->mID;

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

