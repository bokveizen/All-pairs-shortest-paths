/* ============================================================================
 *  LWindow_qt.c
 * ============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Apr 16, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/23 23:32:32 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.10 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qmainwindow.h>
#include <qapplication.h>
#include <qsize.h>
#include <qworkspace.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qframe.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qtextedit.h>
#include <qscrollbar.h>
#include <qsizepolicy.h>
#include <qcursor.h>

#include "LWindow.h"
#include "LWindow_qt.h"
#include "LApplication_qt.h"
#include "LDraw.h"
#include "LDraw_qt.h"
#include "LScrollBar.h"
#include "LScrollBar_qt.h"
#include "LCheckBox.h"
#include "LGroupBox.h"
#include "LListBox.h"
#include "LProgressBar.h"
#include "LPushButton.h"
#include "LRadioButton.h"
#include "LStaticText.h"
#include "LStatusBar.h"
#include "LTextEditor.h"
#include "LTextField.h"
#include "LSizeBox.h"
#include "LThread.h"
#include "LCursor_qt.h"

extern "C" {

#include "LDebug.h"

}

typedef struct tagCustomData {

	LWindow *mWindow;
	i1		mAction;

} tCustomData;

tCustomData gCustomData[100];
i1 theCustomCounter = 0;



typedef Bool (*_LWindow_TResizeHandler) (LWindow*, ui4, ui4);

/*----------------------------------------------
// Create
//----------------------------------------------*/
LWindow *LWindow_Create( LWindow_TWindowType inWindowType, ui4 inStyle )
{
	LWindow		*theWindowPtr;
	LWindow		theWindow; 
	
	switch (inWindowType) {	

	case LWindow_REGULAR:

            if ((inStyle & LWindow_Caption) && (inStyle & LWindow_MinMaxBox)) {
            	theWindow.mType = LWindow_REGULAR;
            	theWindow.mHwnd = new QChildWindow(0);
            }
            else
              if (inStyle & LWindow_Caption ) {
            	theWindow.mType = LWindow_REGULAR;
            	theWindow.mHwnd = new QChildWindow(Qt::WStyle_Customize| Qt::WStyle_SysMenu |
                                                Qt::WStyle_Title);
            }
            else
            {
            	theWindow.mType = LWindow_REGULAR;
            	theWindow.mHwnd = new QChildWindow(Qt::WStyle_Customize);
            }

	
	break;

        case LWindow_FLOATING:


            if (inStyle & LWindow_Caption) {
            	
            	 theWindow.mType = LWindow_FLOATING;
           	 theWindow.mHwnd = new QChildWindow(Qt::WStyle_Customize | Qt::WStyle_SysMenu |
                                               Qt::WStyle_Tool | Qt::WStyle_Title);

            	
            }
            else {
            	theWindow.mType = LWindow_FLOATING;
            	theWindow.mHwnd = new QChildWindow(Qt::WStyle_Customize |  Qt::WStyle_Tool | Qt::WStyle_DialogBorder);
            }


            break;
    }

	
	/* check for call failure */
	if (theWindow.mHwnd == NULL)
		Throw(LWindow_CANT_CREATE);

	 
	 if (inStyle & LWindow_Resizeable) {
	         theWindow.Resizable = true;
	 }
	 else {
	      
	     theWindow.Resizable = false;

	 }

#ifdef PRINT_DEBUG
	qDebug("\n\r theWindow.mHwnd : 0x%x",theWindow.mHwnd);
#endif

	theWindow.mHwnd->setMouseTracking(true);

	theWindow.mCloseHandler = NULL;
	theWindow.mResizeHandler = NULL;
	
	theWindow.mMouseDownHandler = NULL;
	theWindow.mMouseMoveHandler = NULL;
	theWindow.mActivateHandler = NULL;
	theWindow.mSetFocusHandler = NULL;

	theWindow.mStatusBar = FALSE;
	
	theWindow.mWidth = 0;
	theWindow.mLength = 0;
	
	theWindow.mID = LWindow_ID;
	theWindow.mCursorShape = LCursor_ARROW;

	theWindow.mHwnd->setCaption("Window");
	
	theWindow.mHwnd->hide();

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Window ");
#endif

	theWindowPtr = (LWindow* )LMemory_NewObject(LWindow,theWindow);

#ifdef PRINT_DEBUG
	qDebug("\n\r theWindowPtr : 0x%x",theWindowPtr);
#endif

	theWindow.mHwnd->cLWindow = theWindowPtr;


	return  theWindowPtr;
}


