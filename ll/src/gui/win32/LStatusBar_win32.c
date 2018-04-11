/* =============================================================================
 *  LStatusBar_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jul 21, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:07 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LStatusBar.h"
#include "LWindow_win32.h"
#include "LWindow.h" 

#if __LSL_OS_GUI__ == __LSL_Win32__

struct tagLStatusBar {
    HWND mHwnd;
    void (*mHandler) ();
    ui2 mID;
    LWindow* mParentWindow;

	ui1 mNumParts;
	ui4* mPartMarginArray;
};


/*----------------------------
// Create ()
//--------------------------*/
LStatusBar* LStatusBar_Create (LWindow* inWindow, ui1 inNumParts)
{
    LStatusBar thebar;
    LStatusBar* thebarptr;
	ui4 thewidth, theheight;
	int i;

        thebar.mHwnd=CreateWindowEx (0, STATUSCLASSNAME, NULL, WS_CHILD|WS_VISIBLE, 0, 0, 0, 0,
            inWindow->mHwnd, (HMENU) _LApplication_gGlobal.clientcreate.idFirstChild, _LApplication_gGlobal.inInstance, NULL);
 
	/* check for call failure */
	if (thebar.mHwnd==NULL) Throw(LStatusBar_CANT_CREATE);
		
    _LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
    inWindow->mStatusBarHandle=thebar.mHwnd;
    thebar.mParentWindow=inWindow;

    thebar.mID=LStatusBar_ID;

	/*initializes new fields*/
	thebar.mNumParts=inNumParts;
	thebar.mPartMarginArray=LMemory_Malloc (4*inNumParts);
	/*initializes parts...*/
	LWindow_GetClientAreaSize (inWindow, &thewidth, &theheight);
	for (i=0; i<inNumParts; i++)
		thebar.mPartMarginArray[i]=(i+1)*(thewidth/inNumParts);
	SendMessage (thebar.mHwnd, SB_SETPARTS, (WPARAM) inNumParts, (LPARAM) thebar.mPartMarginArray);

    thebarptr=LMemory_NewObject (LStatusBar, thebar);
    SetWindowLong (thebar.mHwnd, GWL_USERDATA, (LONG) thebarptr);
    return thebarptr;
}

/*--------------------------------------
// Destroy ()
//-------------------------------------*/
void LStatusBar_Destroy (LStatusBar* inBar)
{
    (inBar->mParentWindow)->mStatusBarHandle=NULL;
    DestroyWindow (inBar->mHwnd);

	/*extra cleanup*/
	LMemory_Free (&(inBar->mPartMarginArray));

    LMemory_DeleteObject (&inBar);
}

/*--------------------------------------
// SetPart ()
//-------------------------------------*/
void LStatusBar_SetPart (LStatusBar* inBar, ui1 inPartIndex, ui4 inPartRightMargin)
{
	/*some error handling...*/
	if (inPartIndex>inBar->mNumParts)
		Throw (LStatusBar_OUT_OF_RANGE);

	inBar->mPartMarginArray[inPartIndex]=inPartRightMargin;
}

/*--------------------------------------
// SetText ()
//-------------------------------------*/
void LStatusBar_SetText (LStatusBar* inBar, ui1 inPartIndex, i1* inText)
{
	/*some error handling...*/
	if (inPartIndex>inBar->mNumParts)
		Throw (LStatusBar_OUT_OF_RANGE);

    SendMessage (inBar->mHwnd, SB_SETTEXT, (WPARAM) inPartIndex, (LPARAM) inText);
}

/*------------------------------------
// GetParentWindow ()
//----------------------------------*/
LWindow* LStatusBar_GetParentWindow (LStatusBar* inBar)
{
    return inBar->mParentWindow;
}

void LStatusBar_Refresh (LStatusBar* inBar)
{
	SendMessage (inBar->mHwnd, SB_SETPARTS, (WPARAM) inBar->mNumParts, (LPARAM) inBar->mPartMarginArray);
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

