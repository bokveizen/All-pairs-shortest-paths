/* ============================================================================
 *  LApplication_qt.c
 * ============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Apr 16, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/23 23:32:32 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.9 $
*/



#if __LSL_OS_GUI__ == __LSL_QT__

#include <qapplication.h>
#include <qworkspace.h>
#include <qvbox.h>
#include <qframe.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qmainwindow.h>
#include <qstring.h>
#include <qevent.h>

#include "LApplication.h"
#include "LApplication_qt.h"
#include "LWindow.h"
#include "LWindow_qt.h"
#include "LMenu.h"
#include "LMenu_qt.h"
#include "LDialog.h"
#include "LTextEditor_qt.h"

LWindow*  _gApplication_Window[15] = {NULL, NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL, NULL};
int gMenuiHandler;

typedef struct tagCustomData {

	LWindow *mWindow;
	i1		mAction;

} tCustomData;

extern tCustomData gCustomData[100];
extern i1 theCustomCounter;
/*---------------------------------------------------------------------------------
// main
//---------------------------------------------------------------------------------*/
int main(int inArgc, char *inArgv[])
{
	int theResult;
	char theDebug[30];
	i1 theString[100] = "";
	int i;
	
	QObject *theKillerFilter = new KillerFilterApp();

	_LApplication_gGlobal.mApplication = new QApplication( inArgc, inArgv );

	for ( i = 1; i < inArgc; i++ ) {

		strcat( theString, inArgv[i] );
		
		strcat( theString, " " );

#ifdef PRINT_DEBUG
	qDebug( "\n\r theString : ***%s*** 0x%x", theString, theString );
#endif
	
	}
	
#ifndef PRINT_DEBUG
	qDebug( "\n\r strlen( theString ) : %d", strlen( theString ) );


	for ( i = 0; i < strlen( theString ); i++ ) {
	
		qDebug( "\n\r  theString[%d] = %d (%c)", i, theString[i], theString[i] );

	}
#endif

	theString[ strlen( theString ) - 1 ] = '\0';



	_LApplication_gGlobal.inArgs = theString;
//	_LApplication_gGlobal.inArgs = ( i1* )malloc( strlen( theString ) - 1 );
	
//	strcpy( _LApplication_gGlobal.inArgs, theString );


#ifdef PRINT_DEBUG
	qDebug( "\n\r _LApplication_gGlobal.inArgs : ***%s*** 0x%x", _LApplication_gGlobal.inArgs, _LApplication_gGlobal.inArgs );
#endif

	_LApplication_InitWindowManager();

	_LApplication_gGlobal.mMainWindow = new QLeoMainWindow();


	_LApplication_gGlobal.mVb = new QVBox( _LApplication_gGlobal.mMainWindow );

	_LApplication_gGlobal.mVb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

	_LApplication_gGlobal.mWorkspace = new QWorkspace( _LApplication_gGlobal.mVb );

	_LApplication_gGlobal.mMainWindow->setCentralWidget( _LApplication_gGlobal.mVb );

	
	QDesktopWidget *theDesktop = QApplication::desktop();

	// returns desktop width
	int theWidth = theDesktop->width();
	// returns desktop height
	int theHeight = theDesktop->height();

	_LApplication_gGlobal.mMainWindow->setMaximumSize( theWidth, theHeight );
	_LApplication_gGlobal.mMainWindow->setMinimumSize( 500, 500 );
	_LApplication_gGlobal.mMainWindow->setGeometry(0, 0, theWidth, theHeight-21);
	
	_LApplication_gGlobal.mApplication->installEventFilter( theKillerFilter );


//	_gApplication_BringToFront = true;

//	_gWidgetFocused = NULL;
//	_gWidgetFocusedId = 0;

	LApplication_Init();
	
	_LApplication_DispatchBringToFront();
	
	_LApplication_gGlobal.mBringToFront = FALSE;

	_LApplication_gGlobal.mApplication->setMainWidget( _LApplication_gGlobal.mMainWindow );

	_LApplication_gGlobal.mMainWindow->show();


	QRect theRect;

	theRect = _LApplication_gGlobal.mMainWindow->frameGeometry();

#ifdef PRINT_DEBUG

	qDebug( "\n\r Frame : left %d, top %d, right %d,  bottom %d",theRect.left(), theRect.top(), theRect.right(), theRect.bottom() );

#endif
	
	theResult =_LApplication_gGlobal.mApplication->exec();

#ifdef PRINT_DEBUG
	
	sprintf ((char *) theDebug, "%d", LMemory_GetBlocksCount());
	LDialog_DisplayMessageBox (NULL, theDebug,  "LMemory_GetBlocksCount() says:",LDialog_MBOK, LDialog_MBICONINFORMATION);
	qDebug( "\n\r the application result : %d", theResult );

#endif
	
	free( _LApplication_gGlobal.inArgs );

	return theResult;

}