/*---------------------------------------------------------------------------------
//  InstallEvent
//---------------------------------------------------------------------------------*/
void LWindow_InstallEvent(LWindow *inWindow, LWindow_TEventID inEventID, LWindow_THandler inEventHandler)
{
	switch (inEventID) {
		
		case LWindow_CLOSE:
			inWindow->mCloseHandler=(_LWindow_THandler) inEventHandler;
		break;
			
		case LWindow_MOUSE_DOWN:
			inWindow->mMouseDownHandler = (_LWindow_THandler) inEventHandler;
		break;

		case LWindow_ACTIVATE:  /* DA FARE */
			inWindow->mActivateHandler=(_LWindow_THandler) inEventHandler;
		break;
		
		case LWindow_RESIZE:
			inWindow->mResizeHandler = (_LWindow_THandler/*_LWindow_TResizeHandler*/)inEventHandler;
		break;

		case LWindow_MOUSE_MOVE:
        	inWindow->mMouseMoveHandler=(_LWindow_THandler/*_LWindow_THandler_MouseMove*/) inEventHandler;
        	break;
        
		case LWindow_SET_FOCUS: /* DA FARE */
        	inWindow->mSetFocusHandler=(_LWindow_THandler) inEventHandler;
        break;
	}
}

/*----------------------------------------------
// Close 
//----------------------------------------------*/
void LWindow_Close (LWindow* inWindow)
{
	inWindow->mHwnd->close(); 
}

/*----------------------------------------------
// Destroy
//----------------------------------------------*/
void LWindow_Destroy (LWindow* inWindow)
{
	delete inWindow->mHwnd;

	LMemory_DeleteObject(&inWindow);	
}


/*------------------------------------------------
// Finalize
//------------------------------------------------*/
void LWindow_Finalize( LWindow *inWindow )
{
	Bool theFlag = FALSE;
	Bool theBringToFront = FALSE;

	if ( _LApplication_gGlobal.mBringToFront ) {

		QWindowsShowSequence *theShowSequencePtr;

		for ( theShowSequencePtr = _LApplication_gGlobal.mShowSequence.first();
			  theShowSequencePtr;
			  theShowSequencePtr = _LApplication_gGlobal.mShowSequence.next() ) {

				if ( theShowSequencePtr->mWindowPtr == inWindow ) {
			  		theShowSequencePtr->mShow = TRUE;
					theFlag = TRUE;
#ifdef PRINT_DEBUG
	qDebug( "\n\r Found : Show %p mBringToFront %d mShow %d", inWindow, theShowSequencePtr->mBringToFront, theShowSequencePtr->mShow);
#endif
					//theShowSequencePtr = NULL;
				}

		}
		
		QWindowsVisualizationSequence *theVisualizationSequencePtr;
#ifdef PRINT_DEBUG
	qDebug( "\n\r _LApplication_gGlobal.mVisualizationSequence.first()  %p", _LApplication_gGlobal.mVisualizationSequence.first() );
#endif

		if ( _LApplication_gGlobal.mVisualizationSequence.first() ) {
		
			QWindowsVisualizationSequence *theVisualizationSequencePtr = new QWindowsVisualizationSequence();

			theVisualizationSequencePtr->mWindowPtr = inWindow;

			_LApplication_gGlobal.mVisualizationSequence.append( theVisualizationSequencePtr );
			
			theBringToFront = TRUE;


#ifndef PRINT_DEBUG
	qDebug( "\n\r LWindow_BringToFront %p mBringToFront %d mShow %d", inWindow, theVisualizationSequencePtr->mBringToFront, theVisualizationSequencePtr->mShow);
#endif

		}


		if ( !theFlag ) {

			theShowSequencePtr = new QWindowsShowSequence();

			theShowSequencePtr->mWindowPtr = inWindow;
			theShowSequencePtr->mShow = TRUE;
			theShowSequencePtr->mBringToFront = theBringToFront;

#ifdef PRINT_DEBUG
	qDebug( "\n\r New : Show %p mBringToFront %d mShow %d", inWindow, theShowSequencePtr->mBringToFront, theShowSequencePtr->mShow);
#endif


			_LApplication_gGlobal.mShowSequence.append( theShowSequencePtr );
		}


	}
	else {

		inWindow->mHwnd->show();

	}






#ifdef PRINT_DEBUG
		qDebug( "\n\r inWindow->mHwnd->show()");
#endif


}


