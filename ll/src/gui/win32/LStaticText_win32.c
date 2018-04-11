/* =============================================================================
 *  LStaticText_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jul 21, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LStaticText.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

struct tagLStaticText {
    HWND mHwnd;
    void (*mHandler) ();
    ui2 mID;
    LWindow* mParentWindow;
};


/*----------------------------
// Create ()
//--------------------------*/
LStaticText* LStaticText_Create (LWindow* inWindow, i1* inString, LStaticText_TAlign inMode, ui4 inX, ui4 inY, ui4 inDX, ui4 inDY)
{
    LStaticText thetext;
    LStaticText* thetextptr;

    thetext.mParentWindow=inWindow;

    if (inMode==LStaticText_LEFT)
        thetext.mHwnd=CreateWindow ("STATIC", inString, WS_CHILD|WS_VISIBLE|SS_LEFT, inX, inY, inDX, inDY,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    if (inMode==LStaticText_CENTER)
        thetext.mHwnd=CreateWindow ("STATIC", inString, WS_CHILD|WS_VISIBLE|SS_CENTER, inX, inY, inDX, inDY,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    if (inMode==LStaticText_RIGHT)
        thetext.mHwnd=CreateWindow ("STATIC", inString, WS_CHILD|WS_VISIBLE|SS_RIGHT, inX, inY, inDX, inDY,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
	
	/* check for call failure */
	if (thetext.mHwnd==NULL) Throw(LStaticText_CANT_CREATE);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;

    thetext.mID=LStaticText_ID;

    thetextptr=LMemory_NewObject (LStaticText, thetext);
    SetWindowLong (thetext.mHwnd, GWL_USERDATA, (LONG) thetextptr);
    return thetextptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LStaticText_Destroy (LStaticText* inText)
{
    DestroyWindow (inText->mHwnd);
    LMemory_DeleteObject (&inText);
}

void LStaticText_SetFont (LStaticText* inText, LStaticText_TFont inFont)
{
    LOGFONT thelogfont;
    HFONT thehfont;
    int theauxfont;

    switch (inFont)
    {
    case LStaticText_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LStaticText_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LStaticText_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LStaticText_SYSTEMVAR:
        theauxfont=SYSTEM_FONT;
        break;
    }

    GetObject (GetStockObject (theauxfont), sizeof (LOGFONT), (PTSTR) &thelogfont);
    thehfont=CreateFontIndirect (&thelogfont);
    SendMessage (inText->mHwnd, WM_SETFONT, (WPARAM) thehfont, (LPARAM) TRUE);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LStaticText_Move (LStaticText* inText, i4 inX, i4 inY)
{
    SetWindowPos (inText->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LStaticText_Resize (LStaticText* inText, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inText->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*----------------------------
// GetParentWindow ()
//---------------------------*/
LWindow* LStaticText_GetParentWindow (LStaticText* inText)
{
    return inText->mParentWindow;
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

