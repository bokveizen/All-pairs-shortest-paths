/* =============================================================================
 *  LRadioButton_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jun 5, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LRadioButton.h"
#include "LRadioButton_win32.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*----------------------------
// Create ()
//--------------------------*/
LRadioButton* LRadioButton_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, i1* inText, LRadioButton_THandler inHandler)
{
    LRadioButton theradio;
    LRadioButton* theradioptr;

    theradio.mParentWindow=inWindow;

    theradio.mHwnd=CreateWindow ("button", inText, WS_CHILD|WS_VISIBLE|BS_RADIOBUTTON, inX, inY, inDX, inDY,
        inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    
	/* check for call failure */
	if (theradio.mHwnd==NULL) Throw(LRadioButton_CANT_CREATE);	

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
    theradio.mHandler=inHandler;
    theradio.mID=LRadioButton_ID;

    theradioptr=LMemory_NewObject (LRadioButton, theradio);
    SetWindowLong (theradio.mHwnd, GWL_USERDATA, (LONG) theradioptr);
    return theradioptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LRadioButton_Destroy (LRadioButton* inRadio)
{
    LWindow* thelw;

    thelw=LRadioButton_GetParentWindow (inRadio);

    if (inRadio==thelw->mWidgetPointer) /*if default focus*/
        thelw->mWidgetPointer=NULL; /*remove it...*/

    DestroyWindow (inRadio->mHwnd);
    LMemory_DeleteObject (&inRadio);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LRadioButton_Move (LRadioButton* inRadio, i4 inX, i4 inY)
{
    SetWindowPos (inRadio->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LRadioButton_Resize (LRadioButton* inRadio, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inRadio->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*-------------------------------------
// SetFocus ()
//------------------------------------*/
void LRadioButton_SetFocus (LRadioButton* inRadio)
{
    LWindow* thewaux;

    thewaux=LRadioButton_GetParentWindow (inRadio);
    thewaux->mWidgetPointer=(void *) inRadio;
    thewaux->mWidgetType=LRadioButton_ID;

    SetFocus (inRadio->mHwnd);
}

/*-------------------------------------
// SetState ()
//------------------------------------*/
void LRadioButton_SetState (LRadioButton* inRadio, ui2 inCheckedUnchecked)
{
    if (inCheckedUnchecked==LRadioButton_CHECKED)
        SendMessage (inRadio->mHwnd, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
    else
        SendMessage (inRadio->mHwnd, BM_SETCHECK, (WPARAM) BST_UNCHECKED, 0);
}

/*-------------------------------------
// QueryState ()
//------------------------------------*/
ui2 LRadioButton_QueryState (LRadioButton* inRadio)
{
    int i;

    i=(int) SendMessage (inRadio->mHwnd, BM_GETCHECK, 0, 0);
    if (i==BST_CHECKED)
        return LRadioButton_CHECKED;
    else
        return LRadioButton_UNCHECKED;
}

/*-----------------------------------
// GetParentWindow ()
//---------------------------------*/
LWindow* LRadioButton_GetParentWindow (LRadioButton* inButton)
{
    return inButton->mParentWindow;
}

/*---------------------------------
// SetFont ()
//--------------------------------*/
void LRadioButton_SetFont (LRadioButton* inButton, LRadioButton_TFont inFont)
{
LOGFONT thelogfont;
    HFONT thehfont;
    int theauxfont;

    switch (inFont)
    {
    case LRadioButton_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LRadioButton_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LRadioButton_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LRadioButton_SYSTEMVAR:
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
void LRadioButton_Enable (LRadioButton* inButton, Bool inEnabled)
{
    if (inEnabled)
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