/*------------------------------------------------
// SetTitle()
//------------------------------------------------*/
void LWindow_SetTitle(LWindow* inWindow, const i1* inTitle)
{
	inWindow->mHwnd ->setCaption(inTitle);
}

/*------------------------------------------------
// GetTitle
//------------------------------------------------*/
void LWindow_GetTitle( LWindow *inWindow, i1 *outTitle )
{
	
	const i1 *theConvertedString;
	
	QString* theStringPtr = new QString();
	
	*theStringPtr = inWindow->mHwnd->caption();
	
	theConvertedString = theStringPtr->latin1();
	
	strcpy(outTitle,(i1*)theConvertedString);
	
}

/*------------------------------------------------
// Resize
//------------------------------------------------*/
void LWindow_Resize( LWindow *inWindow, ui4 inWidth, ui4 inHeight )
{
	inWindow->mHwnd->resize(inWidth-4, inHeight-25);
	
	if (!inWindow->Resizable)   {

		inWindow->mHwnd->setFixedSize(inWidth-4,inHeight-25);

	}

	inWindow->mWidth = inWidth-4;
	inWindow->mLength = inHeight-25;
}

/*------------------------------------------------
// Move
//------------------------------------------------*/
void LWindow_Move(LWindow* inWindow, ui4 inX, ui4 inY)
{
#ifdef PRINT_DEBUG
	qDebug("\n\r Move : %d, %d", inX, inY);
#endif /* PRINT_DEBUG */

	QPoint thePoint;
	thePoint.setX(int(inX));
	thePoint.setY(int(inY));

	//inWindow->mHwnd->move(int(inX), int(inY));
	inWindow->mHwnd->move(thePoint);
}

/*------------------------------------------------
// BringToFront ()
//------------------------------------------------*/
void LWindow_BringToFront( LWindow *inWindow )
{
	Bool theFlag = FALSE;

	if ( _LApplication_gGlobal.mBringToFront ) {

		QWindowsShowSequence *theShowSequencePtr;

		for ( theShowSequencePtr = _LApplication_gGlobal.mShowSequence.first();
			  theShowSequencePtr;
			  theShowSequencePtr = _LApplication_gGlobal.mShowSequence.next() ) {

				if ( theShowSequencePtr->mWindowPtr == inWindow ) {
			  		theShowSequencePtr->mBringToFront = TRUE;
					theFlag = TRUE;
					//theVisualizationSequencePtr = NULL;
#ifdef PRINT_DEBUG
	qDebug( "\n\r LWindow_BringToFront %p mBringToFront %d mShow %d", inWindow, theShowSequencePtr->mBringToFront, theShowSequencePtr->mShow);
#endif

				}

		}

		if ( !theFlag ) {

			theShowSequencePtr = new QWindowsShowSequence();

			theShowSequencePtr->mWindowPtr = inWindow;
			theShowSequencePtr->mShow = FALSE;
			theShowSequencePtr->mBringToFront = TRUE;

#ifdef PRINT_DEBUG
	qDebug( "\n\r New : Show %p mBringToFront %d mShow %d", inWindow, theShowSequencePtr->mBringToFront, theShowSequencePtr->mShow);
#endif

			_LApplication_gGlobal.mShowSequence.append( theShowSequencePtr );

		}

		QWindowsVisualizationSequence *theVisualizationSequencePtr;

		for ( theVisualizationSequencePtr = _LApplication_gGlobal.mVisualizationSequence.first();
			  theVisualizationSequencePtr;
			  theVisualizationSequencePtr = _LApplication_gGlobal.mVisualizationSequence.next() ) {
			  
			  if ( theVisualizationSequencePtr->mWindowPtr == inWindow )
				_LApplication_gGlobal.mVisualizationSequence.remove( theVisualizationSequencePtr );



			}
			
		
		theVisualizationSequencePtr = new QWindowsVisualizationSequence();

		theVisualizationSequencePtr->mWindowPtr = inWindow;

		_LApplication_gGlobal.mVisualizationSequence.append( theVisualizationSequencePtr );


#ifndef PRINT_DEBUG
	qDebug( "\n\r LWindow_BringToFront %p mBringToFront %d mShow %d", inWindow, theVisualizationSequencePtr->mBringToFront, theVisualizationSequencePtr->mShow);
#endif

	}
	else {

		inWindow->mHwnd->setFocus();

#ifdef PRINT_DEBUG
	qDebug( "\n\r LWindow_BringToFront  setFocus" );
#endif

	}
	

}

