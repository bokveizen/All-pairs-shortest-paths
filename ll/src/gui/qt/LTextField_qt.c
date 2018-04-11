/* ============================================================================
 *  LTextField_qt.c
 * ============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Aug 13, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:12:30 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qlineedit.h>
#include <qapplication.h>

#include "LTextField.h"
#include "LTextField_qt.h"
#include "LWindow_qt.h"

extern "C" {

#include "LString.h"

}

/*----------------------------------------------
// Create
//----------------------------------------------*/
LTextField *LTextField_Create( LWindow *inWindow, ui4 inX, ui4 inY, ui4 inWidth, ui4 inHeight, Bool inFrameOn )
{
	LTextField	*theLineEditPtr;
	LTextField	theLineEdit;

	theLineEdit.mHwndEdit = new QLeoLineEdit(inWindow->mHwnd);
	
	/* check for call failure */
	if (theLineEdit.mHwndEdit == NULL)
		Throw(LTextField_CANT_CREATE);

	theLineEdit.mHwndEdit->resize(inWidth, inHeight);
	theLineEdit.mHwndEdit->move(inX,inY);
	
	theLineEdit.mParentWindow = inWindow;
	
	theLineEdit.mID = LTextField_ID;

	if (inFrameOn)
		theLineEdit.mHwndEdit->setFrameStyle(QFrame::WinPanel/*QFrame::Box*/ | QFrame::Sunken/*QFrame::Raised*/);
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : TextField");
#endif
	
	theLineEditPtr = (LTextField* )LMemory_NewObject(LTextField, theLineEdit);
	
	theLineEdit.mHwndEdit->cLTextField = theLineEditPtr;

	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theLineEditPtr;
	theWindowListItemPtr->mID = LTextField_ID;


	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);

	return  theLineEditPtr;

}


/*----------------------------------------------
// Destroy
//----------------------------------------------*/
void LTextField_Destroy( LTextField *inEditor )
{
	delete inEditor->mHwndEdit;
	LMemory_DeleteObject( &inEditor );
}


/*------------------------------------------------
// Resize
//------------------------------------------------*/
void LTextField_Resize (LTextField* inEditor, ui4 inWidth, ui4 inHeight)
{
       inEditor->mHwndEdit->resize(inWidth, inHeight);
}

/*------------------------------------------------
// Move ()
//------------------------------------------------*/
void LTextField_Move(LTextField* inEditor, ui4 inX, ui4 inY)
{
      inEditor->mHwndEdit->move(inX, inY);
}

/*--------------------------------------------------
// SetTextBuffer ()
//--------------------------------------------------*/
void LTextField_SetTextBuffer (LTextField* inEditor, i1* inBuffer)
{
     inEditor->mHwndEdit->setText(inBuffer);
}

/*--------------------------------------------------
// GetTextBufferSize ()
//-------------------------------------------------*/
i4 LTextField_GetTextBufferSize (LTextField* inEditor)
{
	QString theString;
	i4 theLength;
	
	theString = inEditor->mHwndEdit->displayText();
	
	theLength = (i4)theString.length();
	
	return theLength;
}

/*--------------------------------------------------
// GetTextBuffer ()
//-------------------------------------------------*/
void LTextField_GetTextBuffer (LTextField* inEditor, i1* outBuffer, i4 inLength)
{
	QString theString;
	const i1 *theConvertedString;

	theString = inEditor->mHwndEdit->displayText();
	
	theConvertedString = theString.latin1();
	
	/*strncpy(outBuffer, (i1*)theConvertedString, inLength);   */
	strcpy(outBuffer, (i1*)theConvertedString);
	qDebug( "Lenght %d", inLength);
}

