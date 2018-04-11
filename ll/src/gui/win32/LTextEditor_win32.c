/* ============================================================================
 *  LTextEditor_win32.c
 * ============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Mar 10, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LTextEditor.h"
#include "LTextEditor_win32.h"
#include "LWindow_win32.h"
#include "LString.h"
#include <richedit.h>

#if __LSL_OS_GUI__ == __LSL_Win32__

struct tagLTextEditor {
    HWND mHwndEdit;
    void (*mHandler) (); /*to fix a bug...*/
    ui2 mID;
    LWindow* mParentWindow;

	HINSTANCE mRichEditDLL;
    WNDPROC mDefaultProc;

	Bool (*mLCLKHandler) ();
	Bool (*mLDBLCLKHandler) ();
	Bool (*mRCLKHandler) ();
	Bool (*mKEYDOWNHandler) ();

	Bool mLCLKLater;
	Bool mLDBLCLKLater;
	Bool mRCLKLater;
	Bool mKEYDOWNLater;
};

/* GLOBAL VARIABLES & FUNCTION PROTOTYPES*/
LRESULT CALLBACK _patchproc (HWND, UINT, WPARAM, LPARAM);

/*----------------------------------------------
// Destroy()
//----------------------------------------------*/
void LTextEditor_Destroy (LTextEditor* inEditor)
{
    LWindow* thelw;

    thelw=LTextEditor_GetParentWindow (inEditor);

    if (inEditor==thelw->mWidgetPointer) /*if default focus*/
        thelw->mWidgetPointer=NULL; /*remove it...*/

    DestroyWindow (inEditor->mHwndEdit);
	FreeLibrary ((HMODULE) (inEditor->mRichEditDLL));
    LMemory_DeleteObject (&inEditor);
}

/*----------------------------------------------
// Create()
//----------------------------------------------*/
LTextEditor* LTextEditor_Create (ui4 inStyle, LWindow* inWindow,
                        ui4 inX, ui4 inY, ui4 inWidth, ui4 inHeight, Bool inFrameOn)
{
    LTextEditor theltex;
    LTextEditor *theltexptr;
    ui4 thestyle=0;

	/*required! - enables rich edit control v2.0!!!*/
	/*theltex.mRichEditDLL=LoadLibrary("RichEd32.Dll"); this enabled buggy richedit v1.0*/
	theltex.mRichEditDLL=LoadLibrary("RichEd20.Dll"); 

    thestyle=WS_CHILD|WS_VISIBLE|ES_LEFT|ES_MULTILINE|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_NOHIDESEL;
    if (inStyle&1)
        thestyle=thestyle|WS_VSCROLL;
    if (inStyle&2)
        thestyle=thestyle|WS_HSCROLL;

    if (inFrameOn)
        theltex.mHwndEdit=CreateWindowEx (WS_EX_CLIENTEDGE, RICHEDIT_CLASS, NULL, 
                    (DWORD) thestyle,
                    inX, inY, inWidth, inHeight, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
                    _LApplication_gGlobal.inInstance, NULL);    
    else
        theltex.mHwndEdit=CreateWindow (RICHEDIT_CLASS, NULL, 
                    (DWORD) thestyle,
                    inX, inY, inWidth, inHeight, inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, 
                    _LApplication_gGlobal.inInstance, NULL);

	/* check for call failure */
	if (theltex.mHwndEdit==NULL) Throw(LTextEditor_CANT_CREATE);

	/*subclassing...*/
    theltex.mDefaultProc=(WNDPROC) SetWindowLong (theltex.mHwndEdit, 
                                                GWL_WNDPROC, (LONG) _patchproc);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
   
	theltex.mLCLKHandler=NULL;
	theltex.mLDBLCLKHandler=NULL;
	theltex.mRCLKHandler=NULL;
	theltex.mKEYDOWNHandler=NULL;

    theltex.mHandler=NULL; /*to fix a bug...*/
    theltex.mID=LTextEditor_ID;
    
    theltex.mParentWindow=inWindow; /* [CD020718] */

    theltexptr=LMemory_NewObject (LTextEditor, theltex);

	/*mutual reference pointer...hopefully*/
    SetWindowLong (theltex.mHwndEdit, GWL_USERDATA, (LONG) theltexptr);

	/*512kb dynamic buffer upper bound...*/
	SendMessage (theltex.mHwndEdit, EM_EXLIMITTEXT, 0, 524288);

    return theltexptr;
}

