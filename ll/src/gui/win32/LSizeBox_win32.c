/* =============================================================================
 *  LSizeBox_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Aug 29, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LSizeBox.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

struct tagLSizeBox {
    HWND mHwnd;
    void (*mHandler) ();
    ui2 mID;
    LWindow* mParentWindow;
};

/*----------------------------
// Create ()
//--------------------------*/
LSizeBox* LSizeBox_Create (LWindow* inWindow, i4 inX, i4 inY, ui4 inDX, ui4 inDY)
{
    
    LSizeBox thebox;
    LSizeBox* theboxptr;

    thebox.mParentWindow=inWindow;

    thebox.mHwnd=CreateWindowEx (0, "scrollbar", NULL, WS_CHILD|WS_VISIBLE|SBS_SIZEGRIP, inX, inY, inDX, inDY,
        inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
    
	/* check for call failure */
	if (thebox.mHwnd==NULL) Throw(LSizeBox_CANT_CREATE);

    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;

    thebox.mID=LSizeBox_ID;
    thebox.mHandler=NULL;

    theboxptr=LMemory_NewObject (LSizeBox, thebox);
    SetWindowLong (thebox.mHwnd, GWL_USERDATA, (LONG) theboxptr);
    return theboxptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LSizeBox_Destroy (LSizeBox* inBox)
{
    DestroyWindow (inBox->mHwnd);
    LMemory_DeleteObject (&inBox);
}

/*--------------------------------------
// Move ()
//-------------------------------------*/
void LSizeBox_Move (LSizeBox* inBox, i4 inX, i4 inY)
{
    SetWindowPos (inBox->mHwnd, HWND_TOP, inX, inY, 0, 0, SWP_NOSIZE);
}

/*-------------------------------------
// Resize ()
//------------------------------------*/
void LSizeBox_Resize (LSizeBox* inBox, ui4 inWidth, ui4 inHeight)
{
    SetWindowPos (inBox->mHwnd, HWND_TOP, 0, 0, inWidth, inHeight, SWP_NOMOVE);
}

/*-------------------------------------
// GetParentWindow ()
//------------------------------------*/
LWindow* LSizeBox_GetParentWindow (LSizeBox* inBox)
{
    return inBox->mParentWindow;
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