/*-------------------------------------------------
// SetFont ()
//------------------------------------------------*/
void LTextField_SetFont (LTextField* inEditor, LTextField_TFont inFont)
{
	QFont theFont;
	

	switch (inFont) {

		case LTextField_SYSTEMFIXED:
		    theFont.setFamily( "Courier");
		    theFont.setBold(TRUE);
		    theFont.setPixelSize(12);
		break;

		case LTextField_SYSTEMVAR:
			//theFont.setFamily( "Times");
			theFont.setFamily( "Helvetica");
			theFont.setBold(TRUE);
			theFont.setPixelSize(12);
		break;

		case LTextField_ANSIFIXED :
			theFont.setFamily( "Courier");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(12);
		break;

		case LTextField_ANSIVAR :
			//theFont.setFamily( "Helvetica");
			theFont.setFamily( "Helvetica");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(12);
		break;
	}

	
	inEditor->mHwndEdit->setFont(theFont);   
}

/*----------------------------------------
// SetFocus ()
//---------------------------------------*/
void LTextField_SetFocus (LTextField *inEditor)
{
	inEditor->mHwndEdit->setFocus();
}

/*----------------------------------------
// GetFocus ()
//---------------------------------------*/
LTextField* LTextField_GetFocus ()
{
	LTextField*	theTextFieldPtr;

	if( _LApplication_gGlobal.mApplication->focusWidget() == NULL)
		return NULL;

	theTextFieldPtr = (LTextField*)(((QLeoLineEdit*)_LApplication_gGlobal.mApplication->focusWidget())->cLTextField);

	return theTextFieldPtr;
}

/*----------------------------------------
// Undo ()
//--------------------------------------*/
void LTextField_Undo (LTextField* inEditor)
{
     inEditor->mHwndEdit->undo();
}

/*----------------------------------------
// Cut ()
//--------------------------------------*/
void LTextField_Cut (LTextField* inEditor)
{
    inEditor->mHwndEdit->cut();
}

/*----------------------------------------
// Copy ()
//--------------------------------------*/
void LTextField_Copy (LTextField* inEditor)
{
     inEditor->mHwndEdit->copy();
}

/*----------------------------------------
// Paste ()
//--------------------------------------*/
void LTextField_Paste (LTextField* inEditor)
{
     inEditor->mHwndEdit->paste();
}

/*----------------------------------------
// SelectAll ()
//--------------------------------------*/
void LTextField_SelectAll(LTextField* inEditor)
{
     inEditor->mHwndEdit->selectAll();
}

/*----------------------------------------
// CanUndo ()
//---------------------------------------*/
Bool LTextField_CanUndo (LTextField* inEditor)
{
    
}

/*---------------------------------------
// IsTextAvailable ()
//--------------------------------------*/
Bool LTextField_IsTextAvailable ()
{
    
}

/*----------------------------------------
// IsSelectionAvailable ()
//---------------------------------------*/
Bool LTextField_IsSelectionAvailable (LTextField* inEditor)
{
    
}

/*NEW FUNCTIONS*/

/*----------------------------------
// GetSelection ()
//---------------------------------*/
/*void LTextEditor_GetSelection (LTextEditor* inEditor, i4* outStart, i4* outEnd)
{
    SendMessage (inEditor->mHwndEdit, EM_GETSEL, (WPARAM) outStart, (LPARAM) outEnd);
}*/

/*----------------------------------
// SetSelection ()
//---------------------------------*/
/*void LTextEditor_SetSelection (LTextEditor* inEditor, i4 inStart, i4 inEnd)
{
    SendMessage (inEditor->mHwndEdit, EM_SETSEL, (WPARAM) (int) inStart, (LPARAM) (int) inEnd);
}*/

/*----------------------------------
// ReplaceSelection ()
//---------------------------------*/
/*void LTextEditor_ReplaceSelection (LTextEditor* inEditor, i1* inString)
{
    SendMessage (inEditor->mHwndEdit, EM_REPLACESEL, 0, (LPARAM) inString);
}*/

/*----------------------------------
// ScrollToCursor ()
//---------------------------------*/
/*void LTextEditor_ScrollToCursor (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, EM_SCROLLCARET, 0, 0);
}*/

/*----------------------------------
// Scroll ()
//---------------------------------*/
/*void LTextEditor_Scroll (LTextEditor* inEditor, i4 inCharHor, i4 inLinesVer)
{
    SendMessage (inEditor->mHwndEdit, EM_LINESCROLL, (WPARAM) inCharHor, (LPARAM) inLinesVer);
}*/