/*------------------------------------------------
// Resize ()
//------------------------------------------------*/
void LTextEditor_Resize (LTextEditor* inEditor, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inEditor->mHwndEdit, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE); 
}

/*------------------------------------------------
// Move ()
//------------------------------------------------*/
void LTextEditor_Move(LTextEditor* inEditor, ui4 inX, ui4 inY)
{
    SetWindowPos (inEditor->mHwndEdit, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE); 
}

/*--------------------------------------------------
// SetTextBuffer ()
//--------------------------------------------------*/
void LTextEditor_SetTextBuffer (LTextEditor* inEditor, const i1* inBuffer, ui4 inSize)
{
	i1* theptr;

	theptr=(i1 *) LMemory_Malloc (inSize+1);
	LMemory_Copy (inBuffer, theptr, inSize);
	theptr[inSize]='\0';

    SetWindowText (inEditor->mHwndEdit, (LPCTSTR) theptr);

	LMemory_Free (&theptr);

	/*resets 'modified' flag...*/
	LTextEditor_SetModify (inEditor, FALSE);
}

/*--------------------------------------------------
// GetTextBufferSize ()
//-------------------------------------------------*/
ui4 LTextEditor_GetTextBufferSize (LTextEditor* inEditor)
{
    return (ui4) (GetWindowTextLength (inEditor->mHwndEdit));
}

/*--------------------------------------------------
// GetTextBuffer ()
//-------------------------------------------------*/
void LTextEditor_GetTextBuffer (LTextEditor* inEditor, i1* inBuffer)
{
	void* theauxbuf;

	theauxbuf=LMemory_Malloc (GetWindowTextLength (inEditor->mHwndEdit)+1);

    GetWindowText (inEditor->mHwndEdit, (LPTSTR) theauxbuf, GetWindowTextLength (inEditor->mHwndEdit)+1);
	
	LMemory_Copy (theauxbuf, inBuffer, GetWindowTextLength (inEditor->mHwndEdit));

	LMemory_Free (&theauxbuf);
}

/*-------------------------------------------------
// SetTextFont ()
//------------------------------------------------*/
void LTextEditor_SetTextFont (LTextEditor* inEditor, LTextEditor_TFont inFont)
{
    LOGFONT thelogfont;
    HFONT thehfont;
    RECT therect;
    int theauxfont;

    switch (inFont)
    {
    case LTextEditor_ANSIFIXED:
        theauxfont=ANSI_FIXED_FONT;
        break;
    case LTextEditor_ANSIVAR:
        theauxfont=ANSI_VAR_FONT;
        break;
    case LTextEditor_SYSTEMFIXED:
        theauxfont=SYSTEM_FIXED_FONT;
        break;
    case LTextEditor_SYSTEMVAR:
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
void LTextEditor_SetFocus (LTextEditor *inEditor)
{
    LWindow* thewaux;

    thewaux=LTextEditor_GetParentWindow (inEditor);
    thewaux->mWidgetPointer=(void *) inEditor;
    thewaux->mWidgetType=LTextEditor_ID;

    SetFocus (inEditor->mHwndEdit);
}

/*----------------------------------------
// Undo ()
//--------------------------------------*/
void LTextEditor_Undo (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_UNDO, 0, 0);
}

/*----------------------------------------
// Cut ()
//--------------------------------------*/
void LTextEditor_Cut (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_CUT, 0, 0);
}

/*----------------------------------------
// Copy ()
//--------------------------------------*/
void LTextEditor_Copy (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_COPY, 0, 0);
}

/*----------------------------------------
// Paste ()
//--------------------------------------*/
void LTextEditor_Paste (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_PASTE, 0, 0);
}

