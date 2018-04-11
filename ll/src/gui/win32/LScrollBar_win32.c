/* =============================================================================
 *  LScrollBar_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Aug 26, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LScrollBar.h"
#include "LScrollBar_win32.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*struct tagLScrollBar {
    HWND mHwnd;
    void (*mHandler) ();
    ui2 mID;
    LWindow* mParentWindow;
};*/

/*----------------------------
// Create ()
//--------------------------*/
LScrollBar* LScrollBar_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY, ui2 inHorVer, LScrollBar_THandler inHandler)
{
    
    LScrollBar thebar;
    LScrollBar* thebarptr;

    thebar.mParentWindow=inWindow;

    if (inHorVer==LScrollBar_VER)
        thebar.mHwnd=CreateWindowEx (0, "scrollbar", NULL, WS_CHILD|WS_VISIBLE|SBS_VERT, inX, inY, inDX, inDY,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    if (inHorVer==LScrollBar_HOR)
        thebar.mHwnd=CreateWindowEx (0, "scrollbar", NULL, WS_CHILD|WS_VISIBLE|SBS_HORZ, inX, inY, inDX, inDY,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    
	/* check for call failure */
	if (thebar.mHwnd==NULL) Throw(LScrollBar_CANT_CREATE);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;

    thebar.mID=LScrollBar_ID;
    thebar.mHandler=(_LScrollBar_THandler) inHandler;

    thebarptr=LMemory_NewObject (LScrollBar, thebar);
    SetWindowLong (thebar.mHwnd, GWL_USERDATA, (LONG) thebarptr);
    return thebarptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LScrollBar_Destroy (LScrollBar* inBar)
{
    DestroyWindow (inBar->mHwnd);
    LMemory_DeleteObject (&inBar);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LScrollBar_Move (LScrollBar* inBar, i4 inX, i4 inY)
{
    SetWindowPos (inBar->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*-------------------------------------
// Resize ()
//------------------------------------*/
void LScrollBar_Resize (LScrollBar* inBar, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inBar->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*--------------------------------------
// SetPosition ()
//-------------------------------------*/
void LScrollBar_SetPosition (LScrollBar* inBar, ui2 inPos)
{
    SetScrollPos (inBar->mHwnd, SB_CTL, (int) inPos, TRUE); 
}

/*-------------------------------------
// GetPosition ()
//-----------------------------------*/
ui2 LScrollBar_GetPosition (LScrollBar* inBar)
{
    return (ui2) GetScrollPos (inBar->mHwnd, SB_CTL);
}

/*--------------------------------------
// SetRange ()
//-------------------------------------*/
void LScrollBar_SetRange (LScrollBar* inBar, ui2 inMinRange, ui2 inMaxRange)
{
    SetScrollRange (inBar->mHwnd, SB_CTL, (int) inMinRange, (int) inMaxRange, TRUE);
}

/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LScrollBar_GetParentWindow (LScrollBar* inBar)
{
    return inBar->mParentWindow;
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

