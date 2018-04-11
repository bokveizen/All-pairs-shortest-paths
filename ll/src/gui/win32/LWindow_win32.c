/* ============================================================================
 *  LWindow_win32.c
 * ============================================================================

 *  Author:         (c) 2001 Andrea Ribichini, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        Dec 20, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LWindow.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__


/*---------------------------------------------------------------------------------
//  InstallEvent
//---------------------------------------------------------------------------------*/
void LWindow_InstallEvent(LWindow* inWindow, LWindow_TEventID inEventID,
                          LWindow_THandler inEventHandler) 
{
    switch (inEventID)
    {
    case LWindow_CLOSE:
        inWindow->mCloseHandler=(_LWindow_THandler) inEventHandler;
        break;
    case LWindow_MOUSE_DOWN:
        inWindow->mMouseDownHandler=(_LWindow_THandler) inEventHandler;
        break;
    case LWindow_ACTIVATE:
        inWindow->mActivateHandler=(_LWindow_THandler) inEventHandler;
        break;
    case LWindow_RESIZE:
        inWindow->mResizeHandler=(_LWindow_THandler) inEventHandler;
        break;
    case LWindow_MOUSE_MOVE:
        inWindow->mMouseMoveHandler=(_LWindow_THandler) inEventHandler;
        break;
    case LWindow_SET_FOCUS:
        inWindow->mSetFocusHandler=(_LWindow_THandler) inEventHandler;
        break;
    }
}

/*----------------------------------------------
// Close ()
//----------------------------------------------*/
void LWindow_Close (LWindow* inWindow)
{
    SendMessage (inWindow->mHwnd, WM_CLOSE, 0, 0);  
}

/*----------------------------------------------
// Destroy()
//----------------------------------------------*/
void LWindow_Destroy (LWindow* inWindow)
{
    DestroyWindow (inWindow->mHwnd);
}