/*----------------------------------------
// SelectAll ()
//--------------------------------------*/
void LTextEditor_SelectAll (LTextEditor* inEditor)
{
	CHARRANGE thecharrange;

	thecharrange.cpMin=0;
	thecharrange.cpMax=-1;

    SendMessage (inEditor->mHwndEdit, EM_EXSETSEL, 0, (LPARAM) &thecharrange);
}

/*----------------------------------------
// CanUndo ()
//---------------------------------------*/
Bool LTextEditor_CanUndo (LTextEditor* inEditor)
{
    return (Bool) SendMessage (inEditor->mHwndEdit, EM_CANUNDO, 0, 0);
}

/*---------------------------------------
// IsTextAvailable ()
//--------------------------------------*/
Bool LTextEditor_IsTextAvailable ()
{
    return (Bool) IsClipboardFormatAvailable (CF_TEXT);
}

/*----------------------------------------
// IsSelectionAvailable ()
//---------------------------------------*/
Bool LTextEditor_IsSelectionAvailable (LTextEditor* inEditor)
{
	CHARRANGE thecharrange;

    SendMessage (inEditor->mHwndEdit, EM_EXGETSEL, 0, (LPARAM) &thecharrange);
    if (thecharrange.cpMin==thecharrange.cpMax)
        return FALSE;
    else
        return TRUE;
}

/*NEW FUNCTIONS*/

/*----------------------------------
// GetSelection ()
//---------------------------------*/
void LTextEditor_GetSelection (LTextEditor* inEditor, ui4* outStart, ui4* outEnd)
{
	CHARRANGE thecharrange;

    SendMessage (inEditor->mHwndEdit, EM_EXGETSEL, 0, (LPARAM) &thecharrange);
	
	*outStart=thecharrange.cpMin;
	*outEnd=thecharrange.cpMax;
}

/*----------------------------------
// SetSelection ()
//---------------------------------*/
void LTextEditor_SetSelection (LTextEditor* inEditor, ui4 inStart, ui4 inEnd)
{
	CHARRANGE thecharrange;

	thecharrange.cpMin=inStart;
	thecharrange.cpMax=inEnd;

    SendMessage (inEditor->mHwndEdit, EM_EXSETSEL, 0, (LPARAM) &thecharrange);
}

/*----------------------------------
// ReplaceSelection ()
//---------------------------------*/
void LTextEditor_ReplaceSelection (LTextEditor* inEditor, const i1* inBuffer, ui4 inSize)
{
	i1 *theptr;

	theptr=(i1 *) LMemory_Malloc (inSize+1);
	LMemory_Copy (inBuffer, theptr, inSize);
	theptr[inSize]='\0';

    SendMessage (inEditor->mHwndEdit, EM_REPLACESEL, 0, (LPARAM) theptr);

	LMemory_Free (&theptr);
}

/*----------------------------------
// ScrollToCursor ()
//---------------------------------*/
void LTextEditor_ScrollToCursor (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, EM_SCROLLCARET, 0, 0);
}

/*----------------------------------
// Scroll ()
//---------------------------------*/
void LTextEditor_Scroll (LTextEditor* inEditor, i4 inCharHor, i4 inLinesVer)
{
    SendMessage (inEditor->mHwndEdit, EM_LINESCROLL, (WPARAM) inCharHor, (LPARAM) inLinesVer);
}

/*----------------------------------
// DeleteSelection ()
//---------------------------------*/
void LTextEditor_DeleteSelection (LTextEditor* inEditor)
{
    SendMessage (inEditor->mHwndEdit, WM_CLEAR, 0, 0);
}

/*----------------------------------
// GetLineCount ()
//---------------------------------*/
ui4 LTextEditor_GetLineCount (LTextEditor* inEditor)
{
    return (ui4) SendMessage (inEditor->mHwndEdit, EM_GETLINECOUNT, 0, 0);
}

/*----------------------------------
// GetLineIndex ()
//---------------------------------*/
ui4 LTextEditor_GetLineIndex (LTextEditor* inEditor, ui4 inLine)
{
    return (ui4) SendMessage (inEditor->mHwndEdit, EM_LINEINDEX, (WPARAM) inLine, 0);
}

