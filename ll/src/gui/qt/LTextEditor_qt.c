/* ============================================================================
 *  LTextEditor_qt.c
 * ============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Jul 11, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/23 23:32:32 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.11 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qtextedit.h>
#include <qapplication.h>
#include <qmainwindow.h>

#include "LTextEditor.h"
#include "LTextEditor_qt.h"
#include "LWindow_qt.h"
#include "LGlobals_qt.h"

/*extern "C" {

#include "LString.h"

}*/

/* GLOBAL VARIABLES & FUNCTION PROTOTYPES*/

/*----------------------------------------------
// Create
//----------------------------------------------*/
LTextEditor *LTextEditor_Create( ui4 inStyle, LWindow *inWindow, ui4 inX, ui4 inY, ui4 inWidth, ui4 inHeight, Bool inFrameOn )
{
	LTextEditor		*theTextEditorPtr;
	LTextEditor		theTextEditor;
	i1 				theString[5];
	i1				theNameString[25] = "LTextEditor";
	static ui1 		sTextEditorNumber = 0;

	QObject *killerfilter = new KillerFilter();

	_itoa(sTextEditorNumber, theString);

#ifdef PRINT_DEBUG
	qDebug( "\n\r theString  %s", theString);
	qDebug( "\n\r sTextEditorNumber  %d", sTextEditorNumber);
#endif

	strcat(theNameString, theString);

#ifdef PRINT_DEBUG
	qDebug( "\n\r theNameString %s", theNameString);
#endif


	theTextEditor.mHwndEdit = new QLeoTextEdit(inWindow->mHwnd, theNameString);

	sTextEditorNumber++;

	/* check for call failure */
	if ( theTextEditor.mHwndEdit == NULL )
		Throw(LTextEditor_CANT_CREATE);


	theTextEditor.mHCaret = 0;
	theTextEditor.mHandler = NULL; /*to fix a bug...*/


	theTextEditor.mParentWindow = inWindow;

	theTextEditor.mHwndEdit->setTextFormat(Qt::PlainText);

	theTextEditor.mHwndEdit->move(inX, inY);
	theTextEditor.mHwndEdit->resize(inWidth, inHeight);


	theTextEditor.mHwndEdit->setWordWrap(QTextEdit::NoWrap);

	theTextEditor.mHwndEdit->installEventFilter(killerfilter);

	theTextEditor.mID = LTextEditor_ID;

	theTextEditor.mLCLKHandler = NULL;
	theTextEditor.mLDBLCLKHandler = NULL;
	theTextEditor.mRCLKHandler = NULL;
	theTextEditor.mKEYDOWNHandler = NULL;

	if (inFrameOn)
			theTextEditor.mHwndEdit->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : TextEditor");
#endif

		theTextEditorPtr = (LTextEditor* )LMemory_NewObject(LTextEditor, theTextEditor);

		theTextEditorPtr->mHwndEdit->cLTextEdit = theTextEditorPtr;


		QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

#ifdef PRINT_DEBUG
	qDebug( "\n\r Create TextEditor List Item 0x%x",  theTextEditorPtr);
#endif

		theWindowListItemPtr->mWidgetListPtr = theTextEditorPtr;
		theWindowListItemPtr->mID = LTextEditor_ID;

		inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);

		return theTextEditorPtr;
}


/*----------------------------------------------
// Destroy()
//----------------------------------------------*/
void LTextEditor_Destroy ( LTextEditor *inEditor )
{
	delete inEditor->mHwndEdit;
	LMemory_DeleteObject(&inEditor);
}


/*------------------------------------------------
// Resize ()
//------------------------------------------------*/
void LTextEditor_Resize (LTextEditor* inEditor, ui4 inWidth, ui4 inHeight)
{
	inEditor->mHwndEdit->resize(inWidth, inHeight);
}

/*------------------------------------------------
// Move ()
//------------------------------------------------*/
void LTextEditor_Move(LTextEditor* inEditor, ui4 inX, ui4 inY)
{
	inEditor->mHwndEdit->move(inX, inY);
}

/*--------------------------------------------------
// SetTextBuffer ()
//--------------------------------------------------*/
void LTextEditor_SetTextBuffer (LTextEditor* inEditor, const i1* inBuffer, ui4 inSize)
{
	inEditor->mHwndEdit->setText(inBuffer);
}

/*--------------------------------------------------
// GetTextBufferSize ()
//-------------------------------------------------*/
ui4 LTextEditor_GetTextBufferSize (LTextEditor* inEditor)
{
	QString theString;
	i4 theLength;

	theString = inEditor->mHwndEdit->text();

	theLength = (i4)theString.length();

	return theLength;
}