/*------------------------------------------------
// SetSizeStatus
//------------------------------------------------*/
void LWindow_SetSizeStatus( LWindow *inWindow, LWindow_TSizeStatus inStatus )
{
	 switch (inStatus) {

	    case LWindow_USER:
	    /* only foreground window can be restored!!! */
	        inWindow->mHwnd->showNormal();
	    break;
	    
	    case LWindow_MINIMIZED:
	    /* only foreground window can be minimized!!! */
	        inWindow->mHwnd->showMinimized();
	    break;
	    
	    case LWindow_MAXIMIZED:
	    /* only foreground window can be maximized!!! */
	        LWindow_BringToFront (inWindow);
	        inWindow->mHwnd->showMaximized();
	    break;

    }
	
}

/*-------------------------------------------------
// GetSizeStatus
//-------------------------------------------------*/
LWindow_TSizeStatus LWindow_GetSizeStatus( LWindow *inWindow )
{
	if (inWindow->mHwnd->isMinimized())
        return LWindow_MINIMIZED;
        
    if (inWindow->mHwnd->isMaximized())
        return LWindow_MAXIMIZED;
        
    return LWindow_USER;
}

/*-------------------------------------------------
// GetFrontWindow 
//-------------------------------------------------*/
LWindow *LWindow_GetFrontWindow()
{
	QChildWindow *theChildWindowPtr;
	LWindow *theReturnWindow = NULL;

	theChildWindowPtr = ( QChildWindow* )_LApplication_gGlobal.mWorkspace->activeWindow();
	
	if ( theChildWindowPtr != NULL ) {
	
		theReturnWindow = ( LWindow* )( theChildWindowPtr->cLWindow );

#ifdef PRINT_DEBUG
	qDebug( "\n\r theChildWindowPtr %p", theChildWindowPtr );
	qDebug( "\n\r theChildWindowPtr->cLWindow 0x%x", theChildWindowPtr->cLWindow );
#endif


	}

	return theReturnWindow;
}


/*----------------------------------------------------
// GetClientAreaSize
//---------------------------------------------------*/
void LWindow_GetClientAreaSize(LWindow *inWindow, ui4 *outWidth, ui4 *outHeight )
{
	QSize theSize;

	theSize = inWindow->mHwnd->size();
	*outWidth = (ui4)theSize.width();
	*outHeight =(ui4)theSize.height();
	
#ifdef PRINT_DEBUG	
	qDebug( "\n\r ClientAreaSize : width %d, height %d ",*outWidth, *outHeight);
#endif 		
		
}

/*----------------------------------------------------
// GetPosition ()
//---------------------------------------------------*/
void LWindow_GetPosition(LWindow *inWindow, ui4 *outX, ui4 *outY)
{
	QPoint thePoint;

	thePoint.setX(inWindow->mHwnd->x());
	thePoint.setY(inWindow->mHwnd->y());

	thePoint = inWindow->mHwnd->mapTo( _LApplication_gGlobal.mWorkspace, thePoint );

	*outX = (ui4)thePoint.x();
	*outY = (ui4)thePoint.y();

#ifdef PRINT_DEBUG
	qDebug( "\n\r LWindow_GetPosition : thePoint.x %d, thePoint.y %d ",thePoint.x(),thePoint.y());
	qDebug( "\n\r LWindow_GetPosition : width %d, height %d ",*outX, *outY);
#endif
	
}

