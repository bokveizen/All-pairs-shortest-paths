/* =============================================================================
 *  LCheckBox_qt.c
 * =============================================================================

 *  Author:         (c) Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Jun 26, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/21 19:07:41 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qmainwindow.h>
#include <qwidget.h>
#include <qcheckbox.h>

#include "LCheckBox.h"
#include "LCheckBox_qt.h"
#include "LWindow_qt.h"

/*extern "C" {

#include "LMemory.h"

}*/

/*----------------------------
// Create
//--------------------------*/
LCheckBox *LCheckBox_Create( LWindow *inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1 *inText, LCheckBox_THandler inHandler )
{
	LCheckBox	*theCheckBoxPtr;
	LCheckBox	theCheckBox;

	theCheckBox.mCheckBox = new QLeoCheckBox( inText, ( QWidget* )inWindow->mHwnd );

	/* check for call failure */
	if ( theCheckBox.mCheckBox == NULL )
		Throw( LCheckBox_CANT_CREATE );

	theCheckBox.mCheckBox->setGeometry( inX, inY, inDX, inDY );

	theCheckBox.mParentWindow = inWindow;

	theCheckBox.mID = LCheckBox_ID;

	theCheckBox.mHandler = ( LCheckBox_THandler )inHandler;

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : CheckBox");
#endif
	theCheckBoxPtr = ( LCheckBox* )LMemory_NewObject( LCheckBox, theCheckBox );

	theCheckBox.mCheckBox->cLCheckBox = theCheckBoxPtr;

	QObject::connect( theCheckBoxPtr->mCheckBox, SIGNAL(clicked()), theCheckBoxPtr->mCheckBox, SLOT(slotCheckBoxClicked()) );

	QWidgetDestroyList *theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theCheckBoxPtr;
	theWindowListItemPtr->mID = LCheckBox_ID;

	inWindow->mHwnd->cWidgetList.append( theWindowListItemPtr );

	return  theCheckBoxPtr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LCheckBox_Destroy (LCheckBox* inBox)
{
	delete inBox->mCheckBox;
	LMemory_DeleteObject(&inBox);

}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LCheckBox_Move (LCheckBox* inBox, i4 inX, i4 inY)
{
	inBox->mCheckBox->move(int(inX), int(inY));
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LCheckBox_Resize (LCheckBox* inBox, ui4 inWidth, ui4 inHeight)
{
	inBox->mCheckBox->resize(int(inWidth), int(inHeight));
}

/*-------------------------------------
// SetFocus ()
//------------------------------------*/
void LCheckBox_SetFocus (LCheckBox* inBox)
{
	inBox->mCheckBox->setFocus();
}

/*-------------------------------------
// SetState ()
//------------------------------------*/
void LCheckBox_SetState (LCheckBox* inBox, ui2 inCheckedUnchecked)
{
	if (inCheckedUnchecked == LCheckBox_CHECKED)
		inBox->mCheckBox->setChecked(true);
	else
		inBox->mCheckBox->setChecked(false);
			
	
}

/*-------------------------------------
// QueryState ()
//------------------------------------*/
ui2 LCheckBox_QueryState (LCheckBox* inBox)
{
	return inBox->mCheckBox->isChecked();
}

/*------------------------------------
// GetParentWindow ()
//-----------------------------------*/
LWindow* LCheckBox_GetParentWindow (LCheckBox* inBox)
{
	return inBox->mParentWindow;
}

/*------------------------------------
// SetFont ()
//-----------------------------------*/
void LCheckBox_SetFont (LCheckBox* inBox, LCheckBox_TFont inFont)
{
	QFont theFont;

	switch (inFont) {

		case LCheckBox_SYSTEMFIXED:
		    theFont.setFamily( "Courier");
		    theFont.setBold(TRUE);
		    theFont.setPixelSize(11);
		break;

		case LCheckBox_SYSTEMVAR:
			//theFont.setFamily( "Times");
			theFont.setFamily( "Helvetica");
			theFont.setBold(TRUE);
			theFont.setPixelSize(11);
		break;

		case LCheckBox_ANSIFIXED :
			theFont.setFamily( "Courier");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;

		case LCheckBox_ANSIVAR :
			//theFont.setFamily( "Helvetica");
			theFont.setFamily( "Helvetica");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(11);
		break;
	}

	inBox->mCheckBox->setFont(theFont);
}

/*-----------------------------------
// Enable ()
//----------------------------------*/
void LCheckBox_Enable (LCheckBox* inBox, Bool inEnabled)
{
	inBox->mCheckBox->setEnabled(inEnabled);
}


/*-------------------------------------------
// QLeoCheckBox::QLeoCheckBox ()
//-------------------------------------------*/
QLeoCheckBox::QLeoCheckBox(i1 *inString, QWidget* inWidget)  : QCheckBox(inString, inWidget, NULL)
{
	
	QFont theFont;
	
	setFocusPolicy(QWidget::StrongFocus);
	
	theFont.setPointSize(6);
	setFont(theFont);

}


/*------------------------------------------
// QLeoCheckBox::~QLeoCheckBox ()
//------------------------------------------*/
QLeoCheckBox::~QLeoCheckBox( )
{

}

/*-----------------------------------------
// QLeoCheckBox::slotCheckBoxClicked()
//-----------------------------------------*/
void QLeoCheckBox::slotCheckBoxClicked()
{
	((LCheckBox *)(this->cLCheckBox))->mHandler((LCheckBox*)this->cLCheckBox);
}

/*-------------------------------------
// QLeoCheckBox::focusInEvent ()
//------------------------------------*/
void QLeoCheckBox::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LCheckBox*)(((QLeoCheckBox*)this)->cLCheckBox));
	_LApplication_gGlobal.mWidgetFocusedId = ((LCheckBox*)(((QLeoCheckBox*)this)->cLCheckBox))->mID;

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