/*----------------------------------
// GetLineLength ()
//---------------------------------*/
ui4 LTextEditor_GetLineLength (LTextEditor* inEditor, ui4 inIndex)
{
    return (ui4) SendMessage (inEditor->mHwndEdit, EM_LINELENGTH, (WPARAM) inIndex, 0);
}

/*---------------------------------
// GetLine ()
//--------------------------------*/
ui4 LTextEditor_GetLine (LTextEditor* inEditor, ui4 inLine, i1* inBuffer)
{
    return (ui4) SendMessage (inEditor->mHwndEdit, EM_GETLINE, (WPARAM) inLine, (LPARAM) inBuffer);
}

/*---------------------------------
// GetModify ()
//--------------------------------*/
Bool LTextEditor_GetModify (LTextEditor* inEditor)
{
    return (Bool) SendMessage (inEditor->mHwndEdit, EM_GETMODIFY, 0, 0);
}

/*---------------------------------
// SetModify ()
//--------------------------------*/
void LTextEditor_SetModify (LTextEditor* inEditor, Bool inFlag)
{
    SendMessage (inEditor->mHwndEdit, EM_SETMODIFY, (WPARAM) (UINT) inFlag, 0);
}

/*---------------------------------
// GetFirstVisibleLine ()
//--------------------------------*/
ui4 LTextEditor_GetFirstVisibleLine (LTextEditor* inEditor)
{
    return (ui4) SendMessage (inEditor->mHwndEdit, EM_GETFIRSTVISIBLELINE, 0, 0);
}

/*---------------------------------
// InstallHandler ()
//--------------------------------*/
void LTextEditor_InstallHandler (LTextEditor* inEditor, LTextEditor_TEventID inEventID, Bool inLater, LTextEditor_THandler inHandler)
{
    switch (inEventID)
	{
	case LTextEditor_MOUSE_LCLK:
		inEditor->mLCLKHandler=(_LTextEditor_THandler) inHandler;
		inEditor->mLCLKLater=inLater;
		break;
	case LTextEditor_MOUSE_LDBLCLK:
		inEditor->mLDBLCLKHandler=(_LTextEditor_THandler) inHandler;
		inEditor->mLDBLCLKLater=inLater;
		break;
	case LTextEditor_MOUSE_RCLK:
		inEditor->mRCLKHandler=(_LTextEditor_THandler) inHandler;
		inEditor->mRCLKLater=inLater;
		break;
	case LTextEditor_KEYDOWN:
		inEditor->mKEYDOWNHandler=(_LTextEditor_THandler) inHandler;
		inEditor->mKEYDOWNLater=inLater;
		break;
	}
}

/*---------------------------------
// GetCursorPos ()
//--------------------------------*/
void LTextEditor_GetCursorPos (LTextEditor* inEditor, ui4* outLine, ui4* outCol)
{
	ui4 thelineindex;
	ui4 theline;
	ui4 thecol;
	CHARRANGE thecharrange;

	/*cursor line index*/
	thelineindex=SendMessage (inEditor->mHwndEdit, EM_LINEINDEX, (WPARAM) -1, 0);
	/*cursor line number*/
	theline=SendMessage (inEditor->mHwndEdit, EM_EXLINEFROMCHAR, 0, (LPARAM) thelineindex);
	*outLine=theline;
	/*current selection*/
    SendMessage (inEditor->mHwndEdit, EM_EXGETSEL, 0, (LPARAM) &thecharrange);
	/*cursor column*/
	thecol=thecharrange.cpMax-thelineindex;
	*outCol=thecol;
}

/*--------------------------------
 * GetParentWindow() [CD020718]
 *--------------------------------
*/
LWindow* LTextEditor_GetParentWindow (LTextEditor* inEditor){
    return inEditor->mParentWindow;
}