/*----------------------------------------------
// Create()
//----------------------------------------------*/
LWindow* LWindow_Create (LWindow_TWindowType inWindowType, ui4 inStyle)
{
    LWindow thelwin;
    LWindow *thelwinptr;

    ui4 thestyle=0;

    thestyle=WS_CHILD|WS_CLIPSIBLINGS|WS_SYSMENU|WS_BORDER;

    if (inStyle&LWindow_Caption)
        thestyle=thestyle|WS_CAPTION;
    if (inStyle&LWindow_Resizeable)
        thestyle=thestyle|WS_SIZEBOX;
    if (inStyle&LWindow_MinMaxBox)
        thestyle=thestyle|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;

    switch (inWindowType)
    {
        case LWindow_REGULAR:

            thelwin.mType=LWindow_REGULAR;

            thelwin.mHwnd=CreateWindowEx (WS_EX_MDICHILD, "ChildWindow", "LSL Regular Window",
                (DWORD) thestyle, 
                0, 0, 300, 200,
                _LApplication_gGlobal.hwndClient,
                (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild,
                _LApplication_gGlobal.inInstance, NULL);

            _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;      
            break;

        case LWindow_FLOATING:

            thelwin.mType=LWindow_FLOATING;

            thelwin.mHwnd=CreateWindowEx (WS_EX_MDICHILD|WS_EX_PALETTEWINDOW, "ChildWindow",
                "LSL Floating Window",
                (DWORD) thestyle,
                /*WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER|WS_CAPTION|WS_SYSMENU,*/
                100, 100, 200, 150,
                _LApplication_gGlobal.hwndClient,
                (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild,
                _LApplication_gGlobal.inInstance, NULL);

            _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
            break;  
    }

	/* check for call failure */
	if (thelwin.mHwnd==NULL) Throw(LWindow_CANT_CREATE);

/*initializes event handlers...*/
    thelwin.mCloseHandler=NULL;
    thelwin.mMouseDownHandler=NULL;
    thelwin.mActivateHandler=NULL;
    thelwin.mResizeHandler=NULL;
    thelwin.mMouseMoveHandler=NULL;
    thelwin.mSetFocusHandler=NULL;

    thelwin.mStatusBarHandle=NULL; /*required by status bar*/

    thelwin.mWidgetPointer=NULL; /*will hold pointer to last widget to request focus...*/

	thelwin.mID=LWindow_ID; /*required by LApplication_GetFocus()*/

	thelwin.mCursorShape=LCursor_ARROW;

	thelwin.mMinMaxEnable=FALSE;

    thelwinptr=LMemory_NewObject (LWindow, thelwin);
    /*slot 0 contains *mutual reference* pointer!!!*/
    SetWindowLong (thelwin.mHwnd, 0, (long) thelwinptr);
	/*required by LApplication_GetFocus()!!!!*/
	SetWindowLong (thelwin.mHwnd, GWL_USERDATA, (long) thelwinptr);

    return thelwinptr;
}

/*------------------------------------------------
// Finalize()
//------------------------------------------------*/
void LWindow_Finalize (LWindow* inWindow)
{
    ShowWindow (inWindow->mHwnd, SW_SHOWNORMAL);
/* ***AR Are there any side effects????*/   
/*  SendMessage (inWindow->mHwnd, WM_CHILDACTIVATE, 0, 0);*/
/*Keep an eye on it...*/
    UpdateWindow (inWindow->mHwnd);
}


/*------------------------------------------------
// SetTitle()
//------------------------------------------------*/
void LWindow_SetTitle(LWindow* inWindow, const i1* inTitle)
{
    SetWindowText (inWindow->mHwnd, inTitle);
}

/*------------------------------------------------
// GetTitle()
//------------------------------------------------*/
void LWindow_GetTitle(LWindow* inWindow, i1* outTitle)
{
    GetWindowText (inWindow->mHwnd, outTitle, LWindow_MAX_TITLE); 
}

/*------------------------------------------------
// Resize ()
//------------------------------------------------*/
void LWindow_Resize(LWindow* inWindow, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inWindow->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE); 
}

/*------------------------------------------------
// Move ()
//------------------------------------------------*/
void LWindow_Move(LWindow* inWindow, ui4 inX, ui4 inY)
{
    SetWindowPos (inWindow->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE); 
}

/*------------------------------------------------
// BringToFront ()
//------------------------------------------------*/
void LWindow_BringToFront (LWindow* inWindow)
{
    LWindow* thelwaux;

/*if there is a maximized window...restore it*/
    thelwaux=LWindow_GetFrontWindow ();
    if (thelwaux!=NULL)
    {
        if (LWindow_GetSizeStatus (thelwaux)==LWindow_MAXIMIZED)
            LWindow_SetSizeStatus (thelwaux, LWindow_USER);
    }

    SendMessage (_LApplication_gGlobal.hwndClient, WM_MDIACTIVATE, (WPARAM) inWindow->mHwnd, 0);
/*Are there any side effects????*/  
/*  SendMessage (inWindow->mHwnd, WM_CHILDACTIVATE, 0, 0);*/
/*Keep an eye on it...*/
    UpdateWindow (inWindow->mHwnd);
}

/*------------------------------------------------
// SetSizeStatus ()
//------------------------------------------------*/
void LWindow_SetSizeStatus (LWindow* inWindow, LWindow_TSizeStatus inStatus)
{
    switch (inStatus)
    {
    case LWindow_USER:
        SendMessage (_LApplication_gGlobal.hwndClient, WM_MDIRESTORE, (WPARAM) inWindow->mHwnd, 0);
        break;
    case LWindow_MINIMIZED:
        CloseWindow (inWindow->mHwnd);
        SendMessage (_LApplication_gGlobal.hwndClient, WM_MDIICONARRANGE, 0, 0);
        break;
    case LWindow_MAXIMIZED:
/*only foreground window can be maximized!!!*/
        LWindow_BringToFront (inWindow);
        SendMessage (_LApplication_gGlobal.hwndClient, WM_MDIMAXIMIZE, (WPARAM) inWindow->mHwnd, 0);
        break;
    }
}

/*-------------------------------------------------
// GetSizeStatus ()
//-------------------------------------------------*/
LWindow_TSizeStatus LWindow_GetSizeStatus (LWindow* inWindow)
{
    if (IsIconic (inWindow->mHwnd))
        return LWindow_MINIMIZED;
    if (IsZoomed (inWindow->mHwnd))
        return LWindow_MAXIMIZED;
    return LWindow_USER;
}

/*-------------------------------------------------
// GetFrontWindow ()
//-------------------------------------------------*/
LWindow* LWindow_GetFrontWindow()
{
    HWND theauxhwnd;

    theauxhwnd=(HWND) SendMessage (_LApplication_gGlobal.hwndClient, WM_MDIGETACTIVE, 0, (LPARAM) NULL);
    if (theauxhwnd!=NULL)
        return (LWindow*) GetWindowLong (theauxhwnd, 0);
    else
        return NULL;
}

/*----------------------------------------------------
// GetType ()
//---------------------------------------------------*/
LWindow_TWindowType LWindow_GetType (LWindow* inWindow)
{
    return inWindow->mType;
}

/*----------------------------------------------------
// GetClientAreaSize ()
//---------------------------------------------------*/
void LWindow_GetClientAreaSize (LWindow* inWindow, ui4* outWidth, ui4* outHeight)
{
    RECT theaux;
    
    GetClientRect (inWindow->mHwnd, &theaux);

    *outWidth=theaux.right;
    *outHeight=theaux.bottom;
}

/*----------------------------------------------------
// GetPosition ()
//---------------------------------------------------*/
void LWindow_GetPosition (LWindow* inWindow, ui4* outX, ui4* outY)
{
    WINDOWPLACEMENT thepl;

    thepl.length=sizeof (WINDOWPLACEMENT);
    GetWindowPlacement (inWindow->mHwnd, &thepl);

    *outX=(thepl.rcNormalPosition).left;
    *outY=(thepl.rcNormalPosition).top;
}

/*----------------------------------------------------
 * SetUserData () [CD020718]
 *---------------------------------------------------
*/
void LWindow_SetUserData(LWindow* inWindow, const void* inData){
    inWindow->mUserData=(void*)inData;
}

/*----------------------------------------------------
 * GetUserData () [CD020718]
 *---------------------------------------------------
*/
void* LWindow_GetUserData(LWindow* inWindow){
    return inWindow->mUserData;
}

/*--------------------------------------
// SetCursorShape ()
//--------------------------------------*/
void LWindow_SetCursorShape (LWindow* inWindow, LCursor_TShape inCursorShape)
{
	inWindow->mCursorShape=inCursorShape;
}

/*---------------------------------------
// GetCursorShape ()
//--------------------------------------*/
LCursor_TShape LWindow_GetCursorShape (LWindow* inWindow)
{
	return inWindow->mCursorShape;
}

/*---------------------------------------
// EnableMinMax ()
//--------------------------------------*/
void LWindow_EnableMinMax (LWindow* inWindow, ui4 inMinX, ui4 inMinY, ui4 inMaxX, ui4 inMaxY)
{
	inWindow->mMinX=inMinX;
	inWindow->mMinY=inMinY;
	inWindow->mMaxX=inMaxX;
	inWindow->mMaxY=inMaxY;

	inWindow->mMinMaxEnable=TRUE;
}

/*---------------------------------------
// DisableMinMax ()
//-------------------------------------*/
void LWindow_DisableMinMax (LWindow* inWindow)
{
	inWindow->mMinMaxEnable=FALSE;
}

#endif

/* Copyright (C) 2001 Andrea Ribichini, Camil Demetrescu

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