/*----------------------------------------------------------------------
// SetTitle
//----------------------------------------------------------------------*/
void LApplication_SetTitle( const i1 *inTitle )
{

#ifdef PRINT_DEBUG
	qDebug( "\n\r inTitle : ***%s*** 0x%x", inTitle, inTitle );
	int theLen = strlen( inTitle );
	qDebug( "\n\r inTitle : ***%c*** ***%c*** ***%c***", inTitle[theLen-2], inTitle[theLen-1], inTitle[theLen] );

#endif
	_LApplication_gGlobal.mMainWindow->setCaption( inTitle );
}

/*----------------------------------------------------------------------
// GetTitle
//----------------------------------------------------------------------*/
void LApplication_GetTitle( i1 *outTitle )
{
	QString theString;
	const i1 *theConvertedString;

	theString = _LApplication_gGlobal.mMainWindow->caption();

	theConvertedString = theString.latin1();

	strcpy( outTitle, (i1*)theConvertedString );
	
}

/*----------------------------------------------------------------------
// GetFocus
//----------------------------------------------------------------------*/
Bool LApplication_GetFocus( ui2 *outObjID, void **outObj )
{
	Bool theReturn = FALSE;

	if ( _LApplication_gGlobal.mApplication->focusWidget() == NULL ) {
		
		theReturn = FALSE;

	}
	else {

		if ( _LApplication_gGlobal.mWidgetFocused == NULL) {

			theReturn = FALSE;

		}
		else {

			*outObjID = _LApplication_gGlobal.mWidgetFocusedId;
			*outObj = (void *)_LApplication_gGlobal.mWidgetFocused;

			theReturn = TRUE;
		}

	}

	return theReturn;

}

/*!
	Gets the size of the workspace. This function should be called AFTER the menu initialization phase, 
	in order for the menu bar to be taken into account.
*/

/*----------------------------------------------------------------------
// GetWorkspaceSize
//----------------------------------------------------------------------*/
void LApplication_GetWorkspaceSize( ui4 *outWidth , ui4 *outHeight )
{
	QRect *theRectPtr;
	QRect theRect;
	
	theRect = _LApplication_gGlobal.mMainWindow->geometry();

	theRectPtr =  &theRect;

	*outWidth = theRectPtr->right() - theRectPtr->left() - 6;
	*outHeight = theRectPtr->bottom() - theRectPtr->top() - 38;

#ifdef PRINT_DEBUG

	qDebug("\n\r Workspace : width %d, height %d", theRectPtr->width(), theRectPtr->height());
	qDebug("\n\r Workspace : left %d, top %d, right %d,  bottom %d", theRectPtr->left(), theRectPtr->top(), theRectPtr->right(), theRectPtr->bottom());

#endif

}

