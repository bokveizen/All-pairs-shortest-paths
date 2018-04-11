/* =============================================================================
 *  LStaticText_qt.c
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

#include <qtextedit.h>
#include <qlabel.h>

#include "LStaticText.h"
#include "LStaticText_qt.h"
#include "LWindow_qt.h"

/*----------------------------
// Create 
//--------------------------*/
LStaticText *LStaticText_Create( LWindow *inWindow, i1* inString, LStaticText_TAlign inMode, ui4 inX, ui4 inY, ui4 inDX, ui4 inDY )
{
    LStaticText	*theStaticTextPtr;
    LStaticText	theStaticText;
    QColor theBackgroundColor;
    i1 theWrappedString[200];
	
	theStaticText.mHwnd = new QLeoStaticText((QWidget*)inWindow->mHwnd, "StaticText");

	_WrapStaticText(theStaticText.mHwnd, inString, theWrappedString, inDX);

	/* check for call failure */
	if (theStaticText.mHwnd == NULL)
		Throw(LStaticText_CANT_CREATE);

	theStaticText.mHwnd->setText(theWrappedString);

	switch(inMode) {

	    case LStaticText_LEFT :
	         theStaticText.mHwnd->setAlignment( Qt::AlignLeft );
	    break;

	    case LStaticText_CENTER :
	         theStaticText.mHwnd->setAlignment( Qt::AlignHCenter );
	    break;

	    case LStaticText_RIGHT :
	         theStaticText.mHwnd->setAlignment( Qt::AlignRight );
	    break;

	    default :
	    	return NULL;
	    break;

	}

	theStaticText.mHwnd->resize(inDX, inDY);
	theStaticText.mHwnd->move(inX,inY);

	theStaticText.mParentWindow = inWindow;

	theStaticText.mID = LStaticText_ID;

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Static Text");
#endif
	theStaticTextPtr = (LStaticText* )LMemory_NewObject(LStaticText, theStaticText);

	theStaticText.mHwnd->cLStaticText = theStaticTextPtr;


	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theStaticTextPtr;
	theWindowListItemPtr->mID = LStaticText_ID;

	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);

	return  theStaticTextPtr;

}

/*--------------------------------------
// Destroy
//-------------------------------------*/
void LStaticText_Destroy( LStaticText *inText )
{
	delete inText->mHwnd;
	LMemory_DeleteObject(&inText);
}

/*--------------------------------------
// SetFont ()
//-------------------------------------*/
void LStaticText_SetFont(LStaticText* inText, LStaticText_TFont inFont)
{
	QFont theFont;
	
	switch (inFont) {
		
		case LStaticText_ANSIFIXED:
		    theFont.setFamily("Courier");
		break;
		
		case LStaticText_ANSIVAR:
			theFont.setFamily("Times");
		break;
		
		case LStaticText_SYSTEMFIXED:
			theFont.setFamily("Courier");
		break;
		
		case LStaticText_SYSTEMVAR:
			theFont.setFamily("Helvetica");
		break;
		
		default :
			theFont.setFamily("Times");
		break;
	}
	
	inText->mHwnd->setFont(theFont);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LStaticText_Move (LStaticText* inText, i4 inX, i4 inY)
{
    inText->mHwnd->move(int(inX), int(inY));
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LStaticText_Resize (LStaticText* inText, ui4 inWidth, ui4 inHeight)
{
     inText->mHwnd->resize(int(inWidth), int(inHeight));
}

/*----------------------------
// GetParentWindow ()
//---------------------------*/
LWindow* LStaticText_GetParentWindow (LStaticText* inText)
{
	return inText->mParentWindow;
}


/*-------------------------------------------
// QLeoStaticText::QLeoStaticText ()
//-------------------------------------------*/
QLeoStaticText::QLeoStaticText(QWidget* inWidget, i1* inString)  : QLabel(inWidget, inString)
{
	setFocusPolicy(QWidget::StrongFocus);

}

/*------------------------------------------
// QLeoStaticText::~QLeoStaticText ()
//------------------------------------------*/
QLeoStaticText::~QLeoStaticText()
{

}

/*-------------------------------------
// QLeoStaticText::focusInEvent ()
//------------------------------------*/
void QLeoStaticText::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LStaticText*)(((QLeoStaticText*)this)->cLStaticText));
	_LApplication_gGlobal.mWidgetFocusedId = ((LStaticText*)(((QLeoStaticText*)this)->cLStaticText))->mID;

#ifdef PRINT_DEBUG
	qDebug( "focusInEvent 0x%x   _gWidgetFocusedId 0x%x",  _LApplication_gGlobal.mWidgetFocused, _LApplication_gGlobal.mWidgetFocusedId);
#endif



}


