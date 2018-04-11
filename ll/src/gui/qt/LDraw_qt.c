/* ============================================================================
 *  LDraw_qt.c
 * ============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Jun 25, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/21 19:07:41 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.6 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qpainter.h>
#include <qfontmetrics.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qpixmap.h>
#include <qframe.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qmainwindow.h>
#include <LWindow.h>
#include <qframe.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qmainwindow.h>
#include <qimage.h>
#include <qpointarray.h>
#include <qcursor.h>

#include "LDraw.h"
/*#include "LWindow.h"
#include "LCursor.h"*/
#include "LApplication.h"
#include "LDraw_qt.h"
#include "LApplication_qt.h"
#include "LWindow_qt.h"
#include "LBitmap_qt.h"
#include "LFont.h"
#include "LCursor_qt.h"


/*extern QLeoMainWindow* _gMainWindow;
extern void *_gWidgetFocused;
extern ui2 _gWidgetFocusedId;*/

static QColor gsDraw_TextColor;
static QColor gsDraw_PenColor;  /* Controllare se serve ?! */

/*----------------------------------
// CreateGlobal ()
//---------------------------------*/
LDraw* LDraw_CreateGlobal (LWindow* inWindow, ui4 inX, ui4 inY, ui4 inDX, ui4 inDY, Bool inFrameOn)
{
	LDraw theLDraw;
	LDraw *theDrawPtr;
	

	theLDraw.mQLeoDraw = new QLeoDraw(inWindow->mHwnd);
			

	/* check for call failure */
	if (theLDraw.mQLeoDraw == NULL)
		Throw(LDraw_CANT_CREATE);

	if (inFrameOn == true) {
		theLDraw.mQLeoDraw->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
		theLDraw.mQLeoDraw->setLineWidth(1);
		theLDraw.mQLeoDraw->setMidLineWidth(1);
	}
	
	theLDraw.mQLeoDraw->resize(inDX, inDY);
	theLDraw.mQLeoDraw->move(inX, inY);

	theLDraw.mQLeoDraw->setBackgroundMode(Qt::NoBackground);

	theLDraw.mQLeoDraw->setMouseTracking(true);

	QDesktopWidget *theDesktop = QApplication::desktop();
	theLDraw.mmaxX = theDesktop->width();
	theLDraw.mmaxY = theDesktop->height();
	
	theLDraw.mQPainter = _LApplication_gGlobal.mMainWindow->cGraphicsContext;
	theLDraw.mQPixmap = _LApplication_gGlobal.mMainWindow->cGlobalBuffer;

	theLDraw.mX = 0;
	theLDraw.mY = 0;
	
	QColor theColor(0, 0, 0);
	gsDraw_PenColor = theColor;
	
	theLDraw.mQPixmap->fill(Qt::white);

	theLDraw.mType = LDraw_GLOBAL;
	theLDraw.mMouseDownHandler = NULL;
	theLDraw.mRefreshHandler = NULL;
	theLDraw.mMouseMoveHandler = NULL;
	theLDraw.mQPen = NULL;
	theLDraw.mQBrush = NULL;
	theLDraw.mFrame = inFrameOn;
	theLDraw.mID = LDraw_ID;
	theLDraw.mCursorShape = LCursor_ARROW;
	theLDraw.mParentWindow = inWindow;


#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Draw");
#endif
	
	theDrawPtr = (LDraw *)LMemory_NewObject(LDraw, theLDraw);
	
	theLDraw.mQLeoDraw->cLDraw = theDrawPtr;

	LDraw_SetTextColor(theDrawPtr, 0x00000000);

	
	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theDrawPtr;
	theWindowListItemPtr->mID = LDraw_ID;

	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);

#ifdef PRINT_DEBUG
	qDebug( "\n\r Create GlobalDraw List Item 0x%x",  theDrawPtr);
#endif

	return theDrawPtr;
}

