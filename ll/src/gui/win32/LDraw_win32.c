/* ============================================================================
 *  LDraw_win32.c
 * ============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Apr 30, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LDraw.h"
#include "LDraw_win32.h"
#include "LWindow_win32.h"
#include "LBitmap_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*----------------------------------
// CreateGlobal ()
//---------------------------------*/
LDraw* LDraw_CreateGlobal (LWindow* inWindow, ui4 inX, ui4 inY, ui4 inDX, ui4 inDY, Bool inFrameOn)
{
    LDraw theldraw;
    LDraw *theldrawptr;
    HDC thehdc;
    HBRUSH thehbrush;
    HBITMAP thehbit;

    theldraw.mParentWindow=inWindow;

    if (inFrameOn==TRUE)
        theldraw.mHwndDraw=CreateWindowEx (WS_EX_CLIENTEDGE, "DrawWindow", NULL, WS_CHILD|WS_VISIBLE,
            inX, inY, inDX, inDY, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
            _LApplication_gGlobal.inInstance, NULL);
    else
        theldraw.mHwndDraw=CreateWindow ("DrawWindow", NULL, WS_CHILD|WS_VISIBLE,
            inX, inY, inDX, inDY, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
            _LApplication_gGlobal.inInstance, NULL);

	/* check for call failure */
	if (theldraw.mHwndDraw==NULL) Throw(LDraw_CANT_CREATE);

    theldraw.mmaxX=GetSystemMetrics (SM_CXSCREEN);
    theldraw.mmaxY=GetSystemMetrics (SM_CYSCREEN);
    thehdc=GetDC (theldraw.mHwndDraw);
    theldraw.mMemHdc=CreateCompatibleDC (thehdc);
    thehbit=SelectObject (theldraw.mMemHdc, _LApplication_gGlobal.gHBitmap);
    thehbrush=GetStockObject (WHITE_BRUSH);
    SelectObject (theldraw.mMemHdc, thehbrush);
    PatBlt (theldraw.mMemHdc, 0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN), PATCOPY);
    SelectObject (theldraw.mMemHdc, thehbit);
    ReleaseDC (theldraw.mHwndDraw, thehdc);

    theldraw.mType=LDraw_GLOBAL;

    theldraw.mMouseDownHandler=NULL;
    theldraw.mRefreshHandler=NULL;
    theldraw.mMouseMoveHandler=NULL;

    theldraw.mHpen=NULL;
    theldraw.mHbrush=NULL;

    theldraw.mLocalBuffer=NULL;
    theldraw.mID=LDraw_ID;

    theldraw.Ox=0;
    theldraw.Oy=0;

	theldraw.mCursorShape=LCursor_ARROW;

	theldraw.mFont=NULL;

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;  
    
    theldrawptr=LMemory_NewObject (LDraw, theldraw);
    SetWindowLong (theldraw.mHwndDraw, GWL_USERDATA, (long) theldrawptr); /*mutual reference pointer...*/
    return theldrawptr;
}