/*----------------------------------------------------
// GetType ()
//---------------------------------------------------*/
LWindow_TWindowType LWindow_GetType (LWindow* inWindow)
{
    return inWindow->mType;
}

/*----------------------------------------------------
// SetUserData ()
//---------------------------------------------------*/
void LWindow_SetUserData(LWindow* inWindow, const void* inData)
{ 
    inWindow->mUserData = (void*)inData;
}

/*----------------------------------------------------
// GetUserData
//---------------------------------------------------*/
void* LWindow_GetUserData(LWindow* inWindow)
{ 
    return inWindow->mUserData;
}

/*--------------------------------------
// SetCursorShape
//--------------------------------------*/
void LWindow_SetCursorShape( LWindow *inWindow, LCursor_TShape inCursorShape )
{
	inWindow->mCursorShape = inCursorShape;
}

/*---------------------------------------
// GetCursorShape
//--------------------------------------*/
LCursor_TShape LWindow_GetCursorShape( LWindow *inWindow )
{
	return inWindow->mCursorShape;
}

/*---------------------------------------
// EnableMinMax
//--------------------------------------*/
void LWindow_EnableMinMax( LWindow *inWindow, ui4 inMinX, ui4 inMinY, ui4 inMaxX, ui4 inMaxY )
{

}

/*---------------------------------------
// DisableMinMax
//-------------------------------------*/
void LWindow_DisableMinMax( LWindow *inWindow )
{

}


/*----------------------------------------------------
// QChildWindow::QChildWindow
//---------------------------------------------------*/
QChildWindow::QChildWindow(WFlags inFlags)  : QMainWindow( _LApplication_gGlobal.mWorkspace, "Leo", inFlags | Qt::WDestructiveClose )
{

	cLWindow = NULL;
    
	/* inserito per testare FocusInEvent */
	setFocusPolicy(QWidget::StrongFocus);

}

/*----------------------------------------------------
// QChildWindow::~QChildWindow
//---------------------------------------------------*/
QChildWindow::~QChildWindow()
{

}

/*----------------------------------------------------
// QChildWindow::closeEvent
//---------------------------------------------------*/
void QChildWindow::closeEvent( QCloseEvent *e )
{

	LWindow *theWindowPtr;
	Bool theResult = false;
	LWindow_THandler theHandlePtr;
	Bool theHandleResult;
	

	theWindowPtr = ((LWindow *)(this->cLWindow));

	 if (theWindowPtr->mCloseHandler != NULL ) {

  /* ?? */       	/*theResult = theWindowPtr->mCloseHandler(theWindowPtr);*/

  			theHandlePtr = (LWindow_THandler)theWindowPtr->mCloseHandler;
			theHandleResult = theHandlePtr(theWindowPtr);

         }
#ifdef PRINT_DEBUG
	//qDebug( "\n\r Result %d",  theResult);

#endif

	if (!theResult){
	
	QWidgetDestroyList *theWidgetListItemPtr;

#ifdef PRINT_DEBUG
	//qDebug( "\n\r btheWidgetListItemPtr 0x%x",  cWidgetList.first());

#endif

    	for ( theWidgetListItemPtr = cWidgetList.first(); theWidgetListItemPtr; theWidgetListItemPtr = cWidgetList.next() ){

#ifdef PRINT_DEBUG
	//qDebug( "\n\r atheWidgetListItemPtr 0x%x",  theWidgetListItemPtr);

#endif


		_LWindow_DestroyWidget(theWidgetListItemPtr->mWidgetListPtr, theWidgetListItemPtr->mID );

		delete theWidgetListItemPtr;


	}

	}

#ifdef PRINT_DEBUG
	//qDebug( "\n\r theResult 0x%x",  theResult);

#endif

         if (!theResult){
	 
#ifdef PRINT_DEBUG
	//qDebug( "\n\r the Destroyed Window 0x%x",  ((LWindow *)(this->cLWindow)));

#endif
	 	LWindow_Destroy (((LWindow *)(this->cLWindow)));
         	e->accept ();
	}
	else {
		e->ignore ();
		_LApplication_gGlobal.mCloseApplication = FALSE;

	}

	if (!theHandleResult)
		QMainWindow::closeEvent(e);


}

