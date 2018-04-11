/* ============================================================================
 *  LTextField_win32.c
 * ============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jul 29, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LTextField.h"
#include "LWindow_win32.h"
#include "LString.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

struct tagLTextField {
    HWND mHwndEdit;
    void (*mHandler) (); /*to fix a bug...*/
    ui2 mID;
    LWindow* mParentWindow;

    WNDPROC mDefaultProc;
    Bool (*mReturnHandler) ();
    ui4 mHCaret;
};

/* GLOBAL VARIABLES & FUNCTION PROTOTYPES*/
LRESULT CALLBACK _ltfpatchproc (HWND, UINT, WPARAM, LPARAM);

/*----------------------------------------------
// Destroy()
//----------------------------------------------*/
void LTextField_Destroy (LTextField* inEditor)
{
    LWindow* thelw;

    thelw=LTextField_GetParentWindow (inEditor);

    if (inEditor==thelw->mWidgetPointer) /*if default focus*/
        thelw->mWidgetPointer=NULL; /*remove it...*/

    DestroyWindow (inEditor->mHwndEdit);
    LMemory_DeleteObject (&inEditor);
}

/*----------------------------------------------
// Create()
//----------------------------------------------*/
LTextField* LTextField_Create (LWindow* inWindow, ui4 inX, ui4 inY, ui4 inWidth, ui4 inHeight, Bool inFrameOn)
{
    LTextField theltex;
    LTextField *theltexptr;

        if (inFrameOn)
            theltex.mHwndEdit=CreateWindowEx (WS_EX_CLIENTEDGE, "edit", NULL, 
                    WS_CHILD|WS_VISIBLE|
                    ES_LEFT|
                    ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_NOHIDESEL,
                    inX, inY, inWidth, inHeight, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
                    _LApplication_gGlobal.inInstance, NULL);    
        else
            theltex.mHwndEdit=CreateWindow ("edit", NULL, 
                    WS_CHILD|WS_VISIBLE|
                    ES_LEFT|
                    ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_NOHIDESEL,
                    inX, inY, inWidth, inHeight, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
                    _LApplication_gGlobal.inInstance, NULL);

			/* check for call failure */
	if (theltex.mHwndEdit==NULL) Throw(LTextField_CANT_CREATE);

        theltex.mDefaultProc=(WNDPROC) SetWindowLong (theltex.mHwndEdit, 
                                                GWL_WNDPROC, (LONG) _ltfpatchproc);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
    theltex.mReturnHandler=NULL;
    theltex.mHCaret=0;
    theltex.mHandler=NULL; /*to fix a bug...*/
    theltex.mID=LTextField_ID;
    
    theltex.mParentWindow=inWindow; /* [CD020718] */

    theltexptr=LMemory_NewObject (LTextField, theltex);

/*mutual reference pointer...hopefully*/
    SetWindowLong (theltex.mHwndEdit, GWL_USERDATA, (LONG) theltexptr);

    return theltexptr;
}

/*------------------------------------------------
// Resize ()
//------------------------------------------------*/
void LTextField_Resize (LTextField* inEditor, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inEditor->mHwndEdit, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE); 
}

/*------------------------------------------------
// Move ()
//------------------------------------------------*/
void LTextField_Move(LTextField* inEditor, ui4 inX, ui4 inY)
{
    SetWindowPos (inEditor->mHwndEdit, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE); 
}

/*--------------------------------------------------
// SetTextBuffer ()
//--------------------------------------------------*/
void LTextField_SetTextBuffer (LTextField* inEditor, i1* inBuffer)
{
    SetWindowText (inEditor->mHwndEdit, (LPCTSTR) inBuffer);
}

/*--------------------------------------------------
// GetTextBufferSize ()
//-------------------------------------------------*/
i4 LTextField_GetTextBufferSize (LTextField* inEditor)
{
    return (i4) (1+GetWindowTextLength (inEditor->mHwndEdit));
}

/*--------------------------------------------------
// GetTextBuffer ()
//-------------------------------------------------*/
void LTextField_GetTextBuffer (LTextField* inEditor, i1* inBuffer, i4 inLength)
{
    GetWindowText (inEditor->mHwndEdit, (LPTSTR) inBuffer, (int) inLength);
}