/*----------------------------------
// CreateLocal ()
//---------------------------------*/
LDraw* LDraw_CreateLocal (LWindow* inWindow, ui4 inX, ui4 inY, ui4 inDX, ui4 inDY, ui4 inBDX, ui4 inBDY, Bool inFrameOn)
{
    LDraw theldraw;
    LDraw *theldrawptr;
    HDC thehdc;
    HBITMAP thehbit;
    HBRUSH thehbrush;

    theldraw.mParentWindow=inWindow;

    if (inFrameOn==TRUE)
        theldraw.mHwndDraw=CreateWindowEx (WS_EX_CLIENTEDGE, "DrawWindow", NULL, WS_CHILD|WS_VISIBLE,
            inX, inY, inDX, inDY, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
            _LApplication_gGlobal.inInstance, NULL);
    else
        theldraw.mHwndDraw=CreateWindow ("DrawWindow", NULL, WS_CHILD|WS_VISIBLE,
            inX, inY, inDX, inDY, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
            _LApplication_gGlobal.inInstance, NULL);

	/* check for call failure */
	if (theldraw.mHwndDraw==NULL) Throw(LDraw_CANT_CREATE);

    theldraw.mmaxX=inBDX;
    theldraw.mmaxY=inBDY;
    thehdc=GetDC (theldraw.mHwndDraw);
    theldraw.mMemHdc=CreateCompatibleDC (thehdc);
    thehbit=CreateCompatibleBitmap (thehdc, inBDX, inBDY);
    SelectObject (theldraw.mMemHdc, thehbit);
    thehbrush=GetStockObject (WHITE_BRUSH);
    SelectObject (theldraw.mMemHdc, thehbrush);
    PatBlt (theldraw.mMemHdc, 0, 0, inBDX, inBDY, PATCOPY);
    ReleaseDC (theldraw.mHwndDraw, thehdc);

    theldraw.mType=LDraw_LOCAL;

    theldraw.mMouseDownHandler=NULL;
    theldraw.mRefreshHandler=NULL;
    theldraw.mMouseMoveHandler=NULL;

    theldraw.mHpen=NULL;
    theldraw.mHbrush=NULL;

    theldraw.mLocalBuffer=thehbit;
    theldraw.mID=LDraw_ID;

    theldraw.Ox=0;
    theldraw.Oy=0;

	theldraw.mCursorShape=LCursor_ARROW;

	theldraw.mFont=NULL;

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;  
    
    theldrawptr=LMemory_NewObject (LDraw, theldraw);
    SetWindowLong (theldraw.mHwndDraw, GWL_USERDATA, (long) theldrawptr); /*mutual reference pointer...*/
    return theldrawptr;
}

/*---------------------------------
// Destroy ()
//--------------------------------*/
void LDraw_Destroy (LDraw* inDraw)
{
    DestroyWindow (inDraw->mHwndDraw);
	if (inDraw->mFont!=NULL)
		DeleteObject (inDraw->mFont);
    /*LMemory_DeleteObject (inDraw); ...not here! It's done in the window procedure!!!!*/
}

/*--------------------------------
// Resize ()
//-------------------------------*/
void LDraw_Resize (LDraw* inDraw, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inDraw->mHwndDraw, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE); 
}

/*--------------------------------
// Move ()
//-------------------------------*/
void LDraw_Move (LDraw* inDraw, ui4 inX, ui4 inY)
{
    SetWindowPos (inDraw->mHwndDraw, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE); 
}

/*---------------------------------------------------------------------------------
// InstallEvent ()
//---------------------------------------------------------------------------------*/
void LDraw_InstallEvent(LDraw* inDraw, LDraw_TEventID inEventID,
                          LDraw_THandler inEventHandler) 
{
    switch (inEventID)
    {
    case LDraw_MOUSE_DOWN:
        inDraw->mMouseDownHandler=(_LDraw_THandler) inEventHandler;
        break;
    case LDraw_REFRESH:
        inDraw->mRefreshHandler=(_LDraw_THandler) inEventHandler;
        break;
    case LDraw_MOUSE_MOVE:
        inDraw->mMouseMoveHandler=(_LDraw_THandler) inEventHandler;
        break;
    }
}

/*-----------------------------------------------
// MoveOrigin ()
//---------------------------------------------*/
void LDraw_MoveOrigin (LDraw* inDraw, i4 inX, i4 inY)
{
    inDraw->Ox=inX;
    inDraw->Oy=inY;
}

/*-------------------------------
// SetPixel ()
//------------------------------*/
void LDraw_SetPixel (LDraw* inDraw, i4 inX, i4 inY, ui4 inColor)
{
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    SetPixel (inDraw->mMemHdc, inX, inY, inColor);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*-------------------------------------
// GetPixel ()
//------------------------------------*/
ui4 LDraw_GetPixel (LDraw* inDraw, i4 inX, i4 inY)
{
    ui4 thecolor;
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    thecolor=GetPixel (inDraw->mMemHdc, inX, inY);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);

    return thecolor;
}

