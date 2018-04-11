/* =============================================================================
 *  LSizeBox_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Sep 09, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:12:30 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.5 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include "LSizeBox.h"
#include "LSizeBox_qt.h"
#include "LWindow_qt.h"

/*----------------------------
// Create ()
//--------------------------*/
LSizeBox *LSizeBox_Create( LWindow *inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY )
{
	LSizeBox	*theSizeBoxPtr;
	LSizeBox	theSizeBox;

	theSizeBox.mSizeBox = new QLeoSizeGrip((QWidget*)inWindow->mHwnd, "SizeGrip");
	
	/* check for call failure */
	if (theSizeBox.mSizeBox == NULL) 
		Throw(LSizeBox_CANT_CREATE);

	theSizeBox.mSizeBox->setGeometry(inX, inY, inDX, inDY);
	
	theSizeBox.mID = LSizeBox_ID;
	theSizeBox.mParentWindow = inWindow;

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Size Box");
#endif

	theSizeBoxPtr = (LSizeBox* )LMemory_NewObject(LSizeBox, theSizeBox);

	theSizeBox.mSizeBox->cLSizeBox = theSizeBoxPtr;
	
	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theSizeBoxPtr;
	theWindowListItemPtr->mID = LSizeBox_ID;

	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);

	return  theSizeBoxPtr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LSizeBox_Destroy (LSizeBox* inBox)
{
   	delete inBox->mSizeBox;
	LMemory_DeleteObject(&inBox);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LSizeBox_Move (LSizeBox* inBox, i4 inX, i4 inY)
{
	inBox->mSizeBox->move(int(inX), int(inY));
}

/*-------------------------------------
// Resize ()
//------------------------------------*/
void LSizeBox_Resize (LSizeBox* inBox, ui4 inWidth, ui4 inHeight)
{
	inBox->mSizeBox->resize(int(inWidth), int(inHeight));
}

/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LSizeBox_GetParentWindow (LSizeBox* inBox)
{
	return inBox->mParentWindow;
}

/*-------------------------------------------
// QLeoSizeGrip::QLeoSizeGrip ()
//-------------------------------------------*/
QLeoSizeGrip::QLeoSizeGrip(QWidget* inWidget, i1* inString)  : QSizeGrip(inWidget, inString)
{
	setFocusPolicy(QWidget::StrongFocus);

}

/*------------------------------------------
// QLeoSizeGrip::~QLeoSizeGrip ()
//------------------------------------------*/
QLeoSizeGrip::~QLeoSizeGrip()
{

}

/*-------------------------------------
// QLeoSizeGrip::focusInEvent ()
//------------------------------------*/
void QLeoSizeGrip::focusInEvent( QFocusEvent * inFocusEvent)
{
	_LApplication_gGlobal.mWidgetFocused = ((LSizeBox*)(((QLeoSizeGrip*)this)->cLSizeBox));
	_LApplication_gGlobal.mWidgetFocusedId = ((LSizeBox*)(((QLeoSizeGrip*)this)->cLSizeBox))->mID;

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