/*-------------------------------------------------
// SetFont ()
//------------------------------------------------*/
void LTextField_SetFont (LTextField* inEditor, LTextField_TFont inFont)
{
    LOGFONT thelogfont;
    HFONT thehfont;
    RECT therect;
    int theauxfont;

    switch (inFont)
    {
    case LTextField_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LTextField_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LTextField_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LTextField_SYSTEMVAR:
        theauxfont=SYSTEM_FONT;
        break;
    }

    GetObject (GetStockObject (theauxfont), sizeof (LOGFONT), (PTSTR) &thelogfont);
    thehfont=CreateFontIndirect (&thelogfont);
    SendMessage (inEditor->mHwndEdit, WM_SETFONT, (WPARAM) thehfont, 0);
    GetClientRect (inEditor->mHwndEdit, &therect);
    InvalidateRect (inEditor->mHwndEdit, &therect, TRUE);
}

/*----------------------------------------
// SetFocus ()
//---------------------------------------*/
void LTextField_SetFocus (LTextField *inEditor)
{
    LWindow* thewaux;

    thewaux=LTextField_GetParentWindow (inEditor);
    thewaux->mWidgetPointer=(void *) inEditor;
    thewaux->mWidgetType=LTextField_ID;

    SetFocus (inEditor->mHwndEdit);
}

/*----------------------------------------
// GetFocus ()
//---------------------------------------*/
LTextField* LTextField_GetFocus ()
{
    LTextField* theltf;
    LWindow* theaux;
    HWND thehwnd;
    i1 thebuffer[2048];

    theaux=LWindow_GetFrontWindow ();
    if (theaux==NULL)
        return NULL;
    thehwnd=GetFocus ();
    if (theaux->mHwnd==thehwnd)
        return NULL;
    else
    {
        GetClassName (thehwnd, thebuffer, 2048);
        if (!LString_Compare (thebuffer, "Edit")) /*at this point it's either ltexteditor or ltextfield*/
        { 
            theltf=(LTextField*) GetWindowLong (thehwnd, GWL_USERDATA);
            if (theltf->mID==LTextField_ID) /*mID has to be at same offset for ltexteditor and ltextfield for this to work...*/
                return theltf;
            else 
                return NULL;
        }
        else
            return NULL;
    }
}

/*----------------------------------------
// Undo ()
//--------------------------------------*/
void LTextField_Undo (LTextField* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_UNDO, 0, 0);
}

/*----------------------------------------
// Cut ()
//--------------------------------------*/
void LTextField_Cut (LTextField* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_CUT, 0, 0);
}

/*----------------------------------------
// Copy ()
//--------------------------------------*/
void LTextField_Copy (LTextField* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_COPY, 0, 0);
}

/*----------------------------------------
// Paste ()
//--------------------------------------*/
void LTextField_Paste (LTextField* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_PASTE, 0, 0);
}

/*----------------------------------------
// SelectAll ()
//--------------------------------------*/
void LTextField_SelectAll (LTextField* inEditor)
{
    SendMessage (inEditor->mHwndEdit, EM_SETSEL, 0, -1);
}

/*----------------------------------------
// CanUndo ()
//---------------------------------------*/
Bool LTextField_CanUndo (LTextField* inEditor)
{
    return (Bool) SendMessage (inEditor->mHwndEdit, EM_CANUNDO, 0, 0);
}

/*---------------------------------------
// IsTextAvailable ()
//--------------------------------------*/
Bool LTextField_IsTextAvailable ()
{
    return (Bool) IsClipboardFormatAvailable (CF_TEXT);
}

/*----------------------------------------
// IsSelectionAvailable ()
//---------------------------------------*/
Bool LTextField_IsSelectionAvailable (LTextField* inEditor)
{
    int theSelect;

    theSelect=SendMessage (inEditor->mHwndEdit, EM_GETSEL, 0, 0);
    if (HIWORD (theSelect)==LOWORD (theSelect))
        return FALSE;
    else
        return TRUE;
}

/*NEW FUNCTIONS*/

/*----------------------------------
// GetSelection ()
//---------------------------------*/
/*void LTextEditor_GetSelection (LTextEditor* inEditor, i4* outStart, i4* outEnd)
{
    SendMessage (inEditor->mHwndEdit, EM_GETSEL, (WPARAM) outStart, (LPARAM) outEnd);
}*/

/*----------------------------------
// SetSelection ()
//---------------------------------*/
/*void LTextEditor_SetSelection (LTextEditor* inEditor, i4 inStart, i4 inEnd)
{
    SendMessage (inEditor->mHwndEdit, EM_SETSEL, (WPARAM) (int) inStart, (LPARAM) (int) inEnd);
}*/