/*----------------------------------
// CreateLocal ()
//---------------------------------*/
LDraw* LDraw_CreateLocal (LWindow* inWindow, ui4 inX, ui4 inY, ui4 inDX, ui4 inDY, ui4 inBDX, ui4 inBDY, Bool inFrameOn)
{
	LDraw theLDraw;
	LDraw *theDrawPtr;


	theLDraw.mQLeoDraw = new QLeoDraw(inWindow->mHwnd);
	
	/* check for call failure */
	if (theLDraw.mQLeoDraw == NULL)
		Throw(LDraw_CANT_CREATE);
 
	if (inFrameOn == true) {
		theLDraw.mQLeoDraw->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
		theLDraw.mQLeoDraw->setLineWidth(1);
		theLDraw.mQLeoDraw->setMidLineWidth(1);
	}

	theLDraw.mQLeoDraw->resize(inDX, inDY);
	theLDraw.mQLeoDraw->move(inX, inY);

	theLDraw.mQLeoDraw->setMouseTracking(true);
	
	QColor theGreyColor(220,220,220);
	theLDraw.mQLeoDraw->setPaletteBackgroundColor(theGreyColor);

	theLDraw.mQLeoDraw->setBackgroundMode(Qt::NoBackground);
	
	theLDraw.mQPainter = new QPainter();
	theLDraw.mQPixmap = new QPixmap(inBDX, inBDY);
	
	theLDraw.mX = 0;
	theLDraw.mY = 0;

	theLDraw.mmaxX = inBDX;
	theLDraw.mmaxY = inBDY;
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r theLDraw.mmaxX = %d   theLDraw.mmaxY = %d", theLDraw.mmaxX  , theLDraw.mmaxY);
#endif

	
	QColor theColor(0, 0, 0);
	gsDraw_PenColor = theColor;
	
	theLDraw.mQPixmap->fill(Qt::white);
	
	theLDraw.mType = LDraw_LOCAL;
	theLDraw.mMouseDownHandler = NULL;
	theLDraw.mRefreshHandler = NULL;
	theLDraw.mMouseMoveHandler = NULL;
	theLDraw.mQPen = NULL;
	theLDraw.mQBrush = NULL;
	theLDraw.mFrame = inFrameOn;
	theLDraw.mID = LDraw_ID;
	theLDraw.mCursorShape = LCursor_ARROW;
	theLDraw.mParentWindow = inWindow;

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Draw");

#endif
	theDrawPtr = (LDraw *)LMemory_NewObject(LDraw, theLDraw);
	
	theLDraw.mQLeoDraw->cLDraw = theDrawPtr;
	
	LDraw_SetTextColor(theDrawPtr, 0x00000000);

	
	QWidgetDestroyList* theWindowListItemPtr = new QWidgetDestroyList();

	theWindowListItemPtr->mWidgetListPtr = theDrawPtr;
	theWindowListItemPtr->mID = LDraw_ID;


	inWindow->mHwnd->cWidgetList.append(theWindowListItemPtr);
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r Create LocalDraw List Item 0x%x",  theDrawPtr);
#endif
	
	theLDraw.mQPainter->begin(theLDraw.mQPixmap);
	
	return theDrawPtr;
}

/*---------------------------------
// Destroy ()
//--------------------------------*/
void LDraw_Destroy (LDraw* inDraw)
{
	if (inDraw->mType != LDraw_GLOBAL) {

#ifdef PRINT_DEBUG
	qDebug( "\n\r inDraw->mQPainter->end");

#endif

		inDraw->mQPainter->end();
		
		delete inDraw->mQPainter;
		delete inDraw->mQPixmap;
	}
	

	delete inDraw->mQLeoDraw;
	LMemory_DeleteObject (&inDraw);

}

/*--------------------------------
// Resize ()
//-------------------------------*/
void LDraw_Resize (LDraw* inDraw, ui4 inWidth, ui4 inHeight)
{
#ifdef PRINT_DEBUG
		qDebug( "\n\r inDraw->mQLeoDraw %p,  inDraw %p", inDraw->mQLeoDraw, inDraw);
#endif

	inDraw->mQLeoDraw->resize(int(inWidth),int(inHeight));
}

/*--------------------------------
// Move ()
//-------------------------------*/
void LDraw_Move (LDraw* inDraw, ui4 inX, ui4 inY)
{
	inDraw->mQLeoDraw->move(int(inX),int(inY));
}