/*----------------------------------------------------------------------
// Quit
//----------------------------------------------------------------------*/
void LApplication_Quit()
{
	Bool theResult;
	char theDebug[30];

	QWidgetList theMdiWindows = _LApplication_gGlobal.mWorkspace->windowList();

	for ( int i = 0; i < int( theMdiWindows.count() ); ++i ) {

		QWidget *theWindowPtr = theMdiWindows.at( i );

#ifdef PRINT_DEBUG
	qDebug("\n\r before theWindowPtr : 0x%x", theWindowPtr);
#endif

		theResult = ( ( QChildWindow* )theWindowPtr )->close();

#ifdef PRINT_DEBUG
	qDebug("\n\r after theWindowPtr, theResult : 0x%x", theWindowPtr, theResult);
#endif
		
	}
	
	if ( _LApplication_gGlobal.mCloseApplication ) {

		struct tagMenuHandlerStruct *theMenuHandlerStructPtr;
		struct tagMenuHandlerStruct *theMenuHandlerStructOldPtr;

		theMenuHandlerStructPtr = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;

		while( theMenuHandlerStructPtr != NULL ) {

			theMenuHandlerStructOldPtr = theMenuHandlerStructPtr;
			theMenuHandlerStructPtr = theMenuHandlerStructPtr->mNext;

#ifdef PRINT_DEBUG
	gMenuiHandler--;
	qDebug( "\n\r LMemory_DeleteObject : MenuHandlerStruct %d", gMenuiHandler);

#endif
			LMemory_DeleteObject( &theMenuHandlerStructOldPtr );

		}

		QMenuDestroyList *theMenuListItemPtr;


		for ( theMenuListItemPtr = _LApplication_gGlobal.mMainWindow->cMenuList.first(); theMenuListItemPtr; theMenuListItemPtr = _LApplication_gGlobal.mMainWindow->cMenuList.next() ) {

			/*delete theMenuListItemPtr->mMenuListPtr->mHmenu;

			LMemory_DeleteObject (&(theMenuListItemPtr->mMenuListPtr));*/

			/* Da utilizzare al posto delle 2 precedenti istruzioni la : */
			LMenu_Destroy( theMenuListItemPtr->mMenuListPtr );
			
			delete theMenuListItemPtr;
		
#ifdef PRINT_DEBUG
	qDebug( "\n\r aft QMenuDestroyList 0x%x",  theMenuListItemPtr);
#endif

		}

		_LApplication_gGlobal.mMainWindow->cGraphicsContext->end();

		delete _LApplication_gGlobal.mMainWindow;
		
		_LApplication_gGlobal.mEndApplication = TRUE;

		_LApplication_gGlobal.mApplication->quit();

	}
	else
		_LApplication_gGlobal.mCloseApplication = TRUE;

}

/*----------------------------------------------------------------------
// WindowLayout
//----------------------------------------------------------------------*/
void LApplication_WindowLayout( LApplication_TLayout inLayout )
{
	switch ( inLayout ) {

		case LApplication_CASCADE:
			_LApplication_gGlobal.mWorkspace->cascade();
		break;

		case LApplication_TILE_VERTICALLY:
			_LApplication_gGlobal.mWorkspace->tile();
		break;

		case LApplication_TILE_HORIZONTALLY:
			_LApplication_gGlobal.mWorkspace->tile();
		break;
		
	}
}

/*!
	If inHandler is not NULL, this function will install a handler that will be invoked every inTicks millisecs 
	(if possible, otherwise a lower frequency will be used).
	If inHandler is NULL, any previously installed handler will be removed (inTicks is ignored in this case).
*/

