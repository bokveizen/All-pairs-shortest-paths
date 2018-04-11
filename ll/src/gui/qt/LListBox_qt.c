/* =============================================================================
 *  LListBox_qt.c
 * =============================================================================

 *  Author:         (c) Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Aug 13, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:12:30 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qlistbox.h>
#include <qwidget.h>

#include "LListBox.h"
#include "LListBox_qt.h"
#include "LWindow_qt.h"
#include "LApplication.h"

extern "C" {

#include "LString.h"

}



static Bool sgRemovingItem = FALSE;
/*extern void *_gWidgetFocused;
extern ui2 _gWidgetFocusedId;/*

/*----------------------------
// Create ()
//--------------------------*/
LListBox* LListBox_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, LListBox_THandler inDBLCLKHandler)
{
    LListBox	*theListBoxPtr;
    LListBox	theListBox;

	theListBox.mHwnd = new QLeoListBox((QWidget*)inWindow->mHwnd, "ListBox");
	
	theListBox.mHwnd->resize(inDX, inDY);
	theListBox.mHwnd->move(inX, inY);

	theListBox.mHwnd->cCurrentItemStatus = false;

	theListBox.mParentWindow = inWindow;

	theListBox.mID = LListBox_ID;

	LApplication_GetArgs ();	

	QObject::connect(theListBox.mHwnd, SIGNAL(highlighted(int )), theListBox.mHwnd, SLOT(slotHighlighted(int )));

	LApplication_GetArgs ();

	if ( inDBLCLKHandler != NULL )
		theListBox.mHandler = (LListBox_THandler)inDBLCLKHandler;


#ifdef PRINT_DEBUG
	qDebug( "\n\r inDBLCLKHandler 0x%x theListBox.mHandler 0x%x", inDBLCLKHandler, theListBox.mHandler);
	qDebug( "\n\r LMemory_NewObject : ListBox");

#endif
	theListBoxPtr = (LListBox* )LMemory_NewObject(LListBox, theListBox);
	
	theListBox.mHwnd->cLListBox = theListBoxPtr;


#ifdef PRINT_DEBUG
	qDebug( "\n\r before connect : ListBox");

#endif	//theListBox.mHwnd->sort(TRUE);
	/*LApplication_GetArgs ();

	QObject::connect(theListBoxPtr->mHwnd, SIGNAL(highlighted(int )), theListBoxPtr->mHwnd, SLOT(slotHighlighted(int )));

	LApplication_GetArgs ();*/
	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theListBoxPtr;
	theWindowListItemPtr->mID = LListBox_ID;


	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);
	
	return  theListBoxPtr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LListBox_Destroy (LListBox* inLBox)
{
	delete inLBox->mHwnd;
	LMemory_DeleteObject(&inLBox);
}