/*------------------------------------------
// SetSelectionColor()
//-----------------------------------------*/
void LTextEditor_SetSelectionColor (LTextEditor* inEditor, ui4 inColor)
{
	CHARFORMAT thecharformat;

	thecharformat.cbSize=sizeof (CHARFORMAT); /*required!*/
	thecharformat.dwMask=CFM_COLOR; /*want to change text color*/
	thecharformat.crTextColor=inColor; /*new color*/
	thecharformat.dwEffects=0; /*struct is not initialized to 0!*/

	SendMessage (inEditor->mHwndEdit, EM_SETCHARFORMAT, (WPARAM) SCF_SELECTION, (LPARAM) &thecharformat);
}

/*----------------------------------
// CharFromPos ()
//--------------------------------*/
void LTextEditor_CharFromPos (LTextEditor* inEditor, ui4 inX, ui4 inY, ui4* outCharIndex)
{
	POINTL thepoint;

	thepoint.x=inX;
	thepoint.y=inY;

	*outCharIndex=SendMessage (inEditor->mHwndEdit, EM_CHARFROMPOS, 0, (LPARAM) &thepoint);
}

/*-----------------------------------
// SetSelectionStatus ()
//---------------------------------*/
void LTextEditor_SetSelectionStatus (LTextEditor* inEditor, Bool inVisible)
{
	/*change selection status according to inVisible...*/
	SendMessage (inEditor->mHwndEdit, EM_HIDESELECTION, (WPARAM) (BOOL) !inVisible, 0);
	/*...but keep the ES_NOHIDESEL flag set*/  
	SendMessage (inEditor->mHwndEdit, EM_HIDESELECTION, (WPARAM) (BOOL) FALSE, 1);
}

/*--------------------------------
// _VirtKey2LTxtEdt ()
//------------------------------*/
i4 _VirtKey2LTxtEdt (int inVirtKey)
{
	if (inVirtKey==VK_UP)
		return LTextEditor_UP;
	if (inVirtKey==VK_DOWN)
		return LTextEditor_DOWN;
	if (inVirtKey==VK_LEFT)
		return LTextEditor_LEFT;
	if (inVirtKey==VK_RIGHT)
		return LTextEditor_RIGHT;
	if (inVirtKey==VK_NEXT)
		return LTextEditor_PAGEDOWN;
	if (inVirtKey==VK_PRIOR)
		return LTextEditor_PAGEUP;
	if (inVirtKey==VK_END)
		return LTextEditor_END;
	if (inVirtKey==VK_HOME)
		return LTextEditor_HOME;
	
	/*RichEdit v2.0*/
	if (inVirtKey==VK_DELETE) 
		return LTextEditor_DELETE; 
	if (inVirtKey==VK_RETURN)
		return LTextEditor_RETURN;
	if (inVirtKey==VK_BACK) 
		return LTextEditor_BACKSPACE; 

	return 0xffffffff;
}

/*--------------------------------
// _VirtChar2LTxtEdt ()
//-------------------------------*/
i4 _VirtChar2LTxtEdt (TCHAR inVirtChar)
{
	if (inVirtChar==VK_RETURN)
		return LTextEditor_RETURN;
	/*if (inVirtChar==VK_TAB)
		return LTextEditor_TAB;*/
	if (inVirtChar==VK_BACK) 
		return LTextEditor_BACKSPACE; 

	return 0xffffffff;
}