/*-------------------------------------------------------------------
// SetTimer
//-----------------------------------------------------------------*/
void LApplication_SetTimer( ui4 inTicks, LApplication_THandler inHandler )
{
	
	static Bool sIsTimerOn = FALSE;
	
	if (inTicks && !sIsTimerOn) {

		_LApplication_gGlobal.mMainWindow->cTimer = inHandler;
	
		QObject::connect( _LApplication_gGlobal.mMainWindow->cApplicationTimer, SIGNAL( timeout() ), _LApplication_gGlobal.mMainWindow, SLOT( slotTimer() ) );

		_LApplication_gGlobal.mMainWindow->cApplicationTimer->start( inTicks*2, FALSE );
	
		sIsTimerOn = TRUE;
	}
	else
		if (inTicks == 0) {

			_LApplication_gGlobal.mMainWindow->cApplicationTimer->stop();
			
			QObject::disconnect( _LApplication_gGlobal.mMainWindow->cApplicationTimer, SIGNAL( timeout() ), _LApplication_gGlobal.mMainWindow, SLOT( slotTimer() ) );

			_LApplication_gGlobal.mMainWindow->cTimer = inHandler;
			
			sIsTimerOn = FALSE;
	
		}
		
}

/*---------------------------------------------------------------------------------
// InitWindowManager
//---------------------------------------------------------------------------------*/
void _LApplication_InitWindowManager()
{
	_LApplication_gGlobal.mApplication->setFont( QFont( "helvetica", 12 ) );

	_LApplication_gGlobal.mBringToFront = TRUE;

	_LApplication_gGlobal.mWidgetFocused = NULL;
	_LApplication_gGlobal.mWidgetFocusedId = 0;

	_LApplication_gGlobal.mCloseApplication = TRUE;
	_LApplication_gGlobal.mEndApplication = FALSE;

}

/*---------------------------------------------------------------------------------
// _LApplication_DispatchBringToFront
//---------------------------------------------------------------------------------*/
void _LApplication_DispatchBringToFront()
{
	QWindowsVisualizationSequence *theVisualizationSequencePtr;
	QWindowsShowSequence *theShowSequencePtr;
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r _LApplication_DispatchBringToFront " );
#endif

	for ( theShowSequencePtr = _LApplication_gGlobal.mShowSequence.first();
		  theShowSequencePtr;
		  theShowSequencePtr = _LApplication_gGlobal.mShowSequence.next() ) {

			if ( theShowSequencePtr->mShow == TRUE &&
				 theShowSequencePtr->mBringToFront == FALSE) {

				QCustomEvent *theCustomEvent = new QCustomEvent( QEvent::User, (void *)theShowSequencePtr->mWindowPtr );

				// Qt will delete it when done
				QApplication::postEvent( _LApplication_gGlobal.mMainWindow, theCustomEvent );

#ifdef PRINT_DEBUG
	qDebug( "\n\r Show Window %p", theShowSequencePtr->mWindowPtr );
#endif

			}

	}

	for ( theVisualizationSequencePtr = _LApplication_gGlobal.mVisualizationSequence.first();
		  theVisualizationSequencePtr;
		  theVisualizationSequencePtr = _LApplication_gGlobal.mVisualizationSequence.next() ) {
		  
#ifdef PRINT_DEBUG
	qDebug( "\n\r Sequence BTF %p", theVisualizationSequencePtr->mWindowPtr );
#endif

		for ( theShowSequencePtr = _LApplication_gGlobal.mShowSequence.first();
		  	  theShowSequencePtr;
		 	  theShowSequencePtr = _LApplication_gGlobal.mShowSequence.next() ) {

			if ( theShowSequencePtr->mWindowPtr == theVisualizationSequencePtr->mWindowPtr && 
				 theShowSequencePtr->mShow == TRUE ) {

				QCustomEvent *theCustomEvent = new QCustomEvent( QEvent::User, (void *)theVisualizationSequencePtr->mWindowPtr );

				// Qt will delete it when done
				QApplication::postEvent( _LApplication_gGlobal.mMainWindow, theCustomEvent );

#ifdef PRINT_DEBUG
	qDebug( "\n\r BringToFront Window %p", theVisualizationSequencePtr->mWindowPtr );
#endif

			}
		}

	}

	for ( theVisualizationSequencePtr = _LApplication_gGlobal.mVisualizationSequence.first();
		  theVisualizationSequencePtr;
		  theVisualizationSequencePtr = _LApplication_gGlobal.mVisualizationSequence.next() ) {

			delete theVisualizationSequencePtr;

	}

	for ( theShowSequencePtr = _LApplication_gGlobal.mShowSequence.first();
		  theShowSequencePtr;
		  theShowSequencePtr = _LApplication_gGlobal.mShowSequence.next() ) {

			delete theShowSequencePtr;

	}

}