/*---------------------------------------------------------------------------------
// InstallEvent ()
//---------------------------------------------------------------------------------*/
void LDraw_InstallEvent(LDraw* inDraw, LDraw_TEventID inEventID, LDraw_THandler inEventHandler) 
{
	switch (inEventID) {
		
		case LDraw_MOUSE_DOWN:
			inDraw->mMouseDownHandler = (/*_LDraw_THandler_mouse*/_LDraw_THandler)inEventHandler;
		break;
		
		case LDraw_MOUSE_MOVE:
			inDraw->mMouseMoveHandler = (/*_LDraw_THandler_mouse_move*/_LDraw_THandler)inEventHandler;
		break;
			
		case LDraw_REFRESH:
			inDraw->mRefreshHandler = (/*_LDraw_THandler_refresh*/_LDraw_THandler)inEventHandler;
		break;

	}
	
}


/*-------------------------------
// SetPixel ()
//------------------------------*/
void LDraw_SetPixel (LDraw* inDraw, i4 inX, i4 inY, ui4 inColor)
{

	QImage theImage;

	theImage = inDraw->mQPixmap->convertToImage();
	
	theImage.setPixel(inX, inY, inColor);
	
	inDraw->mQPixmap->convertFromImage(theImage, 0 );

	inDraw->mQLeoDraw->update();

}

/*-------------------------------------
// GetPixel ()
//------------------------------------*/
ui4 LDraw_GetPixel (LDraw* inDraw, i4 inX, i4 inY)
{
	QImage theImage;
	ui4 thePixelColor = 0;
	
	theImage = inDraw->mQPixmap->convertToImage();
	
	QRgb theRgb = theImage.pixel(inX, inY);
	
	thePixelColor  =  qRed(theRgb);
	thePixelColor |= (qGreen(theRgb) << 8);
	thePixelColor |= (qBlue(theRgb) << 16);
	
	inDraw->mQPixmap->convertFromImage(theImage, 0 );

	return	thePixelColor;
}

/*------------------------------------
// MoveTo ()
//-----------------------------------*/
void LDraw_MoveTo (LDraw* inDraw, i4 inX, i4 inY)
{
	inDraw->mQPainter->moveTo( int(inX), int(inY) );
}

/*------------------------------------

// LineTo ()
//-----------------------------------*/
void LDraw_LineTo (LDraw* inDraw, i4 inX, i4 inY)
{

	inDraw->mQPainter->lineTo( int(inX), int(inY) );
	
}

/*------------------------------------
// GetCurrentPosition ()
//-----------------------------------*/
void LDraw_GetCurrentPosition (LDraw* inDraw, i4* outX, i4* outY)
{

	QPoint thePoint;

	thePoint = inDraw->mQPainter->pos();

	*outX = thePoint.x();
	*outY = thePoint.y();
	
}

/*------------------------------------
// Rectangle ()
//-----------------------------------*/
void LDraw_Rectangle (LDraw* inDraw, i4 inTopX, i4 inTopY, i4 inBottomX, i4 inBottomY)
{
	inDraw->mQPainter->drawRect(int(inTopX), int(inTopY), int(inBottomX-inTopX), int(inBottomY-inTopY));
}

/*------------------------------------
// Ellipse ()
//-----------------------------------*/
void LDraw_Ellipse (LDraw* inDraw, i4 inTopX, i4 inTopY, i4 inBottomX, i4 inBottomY)
{
	inDraw->mQPainter->drawEllipse(int(inTopX), int(inTopY), int(inBottomX-inTopX), int(inBottomY-inTopY));
}

/*------------------------------------
// Polygon ()
//-----------------------------------*/
void LDraw_Polygon (LDraw* inDraw, i4* inVertices, ui4 inNumVertices)
{
	
    QPointArray theVerticesArray(inNumVertices);

#ifdef PRINT_DEBUG
	qDebug( "\n\r LDraw_Polygon %d",inNumVertices);
#endif

	int n=0;
	
	for (n=0; n < inNumVertices; n++) {
        	
#ifdef PRINT_DEBUG
	qDebug( "\n\r Vertice(%d) = %d  Vertice(%d) = %d",n*2,(int)*(inVertices+n*2),n*2+1,(int)*(inVertices+n*2+1));
#endif
        	theVerticesArray[n] = QPoint( (int)*(inVertices+n*2), (int)*(inVertices+n*2+1) );
         	
        }
#ifdef PRINT_DEBUG
	qDebug( "\n\r theVerticesArray");
#endif


    inDraw->mQPainter->drawPolygon(theVerticesArray);
    
}

