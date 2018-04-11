/* =============================================================================
 *  LRadioButton_qt.c
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

#include <qmainwindow.h>
#include <qwidget.h>
#include <qradiobutton.h>
#include <qpainter.h>
#include <qpaintdevice.h>

#include "LRadioButton.h"
#include "LRadioButton_qt.h"
#include "LWindow_qt.h"

/*extern void *_gWidgetFocused;
extern ui2 _gWidgetFocusedId;*/

/*----------------------------
// Create 
//--------------------------*/
LRadioButton* LRadioButton_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1* inText, LRadioButton_THandler inHandler)
{
	LRadioButton	*theRadioButtonPtr;
	LRadioButton	theRadioButton;
	
	theRadioButton.mRadioButton = new QLeoRadioButton(inText, (QWidget*)inWindow->mHwnd);
	if (theRadioButton.mRadioButton == NULL)
		return NULL;
	
	theRadioButton.mRadioButton->setGeometry(inX, inY, inDX, inDY);

	theRadioButton.mParentWindow = inWindow;
	
	theRadioButton.mID = LRadioButton_ID;

	theRadioButton.mHandler = ( LRadioButton_THandler )inHandler;
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : RadioButton");
#endif
	theRadioButtonPtr = (LRadioButton* )LMemory_NewObject(LRadioButton, theRadioButton);
	
	theRadioButton.mRadioButton->cLRadioButton = theRadioButtonPtr;
	
	QObject::connect(theRadioButtonPtr->mRadioButton,SIGNAL(clicked()),theRadioButtonPtr->mRadioButton,SLOT(slotRadioButtonClicked()));


	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theRadioButtonPtr;
	theWindowListItemPtr->mID = LRadioButton_ID;


	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);
	
	return  theRadioButtonPtr;
}

/*--------------------------------------
// Destroy 
//-------------------------------------*/
void LRadioButton_Destroy(LRadioButton* inRadio)
{
	delete inRadio->mRadioButton;
	LMemory_DeleteObject(&inRadio);
	
}

/*--------------------------------------
// Move 
//-------------------------------------*/
void LRadioButton_Move(LRadioButton* inRadio, i4 inX, i4 inY)
{
	inRadio->mRadioButton->move(int(inX), int(inY));
}

/*--------------------------------------
// Resize 
//-------------------------------------*/
void LRadioButton_Resize(LRadioButton* inRadio, ui4 inWidth, ui4 inHeight)
{
	inRadio->mRadioButton->resize(int(inWidth), int(inHeight));
}

/*-------------------------------------
// SetFocus ()
//------------------------------------*/
void LRadioButton_SetFocus (LRadioButton* inRadio)
{
	inRadio->mRadioButton->setFocus();
}

/*-------------------------------------
// SetState
//------------------------------------*/
void LRadioButton_SetState(LRadioButton* inRadio, ui2 inCheckedUnchecked)
{
	
	if (inCheckedUnchecked == LRadioButton_CHECKED)
		inRadio->mRadioButton->setChecked(TRUE);
	else
		inRadio->mRadioButton->setChecked(FALSE);
			
}

/*-------------------------------------
// QueryState ()
//------------------------------------*/
ui2 LRadioButton_QueryState(LRadioButton* inRadio)
{
	ui2	theReturnState;
	
	return (theReturnState = inRadio->mRadioButton->isChecked());

}

/*-----------------------------------
// GetParentWindow ()
//---------------------------------*/
LWindow* LRadioButton_GetParentWindow (LRadioButton* inRadio)
{
	return inRadio->mParentWindow;
}

/*---------------------------------
// SetFont ()
//--------------------------------*/
void LRadioButton_SetFont (LRadioButton* inRadio, LRadioButton_TFont inFont)
{
	QFont theFont;


	switch (inFont) {

		case LRadioButton_SYSTEMFIXED:
		    theFont.setFamily( "Courier");
		    theFont.setBold(TRUE);
		    theFont.setPixelSize(11);
		break;

		case LRadioButton_SYSTEMVAR:
			//theFont.setFamily( "Times");
			theFont.setFamily( "Helvetica");
			theFont.setBold(TRUE);
			theFont.setPixelSize(11);
		break;

		case LRadioButton_ANSIFIXED :
			theFont.setFamily( "Courier");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;

		case LRadioButton_ANSIVAR :
			//theFont.setFamily( "Helvetica");
			theFont.setFamily( "Helvetica");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;
	}


	inRadio->mRadioButton->setFont(theFont);
}

/*-----------------------------------
// Enable ()
//----------------------------------*/
void LRadioButton_Enable (LRadioButton* inRadio, Bool inEnabled)
{
	inRadio->mRadioButton->setEnabled(inEnabled);
}


/*-------------------------------------------
// QLeoRadioButton::QLeoRadioButton ()
//-------------------------------------------*/
QLeoRadioButton::QLeoRadioButton(i1 *inString, QWidget* inWidget)  : QRadioButton(inString, inWidget, NULL)
{

	setFocusPolicy(QWidget::StrongFocus);

}


/*------------------------------------------
// QLeoRadioButton::~QLeoRadioButton ()
//------------------------------------------*/
QLeoRadioButton::~QLeoRadioButton( )
{

}

/*-----------------------------------------
// QLeoRadioButton::slotRadioButtonClicked()
//-----------------------------------------*/
void QLeoRadioButton::slotRadioButtonClicked()
{
	((LRadioButton *)(this->cLRadioButton))->mHandler((LRadioButton*)this->cLRadioButton);
}

/*-------------------------------------
// QLeoRadioButton::focusInEvent ()
//------------------------------------*/
void QLeoRadioButton::focusInEvent( QFocusEvent * inFocusEvent)
{
	_LApplication_gGlobal.mWidgetFocused = ((LRadioButton*)(((QLeoRadioButton*)this)->cLRadioButton));
	_LApplication_gGlobal.mWidgetFocusedId = ((LRadioButton*)(((QLeoRadioButton*)this)->cLRadioButton))->mID;

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