/*-----------------------------------
// SetDropHandler
//----------------------------------*/
void LApplication_SetDropHandler ( LApplication_TDropHandler inHandler )
{
	if ( inHandler == NULL )
	{
		/* disables drag'n'drop */
		_LApplication_gGlobal.mMainWindow->setAcceptDrops( FALSE );
		_LApplication_gGlobal.mDropHandler = NULL;
	}
	else
	{
		_LApplication_gGlobal.mDropHandler = inHandler;
		/*enables drag'n'drop*/
		_LApplication_gGlobal.mMainWindow->setAcceptDrops( TRUE );
	}
}

/*-----------------------------------
// GetArgs
//----------------------------------*/
i1 *LApplication_GetArgs()
{


#ifndef PRINT_DEBUG
	qDebug( "\n\r _LApplication_gGlobal.inArgs : ***%s*** 0x%x", _LApplication_gGlobal.inArgs, _LApplication_gGlobal.inArgs );

	qDebug( "\n\r mApplication %p", _LApplication_gGlobal.mApplication );

	qDebug( "\n\r mMainWindow %p", _LApplication_gGlobal.mMainWindow );
	qDebug( "\n\r mWorkspace %p", _LApplication_gGlobal.mWorkspace );
	qDebug( "\n\r mVb %p", _LApplication_gGlobal.mVb );

	qDebug( "\n\r mWidgetFocused %p", _LApplication_gGlobal.mWidgetFocused );
	qDebug( "\n\r mWidgetFocusedId %d", _LApplication_gGlobal.mWidgetFocusedId );
	qDebug( "\n\r mBringToFront %d", _LApplication_gGlobal.mBringToFront );

	qDebug( "\n\r mEndApplication %d", _LApplication_gGlobal.mEndApplication );
	qDebug( "\n\r mCloseApplication %d", _LApplication_gGlobal.mCloseApplication );


#endif
	return ( (i1*)_LApplication_gGlobal.inArgs );
}

/*-----------------------------------------------------
// SetCheckBlocksCount
//----------------------------------------------------*/
void LApplication_SetCheckBlocksCount( Bool inStatus )
{
	//_LApplication_gGlobal.checkblockscount=inStatus;
}

/*----------------------------------------------------
// GetSystemInfo
//---------------------------------------------------*/
void LApplication_GetSystemInfo( i1* outBuffer, ui4 inBufSize )
{
}

/*---------------------------------------------------------------------------------
// _LApplication_BringWindowsToFront
//---------------------------------------------------------------------------------*/
void _LApplication_BringWindowsToFront()
{
    int i = 0;

    while ( _gApplication_Window[i] != NULL ) {

    	(_gApplication_Window[i++])->mHwnd->lower();

#ifdef PRINT_DEBUG
	qDebug( "\n\r _gApplication_Window[%d] = 0x%x",i-1,_gApplication_Window[i-1] );
#endif

    }
	
}

/*----------------------------------------------------
// QLeoMainWindow::QLeoMainWindow
//---------------------------------------------------*/
QLeoMainWindow::QLeoMainWindow() : QMainWindow()
{
	cFirstMenuHandlerStruct = NULL;
	cApplicationTimer = new QTimer();

	QDesktopWidget *theDesktop = QApplication::desktop();

	int theWidth = theDesktop->width();
	int theHeight = theDesktop->height();                 

	cGlobalBuffer =  new QPixmap( theWidth, theHeight );
	cGraphicsContext = new QPainter();

	cGraphicsContext->begin( cGlobalBuffer );

	for (int i = 0; i < 10; i++)
		cMenuInitHandler[i] = NULL;

}