/*------------------------------------
// SetPen ()
//-----------------------------------*/
void LDraw_SetPen (LDraw* inDraw, ui4 inColor, ui4 inWidth)
{
 	int theRed   =   inColor & 0x000000ff;
	int theGreen = ( inColor & 0x0000ff00 ) >> 8;
	int theBlue  = ( inColor & 0x00ff0000 ) >> 16;
	
	QColor theColor(theRed, theGreen, theBlue);
	
	QPen thePen(theColor, inWidth);
	
	gsDraw_PenColor = theColor;
	
	inDraw->mQPainter->setPen(thePen);
	
}

/*------------------------------------
// SetBrush ()
//-----------------------------------*/
void LDraw_SetBrush (LDraw* inDraw, ui4 inColor)
{
	int theRed   =   inColor & 0x000000ff;
	int theGreen = ( inColor & 0x0000ff00 ) >> 8;
	int theBlue  = ( inColor & 0x00ff0000 ) >> 16;
	
	QColor theColor(theRed, theGreen, theBlue);
	
	QBrush theBrush(theColor);
	
	gsDraw_PenColor = theColor;
	
#ifndef PRINT_DEBUG
	qDebug( "\n\r inDraw 0x%x",inDraw);
#endif

	inDraw->mQPainter->setBrush(theBrush);
	
#ifndef PRINT_DEBUG
	qDebug( "\n\r inDraw->mQPainter->setBrush(theBrush) 0x%x",inDraw->mQPainter);
#endif

	
}

/*-----------------------------------
// SetPenToTransparent ()
//----------------------------------*/
void LDraw_SetPenToTransparent (LDraw* inDraw)
{
	inDraw->mQPainter->setBackgroundMode(Qt::TransparentMode);
}

/*-----------------------------------
// SetBrushToTransparent ()
//----------------------------------*/
void LDraw_SetBrushToTransparent (LDraw* inDraw)
{
	inDraw->mQPainter->setBackgroundMode(Qt::TransparentMode);
}

/*--------------------------------------------------
// Update ()
//-------------------------------------------------*/
void LDraw_Update(LDraw* inDraw)
{
	inDraw->mQLeoDraw->update();
}

/*--------------------------------------------------
// SaturateBuffer ()
//-------------------------------------------------*/
void LDraw_SaturateBuffer (LDraw* inDraw)
{
	QBrush theBrush;
	QColor theColor;
	
	theBrush = inDraw->mQPainter->brush();
	
	theColor = theBrush.color();
	
	inDraw->mQPixmap->fill(theColor/*Qt::white*/);
	
}

/*-------------------------------------------------
// BlitBitmap ()
//------------------------------------------------*/
void LDraw_BlitBitmap (LDraw* inDraw, LBitmap* inBitmap, i4 inX, i4 inY)
{
	bitBlt( inDraw->mQPixmap , inX, inY, inBitmap->mHbitmap );	
}

/*-----------------------------------
// GetBitmap ()
//----------------------------------*/
LBitmap* LDraw_GetBitmap (LDraw* inDraw, i4 inX, i4 inY, ui4 inWidth, ui4 inHeight)
{
	LBitmap theBitMap;
	LBitmap* theBitMapPtr;
	
	QPixmap* thePixMapPtr = new QPixmap(inWidth, inHeight);
	
	bitBlt( thePixMapPtr , 0, 0, inDraw->mQPixmap, inX, inY, inWidth,inHeight );
	
	theBitMap.mHbitmap = thePixMapPtr;
	
	theBitMap.mDX = thePixMapPtr->width();
	theBitMap.mDY = thePixMapPtr->height();

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Bitmap Draw");
#endif
	theBitMapPtr = (LBitmap*)LMemory_NewObject (LBitmap, theBitMap);
	
	QBitmapDestroyList* theBitmapListItemPtr = new QBitmapDestroyList();

	theBitmapListItemPtr->mBitmapListPtr = theBitMapPtr;

	_LApplication_gGlobal.mMainWindow->cBitmapList.append(theBitmapListItemPtr);

	return theBitMapPtr;
		
}