/*------------------------------------
// MoveTo ()
//-----------------------------------*/
void LDraw_MoveTo (LDraw* inDraw, i4 inX, i4 inY)
{
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    MoveToEx (inDraw->mMemHdc, inX, inY, NULL);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*------------------------------------
// LineTo ()
//-----------------------------------*/
void LDraw_LineTo (LDraw* inDraw, i4 inX, i4 inY)
{
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    LineTo (inDraw->mMemHdc, inX, inY);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*------------------------------------
// GetCurrentPosition ()
//-----------------------------------*/
void LDraw_GetCurrentPosition (LDraw* inDraw, i4* outX, i4* outY)
{
    POINT thepoint;
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    GetCurrentPositionEx (inDraw->mMemHdc, &thepoint);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);

    *outX=thepoint.x;
    *outY=thepoint.y;
}

/*------------------------------------
// Rectangle ()
//-----------------------------------*/
void LDraw_Rectangle (LDraw* inDraw, i4 inTopX, i4 inTopY, i4 inBottomX, i4 inBottomY)
{
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    Rectangle (inDraw->mMemHdc, inTopX, inTopY, inBottomX, inBottomY);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*------------------------------------
// Ellipse ()
//-----------------------------------*/
void LDraw_Ellipse (LDraw* inDraw, i4 inTopX, i4 inTopY, i4 inBottomX, i4 inBottomY)
{
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    Ellipse (inDraw->mMemHdc, inTopX, inTopY, inBottomX, inBottomY);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*------------------------------------
// Polygon ()
//-----------------------------------*/
void LDraw_Polygon (LDraw* inDraw, i4* inVertices, ui4 inNumVertices)
{
    POINT *thepoint;
    ui4 i;
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    thepoint=(POINT *)LMemory_Malloc (sizeof (POINT)*inNumVertices);
    for (i=0; i<inNumVertices; i=i+2)
    {
        thepoint[i].x=*(inVertices+i);
        thepoint[i].y=*(inVertices+i+1);
    }

    Polygon (inDraw->mMemHdc, thepoint, inNumVertices);

    LMemory_Free (&thepoint);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*------------------------------------
// SetPen ()
//-----------------------------------*/
void LDraw_SetPen (LDraw* inDraw, ui4 inColor, ui4 inWidth)
{
    HPEN theHpen;

    theHpen=CreatePen (PS_SOLID, inWidth, inColor);

    SelectObject (inDraw->mMemHdc, theHpen);

    if (inDraw->mHpen!=NULL)
        DeleteObject (inDraw->mHpen);
    inDraw->mHpen=theHpen;
}

/*------------------------------------
// SetBrush ()
//-----------------------------------*/
void LDraw_SetBrush (LDraw* inDraw, ui4 inColor)
{
    HBRUSH theHbrush;

    theHbrush=CreateSolidBrush (inColor);

    SelectObject (inDraw->mMemHdc, theHbrush);

    if (inDraw->mHbrush!=NULL)
        DeleteObject (inDraw->mHbrush);
    inDraw->mHbrush=theHbrush;
}

/*-----------------------------------
// SetPenToTransparent ()
//----------------------------------*/
void LDraw_SetPenToTransparent (LDraw* inDraw)
{
    SelectObject (inDraw->mMemHdc, GetStockObject (NULL_PEN));
}

/*-----------------------------------
// SetBrushToTransparent ()
//----------------------------------*/
void LDraw_SetBrushToTransparent (LDraw* inDraw)
{
    SelectObject (inDraw->mMemHdc, GetStockObject (NULL_BRUSH));
}

/*--------------------------------------------------
// Update ()
//-------------------------------------------------*/
void LDraw_Update (LDraw* inDraw)
{
    InvalidateRect (inDraw->mHwndDraw, NULL, 0);
}

/*--------------------------------------------------
// SaturateBuffer ()
//-------------------------------------------------*/
void LDraw_SaturateBuffer (LDraw* inDraw)
{
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    PatBlt (inDraw->mMemHdc, 0, 0, inDraw->mmaxX, inDraw->mmaxY, PATCOPY);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*-------------------------------------------------
// BlitBitmap ()
//------------------------------------------------*/
void LDraw_BlitBitmap (LDraw* inDraw, LBitmap* inBitmap, i4 inX, i4 inY)
{
    HBITMAP thehbit, thehbit2;
    HDC thehdc;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    thehdc=CreateCompatibleDC (inDraw->mMemHdc);
    thehbit2=SelectObject (thehdc, inBitmap->mHbitmap);
    BitBlt (inDraw->mMemHdc, inX, inY, inBitmap->mDX, inBitmap->mDY, thehdc, 0, 0, SRCCOPY);
    SelectObject (thehdc, thehbit2);
    DeleteDC (thehdc);


    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);

}

/*-----------------------------------
// GetBitmap ()
//----------------------------------*/
LBitmap* LDraw_GetBitmap (LDraw* inDraw, i4 inX, i4 inY, ui4 inWidth, ui4 inHeight)
{
    HBITMAP thehbit, thehbitaux;
    HDC thehdc;
    LBitmap thebitmap;
    LBitmap* thebitmapptr;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);
    
    thebitmap.mHbitmap=CreateCompatibleBitmap (inDraw->mMemHdc, inWidth, inHeight);
    thebitmap.mDX=inWidth;
    thebitmap.mDY=inHeight;
    
    thehdc=CreateCompatibleDC (inDraw->mMemHdc);
    thehbitaux=SelectObject (thehdc, thebitmap.mHbitmap);
    BitBlt (thehdc, 0, 0, inWidth, inHeight, inDraw->mMemHdc, inX, inY, SRCCOPY);
    SelectObject (thehdc, thehbitaux);
    DeleteDC (thehdc);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);

    thebitmapptr=LMemory_NewObject (LBitmap, thebitmap);
    return thebitmapptr;
}

/*----------------------------------------
// DrawText ()
//--------------------------------------*/
void LDraw_DrawText (LDraw* inDraw, i4 inX, i4 inY, i1* inString, ui4 inLength)
{
    HBITMAP thehbit;

    if (inDraw->mType==LDraw_GLOBAL)
        thehbit=SelectObject (inDraw->mMemHdc, _LApplication_gGlobal.gHBitmap);

    TextOut (inDraw->mMemHdc, inX, inY, inString, inLength);

    if (inDraw->mType==LDraw_GLOBAL)
        SelectObject (inDraw->mMemHdc, thehbit);
}

/*-------------------------------
// SetTextColor ()
//------------------------------*/
void LDraw_SetTextColor (LDraw* inDraw, ui4 inColor)
{
    SetTextColor (inDraw->mMemHdc, inColor);
}

/*------------------------------
// SetBGTextColor ()
//-----------------------------*/
void LDraw_SetBGTextColor (LDraw* inDraw, ui4 inColor)
{
    SetBkColor (inDraw->mMemHdc, inColor);
}

/*-----------------------------
// SetBGTextMode ()
//----------------------------*/
void LDraw_SetBGTextMode (LDraw* inDraw, LDraw_TBGTextMode inMode)
{
    if (inMode==LDraw_OPAQUE)
        SetBkMode (inDraw->mMemHdc, OPAQUE);
    if (inMode==LDraw_TRANSPARENT)
        SetBkMode (inDraw->mMemHdc, TRANSPARENT);
}

/*------------------------------
// GetLineSpacing ()
//-----------------------------*/
ui4 LDraw_GetLineSpacing (LDraw* inDraw)
{
    TEXTMETRIC thetm;

    GetTextMetrics (inDraw->mMemHdc, &thetm);

    return (ui4)(thetm.tmHeight+thetm.tmExternalLeading);
}

/*-------------------------------
// GetStringPixLength ()
//------------------------------*/
ui4 LDraw_GetStringPixLength (LDraw* inDraw, i1* inString, ui4 inLength)
{
    SIZE thesize;

    GetTextExtentPoint32 (inDraw->mMemHdc, inString, inLength, &thesize);

    return (ui4) thesize.cx;
}

/*-------------------------------
// SetTextFont ()
//------------------------------*/
void LDraw_SetTextFont (LDraw* inDraw, LFont inFont)
{
	HFONT thefont;
	LOGFONT thelogfont;
	TCHAR thearial[LF_FACESIZE]="Arial";
	TCHAR thecourier[LF_FACESIZE]="Courier";
	TCHAR thecouriernew[LF_FACESIZE]="Courier New";
	TCHAR thesymbol[LF_FACESIZE]="Symbol";
	TCHAR thetimesnewroman[LF_FACESIZE]="Times New Roman";
	TCHAR theverdana[LF_FACESIZE]="Verdana";

	int i;

	/*fills in the struct...*/
	if (inFont.mName==LFont_ARIAL)
		for (i=0; i<LF_FACESIZE; i++)
			thelogfont.lfFaceName[i]=thearial[i];
	if (inFont.mName==LFont_COURIER)
		for (i=0; i<LF_FACESIZE; i++)
			thelogfont.lfFaceName[i]=thecourier[i];
	if (inFont.mName==LFont_COURIER_NEW)
		for (i=0; i<LF_FACESIZE; i++)
			thelogfont.lfFaceName[i]=thecouriernew[i];
	if (inFont.mName==LFont_SYMBOL)
		for (i=0; i<LF_FACESIZE; i++)
			thelogfont.lfFaceName[i]=thesymbol[i];
	if (inFont.mName==LFont_TIMES_NEW_ROMAN)
		for (i=0; i<LF_FACESIZE; i++)
			thelogfont.lfFaceName[i]=thetimesnewroman[i];
	if (inFont.mName==LFont_VERDANA)
		for (i=0; i<LF_FACESIZE; i++)
			thelogfont.lfFaceName[i]=theverdana[i];

	thelogfont.lfHeight=(LONG) inFont.mSize;
	thelogfont.lfWidth=0;
	thelogfont.lfEscapement=0;
	thelogfont.lfOrientation=0;

	if (inFont.mBold)
		thelogfont.lfWeight=FW_BOLD;
	else
		thelogfont.lfWeight=FW_NORMAL;

	if (inFont.mItalic)
		thelogfont.lfItalic=1;
	else
		thelogfont.lfItalic=0;

	if (inFont.mUnderlined)
		thelogfont.lfUnderline=1;
	else
		thelogfont.lfUnderline=0;

	thelogfont.lfStrikeOut=0;

	thelogfont.lfCharSet=ANSI_CHARSET;
	thelogfont.lfOutPrecision=OUT_DEFAULT_PRECIS;
	thelogfont.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	thelogfont.lfQuality=DEFAULT_QUALITY;
	
	thelogfont.lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;

	/*calls the relevant api*/
	thefont=CreateFontIndirect (&thelogfont);
	SelectObject (inDraw->mMemHdc, thefont);
	if (inDraw->mFont!=NULL)
		DeleteObject (inDraw->mFont);
	inDraw->mFont=thefont;
}

/*--------------------------------
// GetParentWindow ()
//-------------------------------*/
LWindow* LDraw_GetParentWindow (LDraw* inDraw)
{
    return inDraw->mParentWindow;
}

/*--------------------------------------
// SetCursorShape ()
//--------------------------------------*/
void LDraw_SetCursorShape (LDraw* inDraw, LCursor_TShape inCursorShape)
{
	inDraw->mCursorShape=inCursorShape;
}

/*---------------------------------------
// GetCursorShape ()
//--------------------------------------*/
LCursor_TShape LDraw_GetCursorShape (LDraw* inDraw)
{
	return inDraw->mCursorShape;
}

#endif

/* Copyright (C) 2002 Andrea Ribichini

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