/*----------------------------------------------------
// QLeoMainWindow::~QLeoMainWindow
//---------------------------------------------------*/
QLeoMainWindow::~QLeoMainWindow()
{
	delete _LApplication_gGlobal.mMainWindow->cGlobalBuffer;
	delete _LApplication_gGlobal.mMainWindow->cGraphicsContext;
	delete _LApplication_gGlobal.mMainWindow->cApplicationTimer;

	delete _LApplication_gGlobal.mWorkspace;

	delete _LApplication_gGlobal.mVb;

}

/*----------------------------------------------------
// QLeoMainWindow::slotTimer
//---------------------------------------------------*/
void QLeoMainWindow::slotTimer()
{
	cTimer();

}

/*----------------------------------------------------
// QLeoMainWindow::slotMenuPressed
//---------------------------------------------------*/
void QLeoMainWindow::slotMenuPressed( int inId )
{
	struct tagMenuHandlerStruct* theMenuHandlerStructPtr;

	theMenuHandlerStructPtr = cFirstMenuHandlerStruct;

	while( theMenuHandlerStructPtr != NULL )
		if ( theMenuHandlerStructPtr->mMenuId != inId )
			theMenuHandlerStructPtr = theMenuHandlerStructPtr->mNext;
		else {
#ifdef PRINT_DEBUG
	qDebug( "\n\r theMenuHandlerStructPtr->mMenuHandlerParam 0x%x",  theMenuHandlerStructPtr->mMenuHandlerParam);
#endif
			theMenuHandlerStructPtr->mMenuHandler( theMenuHandlerStructPtr->mMenuHandlerParam/*/NULL*//* ??? */);
			theMenuHandlerStructPtr = NULL;
		}

}

/*----------------------------------------------------
// QLeoMainWindow::closeEvent
//---------------------------------------------------*/
void QLeoMainWindow::closeEvent( QCloseEvent *inEvent )
{
	LApplication_Quit();
	inEvent->accept();

}

/*----------------------------------------------------
// QLeoMainWindow::customEvent
//---------------------------------------------------*/
void QLeoMainWindow::customEvent( QCustomEvent *inEvent )
{
	tCustomData *theCustomData;
	i1 theAction;
	LWindow *theHandlerPtr;
#ifndef PRINT_DEBUG
	qDebug( "\n\r customEvent");
#endif

	if ( inEvent->type() == QEvent::User ) {


		
		//theCustomData = (tCustomData *)( inEvent->data() );

		theHandlerPtr = (LWindow *)( inEvent->data() );
		//theHandlerPtr = (LWindow *)( theCustomData->mWindow );

		theAction =(i1)( theCustomData->mAction );

		if (theHandlerPtr != NULL) {

			//if ( theAction == 1)
				theHandlerPtr->mHwnd->show();

			//if ( theAction == 0) {
				theHandlerPtr->mHwnd->raise();
				theHandlerPtr->mHwnd->setFocus();
			//}

#ifdef PRINT_DEBUG
	qDebug( "\n\r customEvent theHandlerPtr %p", theHandlerPtr );
#endif

    	}
	}

#ifndef PRINT_DEBUG
	qDebug( "\n\r customEvent  Done theHandlerPtr %p", theHandlerPtr);
#endif

}

/*----------------------------------------------------
// QMenuDestroyList::QMenuDestroyList
//---------------------------------------------------*/
QMenuDestroyList::QMenuDestroyList()
{

}

/*----------------------------------------------------
// QMenuDestroyList::~QMenuDestroyList
//---------------------------------------------------*/
QMenuDestroyList::~QMenuDestroyList()
{

}

/*----------------------------------------------------
// QBitmapDestroyList::QBitmapDestroyList
//---------------------------------------------------*/
QBitmapDestroyList::QBitmapDestroyList()
{

}