/*----------------------------------------
// DrawText ()
//--------------------------------------*/
void LDraw_DrawText (LDraw* inDraw, i4 inX, i4 inY, i1* inString, ui4 inLength)
{
	QPen theCurrentPen(inDraw->mQPainter->pen()); 
	QPen thePen(gsDraw_TextColor,0);
	
	inDraw->mQPainter->setPen(thePen);
	
	QString theString = QString::fromLatin1(inString, inLength );
	
	QFontMetrics theFontMetrics = inDraw->mQPainter->fontMetrics();

	int theFontHeight = theFontMetrics.height();

	inDraw->mQPainter->drawText(int(inX), int(inY) + theFontHeight, theString, int(inLength), QPainter::Auto );

	inDraw->mQPainter->setPen(theCurrentPen);
}

/*-------------------------------
// SetTextColor ()
//------------------------------*/
void LDraw_SetTextColor (LDraw* inDraw, ui4 inColor)
{
	int theRed   =   inColor & 0x000000ff;
	int theGreen = ( inColor & 0x0000ff00 ) >> 8;
	int theBlue  = ( inColor & 0x00ff0000 ) >> 16;
	
	QColor theColor(theRed, theGreen, theBlue);
	
	gsDraw_TextColor = theColor;
	
}

/*------------------------------
// SetBGTextColor ()
//-----------------------------*/
void LDraw_SetBGTextColor (LDraw* inDraw, ui4 inColor)
{
	int theRed   =   inColor & 0x000000ff;
	int theGreen = ( inColor & 0x0000ff00 ) >> 8;
	int theBlue  = ( inColor & 0x00ff0000 ) >> 16;
	
	QColor theColor(theRed, theGreen, theBlue);
	
	inDraw->mQPainter->setBackgroundColor ( theColor ); 
	
}

/*-----------------------------
// SetBGTextMode ()
//----------------------------*/
void LDraw_SetBGTextMode (LDraw* inDraw, LDraw_TBGTextMode inMode)
{
	switch(inMode) {
		
		case LDraw_OPAQUE :
			inDraw->mQPainter->setBackgroundMode(Qt::OpaqueMode);
		break;
   		
   		case LDraw_TRANSPARENT :
			inDraw->mQPainter->setBackgroundMode(Qt::TransparentMode);
		break;
		
		default  :
			inDraw->mQPainter->setBackgroundMode(Qt::TransparentMode);
		break;
    
	}
	
}

/*------------------------------
// GetLineSpacing ()
//-----------------------------*/
ui4 LDraw_GetLineSpacing (LDraw* inDraw)
{
	
#ifdef PRINT_DEBUG	
	qDebug( "\n\r inDraw->mQPainter->fontMetrics().lineSpacing() %d",inDraw->mQPainter->fontMetrics().lineSpacing());
#endif

	return  ui4(inDraw->mQPainter->fontMetrics().lineSpacing());
}

/*-------------------------------
// GetStringPixLength ()
//------------------------------*/
ui4 LDraw_GetStringPixLength (LDraw* inDraw, i1* inString, ui4 inLength)
{

#ifndef PRINT_DEBUG	
	qDebug( "\n\r inDraw->mQPainter->fontMetrics().width(inString) %d",inDraw->mQPainter->fontMetrics().width(inString));
#endif
	
	return  ui4(inDraw->mQPainter->fontMetrics().width(inString));
	
}

/*-------------------------------
// SetTextFont ()
//------------------------------*/
void LDraw_SetTextFont (LDraw* inDraw, LFont inFont)
{
	QFont theFont;
	
	switch (inFont.mName) {
		
		case LFont_COURIER_NEW:
		     theFont.setFamily("Courier");
		break;
		
		case LFont_COURIER:
		     theFont.setFamily("Fixed");
		break;
		
		case LFont_TIMES_NEW_ROMAN:
			theFont.setFamily("Times");
		break;
		
		case LFont_SYMBOL:
			theFont.setFamily("Symbol");
		break;

		case LFont_VERDANA:
			theFont.setFamily("Verdana");
		break;
		
		case LFont_ARIAL:
			theFont.setFamily("Newspaper");
		break;
	}
	
	inDraw->mQPainter->setFont( theFont );

}

