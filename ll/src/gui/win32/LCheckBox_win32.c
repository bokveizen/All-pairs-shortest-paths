/* =============================================================================
 *  LCheckBox_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        May 28, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LCheckBox.h"
#include "LCheckBox_win32.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*----------------------------
// Create ()
//--------------------------*/
LCheckBox* LCheckBox_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1* inText, LCheckBox_THandler inHandler)
{
    LCheckBox thebox;
    LCheckBox* theboxptr;

    thebox.mParentWindow=inWindow;

    thebox.mHwnd=CreateWindow ("button", inText, WS_CHILD|WS_VISIBLE|/*BS_CHECKBOX*/BS_3STATE, inX, inY, inDX, inDY,
        inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
  
	/* check for call failure */
	if (thebox.mHwnd==NULL) Throw(LCheckBox_CANT_CREATE);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
    thebox.mHandler=inHandler;
    thebox.mID=LCheckBox_ID;

    theboxptr=LMemory_NewObject (LCheckBox, thebox);
    SetWindowLong (thebox.mHwnd, GWL_USERDATA, (LONG) theboxptr);
    return theboxptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LCheckBox_Destroy (LCheckBox* inBox)
{
    LWindow* thelw;

    thelw=LCheckBox_GetParentWindow (inBox);

    if (inBox==thelw->mWidgetPointer) /*if default focus*/
        thelw->mWidgetPointer=NULL; /*remove it...*/

    DestroyWindow (inBox->mHwnd);
    LMemory_DeleteObject (&inBox);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LCheckBox_Move (LCheckBox* inBox, i4 inX, i4 inY)
{
    SetWindowPos (inBox->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LCheckBox_Resize (LCheckBox* inBox, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inBox->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*-------------------------------------
// SetFocus ()
//------------------------------------*/
void LCheckBox_SetFocus (LCheckBox* inBox)
{
    LWindow* thewaux;

    thewaux=LCheckBox_GetParentWindow (inBox);
    thewaux->mWidgetPointer=(void *) inBox;
    thewaux->mWidgetType=LCheckBox_ID;
    SetFocus (inBox->mHwnd);
}

/*-------------------------------------
// SetState ()
//------------------------------------*/
void LCheckBox_SetState (LCheckBox* inBox, ui2 inState)
{
    if (inState==LCheckBox_CHECKED)
    {
        SendMessage (inBox->mHwnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
        return;
    }
    if (inState==LCheckBox_UNCHECKED)
    {
        SendMessage (inBox->mHwnd, BM_SETCHECK, (WPARAM) BST_UNCHECKED, 0);
        return;
    }
    if (inState==LCheckBox_INDETERMINATE)
    {
        SendMessage (inBox->mHwnd, BM_SETCHECK, (WPARAM) BST_INDETERMINATE, 0);
        return;
    }
}

/*-------------------------------------
// QueryState ()
//------------------------------------*/
ui2 LCheckBox_QueryState (LCheckBox* inBox)
{
    int i;

    i=(int) SendMessage (inBox->mHwnd, BM_GETCHECK, 0, 0);
    if (i==BST_CHECKED)
        return LCheckBox_CHECKED;
    if (i==BST_UNCHECKED)
        return LCheckBox_UNCHECKED;
    if (i==BST_INDETERMINATE)
        return LCheckBox_INDETERMINATE;
}

/*------------------------------------
// GetParentWindow ()
//-----------------------------------*/
LWindow* LCheckBox_GetParentWindow (LCheckBox* inBox)
{
    return inBox->mParentWindow;
}

/*------------------------------------
// SetFont ()
//-----------------------------------*/
void LCheckBox_SetFont (LCheckBox* inBox, LCheckBox_TFont inFont)
{
    LOGFONT thelogfont;
    HFONT thehfont;
    int theauxfont;

    switch (inFont)
    {
    case LCheckBox_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LCheckBox_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LCheckBox_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LCheckBox_SYSTEMVAR:
        theauxfont=SYSTEM_FONT;
        break;
    }

    GetObject (GetStockObject (theauxfont), sizeof (LOGFONT), (PTSTR) &thelogfont);
    thehfont=CreateFontIndirect (&thelogfont);
    SendMessage (inBox->mHwnd, WM_SETFONT, (WPARAM) thehfont, (LPARAM) TRUE);
}

/*-----------------------------------
// Enable ()
//----------------------------------*/
void LCheckBox_Enable (LCheckBox* inBox, Bool inEnabled)
{
    if (inEnabled)
        EnableWindow (inBox->mHwnd, TRUE);
    else
        EnableWindow (inBox->mHwnd, FALSE);
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

