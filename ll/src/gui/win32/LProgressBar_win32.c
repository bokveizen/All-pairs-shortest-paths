/* =============================================================================
 *  LProgressBar_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jul 20, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LProgressBar.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

struct tagLProgressBar {
    HWND mHwnd;
    void (*mHandler) ();
    ui2 mID;
    LWindow* mParentWindow;
};


/*----------------------------
// Create ()
//--------------------------*/
LProgressBar* LProgressBar_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY)
{
    LProgressBar thebar;
    LProgressBar* thebarptr;

    thebar.mParentWindow=inWindow;

    thebar.mHwnd=CreateWindowEx (0, PROGRESS_CLASS, NULL, WS_CHILD|WS_VISIBLE|WS_BORDER, inX, inY, inDX, inDY,
        inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    
	/* check for call failure */
	if (thebar.mHwnd==NULL) Throw(LProgressBar_CANT_CREATE);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;

    thebar.mID=LProgressBar_ID;

    thebarptr=LMemory_NewObject (LProgressBar, thebar);
    SetWindowLong (thebar.mHwnd, GWL_USERDATA, (LONG) thebarptr);
    return thebarptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LProgressBar_Destroy (LProgressBar* inBar)
{
    DestroyWindow (inBar->mHwnd);
    LMemory_DeleteObject (&inBar);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LProgressBar_Move (LProgressBar* inBar, i4 inX, i4 inY)
{
    SetWindowPos (inBar->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*--------------------------------------
// SetPosition ()
//-------------------------------------*/
void LProgressBar_SetPosition (LProgressBar* inBar, ui2 inPos)
{
    SendMessage (inBar->mHwnd, PBM_SETPOS, (WPARAM) inPos, 0); 
}

/*--------------------------------------
// SetRange ()
//-------------------------------------*/
void LProgressBar_SetRange (LProgressBar* inBar, ui2 inMinRange, ui2 inMaxRange)
{
    SendMessage (inBar->mHwnd, PBM_SETRANGE, 0, MAKELPARAM (inMinRange, inMaxRange));
}

/*-------------------------------------
// SetStep ()
//------------------------------------*/
void LProgressBar_SetStep (LProgressBar* inBar, ui2 inStep)
{
    SendMessage (inBar->mHwnd, PBM_SETSTEP, (WPARAM) inStep, 0);
}

/*-------------------------------------
// StepIt ()
//------------------------------------*/
void LProgressBar_StepIt (LProgressBar* inBar)
{
    SendMessage (inBar->mHwnd, PBM_STEPIT, 0, 0);
}

/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LProgressBar_GetParentWindow (LProgressBar* inBar)
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