/*----------------------------------
// DeleteSelection ()
//---------------------------------*/
/*void LTextEditor_DeleteSelection (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_CLEAR, 0, 0);
}*/

/*----------------------------------
// GetLineCount ()
//---------------------------------*/
/*i4 LTextEditor_GetLineCount (LTextEditor* inEditor)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_GETLINECOUNT, 0, 0);
}*/

/*----------------------------------
// GetLineIndex ()
//---------------------------------*/
/*i4 LTextEditor_GetLineIndex (LTextEditor* inEditor, i4 inLine)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_LINEINDEX, (WPARAM) inLine, 0);
}*/

/*----------------------------------
// GetLineLength ()
//---------------------------------*/
/*i4 LTextEditor_GetLineLength (LTextEditor* inEditor, i4 inIndex)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_LINELENGTH, (WPARAM) inIndex, 0);
}*/

/*---------------------------------
// GetLine ()
//--------------------------------*/
/*i4 LTextEditor_GetLine (LTextEditor* inEditor, i4 inLine, i1* inBuffer)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_GETLINE, (WPARAM) inLine, (LPARAM) inBuffer);
}*/

/*---------------------------------
// GetModify ()
//--------------------------------*/
/*Bool LTextEditor_GetModify (LTextEditor* inEditor)
{
    return (Bool) SendMessage (inEditor->mHwndEdit, EM_GETMODIFY, 0, 0);
}*/

/*---------------------------------
// SetModify ()
//--------------------------------*/
/*void LTextEditor_SetModify (LTextEditor* inEditor, Bool inFlag)
{
    SendMessage (inEditor->mHwndEdit, EM_SETMODIFY, (WPARAM) (UINT) inFlag, 0);
}*/

/*---------------------------------
// GetFirstVisibleLine ()
//--------------------------------*/
/*i4 LTextEditor_GetFirstVisibleLine (LTextEditor* inEditor)
{
    //line number or line index???? It should be tested...
    return SendMessage (inEditor->mHwndEdit, EM_GETFIRSTVISIBLELINE, 0, 0);
}*/

/*---------------------------------
// InstallReturnHandler ()
//--------------------------------*/
void LTextField_InstallReturnHandler (LTextField* inEditor, LTextField_THandler inHandler)
{
      inEditor->mReturnHandler = (LTextField_THandler) inHandler;
}
/*---------------------------------
// GetCursorLine ()
//--------------------------------*/
/*i4 LTextEditor_GetCursorLine (LTextEditor* inEditor)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_LINEFROMCHAR, (WPARAM) -1, 0);
}*/

/*--------------------------------
 * GetParentWindow() [CD020718]
 *--------------------------------
*/
LWindow* LTextField_GetParentWindow (LTextField* inEditor)
{
	return inEditor->mParentWindow;
}

/*-----------------------------------
// Enable ()
//----------------------------------*/
void LTextField_Enable (LTextField* inEditor, Bool inEnabled)
{
	 inEditor->mHwndEdit->setEnabled(inEnabled);
}

/*-----------------------------------
// QLeoLineEdit::QLeoLineEdit ()
//----------------------------------*/
QLeoLineEdit::QLeoLineEdit(QMainWindow *inWindow)  : QLineEdit(inWindow, "TextField")
{
	setFocusPolicy(QWidget::StrongFocus);

}

/*-----------------------------------
// QLeoLineEdit::~QLeoLineEdit ()
//----------------------------------*/
QLeoLineEdit::~QLeoLineEdit()
{

}

/*-------------------------------------
// QLeoLineEdit::focusInEvent ()
//------------------------------------*/
void QLeoLineEdit::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LTextField*)(((QLeoLineEdit*)this)->cLTextField));
	_LApplication_gGlobal.mWidgetFocusedId = ((LTextField*)(((QLeoLineEdit*)this)->cLTextField))->mID;

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