/*--------------------------------
// _patchproc ()
//-------------------------------*/
LRESULT CALLBACK _patchproc (HWND inHwnd, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
	LTextEditor *theaux;
	Bool thedecider;
	i4 theparameter;

	theaux=(LTextEditor *) GetWindowLong (inHwnd, GWL_USERDATA);

	switch (inMessage)
	{
	case WM_CHAR:
		if (theaux->mKEYDOWNHandler==NULL) /*no handler installed*/
			break;
		if (theaux->mKEYDOWNLater==FALSE) /*regular processing...*/
		{
			theparameter=_VirtChar2LTxtEdt ((TCHAR) inWParam);
			if (theparameter==0xffffffff) /*regular char...*/
				thedecider=theaux->mKEYDOWNHandler (theaux, (i4) (TCHAR) inWParam);
			else /*virtual char...*/
				return 0;
				/*thedecider=theaux->mKEYDOWNHandler (theaux, theparameter);*/
			if (thedecider==TRUE)
				return 0; /*as a WM_CHAR should*/
			else
				return CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);
		}
		else /*inverted processing...*/
		{
			/*CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam); /*system handler first...*/

			theparameter=_VirtChar2LTxtEdt ((TCHAR) inWParam);
			if (theparameter==0xffffffff) /*regular char...*/
			{
				CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);
				theaux->mKEYDOWNHandler (theaux, (i4) (TCHAR) inWParam);
			}
			else /*virtual char...*/
				return 0;
				/*theaux->mKEYDOWNHandler (theaux, theparameter);*/

			return 0;
		}
	case WM_KEYDOWN:
		if (theaux->mKEYDOWNHandler==NULL) /*no handler installed*/
			break;
		if (theaux->mKEYDOWNLater==FALSE) /*regular processing...*/
		{
			theparameter=_VirtKey2LTxtEdt ((int) inWParam);
			if (theparameter==0xffffffff) /*regular key...*/
				return 0;
				/*thedecider=theaux->mKEYDOWNHandler (theaux, (i4) (int) inWParam);*/
			else /*virtual key...*/
				thedecider=theaux->mKEYDOWNHandler (theaux, theparameter);
			if (thedecider==TRUE)
				return 0; /*as a WM_KEYDOWN should*/
			else
				return CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);
		}
		else /*inverted processing...*/
		{
			/*CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam); /*system handler first...*/
			
			theparameter=_VirtKey2LTxtEdt ((int) inWParam);
			if (theparameter==0xffffffff) /*regular key...*/
				return 0;
				/*theaux->mKEYDOWNHandler (theaux, (i4) (int) inWParam);*/
			else /*virtual key...*/
			{
				CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);
				theaux->mKEYDOWNHandler (theaux, theparameter);
			}

			return 0;
		}
	case WM_LBUTTONDOWN:
		if (theaux->mLCLKHandler==NULL) /*no handler installed*/
            break;
		if (theaux->mLCLKLater==FALSE) /*regular processing...*/
		{
			thedecider=theaux->mLCLKHandler (theaux, LOWORD (inLParam), HIWORD (inLParam));
			if (thedecider==TRUE)
				return 0;
			else
				return CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);	
		}
		else /*inverted processing...*/
		{
			CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam); /*system handler first...*/
			theaux->mLCLKHandler (theaux, LOWORD (inLParam), HIWORD (inLParam));
			return 0;
		}
	case WM_LBUTTONDBLCLK:
		if (theaux->mLDBLCLKHandler==NULL) /*no handler installed*/
            break;
		if (theaux->mLDBLCLKLater==FALSE) /*regular processing...*/
		{
			thedecider=theaux->mLDBLCLKHandler (theaux, LOWORD (inLParam), HIWORD (inLParam));
			if (thedecider==TRUE)
				return 0;
			else
				return CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);
		}
		else /*inverted processing...*/
		{
			CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam); /*system handler first...*/
			theaux->mLDBLCLKHandler (theaux, LOWORD (inLParam), HIWORD (inLParam));
			return 0;
		}
	case WM_RBUTTONDOWN:
		if (theaux->mRCLKHandler==NULL) /*no handler installed*/
            break;
		if (theaux->mRCLKLater==FALSE) /*regular processing...*/
		{
			thedecider=theaux->mRCLKHandler (theaux, LOWORD (inLParam), HIWORD (inLParam));
			if (thedecider==TRUE)
				return 0;
			else
				return CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam);	
		}
		else /*inverted processing...*/
		{
			CallWindowProc (theaux->mDefaultProc, inHwnd, inMessage, inWParam, inLParam); /*system handler first...*/
			theaux->mRCLKHandler (theaux, LOWORD (inLParam), HIWORD (inLParam));
			return 0;
		}
	case WM_RBUTTONUP:
		return 0; /*otherwise this event would activate the editor*/
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
