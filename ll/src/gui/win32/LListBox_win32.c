/* =============================================================================
 *  LListBox_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jul 24, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 19:34:59 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.2 $
*/

#include "LListBox.h"
#include "LListBox_win32.h"
#include "LWindow_win32.h"
#include "LString.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*----------------------------
// Create ()
//--------------------------*/
LListBox* LListBox_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, LListBox_THandler inDBLCLKHandler)
{
    LListBox thelbox;
    LListBox* thelboxptr;

    thelbox.mParentWindow=inWindow;

    thelbox.mHwnd=CreateWindowEx (WS_EX_CLIENTEDGE, "listbox", NULL, WS_CHILD|WS_VISIBLE|LBS_STANDARD|LBS_DISABLENOSCROLL, inX, inY, inDX, inDY,
        inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    
	/* check for call failure */
	if (thelbox.mHwnd==NULL) Throw(LListBox_CANT_CREATE);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;

    thelbox.mHandler=inDBLCLKHandler;
    thelbox.mID=LListBox_ID;

    thelboxptr=LMemory_NewObject (LListBox, thelbox);
    SetWindowLong (thelbox.mHwnd, GWL_USERDATA, (LONG) thelboxptr);
    return thelboxptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LListBox_Destroy (LListBox* inLBox)
{
    LWindow* thelw;

    thelw=LListBox_GetParentWindow (inLBox);

    if (inLBox==thelw->mWidgetPointer) /*if default focus*/
        thelw->mWidgetPointer=NULL; /*remove it...*/

    DestroyWindow (inLBox->mHwnd);
    LMemory_DeleteObject (&inLBox);
}

void LListBox_SetFont (LListBox* inLBox, LListBox_TFont inFont)
{
    LOGFONT thelogfont;
    HFONT thehfont;
    int theauxfont;

    switch (inFont)
    {
    case LListBox_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LListBox_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LListBox_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LListBox_SYSTEMVAR:
        theauxfont=SYSTEM_FONT;
        break;
    }

    GetObject (GetStockObject (theauxfont), sizeof (LOGFONT), (PTSTR) &thelogfont);
    thehfont=CreateFontIndirect (&thelogfont);
    SendMessage (inLBox->mHwnd, WM_SETFONT, (WPARAM) thehfont, (LPARAM) TRUE);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LListBox_Move (LListBox* inLBox, i4 inX, i4 inY)
{
    SetWindowPos (inLBox->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*--------------------------------------
// Resize ()
//-------------------------------------*/
void LListBox_Resize (LListBox* inLBox, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inLBox->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*-------------------------------------
// SetFocus ()
//------------------------------------*/
void LListBox_SetFocus (LListBox* inLBox)
{
    LWindow* thewaux;

    thewaux=LListBox_GetParentWindow (inLBox);
    thewaux->mWidgetPointer=(void *) inLBox;
    thewaux->mWidgetType=LListBox_ID;

    SetFocus (inLBox->mHwnd);
}

/*----------------------------
// GetParentWindow ()
//---------------------------*/
LWindow* LListBox_GetParentWindow (LListBox* inLBox)
{
    return inLBox->mParentWindow;
}

/*------------------------------
// AddItem ()
//-----------------------------*/
void LListBox_AddItem (LListBox* inLBox, i1* inString)
{
    SendMessage (inLBox->mHwnd, LB_ADDSTRING, 0, (LPARAM) inString);
}

/*------------------------------
// RemoveItem ()
//-----------------------------*/
void LListBox_RemoveItem (LListBox* inLBox, i1* inString)
{
    ui2 theNumItems;
    ui2 theIndex;
    i1 theBuffer[LListBox_MAX_TEXT];

/*gets number of items*/
    theNumItems=(ui2) SendMessage (inLBox->mHwnd, LB_GETCOUNT, 0, 0);

/*searches through the listbox for matching item*/
    for (theIndex=0; theIndex<=theNumItems-1; theIndex++)
    {
        SendMessage (inLBox->mHwnd, LB_GETTEXT, (WPARAM) theIndex, (LPARAM) theBuffer);
        if (!LString_Compare (inString, theBuffer))
        {
            SendMessage (inLBox->mHwnd, LB_DELETESTRING, (WPARAM) theIndex, 0);
            return; /*very bad programming practice...*/
        }
    }
}

/*----------------------------------------
// GetCurrentSelection ()
//---------------------------------------*/
Bool LListBox_GetCurrentSelection (LListBox* inLBox, i1* outString)
{
    LRESULT theaux;
    ui2 theIndex;

    theaux=SendMessage (inLBox->mHwnd, LB_GETCURSEL, 0, 0);
    
    /*no current selection...*/
    if (theaux==LB_ERR)
        return FALSE;

    theIndex=(ui2) theaux; /*keep an eye on this...*/
    SendMessage (inLBox->mHwnd, LB_GETTEXT, (WPARAM) theIndex, (LPARAM) outString);
    return TRUE;
}

/*----------------------------------------------
// SetCurrentSelection ()
//---------------------------------------------*/
void LListBox_SetCurrentSelection (LListBox* inLBox, i1* inString)
{
    ui2 theNumItems;
    ui2 theIndex;
    i1 theBuffer[LListBox_MAX_TEXT];

    /*reset selection...*/
    if (inString==NULL)
    {
        SendMessage (inLBox->mHwnd, LB_SETCURSEL, (WPARAM) -1, 0);
        return;
    }

/*gets number of items*/
    theNumItems=(ui2) SendMessage (inLBox->mHwnd, LB_GETCOUNT, 0, 0);

/*searches through the listbox for matching item*/
    for (theIndex=0; theIndex<=theNumItems-1; theIndex++)
    {
        SendMessage (inLBox->mHwnd, LB_GETTEXT, (WPARAM) theIndex, (LPARAM) theBuffer);
        if (!LString_Compare (inString, theBuffer))
        {
            SendMessage (inLBox->mHwnd, LB_SETCURSEL, (WPARAM) theIndex, 0);
            return; /*very bad programming practice...*/
        }
    }
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

