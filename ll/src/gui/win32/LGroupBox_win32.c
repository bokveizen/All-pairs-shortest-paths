/* ==============================================================================
 *  LGroupBox_win32.c
 * ==============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jun 6, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LGroupBox.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

struct tagLGroupBox {
    HWND mHwnd;
    void (*mHandler) ();
    ui2 mID;
    LWindow* mParentWindow;
};

/*----------------------------
// Create ()
//--------------------------*/
LGroupBox* LGroupBox_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1* inText)
{
    LGroupBox thegroup;
    LGroupBox* thegroupptr;

    thegroup.mParentWindow=inWindow;

    thegroup.mHwnd=CreateWindow ("button", inText, WS_CHILD|WS_VISIBLE|BS_GROUPBOX, inX, inY, inDX, inDY,
        inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    
	/* check for call failure */
	if (thegroup.mHwnd==NULL) Throw(LGroupBox_CANT_CREATE);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;

    thegroup.mID=LGroupBox_ID;

    thegroupptr=LMemory_NewObject (LGroupBox, thegroup);
    SetWindowLong (thegroup.mHwnd, GWL_USERDATA, (LONG) thegroupptr); /*unnecessary for the time being...*/
    return thegroupptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LGroupBox_Destroy (LGroupBox* inGroup)
{
    DestroyWindow (inGroup->mHwnd);
    LMemory_DeleteObject (&inGroup);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LGroupBox_Move (LGroupBox* inGroup, i4 inX, i4 inY)
{
    SetWindowPos (inGroup->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LGroupBox_Resize (LGroupBox* inGroup, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inGroup->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LGroupBox_GetParentWindow (LGroupBox* inGroup)
{
    return inGroup->mParentWindow;
}

/*------------------------------------
// SetFont ()
//-----------------------------------*/
void LGroupBox_SetFont (LGroupBox* inGroup, LGroupBox_TFont inFont)
{
    LOGFONT thelogfont;
    HFONT thehfont;
    int theauxfont;

    switch (inFont)
    {
    case LGroupBox_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LGroupBox_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LGroupBox_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LGroupBox_SYSTEMVAR:
        theauxfont=SYSTEM_FONT;
        break;
    }

    GetObject (GetStockObject (theauxfont), sizeof (LOGFONT), (PTSTR) &thelogfont);
    thehfont=CreateFontIndirect (&thelogfont);
    SendMessage (inGroup->mHwnd, WM_SETFONT, (WPARAM) thehfont, (LPARAM) TRUE);
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