/*--------------------------------
// GetParentWindow ()
//-------------------------------*/
LWindow* LDraw_GetParentWindow (LDraw* inDraw)
{
	return inDraw->mParentWindow;
}

/*-----------------------------------------------
// MoveOrigin ()
//---------------------------------------------*/
void LDraw_MoveOrigin (LDraw* inDraw, i4 inX, i4 inY)
{
	inDraw->mX = inX;
	inDraw->mY = inY;
}

/*--------------------------------------
// SetCursorShape ()
//--------------------------------------*/
void LDraw_SetCursorShape (LDraw* inDraw, LCursor_TShape inCursorShape)
{
#ifdef PRINT_DEBUG
	//qDebug( "\n\r LDraw_SetCursorShape inCursorShape 0x%x",inCursorShape);

#endif


	inDraw->mCursorShape = inCursorShape;
#ifdef PRINT_DEBUG
	//qDebug( "\n\r LDraw_SetCursorShape mCursorShape 0x%x",inDraw->mCursorShape);

#endif

}

/*---------------------------------------
// GetCursorShape ()
//--------------------------------------*/
LCursor_TShape LDraw_GetCursorShape (LDraw* inDraw)
{
	return inDraw->mCursorShape;
}


/*-----------------------------------------------
// QLeoDraw::QLeoDraw
//---------------------------------------------*/
QLeoDraw::QLeoDraw(QChildWindow *inMainWindow) : QFrame((QWidget*)inMainWindow)
{
	cLDraw = NULL;
	setFocusPolicy(QWidget::StrongFocus);
}

/*-----------------------------------------------
// QLeoDraw::~QLeoDraw
//---------------------------------------------*/
QLeoDraw::~QLeoDraw()
{

}

/*-------------------------------------
// QLeoDraw::focusInEvent ()
//------------------------------------*/
void QLeoDraw::focusInEvent( QFocusEvent *inFocusEvent )
{

	_LApplication_gGlobal.mWidgetFocused = ((LDraw*)(((QLeoDraw*)this)->cLDraw));
	_LApplication_gGlobal.mWidgetFocusedId = ((LDraw*)(((QLeoDraw*)this)->cLDraw))->mID;

#ifdef PRINT_DEBUG
	qDebug( "focusInEvent 0x%x   _gWidgetFocusedId 0x%x",  _LApplication_gGlobal.mWidgetFocused, _LApplication_gGlobal.mWidgetFocusedId);
#endif

}