/*----------------------------------------------------
// QChildWindow::resizeEvent
//---------------------------------------------------*/
void QChildWindow::resizeEvent( QResizeEvent *inEvent )
{
	Bool theHandleResult;
	QSize theSize;

	theSize = inEvent->size();

#ifdef PRINT_DEBUG
	qDebug( "QChildWindow::resizeEvent  LWindow %p", ((LWindow *)(this->cLWindow)) );
#endif
	

	
	LWindow_THandler theHandlePtr;

	if ( ((LWindow *)(this->cLWindow))->mStatusBar == TRUE ) {

		statusBar()->fontMetrics().height();
		statusBar()->resize(theSize.width(), statusBar()->fontMetrics().height()+5/*25*/);
		statusBar()->move(0, theSize.height() - (statusBar()->fontMetrics().height()+5/*25*/));

		statusBar()->raise();

	}

	if ( ((LWindow *)(this->cLWindow))->mResizeHandler != NULL ) {

 /* ?? */        //	((LWindow *)(this->cLWindow))->mResizeHandler((LWindow *)(this->cLWindow),ui4(theSize.width()), ui4(theSize.height()));

		theHandlePtr = ( LWindow_THandler )( ( LWindow * )( this->cLWindow ) )->mResizeHandler;
		theHandleResult = theHandlePtr( ( LWindow * )(this->cLWindow), ui4( theSize.width() ), ui4( theSize.height() ) );

	}

	if ( !theHandleResult )
		QMainWindow::resizeEvent( inEvent );

}

/*-------------------------------------
// QChildWindow::focusInEvent ()
//------------------------------------*/
void QChildWindow::focusInEvent( QFocusEvent * inFocusEvent)
{

	_LApplication_gGlobal.mWidgetFocused = ((LWindow*)(((QChildWindow*)this)->cLWindow));
	_LApplication_gGlobal.mWidgetFocusedId = ((LWindow*)(((QChildWindow*)this)->cLWindow))->mID;

#ifdef PRINT_DEBUG
	qDebug( "focusInEvent 0x%x   _gWidgetFocusedId 0x%x",  _LApplication_gGlobal.mWidgetFocused, _LApplication_gGlobal.mWidgetFocusedId);
#endif

}

/*-------------------------------------
// QChildWindow::mousePressEvent ()
//------------------------------------*/
void QChildWindow::mousePressEvent( QMouseEvent *inMouseEvent)
{
	LWindow_THandler theHandlePtr;
	Bool theHandleResult;

#ifdef PRINT_DEBUG
	//qDebug( "\n\r QChildWindow::mouseMovePress ");

#endif
	if (((LWindow *)(this->cLWindow))->mMouseDownHandler != NULL ) {

#ifdef PRINT_DEBUG
	//qDebug( "\n\r QChildWindow::mouseMovePress mMouseDownHandler 0x%x",  ((LWindow *)(this->cLWindow))->mMouseDownHandler);

#endif

		theHandlePtr = (LWindow_THandler)((LWindow *)(this->cLWindow))->mMouseDownHandler;
		theHandleResult = theHandlePtr((LWindow *)(this->cLWindow), inMouseEvent->x(),inMouseEvent->y());


	}

	if (!theHandleResult) /* ??? */
		QMainWindow::mousePressEvent( inMouseEvent );

}