/*----------------------------------------------------
// QBitmapDestroyList::~QBitmapDestroyList
//---------------------------------------------------*/
QBitmapDestroyList::~QBitmapDestroyList()
{

}

/*----------------------------------------------------
// QWindowsVisualizationSequence::QWindowsVisualizationSequence
//---------------------------------------------------*/
QWindowsVisualizationSequence::QWindowsVisualizationSequence()
{

}

/*----------------------------------------------------
// QWindowsVisualizationSequence::~QWindowsVisualizationSequence
//---------------------------------------------------*/
QWindowsVisualizationSequence::~QWindowsVisualizationSequence()
{

}

/*----------------------------------------------------
// QWindowsShowSequence::QWindowsShowSequence
//---------------------------------------------------*/
QWindowsShowSequence::QWindowsShowSequence()
{

}

/*----------------------------------------------------
// QWindowsShowSequence::~QWindowsShowSequence
//---------------------------------------------------*/
QWindowsShowSequence::~QWindowsShowSequence()
{

}

/*----------------------------------------------------
// QLeoMainWindow::dragEnterEvent
//---------------------------------------------------*/
void QLeoMainWindow::dragEnterEvent( QDragEnterEvent *inEvent )
{
	inEvent->accept();
}

/*----------------------------------------------------
// QLeoMainWindow::dragLeaveEvent
//---------------------------------------------------*/
void QLeoMainWindow::dragLeaveEvent( QDragLeaveEvent *inEvent )
{
}

/*----------------------------------------------------
// QLeoMainWindow::dragMoveEvent
//---------------------------------------------------*/
void QLeoMainWindow::dragMoveEvent( QDragMoveEvent *inEvent )
{
	inEvent->accept();
}

/*----------------------------------------------------
// QLeoMainWindow::dropEvent
//---------------------------------------------------*/
void QLeoMainWindow::dropEvent( QDropEvent *inEvent )
{
	_LApplication_gGlobal.mDropHandler();
}

