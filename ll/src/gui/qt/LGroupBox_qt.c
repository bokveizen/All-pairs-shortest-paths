/* =============================================================================
 *  LGroupBox_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Jun 26, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:12:30 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qmainwindow.h>
#include <qwidget.h>
#include <qgroupbox.h>

#include "LWindow_qt.h"
#include "LGroupBox_qt.h"
#include "LGroupBox.h"

/*----------------------------
// Create 
//--------------------------*/
LGroupBox *LGroupBox_Create( LWindow *inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1 *inText )
{
	LGroupBox	*theGroupBoxPtr;
	LGroupBox	theGroupBox;

	theGroupBox.mGroupBox = new QLeoGroupBox(inText, (QWidget*)inWindow->mHwnd);
	
	/* check for call failure */
	if (theGroupBox.mGroupBox == NULL)
		Throw(LGroupBox_CANT_CREATE);

	
	theGroupBox.mGroupBox->setGeometry(inX, inY, inDX, inDY);

	theGroupBox.mGroupBox->lower();
	
	theGroupBox.mParentWindow = inWindow;
	
	theGroupBox.mID = LGroupBox_ID;

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : GroupBox");
#endif

	theGroupBoxPtr = (LGroupBox* )LMemory_NewObject(LGroupBox,theGroupBox);
	
	theGroupBox.mGroupBox->cLGroupBox = theGroupBoxPtr;

	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theGroupBoxPtr;
	theWindowListItemPtr->mID = LGroupBox_ID;

	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);

	return theGroupBoxPtr;
}

/*--------------------------------------
// Destroy 
//-------------------------------------*/
void LGroupBox_Destroy (LGroupBox* inGroup)
{
	delete inGroup->mGroupBox;
	LMemory_DeleteObject(&inGroup);

}

/*--------------------------------------
// Move 
//-------------------------------------*/
void LGroupBox_Move(LGroupBox* inGroup, i4 inX, i4 inY)
{
	inGroup->mGroupBox->move(int(inX), int(inY));
}

/*--------------------------------------
// Resize 
//-------------------------------------*/
void LGroupBox_Resize (LGroupBox* inGroup, ui4 inWidth, ui4 inHeight)
{
	inGroup->mGroupBox->resize(int(inWidth), int(inHeight));
}

/*------------------------------------
// SetFont ()
//-----------------------------------*/
void LGroupBox_SetFont (LGroupBox* inGroup, LGroupBox_TFont inFont)
{
	QFont theFont;

	switch (inFont) {

		case LGroupBox_SYSTEMFIXED:
		    theFont.setFamily( "Courier");
		    theFont.setBold(TRUE);
		    theFont.setPixelSize(11);
		break;

		case LGroupBox_SYSTEMVAR:
			//theFont.setFamily( "Times");
			theFont.setFamily( "Helvetica");
			theFont.setBold(TRUE);
			theFont.setPixelSize(11);
		break;

		case LGroupBox_ANSIFIXED :
			theFont.setFamily( "Courier");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;

		case LGroupBox_ANSIVAR :
			//theFont.setFamily( "Helvetica");
			theFont.setFamily( "Helvetica");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;
	}
	
	inGroup->mGroupBox->setFont(theFont);
}


/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LGroupBox_GetParentWindow (LGroupBox* inGroup)
{
    return inGroup->mParentWindow;
}

/*-------------------------------------------
// QLeoGroupBox::QLeoGroupBox ()
//-------------------------------------------*/
QLeoGroupBox::QLeoGroupBox(i1 *inString, QWidget* inWidget)  : QGroupBox(inString, inWidget, NULL)
{

	setFocusPolicy(QWidget::StrongFocus);

}


/*------------------------------------------
// QLeoGroupBox::~QLeoGroupBox ()
//------------------------------------------*/
QLeoGroupBox::~QLeoGroupBox( )
{

}

/*-------------------------------------
// QLeoGroupBox::focusInEvent ()
//------------------------------------*/
void QLeoGroupBox::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LGroupBox*)(((QLeoGroupBox*)this)->cLGroupBox));
	_LApplication_gGlobal.mWidgetFocusedId = ((LGroupBox*)(((QLeoGroupBox*)this)->cLGroupBox))->mID;

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
