/* =============================================================================
 *  LPushButton_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        May 19, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LPushButton.h"

#include "LPushButton_win32.h"
#include "LWindow_win32.h"
#include "LBitmap_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*----------------------------
// Create ()
//--------------------------*/
LPushButton* LPushButton_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1* inText, LBitmap* inBitmap, LPushButton_THandler inHandler)
{
    LPushButton thebutton;
    LPushButton* thebuttonptr;

    thebutton.mParentWindow=inWindow;

    if (inBitmap==NULL)
        thebutton.mHwnd=CreateWindow ("button", inText, WS_CHILD|WS_VISIBLE, inX, inY, inDX, inDY,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    else
    {
        thebutton.mHwnd=CreateWindow ("button", inText, WS_CHILD|WS_VISIBLE|BS_BITMAP, inX, inY, inDX, inDY,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
        /*applies the bitmap to the button*/
        SendMessage (thebutton.mHwnd, BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) inBitmap->mHbitmap);

    }

	/* check for call failure */
	if (thebutton.mHwnd==NULL) Throw(LPushButton_CANT_CREATE);
    
    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
    thebutton.mHandler=inHandler;
    thebutton.mID=LPushButton_ID;

    thebuttonptr=LMemory_NewObject (LPushButton, thebutton);
    SetWindowLong (thebutton.mHwnd, GWL_USERDATA, (LONG) thebuttonptr);
    return thebuttonptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LPushButton_Destroy (LPushButton* inButton)
{
    LWindow* thelw;

    thelw=LPushButton_GetParentWindow (inButton);

    if (inButton==thelw->mWidgetPointer) /*if default focus*/
        thelw->mWidgetPointer=NULL; /*remove it...*/

    DestroyWindow (inButton->mHwnd);
    LMemory_DeleteObject (&inButton);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LPushButton_Move (LPushButton* inButton, i4 inX, i4 inY)
{
    SetWindowPos (inButton->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LPushButton_Resize (LPushButton* inButton, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inButton->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*-------------------------------------
// SetFocus ()
//------------------------------------*/
void LPushButton_SetFocus (LPushButton* inButton)
{
    LWindow* thewaux;

    thewaux=LPushButton_GetParentWindow (inButton);
    thewaux->mWidgetPointer=(void *) inButton;
    thewaux->mWidgetType=LPushButton_ID;

    SetFocus (inButton->mHwnd);
}

/*------------------------------------
// GetParentWindow ()
//-----------------------------------*/
LWindow* LPushButton_GetParentWindow (LPushButton* inButton)
{
    return inButton->mParentWindow;
}

/*-----------------------------------
// SetFont ()
//----------------------------------*/
void LPushButton_SetFont (LPushButton* inButton, LPushButton_TFont inFont)
{
LOGFONT thelogfont;
    HFONT thehfont;
    int theauxfont;

    switch (inFont)
    {
    case LPushButton_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LPushButton_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LPushButton_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LPushButton_SYSTEMVAR:
        theauxfont=SYSTEM_FONT;
        break;
    }

    GetObject (GetStockObject (theauxfont), sizeof (LOGFONT), (PTSTR) &thelogfont);
    thehfont=CreateFontIndirect (&thelogfont);
    SendMessage (inButton->mHwnd, WM_SETFONT, (WPARAM) thehfont, (LPARAM) TRUE);
}

/*-----------------------------------
// Enable ()
//----------------------------------*/
void LPushButton_Enable (LPushButton* inButton, Bool inEnabled)
{
    if (inEnabled==TRUE)
        EnableWindow (inButton->mHwnd, TRUE);
    else
        EnableWindow (inButton->mHwnd, FALSE);
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