/*----------------------------
// _WrapStaticText ()
//---------------------------*/
void _WrapStaticText(QLabel *inStatusBar, i1 *inString, i1 *outString, i4 inDX)
{

	i4 theIndex, theOldTextIndex;
	i4 theIncrement = 0;
	i4 theReminderLength = 0;
	i4 theLastSpace = 0;
	i4 theOldLastSpace = 0;
	i4 i,n;
	i4 theTextWrap;
	i1 *thePartialString;

#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : inDX %d", inDX);
#endif
	
	
	thePartialString = outString;
	outString[0] = ' ';
	for (theIndex = 1, theOldTextIndex=0; theOldTextIndex < strlen(inString)/*theIndex < strlen(inString)+theIncrement*/; theIndex++, theOldTextIndex++) {


		outString[theIndex] = inString[theOldTextIndex];
		outString[theIndex+1] = '\0';
		
#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : outString \n\r%s", outString);
#endif
		
		//if ( strlen(theReturnString)+1 > inDX+theReminderLength ) {

		if ( outString[theIndex] == ' ' /*!(isgraph(outString[theIndex])*/ ) {

			theLastSpace = theIndex;
			theOldLastSpace = theOldTextIndex;
			
#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : theLastSpace %d   theOldLastSpace %d", theLastSpace, theOldLastSpace);
	//qDebug( "\n\r STATIC  : outString[theLastSpace-1] %c ", outString[theLastSpace-1]);
	//qDebug( "\n\r STATIC  : inString[theOldLastSpace-1] %c ", inString[theOldLastSpace-1]);

#endif

		}

#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : inStatusBar->fontMetrics().width(thePartialString)= %d inDX= %d", inStatusBar->fontMetrics().width(thePartialString), inDX);
	//qDebug( "\n\r STATIC  : thePartialString \n\r%s", thePartialString);
	//qDebug( "\n\r STATIC  : theLastSpace %d   theIndex-1 %d", theLastSpace, theIndex-1);
#endif
		
		if ( inStatusBar->fontMetrics().width(thePartialString)/*inStatusBar->fontMetrics().width(outString)-theReminderLength*/ > inDX/*-10*/) {

#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : inStatusBar->fontMetrics().width(thePartialString)= %d, theIndex = %d", inStatusBar->fontMetrics().width(thePartialString),theIndex);
	//qDebug( "\n\r STATIC  : theLastSpace %d   theIndex-1 %d", theLastSpace, theIndex-1);
#endif
			if ( theLastSpace == theIndex-1 ) {

				theIndex--;//
				theReminderLength = inStatusBar->fontMetrics().width(outString);

				outString[theIndex++] = '\n';
				outString[theIndex++] = '\r';
				outString[theIndex] = inString[theOldTextIndex];
				
				thePartialString = &outString[theIndex];

				theIncrement += /*2*/1;


			}
			else {

#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : ( theLastSpace == theIndex )");
	//qDebug( "\n\r STATIC  : theLastSpace %d   theIndex %d", theLastSpace, theIndex);
	//qDebug( "\n\r STATIC  : outString[theLastSpace] %c ", outString[theLastSpace]);
	//qDebug( "\n\r STATIC  : inString[theOldLastSpace] %c ", inString[theOldLastSpace]);
#endif
				theTextWrap = theIndex;
				
				theIndex = theLastSpace/*+1*/;
				outString[theIndex++] = '\n';
				outString[theIndex++] = '\r';
				
				thePartialString = &outString[theIndex];

				for (i = theLastSpace, n = theOldLastSpace+1; i < theTextWrap; i++, n++) {



					//outString[i] = ' ';
					outString[(theIndex++)] = inString[n];
					//theIndex++;

#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : n %d i %d ", n, i);
	//qDebug( "\n\r STATIC  : outString[theIndex] %c , theIndex %d ", outString[theIndex -1], theIndex-1);
	//qDebug( "\n\r STATIC  : inString[n] %c ", inString[n]);
	//qDebug( "\n\r STATIC  : outString[theIndex-1] %c ", outString[theIndex-1]);
#endif

				}

#ifdef PRINT_DEBUG
	//qDebug( "\n\r STATIC  : n %d  ", n);
#endif
				
				theIndex--;
				theReminderLength = inStatusBar->fontMetrics().width(outString);

				//thePartialString = &outString[theTextWrap - 2];
				
#ifdef PRINT_DEBUG

	//qDebug( "\n\r STATIC  : thePartialString \n\r%s", thePartialString);
	//qDebug( "\n\r STATIC  : theTextWrap %d ", theTextWrap);

#endif

				theIncrement += 1/*2*/;

			}

		}

	}
	
	outString[theIndex+1] = '\0';


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