/*-----------------------------------
// SetFont ()
//----------------------------------*/
void LListBox_SetFont (LListBox* inLBox, LListBox_TFont inFont)
{
	QFont theFont;

	switch (inFont) {

		case LListBox_SYSTEMFIXED:
		    theFont.setFamily( "Courier");
		    theFont.setBold(TRUE);
		    theFont.setPixelSize(12);
		break;

		case LListBox_SYSTEMVAR:
			//theFont.setFamily( "Times");
			theFont.setFamily( "Helvetica");
			theFont.setBold(TRUE);
			theFont.setPixelSize(12);
		break;

		case LListBox_ANSIFIXED :
			theFont.setFamily( "Courier");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(12);
		break;

		case LListBox_ANSIVAR :
			//theFont.setFamily( "Helvetica");
			theFont.setFamily( "Helvetica");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(12);
		break;
	}
	
	inLBox->mHwnd->setFont(theFont);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LListBox_Move (LListBox* inLBox, i4 inX, i4 inY)
{
     inLBox->mHwnd->move(inX, inY);
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LListBox_Resize (LListBox* inLBox, ui4 inWidth, ui4 inHeight)
{
     inLBox->mHwnd->resize(inWidth, inHeight);
}

/*-------------------------------------
// SetFocus ()
//------------------------------------*/
void LListBox_SetFocus (LListBox* inLBox)
{
	inLBox->mHwnd->setFocus();
}

/*----------------------------
// GetParentWindow ()
//---------------------------*/
LWindow* LListBox_GetParentWindow (LListBox* inLBox)
{
	return inLBox->mParentWindow;
}

/*------------------------------
// AddItem ()
//-----------------------------*/
void LListBox_AddItem (LListBox* inLBox, i1* inString)
{
	inLBox->mHwnd->insertItem(inString);

	inLBox->mHwnd->sort(TRUE);

}

/*------------------------------
// RemoveItem ()
//-----------------------------*/
void LListBox_RemoveItem (LListBox* inLBox, i1* inString)
{
	i1 theIndex;
	QString theString;
	i1 theCurrentSelection[30];
	
 	for (theIndex = 0; theIndex < inLBox->mHwnd->count() ; theIndex++) {

			theString  = inLBox->mHwnd->text(theIndex);

			if (!strcmp(theString.latin1(), inString)) {

				
				LListBox_GetCurrentSelection(inLBox, theCurrentSelection);
				
#ifdef PRINT_DEBUG
	qDebug( "\n\r theCurrentSelection %s",theCurrentSelection);
#endif

				if (!strcmp(theString.latin1(), theCurrentSelection)) {

					inLBox->mHwnd->cCurrentItemStatus = false;
					sgRemovingItem = TRUE;
					
				}
#ifdef PRINT_DEBUG
	qDebug( "\n\r cCurrentItemStatus %d   theString.latin1() %s",inLBox->mHwnd->cCurrentItemStatus, theString.latin1());
#endif

				inLBox->mHwnd->removeItem(theIndex);

				return;

			}
	}

}

/*----------------------------------------
// GetCurrentSelection ()
//---------------------------------------*/
Bool LListBox_GetCurrentSelection (LListBox* inLBox, i1* outString)
{
	QString theString;
	const i1 *theConvertedString;

	int i = false;
	int a = NULL;

#ifdef PRINT_DEBUG
	qDebug( "\n\r inLBox->mHwnd 0x%x   ",inLBox->mHwnd);
	qDebug( "\n\r cCurrentItemStatus %d  false = %d NULL = %d",inLBox->mHwnd->cCurrentItemStatus, i, a);
#endif

	if (inLBox->mHwnd->cCurrentItemStatus == false)
		return false;
		


	theString = inLBox->mHwnd->currentText();
	if (theString == NULL)
		return false;

	theConvertedString = theString.latin1();

	
	
	strcpy(outString,(i1*)theConvertedString);
	


	return true;
}

/*----------------------------------------------
// SetCurrentSelection ()
//---------------------------------------------*/
void LListBox_SetCurrentSelection (LListBox* inLBox, i1* inString)
{
	
	int theIndex;
	QString theString;

	if ( inString == NULL) {
	
		theIndex = inLBox->mHwnd->currentItem();

		inLBox->mHwnd->setSelected(theIndex, false);
		inLBox->mHwnd->clearSelection();
		inLBox->mHwnd->cCurrentItemStatus = false;

	}
	else {

		for (theIndex = 0; theIndex < inLBox->mHwnd->count() ; theIndex++) {

			theString  = inLBox->mHwnd->text(theIndex);
			if (!strcmp(theString.latin1(), inString)) {
				inLBox->mHwnd->setSelected(theIndex, true);
				inLBox->mHwnd->cCurrentItemStatus = true;
				return;
			}
		}
	}

}

/*-------------------------------------------
// QLeoListBox::QLeoListBox ()
//-------------------------------------------*/
QLeoListBox::QLeoListBox(QWidget* inWidget, char *inString)  : QListBox(inWidget, inString)
{
	cCurrentItemStatus = false;
	setFocusPolicy(QWidget::StrongFocus);

}

/*------------------------------------------
// QLeoListBox::~QLeoListBox ()
//------------------------------------------*/
QLeoListBox::~QLeoListBox()
{

}

/*-----------------------------------------
// QLeoListBox::slotHighlighted ()
//-----------------------------------------*/
void QLeoListBox::slotHighlighted(int inIndex)
{

	if (sgRemovingItem != TRUE)
		cCurrentItemStatus = TRUE;
	else
		sgRemovingItem = FALSE;

#ifdef PRINT_DEBUG
	qDebug( "\n\r slotHighlighted................");

#endif

}

/*-------------------------------------
// QLeoListBox::focusInEvent ()
//------------------------------------*/
void QLeoListBox::focusInEvent(QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LListBox*)(((QLeoListBox*)this)->cLListBox));
	_LApplication_gGlobal.mWidgetFocusedId = ((LListBox*)(((QLeoListBox*)this)->cLListBox))->mID;

#ifdef PRINT_DEBUG
	qDebug( "focusInEvent 0x%x   _gWidgetFocusedId 0x%x",  _LApplication_gGlobal.mWidgetFocused, _LApplication_gGlobal.mWidgetFocusedId);
#endif



}

/*-------------------------------------
// QLeoListBox::mouseDoubleClickEvent ()
//------------------------------------*/
void QLeoListBox::mouseDoubleClickEvent( QMouseEvent *inMouseEvent )
{

	((LListBox *)(this->cLListBox))->mHandler((LListBox *)this->cLListBox);

#ifdef PRINT_DEBUG
	qDebug( "mouseDoubleClickEvent");
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