/*--------------------------------------------------
// GetTextBuffer ()
//-------------------------------------------------*/
void LTextEditor_GetTextBuffer (LTextEditor* inEditor, i1* outBuffer/*, ui4 inLength*/)
{

	QString theString;
	const i1 *theConvertedString;

	theString = inEditor->mHwndEdit->text();

	theConvertedString = theString.latin1();

	/*strncpy(outBuffer,(i1*)theConvertedString, inLength);*/
	strcpy(outBuffer,(i1*)theConvertedString/*, inLength*/);
}

/*-------------------------------------------------
// SetTextFont ()
//------------------------------------------------*/
void LTextEditor_SetTextFont (LTextEditor* inEditor, LTextEditor_TFont inFont)
{
	QFont theFont;

	switch (inFont) {

		case LTextEditor_SYSTEMFIXED:
		    theFont.setFamily( "Courier");
		    theFont.setBold(TRUE);
		    theFont.setPixelSize(14);
		break;

		case LTextEditor_SYSTEMVAR:
			//theFont.setFamily( "Times");
			theFont.setFamily( "Helvetica");
			theFont.setBold(TRUE);
			theFont.setPixelSize(12);
		break;

		case LTextEditor_ANSIFIXED :
			theFont.setFamily( "Courier");
			theFont.setWeight(QFont::Normal);
			theFont.setPixelSize(14);
		break;

		case LTextEditor_ANSIVAR :
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
void LTextEditor_SetFocus (LTextEditor *inEditor)
{
	inEditor->mHwndEdit->setFocus();
}

/*----------------------------------------
// GetFocus ()
//---------------------------------------*/
LTextEditor* LTextEditor_GetFocus ()
{
	LTextEditor*	theTextEditorPtr;
	const i1 *theNameString;


	if( _LApplication_gGlobal.mApplication->focusWidget() == NULL) {
#ifdef PRINT_DEBUG
	qDebug( "\n\r QLeoTextEdit : _LApplication_gGlobal.mApplication->focusWidget() = NULL");

#endif
		return NULL;
	}

		theNameString = ((QLeoTextEdit*)_LApplication_gGlobal.mWidgetFocused)->name();
#ifdef PRINT_DEBUG
	qDebug( "\n\r QLeoTextEdit : _LApplication_gGlobal.mApplication->focusWidget() =  0x%x   Name %s",  _LApplication_gGlobal.mApplication->focusWidget(),theNameString);

#endif
#ifdef PRINT_DEBUG
	qDebug( "\n\r QLeoTextEdit : %d",_LApplication_gGlobal.mApplication->focusWidget()->inherits("QLeoTextEdit") );


#endif

	if(((QLeoTextEdit*)_LApplication_gGlobal.mWidgetFocused)->inherits("QLeoTextEdit") ) {

#ifdef PRINT_DEBUG
	qDebug( "\n\r QLeoTextEdit inherits: %d",_LApplication_gGlobal.mApplication->focusWidget()->inherits("QLeoTextEdit") );
	qDebug( "\n\r QLeoTextEdit _LApplication_gGlobal.mWidgetFocused inherits: %d",((QLeoTextEdit*)_LApplication_gGlobal.mWidgetFocused)->inherits("QLeoTextEdit") );


#endif

		theTextEditorPtr = (LTextEditor*)(((QLeoTextEdit*)_LApplication_gGlobal.mWidgetFocused)->cLTextEdit);
	}
	else
		return NULL;


	return theTextEditorPtr;
}

/*----------------------------------------
// Undo ()
//--------------------------------------*/
void LTextEditor_Undo (LTextEditor* inEditor)
{
	inEditor->mHwndEdit->undo();
}

/*----------------------------------------
// Cut ()
//--------------------------------------*/
void LTextEditor_Cut (LTextEditor* inEditor)
{
	inEditor->mHwndEdit->cut();
}

/*----------------------------------------
// Copy ()
//--------------------------------------*/
void LTextEditor_Copy (LTextEditor* inEditor)
{
	inEditor->mHwndEdit->copy();
}

/*----------------------------------------
// Paste ()
//--------------------------------------*/
void LTextEditor_Paste (LTextEditor* inEditor)
{
	inEditor->mHwndEdit->paste();
}

/*----------------------------------------
// SelectAll ()
//--------------------------------------*/
void LTextEditor_SelectAll( LTextEditor *inEditor )
{
	inEditor->mHwndEdit->selectAll( TRUE );
}

/*----------------------------------------
// CanUndo ()
//---------------------------------------*/
Bool LTextEditor_CanUndo (LTextEditor* inEditor)
{
#ifdef PRINT_DEBUG
	qDebug( "\n\r b LTextEditor_CanUndo  inEditor 0x%x",inEditor);
#endif 		/* ??? */
	return inEditor->mHwndEdit->isUndoAvailable();

#ifdef PRINT_DEBUG
	qDebug( "\n\r a LTextEditor_CanUndo  inEditor 0x%x",inEditor);
#endif

}

/*---------------------------------------
// IsTextAvailable ()
//--------------------------------------*/
Bool LTextEditor_IsTextAvailable ()
{
	return FALSE;
}

/*----------------------------------------
// IsSelectionAvailable ()
//---------------------------------------*/
Bool LTextEditor_IsSelectionAvailable (LTextEditor* inEditor)
{
	return inEditor->mHwndEdit->hasSelectedText();
}

/*----------------------------------
// GetSelection ()
//---------------------------------*/
void LTextEditor_GetSelection (LTextEditor* inEditor, ui4* outStart, ui4* outEnd)
{
	int theIndexFrom;
	int theIndexTo;
	int theParaFrom;
	int theParaTo;
	i4 theIndex;
	i4 theCharactersNumber = 0;
	ui4 theCursorLine;
	ui4 theCursorIndex;


	inEditor->mHwndEdit->getSelection(&theParaFrom, &theIndexFrom, &theParaTo, &theIndexTo);
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r theParaFrom %d,   theIndexFrom %d", theParaFrom, theIndexFrom );
	qDebug( "\n\r theParaTo %d,   theIndexTo %d", theParaTo, theIndexTo );

#endif

	if (theParaFrom == -1 && theIndexFrom == -1 && theParaTo == -1 && theIndexTo == -1) {

		LTextEditor_GetCursorPos( inEditor, &theCursorLine, &theCursorIndex);



		for ( theIndex = 0; theIndex < theCursorLine; theIndex++) {

			theCharactersNumber += inEditor->mHwndEdit->paragraphLength(theIndex);

		}

		*outStart = i4(theCursorIndex) + theCharactersNumber;

		*outEnd = *outStart;

		return;

	}


	for ( theIndex = 0; theIndex < theParaFrom; theIndex++) {

		theCharactersNumber += inEditor->mHwndEdit->paragraphLength(theIndex);

	}

	*outStart = i4(theIndexFrom) + theCharactersNumber;


	for ( theIndex = i4(theParaFrom); theIndex < theParaTo; theIndex++) {

		theCharactersNumber += inEditor->mHwndEdit->paragraphLength(theIndex);

	}


	*outEnd = i4(theIndexTo) + theCharactersNumber;
	


}

/*----------------------------------
// SetSelection ()
//---------------------------------*/
void LTextEditor_SetSelection (LTextEditor* inEditor, ui4 inStart, ui4 inEnd)
{
	i4 theIndex;
	i4 theCharactersNumber = 0;
	int theStartPara = 0;
	int theStartIndex = 0;
	int theEndPara = 0;
	int theEndIndex = 0;
	Bool theFlagStart = TRUE;

#ifdef PRINT_DEBUG
	qDebug( "\n\r *********** START SETSELECTION ************ " );

	qDebug( "\n\r inStart %d,   inEnd %d", inStart, inEnd );
#endif

	for ( theIndex = 0; theIndex < inEditor->mHwndEdit->lines(); theIndex++) {

		theCharactersNumber += inEditor->mHwndEdit->paragraphLength(theIndex);

#ifdef PRINT_DEBUG
	qDebug( "\n\r theCharactersNumber %d", theCharactersNumber );
	qDebug( "\n\r lines %d", inEditor->mHwndEdit->lines() );
	qDebug( "\n\r theIndex %d", theIndex );
#endif

		if ( inStart == inEnd && (theCharactersNumber >= inStart || inEditor->mHwndEdit->lines()-1 == theIndex)) {

			theStartPara = theIndex;
			theStartIndex = inEditor->mHwndEdit->paragraphLength(theIndex) + inStart - theCharactersNumber;

#ifdef PRINT_DEBUG
	qDebug( "\n\r theStartPara %d,   theStartIndex %d", theStartPara, theStartIndex );
#endif



		}


		if (theCharactersNumber > inStart && theFlagStart) {

			theStartPara = theIndex;
			theStartIndex = inEditor->mHwndEdit->paragraphLength(theIndex) + inStart - theCharactersNumber;

#ifdef PRINT_DEBUG
	qDebug( "\n\r theStartPara %d,   theStartIndex %d", theStartPara, theStartIndex );
#endif

			theFlagStart = FALSE;

		}

		if (theCharactersNumber > inStart && theCharactersNumber > inEnd /*&& theFlagStartEnd*/) {

			theEndPara = theIndex;
			theEndIndex = inEditor->mHwndEdit->paragraphLength(theIndex) - (theCharactersNumber - inEnd);
#ifdef PRINT_DEBUG
	qDebug( "\n\r theStartPara %d,   theStartIndex %d", theStartPara, theStartIndex );
	qDebug( "\n\r theEndPara %d,   theEndIndex %d", theEndPara, theEndIndex );

#endif

			break;

		}

		if (theCharactersNumber > inEnd - 1) {

			theEndPara = theIndex;
			theEndIndex = inEditor->mHwndEdit->paragraphLength(theIndex) - (theCharactersNumber - inEnd);

#ifdef PRINT_DEBUG
	qDebug( "\n\r theStartPara %d,   theStartIndex %d", theStartPara, theStartIndex );
	qDebug( "\n\r theEndPara %d,   theEndIndex %d", theEndPara, theEndIndex );

#endif

			break;

		}


	}
			
	if ( inStart == inEnd ) {
		inEditor->mHwndEdit->setCursorPosition(theStartPara, theStartIndex);
		inEditor->mHwndEdit->setSelection(theStartPara, theStartIndex, theStartPara, theStartIndex);
	}
	else
		inEditor->mHwndEdit->setSelection(theStartPara, theStartIndex, theEndPara, theEndIndex);
		
#ifdef PRINT_DEBUG
	qDebug( "\n\r *********** END SETSELECTION *******************" );

#endif

}

/*----------------------------------
// ReplaceSelection ()
//---------------------------------*/
void LTextEditor_ReplaceSelection (LTextEditor* inEditor, const i1* inString, ui4 inSize)
{

	int theIndexFrom;
	int theIndexTo;
	int theParaFrom;
	int theParaTo;
	i1 theReplacedString[inSize+1];


	strncpy(theReplacedString, inString, inSize);
	theReplacedString[inSize] = 0;
#ifdef PRINT_DEBUG
	qDebug( "\n\r\n\r LTextEditor_ReplaceSelection" );
	qDebug( "\n\r theReplacedString ****%s****", theReplacedString );
#endif




	inEditor->mHwndEdit->getSelection(&theParaFrom, &theIndexFrom, &theParaTo, &theIndexTo);

#ifdef PRINT_DEBUG
	qDebug( "\n\r theParaFrom %d theIndexFrom %d", theParaFrom, theIndexFrom );
	qDebug( "\n\r theParaTo %d theIndexTo %d", theParaTo, theIndexTo );
#endif

	if (theParaFrom == -1 && theIndexFrom == -1 && theParaTo == -1 && theIndexTo == -1) {

		LTextEditor_GetCursorPos( inEditor, (ui4*)&theParaFrom, (ui4*)&theIndexFrom );

	}

	inEditor->mHwndEdit->removeSelectedText();

	inEditor->mHwndEdit->insertAt(/*inString*/theReplacedString, theParaFrom, theIndexFrom);

	// Inserted to fix tab insertion bug. To check regressions!
	inEditor->mHwndEdit->setSelection(theParaFrom, theIndexFrom + inSize, theParaFrom, theIndexFrom + inSize);
#ifdef PRINT_DEBUG
	qDebug( "\n\r theParaFrom %d theIndexFrom+inSize %d", theParaFrom, theIndexFrom+ inSize );
#endif

}

/*----------------------------------
// ScrollToCursor ()
//---------------------------------*/
void LTextEditor_ScrollToCursor (LTextEditor* inEditor)
{
	inEditor->mHwndEdit->ensureCursorVisible();

}

/*----------------------------------
// Scroll ()
//---------------------------------*/
void LTextEditor_Scroll (LTextEditor* inEditor, i4 inCharHor, i4 inLinesVer)
{
#ifdef PRINT_DEBUG
	qDebug( "\n\r x %d, y %d ", inEditor->mHwndEdit->contentsX(), inEditor->mHwndEdit->contentsY()  );
#endif

	int theFontHeight = inEditor->mHwndEdit->fontMetrics().height();

	QChar theChar('a');
	int theCharLength = inEditor->mHwndEdit->fontMetrics().width(theChar);

	inEditor->mHwndEdit->setContentsPos( (inCharHor + inEditor->mHwndEdit->contentsX()/theCharLength)*theCharLength , (inLinesVer + inEditor->mHwndEdit->contentsY()/theFontHeight)*theFontHeight);

}

/*----------------------------------
// DeleteSelection ()
//---------------------------------*/
void LTextEditor_DeleteSelection (LTextEditor* inEditor)
{
	inEditor->mHwndEdit->del();
}

/*----------------------------------
// GetLineCount ()
//---------------------------------*/
ui4 LTextEditor_GetLineCount (LTextEditor* inEditor)
{
	i4 theLines;

	return ( theLines = (i4)(inEditor->mHwndEdit->lines()) );

}

/*----------------------------------
// GetLineIndex ()
//---------------------------------*/
ui4 LTextEditor_GetLineIndex (LTextEditor* inEditor, ui4 inLine)
{
	ui4 theIndex;
	ui4 theCharactersNumber = 0;
	
	if (inLine > 0) {

	for ( theIndex = 0; theIndex < inLine; theIndex++) {
		theCharactersNumber += inEditor->mHwndEdit->paragraphLength(theIndex);

	}

#ifdef PRINT_DEBUG
	qDebug( "\n\r theCharactersNumber %d",  theCharactersNumber);
#endif
	}

	return theCharactersNumber;

}

/*----------------------------------
// GetLineLength ()
//---------------------------------*/
ui4 LTextEditor_GetLineLength (LTextEditor* inEditor, ui4 inIndex)
{
	ui4 theCharactersNumber = 0;
	ui4 theLine;
#ifdef PRINT_DEBUG
	qDebug( "\n\r ********* START GETLINELENGTH **********");
		qDebug( "\n\r inIndex 0x%x", inIndex);

#endif

	for( theLine = 0; theLine <= inEditor->mHwndEdit->paragraphs() && inIndex >= theCharactersNumber; theLine++) {

		theCharactersNumber += inEditor->mHwndEdit->paragraphLength(theLine);
#ifdef PRINT_DEBUG
	qDebug( "\n\r paragraphLength(%d) %d", theLine, inEditor->mHwndEdit->paragraphLength(theLine));

	qDebug( "\n\r theCharactersNumber %d",theCharactersNumber);
#endif

	}

#ifdef PRINT_DEBUG
	qDebug( "\n\r theLine %d, theCharactersNumber %d, lines %d",  theLine, theCharactersNumber, inEditor->mHwndEdit->paragraphs());
#endif

#ifdef PRINT_DEBUG
	qDebug( "\n\r ********* END GETLINELENGTH **********");
#endif
	// If the Index > theCharactersNumber but theLine > paragraphs is returned theLine=paragraphs (???)
	if( (i4)(theCharactersNumber - inIndex) >= 0 )
		return ( theCharactersNumber - inIndex );
	else 
		return 1;


}

/*---------------------------------
// GetLine ()
//--------------------------------*/
ui4 LTextEditor_GetLine (LTextEditor* inEditor, ui4 inLine, i1* outBuffer)
{
	QString theString;
	/* const */ i1 *theConvertedString;
#ifdef PRINT_DEBUG
	qDebug( "\n\r ***** START GETLINE *******");
#endif

	theString = inEditor->mHwndEdit->text( int(inLine) );

	theConvertedString = (i1*)theString.latin1();
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r b theConvertedString ****%s****, strlen(theConvertedString)-1 %d",  theConvertedString, strlen(theConvertedString)-1);
#endif


	//strcpy(outBuffer, (i1*)theConvertedString);
	/*if ( theConvertedString[0] == 0 )
		outBuffer[0] = 0;
	else*/
	if ( strlen(theConvertedString)-1 == 0 ) {
		strncpy(outBuffer, (i1*)theConvertedString, strlen(theConvertedString));
		outBuffer[1] = '\0';
	}
	else
		strncpy(outBuffer, (i1*)theConvertedString, strlen(theConvertedString)-1);

#ifdef PRINT_DEBUG
	qDebug( "\n\r a theConvertedString ****%s****, theConvertedString[0] 0x%x",  theConvertedString, theConvertedString[0]);
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r ***** END GETLINE *******");
#endif

#endif

}

/*---------------------------------
// GetModify ()
//--------------------------------*/
Bool LTextEditor_GetModify (LTextEditor* inEditor)
{
	return inEditor->mHwndEdit->isModified();
}

/*---------------------------------
// SetModify ()
//--------------------------------*/
void LTextEditor_SetModify (LTextEditor* inEditor, Bool inFlag)
{
	inEditor->mHwndEdit->setModified(inFlag);
}

/*---------------------------------
// GetFirstVisibleLine ()
//--------------------------------*/
ui4 LTextEditor_GetFirstVisibleLine (LTextEditor* inEditor)
{

}

/*---------------------------------
// InstallHandler ()
//--------------------------------*/
void LTextEditor_InstallHandler( LTextEditor *inEditor, LTextEditor_TEventID inEventID, Bool inLater, LTextEditor_THandler inHandler )
{
    switch ( inEventID ) {

		case LTextEditor_MOUSE_LCLK :

			inEditor->mLCLKHandler = ( _LTextEditor_THandler ) inHandler;
			inEditor->mLCLKLater = inLater;

		break;

		case LTextEditor_MOUSE_LDBLCLK :

			inEditor->mLDBLCLKHandler = ( _LTextEditor_THandler ) inHandler;
			inEditor->mLDBLCLKLater = inLater;

		break;

		case LTextEditor_MOUSE_RCLK :

			inEditor->mRCLKHandler = ( _LTextEditor_THandler ) inHandler;
			inEditor->mRCLKLater = inLater;

		break;

		case LTextEditor_KEYDOWN :

			inEditor->mKEYDOWNHandler = ( _LTextEditor_THandler ) inHandler;
			inEditor->mKEYDOWNLater = inLater;

		break;
		
		default :
		
		break;

	}

}

/*---------------------------------
// GetCursorPos ()
//--------------------------------*/
void LTextEditor_GetCursorPos( LTextEditor* inEditor, ui4* outLine, ui4* outCol )
{
	int thePara;
	int theIndex;

	inEditor->mHwndEdit->getCursorPosition( &thePara, &theIndex );

	*outLine = (ui4)thePara;
	*outCol = (ui4)theIndex;

}

/*---------------------------------
// GetParentWindow ()
//--------------------------------*/
LWindow* LTextEditor_GetParentWindow(LTextEditor *inEditor)
{
    return inEditor->mParentWindow;
}

/*------------------------------------------
// SetSelectionColor()
//-----------------------------------------*/
void LTextEditor_SetSelectionColor (LTextEditor* inEditor, ui4 inColor)
{
	//const QColor theBackColor(0,0,180);
	int theRed   =   inColor & 0x000000ff;
	int theGreen = ( inColor & 0x0000ff00 ) >> 8;
	int theBlue  = ( inColor & 0x00ff0000 ) >> 16;

	QColor theColor(theRed, theGreen, theBlue);

	//inEditor->mHwndEdit->setSelectionAttributes( 1, theBackColor, FALSE );
	inEditor->mHwndEdit->setColor(theColor);

}

/*----------------------------------
// CharFromPos ()
//--------------------------------*/
void LTextEditor_CharFromPos( LTextEditor *inEditor, ui4 inX, ui4 inY, ui4 *outCharIndex )
{
	/* int QTextEdit::charAt ( const QPoint & pos, int * para ) const

Returns the index of the character (relative to its paragraph) at position pos (in contents coordinates). 
	If para is not 0, *para is set to
the character's paragraph.  */
	QPoint thePoint;
	int thePara;
	int theCharIndex;
	ui4 theLineIndex;
	
	thePoint.setX( int(inX) );
	thePoint.setY( int(inY) );
	
	theCharIndex = inEditor->mHwndEdit->charAt( thePoint, &thePara );

	theLineIndex = LTextEditor_GetLineIndex( inEditor, ui4( thePara ) );
	
	*outCharIndex = theLineIndex + ui4( theCharIndex );
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r theLineIndex %d  theCharIndex %d  *outCharIndex %d",  theLineIndex, theCharIndex, *outCharIndex);
#endif

}

/*-----------------------------------
// SetSelectionStatus ()
//---------------------------------*/
void LTextEditor_SetSelectionStatus (LTextEditor* inEditor, Bool inVisible)
{
	//inEditor->mHwndEdit->selectAll( inVisible );
}


/*-------------------------------------
// QLeoTextEdit::QLeoTextEdit ()
//------------------------------------*/
QLeoTextEdit::QLeoTextEdit(QMainWindow *inWindow, i1* inString)  : QTextEdit(inWindow, inString/*"TextEditor"*/)
{
	/* inserito per testare FocusInEvent */
	setFocusPolicy(QWidget::StrongFocus);
}

/*-------------------------------------
// QLeoTextEdit::~QLeoTextEdit ()
//------------------------------------*/
QLeoTextEdit::~QLeoTextEdit()
{

}

/*-------------------------------------
// QLeoTextEdit::slotReturnPressed ()
//------------------------------------*/
void QLeoTextEdit::slotReturnPressed()
{
       int para, index;
       QString  theString, theStringSpace;



       getCursorPosition ( &para, &index );


		theString = text( para-1 );

                int  theIndent = 0;
		int  theTabIndent = 0;


               uint i = 0;

                while ( theString.at(i) == ' ' || theString.at(i) == '\t') {

                	if (theString.at(i) == 32) {
				theIndent++;
				if ( i < theString.length() )
                			theStringSpace.append(' ');

			}

                       	if (theString.at(i) == '\t') {
				theTabIndent++;
				if ( i < theString.length() )
                			theStringSpace.append('\t');

			}
		       

		       
			i++;
		}


        if (theString.at(i) == NULL) {

		theStringSpace.remove(theStringSpace.length()-1, 1);

	}


	if (theIndent || theTabIndent)
		insert(theStringSpace, 0, 1, 1);


}

/*-------------------------------------
// QLeoTextEdit::mousePressEvent ()
//------------------------------------*/
void QLeoTextEdit::mousePressEvent( QMouseEvent *inEvent )
{
	LTextEditor_THandler theHandlePtr;
	
	QTextEdit::mousePressEvent( inEvent );

	if ( (( QMouseEvent* )inEvent)->type() == QEvent::MouseButtonPress ) {

		switch( (( QMouseEvent* )inEvent)->button() ) {

			case Qt::LeftButton :

				if ( ((LTextEditor *)(this->cLTextEdit))->mLCLKHandler != NULL ) {

					if ( ((LTextEditor *)(this->cLTextEdit))->mLCLKLater ) {

						theHandlePtr = ( LTextEditor_THandler )( ((LTextEditor *)(this->cLTextEdit))->mLCLKHandler );

						theHandlePtr( (LTextEditor *)(this->cLTextEdit), (( QMouseEvent* )inEvent)->x(), (( QMouseEvent* )inEvent)->y() );

					}
				}

			break;

			case Qt::RightButton :

				if ( ((LTextEditor *)(this->cLTextEdit))->mRCLKHandler != NULL ) {

					if ( ((LTextEditor *)(this->cLTextEdit))->mRCLKHandler ) {

						theHandlePtr = ( LTextEditor_THandler )( ((LTextEditor *)(this->cLTextEdit))->mRCLKHandler );

						theHandlePtr( (LTextEditor *)(this->cLTextEdit), (( QMouseEvent* )inEvent)->x(), (( QMouseEvent* )inEvent)->y());

					}
				}

			break;

			default :
				/* ERROR */
			break;

		}
	}
	else
		if ( (( QMouseEvent* )inEvent)->type() == QEvent::MouseButtonDblClick ) {

			if ( ((LTextEditor *)(this->cLTextEdit))->mLDBLCLKHandler != NULL ) {

				if ( ((LTextEditor *)(this->cLTextEdit))->mLDBLCLKLater ) {

					theHandlePtr = ( LTextEditor_THandler )( ((LTextEditor *)(this->cLTextEdit))->mLDBLCLKHandler );

					theHandlePtr( (LTextEditor *)(this->cLTextEdit), (( QMouseEvent* )inEvent)->x(), (( QMouseEvent* )inEvent)->y());

				}
			}

		}
		else {
			/* ERROR */
		}

#ifdef PRINT_DEBUG
	//qDebug( "Mouse pressed" );
#endif
}

/*-------------------------------------
// QLeoTextEdit::keyPressEvent ()
//------------------------------------*/
void QLeoTextEdit::keyPressEvent( QKeyEvent *inEvent )
{
	LTextEditor_THandler theHandlePtr;
	int theVirtualKey;

	QKeyEvent *theKey = ( QKeyEvent * )inEvent;

	QTextEdit::keyPressEvent( inEvent );

	if ( ((LTextEditor *)(this->cLTextEdit))->mKEYDOWNHandler != NULL ) {

		if ( ((LTextEditor *)(this->cLTextEdit))->mKEYDOWNLater ) {

			theHandlePtr = ( LTextEditor_THandler )( ((LTextEditor *)(this->cLTextEdit))->mKEYDOWNHandler );

			theVirtualKey = _VirtKey2LTxtEdt( theKey->key() );

#ifdef PRINT_DEBUG
	qDebug( "\n\r theVirtualKey 0x%x", theVirtualKey );
#endif

			if ( theVirtualKey == 0xffffffff )
				theHandlePtr( (LTextEditor *)(this->cLTextEdit), i4( theKey->key() ) );
			else
				theHandlePtr( (LTextEditor *)(this->cLTextEdit), i4( theVirtualKey ) );

		}
	}


#ifdef PRINT_DEBUG
	//qDebug( "Key pressed" );
#endif
}


/*-------------------------------------
// QLeoTextEdit::focusInEvent ()
//------------------------------------*/
void QLeoTextEdit::focusInEvent( QFocusEvent *inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LTextEditor*)(((QLeoTextEdit*)this)->cLTextEdit));
	_LApplication_gGlobal.mWidgetFocusedId = ((LTextEditor*)(((QLeoTextEdit*)this)->cLTextEdit))->mID;

#ifdef PRINT_DEBUG
	qDebug( "focusInEvent 0x%x   _gWidgetFocusedId 0x%x",  _LApplication_gGlobal.mWidgetFocused, _LApplication_gGlobal.mWidgetFocusedId);
#endif

}