/*-----------------------------------------------
// QLeoDraw::paintEvent
//---------------------------------------------*/
void QLeoDraw::paintEvent(QPaintEvent *inEventPtr)
{

	bool theResult;
	LDraw_THandler theHandlePtr;


	QFrame::paintEvent(inEventPtr);
#ifndef PROVA

	if (((LDraw *)(this->cLDraw))->mRefreshHandler != NULL ){
	
		theHandlePtr = (LDraw_THandler)(((LDraw *)(this->cLDraw))->mRefreshHandler);

		theResult = theHandlePtr((LDraw *)(this->cLDraw));
	}

	if (((LDraw *)(this->cLDraw))->mFrame == true)

		bitBlt((((LDraw *)(this->cLDraw))->mQLeoDraw),
			FRAME_X,
			FRAME_Y,
			((LDraw *)(this->cLDraw))->mQPixmap,
			-1* ((LDraw *)(this->cLDraw))->mX,
			-1*((LDraw *)(this->cLDraw))->mY,
			width() - FRAME_X*2,
			height() - FRAME_Y*2);

	else
		bitBlt((((LDraw *)(this->cLDraw))->mQLeoDraw),
			0,
			0,
			((LDraw *)(this->cLDraw))->mQPixmap,
			-1*((LDraw *)(this->cLDraw))->mX,
			-1*((LDraw *)(this->cLDraw))->mY,
			width(),
			height());

#ifdef PRINT_DEBUG
	//qDebug( "\n\r width = %d   height = %d",  width(), height());
#endif
	

	int theFrameWidth = (((LDraw *)(this->cLDraw))->mQLeoDraw)->width();
	int theFrameHeight = (((LDraw *)(this->cLDraw))->mQLeoDraw)->height();

	if (((LDraw *)(this->cLDraw))->mFrame == true) {

		if ( ((LDraw *)(this->cLDraw))->mmaxX + FRAME_X < theFrameWidth) {

			QPixmap theQPixmap(theFrameWidth - (((LDraw *)(this->cLDraw))->mmaxX) - (FRAME_X*2),
	  			 	   		   theFrameHeight-(FRAME_Y*2 ));

			QColor theColor(195,195,195);
			theQPixmap.fill(theColor);

			bitBlt((((LDraw *)(this->cLDraw))->mQLeoDraw),
				((LDraw *)(this->cLDraw))->mmaxX + FRAME_X,
				FRAME_X,
		   		&theQPixmap);

#ifdef PRINT_DEBUG
	//qDebug( "\n\r ((LDraw *)(this->cLDraw))->mmaxX  = %d   theFrameWidth = %d", ((LDraw *)(this->cLDraw))->mmaxX  , theFrameWidth);
#endif


		}
	}

	else 
		if ( ((LDraw *)(this->cLDraw))->mmaxX < theFrameWidth) {

			QPixmap theQPixmap(theFrameWidth - (((LDraw *)(this->cLDraw))->mmaxX),
	  			 	   		   theFrameHeight);

			QColor theColor(195,195,195);
			theQPixmap.fill(theColor);

			bitBlt((((LDraw *)(this->cLDraw))->mQLeoDraw),
				((LDraw *)(this->cLDraw))->mmaxX,
				0 ,
		   		&theQPixmap);

#ifdef PRINT_DEBUG
	//qDebug( "\n\rfalse  ((LDraw *)(this->cLDraw))->mmaxX  = %d   theFrameWidth = %d", ((LDraw *)(this->cLDraw))->mmaxX  , theFrameWidth);

#endif


		}



	if (((LDraw *)(this->cLDraw))->mFrame == true) {
	
		if ( ((LDraw *)(this->cLDraw))->mmaxY + FRAME_Y  < theFrameHeight) {


			QPixmap theQPixmap((((LDraw *)(this->cLDraw))->mmaxX + FRAME_X),
		                   	theFrameHeight - (((LDraw *)(this->cLDraw))->mmaxY) - (FRAME_Y*2));

			// the color is set grey
			QColor theColor(195,195,195);
			theQPixmap.fill(theColor);

			bitBlt((((LDraw *)(this->cLDraw))->mQLeoDraw ),
				FRAME_X,
				((LDraw *)(this->cLDraw))->mmaxY + FRAME_Y/*6*/,
				&theQPixmap);

		}
	}
	else 
		if ( ((LDraw *)(this->cLDraw))->mmaxY   < theFrameHeight) {


			QPixmap theQPixmap((((LDraw *)(this->cLDraw))->mmaxX ),
		                   	theFrameHeight - (((LDraw *)(this->cLDraw))->mmaxY) );

			// the color is set grey
			QColor theColor(195,195,195);
			theQPixmap.fill(theColor);

			bitBlt((((LDraw *)(this->cLDraw))->mQLeoDraw ),
				0,
				((LDraw *)(this->cLDraw))->mmaxY,
				&theQPixmap);

	}

	QFrame::paintEvent(inEventPtr);

#endif /* PROVA */

}

/*-----------------------------------------------
// QLeoDraw::mousePressEvent
//---------------------------------------------*/
void QLeoDraw::mousePressEvent(QMouseEvent *inEventPtr)
{
		LDraw_THandler theHandlePtr;


	if (((LDraw *)(this->cLDraw))->mMouseDownHandler != NULL )
	{
				theHandlePtr = (LDraw_THandler)(((LDraw *)(this->cLDraw))->mMouseDownHandler);


		if (((LDraw *)(this->cLDraw))->mFrame == TRUE)
			theHandlePtr(   (LDraw *)(this->cLDraw),
									inEventPtr->x() - FRAME_X,
									inEventPtr->y() - FRAME_Y);
		else
			theHandlePtr(   (LDraw *)(this->cLDraw),
									inEventPtr->x(),
									inEventPtr->y());
}
}