/*-------------------------------------
// QChildWindow::mouseMoveEvent ()
//------------------------------------*/
void QChildWindow::mouseMoveEvent( QMouseEvent * inMouseEvent)
{

	int theShape;
	QCursor theCursor;
	LWindow_THandler theHandlePtr;
	Bool theHandleResult;

#ifdef PRINT_DEBUG
	//qDebug( "\n\r QChildWindow::mouseMoveEvent ");

#endif
	if (((LWindow *)(this->cLWindow))->mMouseMoveHandler != NULL ) {

#ifdef PRINT_DEBUG
	//qDebug( "\n\r QChildWindow::mouseMoveEvent mMouseMoveHandler 0x%x",  ((LWindow *)(this->cLWindow))->mMouseMoveHandler);

#endif

/* ?? *///((LWindow *)(this->cLWindow))->mMouseMoveHandler((LWindow *)(this->cLWindow), inMouseEvent->x(),
		                                                                      //inMouseEvent->y());


	theHandlePtr = (LWindow_THandler)((LWindow *)(this->cLWindow))->mMouseMoveHandler;
	theHandleResult = theHandlePtr((LWindow *)(this->cLWindow), inMouseEvent->x(),inMouseEvent->y());


	}

	theShape = _LCursor_GetShape( ((LWindow *)(this->cLWindow))->mCursorShape );


	theCursor.setShape( theShape );

	this->setCursor(theCursor);

/* ??? */if (!theHandleResult)
		QMainWindow::mouseMoveEvent(inMouseEvent);
#ifdef PRINT_DEBUG
	//qDebug( "mouseMoveEvent");
#endif



}


/*----------------------------------------------------
// _LWindow_DestroyWidget
//---------------------------------------------------*/
void _LWindow_DestroyWidget(void *inWidgetListItemPtr, ui2 inID)
{
switch (inID)
    {
    case LCheckBox_ID:

#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy CheckBox");

#endif
        LCheckBox_Destroy ((LCheckBox*) inWidgetListItemPtr);
        break;
    case LDraw_ID:

#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy Draw");

#endif
        LDraw_Destroy ((LDraw*) inWidgetListItemPtr);
        break;
    case LGroupBox_ID:
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy GroupBox");

#endif

        LGroupBox_Destroy ((LGroupBox*) inWidgetListItemPtr);
        break;
    case LListBox_ID:
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy ListBox");

#endif

        LListBox_Destroy ((LListBox*) inWidgetListItemPtr);
        break;
    case LProgressBar_ID:
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy ProgressBar");

#endif

        LProgressBar_Destroy ((LProgressBar*) inWidgetListItemPtr);
        break;
    case LPushButton_ID:
    
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy PushButton");

#endif

        LPushButton_Destroy ((LPushButton*) inWidgetListItemPtr);
        break;
    case LRadioButton_ID:
    
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy RadioButton");

#endif

        LRadioButton_Destroy ((LRadioButton*) inWidgetListItemPtr);
        break;
    case LStaticText_ID:
 #ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy StaticText");

#endif


        LStaticText_Destroy ((LStaticText*) inWidgetListItemPtr);
        break;
    case LStatusBar_ID:
    
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy StatusBar");

#endif

        LStatusBar_Destroy ((LStatusBar*) inWidgetListItemPtr);
        break;

    case LTextEditor_ID:
    
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy TextEditor");

#endif

       LTextEditor_Destroy ((LTextEditor*) inWidgetListItemPtr);
        break;

    case LTextField_ID:
    
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy TextField");

#endif


        LTextField_Destroy ((LTextField*) inWidgetListItemPtr);
        break;
    
    case LScrollBar_ID:
    
#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy ScrollBar");

#endif

        LScrollBar_Destroy ((LScrollBar*) inWidgetListItemPtr);
        break;

    	case LSizeBox_ID:

#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy SizeBox");

#endif

        LSizeBox_Destroy ((LSizeBox*) inWidgetListItemPtr);
        break;
	
	case LThread_ID:

#ifdef PRINT_DEBUG
	qDebug( "\n\r Destroy Thread");

#endif

        LThread_Delete ((LThread*) inWidgetListItemPtr);
        break;


    }


}

/*----------------------------------------------------
// QWidgetDestroyList::QWidgetDestroyList
//---------------------------------------------------*/
QWidgetDestroyList::QWidgetDestroyList()
{



}

/*----------------------------------------------------
// QWidgetDestroyList::~QWidgetDestroyList
//---------------------------------------------------*/
QWidgetDestroyList::~QWidgetDestroyList()
{

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