/*-------------------------------------
// QLeoTextEdit::mouseMoveEvent ()
//------------------------------------*/
void QLeoTextEdit::mouseMoveEvent( QMouseEvent *inEvent )
{

}

/*-------------------------------------
// _itoa ()
//------------------------------------*/
void  _itoa(int n, i1 *s)
{
	int i, sign;

	if ((sign = n) < 0)
		n = -n;
	i = 0;

	do {
		s[i++] = n % 10 +'0';

	} while ((n/=10) > 0);

	if (sign < 0)
		s[i++] = '-';
	
	s[i] = '\0';
	
#ifdef PRINT_DEBUG
	qDebug( "s %s",  s);
#endif

	_reverse(s);
	
#ifdef PRINT_DEBUG
	qDebug( "s %s",  s);
#endif

}

/*-------------------------------------
// _reverse ()
//------------------------------------*/
void  _reverse(i1 *s)
{
	int c, i, j;
	
	for ( i=0, j = strlen(s) - 1; i<j; i++, j-- ) {
	
	
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/*-------------------------------------
// KillerFilter::eventFilter ()
//------------------------------------*/
bool KillerFilter::eventFilter(QObject * object, QEvent * event)
{
			QTextEdit *theEdit;
			int para, index;
			static int  sCursorVerticalPosition = -1;
			int theNumberOfParagraph = -1;
			LTextEditor_THandler theHandlePtr;
			int theVirtualKey;


			theEdit = (QTextEdit*)object;
			
			//theEdit = ((LTextEditor*)(((QLeoTextEdit*)object)->cLTextEdit));

			if (event->type() == QEvent::KeyPress ) {
			
#ifdef PRINT_DEBUG
	//qDebug( "((LTextEditor*)(((QLeoTextEdit*)theEdit)->cLTextEdit))->mKEYDOWNHandler 0x%x",  ((LTextEditor*)(((QLeoTextEdit*)theEdit)->cLTextEdit))->mKEYDOWNHandler);
#endif
				QKeyEvent *theKey = (QKeyEvent *)event;
				
				
				if (((LTextEditor*)(((QLeoTextEdit*)theEdit)->cLTextEdit))->mKEYDOWNHandler != NULL)
				{
					if ( !(((LTextEditor*)(((QLeoTextEdit*)theEdit)->cLTextEdit))->mKEYDOWNLater) ) {
#ifdef PRINT_DEBUG
	//qDebug( "KeyLater 0x%x",  (((LTextEditor*)(((QLeoTextEdit*)theEdit)->cLTextEdit))->mKEYDOWNLater) );
#endif

						theHandlePtr = (LTextEditor_THandler)((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mKEYDOWNHandler;

						theVirtualKey = _VirtKey2LTxtEdt( theKey->key() );

#ifdef PRINT_DEBUG
	qDebug( "\n\r theVirtualKey 0x%x  theKey->key() 0x%x", theVirtualKey, theKey->key() );
#endif

						if ( theVirtualKey == 0xffffffff )
							theVirtualKey = theKey->key();
							
#ifdef PRINT_DEBUG
	qDebug( "\n\r theVirtualKey 0x%x  theKey->key() 0x%x", theVirtualKey, theKey->key() );
#endif

						
						if( theHandlePtr( (LTextEditor*)(((QLeoTextEdit*)theEdit)->cLTextEdit), theVirtualKey ) )
							return TRUE;
						else
							return FALSE;
					}
					else
						return FALSE;
				}
				else 
					return FALSE;
			}
			else
				return FALSE;
}

/*--------------------------------
// _VirtKey2LTxtEdt ()
//------------------------------*/
i4 _VirtKey2LTxtEdt (int inVirtKey)
{
	if (inVirtKey==0x1013)
		return LTextEditor_UP;
	if (inVirtKey==0x1015)
		return LTextEditor_DOWN;
	if (inVirtKey==0x1012)
		return LTextEditor_LEFT;
	if (inVirtKey==0x1014)
		return LTextEditor_RIGHT;
	if (inVirtKey==4119)
		return LTextEditor_PAGEDOWN;
	if (inVirtKey==4118)
		return LTextEditor_PAGEUP;
	if (inVirtKey==4113)
		return LTextEditor_END;
	if (inVirtKey==4112)
		return LTextEditor_HOME;

	/*RichEdit v2.0*/
	if (inVirtKey==4103)
		return LTextEditor_DELETE;
	if (inVirtKey==0x1004)
		return LTextEditor_RETURN;
	if (inVirtKey==0x1003)
		return LTextEditor_BACKSPACE;
		
	/*if (inVirtChar==VK_RETURN)
		return LTextEditor_RETURN;*/
	/*if (inVirtChar==VK_TAB)
		return LTextEditor_TAB;*/
	//if (inVirtChar==VK_BACK)
		//return LTextEditor_BACKSPACE;

	return 0xffffffff;
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