/*----------------------------------
// ReplaceSelection ()
//---------------------------------*/
/*void LTextEditor_ReplaceSelection (LTextEditor* inEditor, i1* inString)
{
    SendMessage (inEditor->mHwndEdit, EM_REPLACESEL, 0, (LPARAM) inString);
}*/

/*----------------------------------
// ScrollToCursor ()
//---------------------------------*/
/*void LTextEditor_ScrollToCursor (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, EM_SCROLLCARET, 0, 0);
}*/

/*----------------------------------
// Scroll ()
//---------------------------------*/
/*void LTextEditor_Scroll (LTextEditor* inEditor, i4 inCharHor, i4 inLinesVer)
{
    SendMessage (inEditor->mHwndEdit, EM_LINESCROLL, (WPARAM) inCharHor, (LPARAM) inLinesVer);
}*/

/*----------------------------------
// DeleteSelection ()
//---------------------------------*/
/*void LTextEditor_DeleteSelection (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_CLEAR, 0, 0);
}*/

/*----------------------------------
// GetLineCount ()
//---------------------------------*/
/*i4 LTextEditor_GetLineCount (LTextEditor* inEditor)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_GETLINECOUNT, 0, 0);
}*/

/*----------------------------------
// GetLineIndex ()
//---------------------------------*/
/*i4 LTextEditor_GetLineIndex (LTextEditor* inEditor, i4 inLine)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_LINEINDEX, (WPARAM) inLine, 0);
}*/

/*----------------------------------
// GetLineLength ()
//---------------------------------*/
/*i4 LTextEditor_GetLineLength (LTextEditor* inEditor, i4 inIndex)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_LINELENGTH, (WPARAM) inIndex, 0);
}*/

/*---------------------------------
// GetLine ()
//--------------------------------*/
/*i4 LTextEditor_GetLine (LTextEditor* inEditor, i4 inLine, i1* inBuffer)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_GETLINE, (WPARAM) inLine, (LPARAM) inBuffer);
}*/

/*---------------------------------
// GetModify ()
//--------------------------------*/
/*Bool LTextEditor_GetModify (LTextEditor* inEditor)
{
    return (Bool) SendMessage (inEditor->mHwndEdit, EM_GETMODIFY, 0, 0);
}*/

/*---------------------------------
// SetModify ()
//--------------------------------*/
/*void LTextEditor_SetModify (LTextEditor* inEditor, Bool inFlag)
{
    SendMessage (inEditor->mHwndEdit, EM_SETMODIFY, (WPARAM) (UINT) inFlag, 0);
}*/

/*---------------------------------
// GetFirstVisibleLine ()
//--------------------------------*/
/*i4 LTextEditor_GetFirstVisibleLine (LTextEditor* inEditor)
{
    //line number or line index???? It should be tested...
    return SendMessage (inEditor->mHwndEdit, EM_GETFIRSTVISIBLELINE, 0, 0);
}*/

/*---------------------------------
// InstallReturnHandler ()
//--------------------------------*/
void LTextField_InstallReturnHandler (LTextField* inEditor, LTextField_THandler inHandler)
{
    inEditor->mReturnHandler=inHandler;
}

/*---------------------------------
// GetCursorLine ()
//--------------------------------*/
/*i4 LTextEditor_GetCursorLine (LTextEditor* inEditor)
{
    return (i4) SendMessage (inEditor->mHwndEdit, EM_LINEFROMCHAR, (WPARAM) -1, 0);
}*/

/*--------------------------------
 * GetParentWindow() [CD020718]
 *--------------------------------
*/
LWindow* LTextField_GetParentWindow (LTextField* inEditor){
    return inEditor->mParentWindow;
}

/*-----------------------------------
// Enable ()
//----------------------------------*/
void LTextField_Enable (LTextField* inEditor, Bool inEnabled)
{
    if (inEnabled==TRUE)
        EnableWindow (inEditor->mHwndEdit, TRUE);
    else
        EnableWindow (inEditor->mHwndEdit, FALSE);
}

/*--------------------------------
// _ltfpatchproc ()
//-------------------------------*/
LRESULT CALLBACK _ltfpatchproc (HWND inHwnd, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    LTextField* theaux;

    theaux=(LTextField*) GetWindowLong (inHwnd, GWL_USERDATA); 

    switch (inMessage)
    {
        case WM_CHAR:
            if (inWParam==VK_RETURN)
            {
                if (theaux->mReturnHandler==NULL)
                    break;
                else
                    if (theaux->mReturnHandler (theaux))
                        return 0;
                    else
                        break;
            }
        break;
    }

    return CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);
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