/*-----------------------------------------------
// QLeoDraw::mouseMoveEvent
//---------------------------------------------*/
void QLeoDraw::mouseMoveEvent(QMouseEvent *inEventPtr)
{
	LWindow* theWindowPtr;
	int theShape;
	QCursor theCursor;
	LDraw_THandler theHandlePtr;

	
#ifdef PRINT_DEBUG
	//qDebug( "\n\r QLeoDraw::mouseMoveEvent PPPPPPPPPPPPPPPPPPPPPPP");

#endif
	
	if (((LDraw *)(this->cLDraw))->mMouseMoveHandler != NULL ) {

#ifdef PRINT_DEBUG
	//qDebug( "\n\r QLeoDraw::mouseMoveEvent ");

#endif

		theWindowPtr = LDraw_GetParentWindow((LDraw *)(this->cLDraw));

#ifdef PRINT_DEBUG
	//qDebug( "\n\r QLeoDraw::mouseMoveEvent theWindowPtr 0x%x", theWindowPtr);

#endif
				theHandlePtr = (LDraw_THandler)(((LDraw *)(this->cLDraw))->mMouseMoveHandler);

		/*if (!*/theHandlePtr(	(LDraw *)(this->cLDraw)/*theWindowPtr*/,
									inEventPtr->x() - FRAME_X,
									inEventPtr->y() - FRAME_Y);/*)*/
			//LCursor_SetShape(LCursor_ARROW);
			//QApplication::restoreOverrideCursor();
#ifdef PRINT_DEBUG

	//qDebug( "\n\r QLeoDraw::mouseMoveEvent x %d, y %d", inEventPtr->x() - FRAME_X, inEventPtr->y() - FRAME_Y);
	//qDebug( "\n\r QLeoDraw::mouseMoveEvent mMouseMoveHandler 0x%x", ((LDraw *)(this->cLDraw))->mMouseMoveHandler);

#endif


	}

#ifdef PRINT_DEBUG
	//qDebug( "\n\r mCursorShape 0x%x",((LDraw *)(this->cLDraw))->mCursorShape);
	//qDebug( "\n\r LDraw_GetCursorShape 0x%x",LDraw_GetCursorShape ((LDraw *)this->cLDraw));

#endif

	theShape = _LCursor_GetShape( LDraw_GetCursorShape ((LDraw *)this->cLDraw) );


	/*switch (
		LDraw_GetCursorShape ((LDraw *)this->cLDraw)	) {
		case LCursor_APPSTARTING:

			theShape = Qt::WaitCursor;
			break;
		case LCursor_ARROW:
			theShape = Qt::ArrowCursor;
			break;
		case LCursor_CROSS:
			theShape = Qt::CrossCursor;
			break;
		case LCursor_IBEAM:
			theShape = Qt::IbeamCursor;
			break;
		case LCursor_NO:
			theShape = Qt::BlankCursor;
			break;
		case LCursor_SIZEALL:
			theShape = Qt::SizeAllCursor;
			break;
		case LCursor_SIZENESW:

			theShape = Qt::SizeBDiagCursor;
			break;
		case LCursor_SIZENS:

			theShape = Qt::SizeVerCursor;
			break;
		case LCursor_SIZENWSE:

			theShape = Qt::SizeFDiagCursor;
			break;
		case LCursor_SIZEWE:

			theShape = Qt::SizeHorCursor;
			break;
		case LCursor_UPARROW:
			theShape = Qt::UpArrowCursor;
			break;
		case LCursor_WAIT:
			theShape = Qt::WaitCursor;
			break;
		default :
			theShape = Qt::ArrowCursor;
		break;
	}*/

	theCursor.setShape( theShape );

	this->setCursor( theCursor );


	/*else
		LCursor_SetShape(LCursor_ARROW);*/

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