/*----------------------------------------------------
// KillerFilterApp::eventFilter
/---------------------------------------------------*/
bool KillerFilterApp::eventFilter( QObject *inObject, QEvent *inEvent)
{
	QTextEdit *theEdit;
	int para, index;
	static int  sCursorVerticalPosition = -1;
	int theNumberOfParagraph = -1;
	LTextEditor_THandler theHandlePtr;

	theEdit = (QTextEdit*)inObject;
			
	if ( _LApplication_gGlobal.mEndApplication ) {
		
		return FALSE;
	
	}

	if ( inEvent->type() == QEvent::WindowActivate ) {

		QWidget *theWidgetPtr;

		if( ( theWidgetPtr = _LApplication_gGlobal.mApplication->focusWidget() ) != NULL ) {

		}

	}

	if ( (	inEvent->type() == QEvent::MouseButtonPress ||
			inEvent->type() == QEvent::MouseButtonRelease ||
			inEvent->type() == QEvent::MouseMove ||
			inEvent->type() == QEvent::MouseButtonDblClick ) ) {

		QWidget *theWidgetPtr;

		if( ( theWidgetPtr = _LApplication_gGlobal.mApplication->focusWidget() ) != NULL) {
			
			if ( _LApplication_gGlobal.mWorkspace->activeWindow() == NULL )
				return FALSE;

			QPoint thePoint, theSPoint;
			thePoint = ((QMouseEvent*)inEvent)->globalPos();

			QRect theRect = ( ( QMainWindow* )( _LApplication_gGlobal.mWorkspace->activeWindow() ))->geometry();

			theSPoint.setX( theRect.left() );
			theSPoint.setY( theRect.top() );

			QPoint theMapPoint;

			theSPoint = _LApplication_gGlobal.mApplication->focusWidget()->mapToGlobal( theSPoint );

			QPoint theGlobalPoint = ( (QMouseEvent*)inEvent )->pos();

			theGlobalPoint = theWidgetPtr->mapToGlobal( theGlobalPoint );

			int theTitleWidth;

			theTitleWidth = _LApplication_gGlobal.mWorkspace->activeWindow()->width();
			
			if ( !( _LApplication_gGlobal.mApplication->focusWidget()->hasMouse() ) ) {
					
				return FALSE;

			}

			if( _LApplication_gGlobal.mApplication->focusWidget()->inherits( "QLeoTextEdit" ) &&
				((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mID == LTextEditor_ID ) {

				LTextEditor *theTextEditor = (LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit);
				QScrollBar *theVerticalScrollPtr = ((QTextEdit*)_LApplication_gGlobal.mApplication->focusWidget())->verticalScrollBar();

				if ( theVerticalScrollPtr != NULL ) {

					QPoint theVerticalScrollPoint;
					theVerticalScrollPoint = theVerticalScrollPtr->pos();

					if ( theVerticalScrollPoint.x() != 0 ||
						 theVerticalScrollPoint.y() != 0  ) {

						theVerticalScrollPoint = _LApplication_gGlobal.mApplication->focusWidget()->mapToGlobal( theVerticalScrollPoint );

						if ( theVerticalScrollPoint.x() <  thePoint.x() ) {

							return FALSE;
						
						}
					}
				}

				QScrollBar *theHorizontalScrollPtr = ((QTextEdit*)_LApplication_gGlobal.mApplication->focusWidget())->horizontalScrollBar();

				if ( theHorizontalScrollPtr != NULL ) {

					QPoint theHorizontalScrollPoint;
					theHorizontalScrollPoint = theHorizontalScrollPtr->pos();

					if ( theHorizontalScrollPoint.x() != 0 ||
						 theHorizontalScrollPoint.y() != 0  ) {

						theHorizontalScrollPoint = _LApplication_gGlobal.mApplication->focusWidget()->mapToGlobal( theHorizontalScrollPoint );

						if ( theHorizontalScrollPoint.y() < thePoint.y() ) {

							return FALSE;

						}
					}
				}

				if ( inEvent->type() == QEvent::MouseButtonPress &&
					 ((QMouseEvent*)inEvent)->button() == Qt::LeftButton) {

					if ( ((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mLCLKHandler != NULL ) {

						if ( !(((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mLCLKLater) ) {

							theHandlePtr = (LTextEditor_THandler)((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mLCLKHandler;

							if (theHandlePtr( theTextEditor, ((QMouseEvent*)inEvent)->x(), ((QMouseEvent*)inEvent)->x()) )
								return TRUE;
							else
								return FALSE;
						}
						else
							return FALSE;
					}
				}

				if ( inEvent->type() == QEvent::MouseButtonPress &&
					 ((QMouseEvent*)inEvent)->button() == Qt::RightButton) {

					if ( ((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mRCLKHandler != NULL ) {
								
						if ( !(((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mRCLKLater) ) {

							theHandlePtr = (LTextEditor_THandler)((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mRCLKHandler;

							if ( theHandlePtr(theTextEditor, ((QMouseEvent*)inEvent)->x(), ((QMouseEvent*)inEvent)->y()) )
								return TRUE;
							else
								return FALSE;

						}
						else
							return FALSE;
					}
				}

				if ( inEvent->type() == QEvent::MouseButtonDblClick) {

					if ( ((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mLDBLCLKHandler != NULL ) {

						if ( !(((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mLDBLCLKLater) ) {

							theHandlePtr = (LTextEditor_THandler)((LTextEditor*)((((QLeoTextEdit*)_LApplication_gGlobal.mApplication->focusWidget()))->cLTextEdit))->mLDBLCLKHandler;

								if (theHandlePtr( theTextEditor, ((QMouseEvent*)inEvent)->x(), ((QMouseEvent*)inEvent)->y()) )
									return TRUE;
								else
									return FALSE;

						}
						else
							return FALSE;
					}
				}

				return FALSE;

			}
			else
   				return FALSE;
		}
	}
 	else
		return FALSE;

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



