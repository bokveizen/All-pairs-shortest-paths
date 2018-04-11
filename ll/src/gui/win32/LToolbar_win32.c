/* =============================================================================
 *  LToolbar_win32.c
 * =============================================================================

 *  Author:         (c) 2003 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Apr 28, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/20 16:00:08 $
 *  Changed by:     $Author: ribbi $
 *  Revision:       $Revision: 1.2 $
*/

#include "LApplication.h"
#include "LToolbar.h"
#include "LToolbar_win32.h"
#include "LBitmap_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*--------------------------------------------
// Create ()
//------------------------------------------*/
LToolbar* LToolbar_Create (ui2 inNumButtons, LBitmap* inBitmap, LToolbar_TButton* inButtonArray)
{
	LToolbar thebar;
	LToolbar* thebarptr;
	LPTBBUTTON thebuttonarray;
	ui2 i;
	ui4 thedx, thedy;
	LPARAM thelparam;

	HBITMAP thehbitmap;
	HDC thehdcsrc, thehdcdest, thehdc;
	HGDIOBJ thebackup0, thebackup1;

	LToolbar_TButton* theTButton;

	/*allocates TButton array*/
	theTButton=(LToolbar_TButton*) LMemory_Malloc (sizeof(LToolbar_TButton)*inNumButtons);
	/*duplicates inButtonArray...*/
	for (i=0; i<inNumButtons; i++)
	{
		theTButton[i]=inButtonArray[i];
	}
	/*copies the pointer into LToolbar struct*/
	thebar.mTButtonArray=theTButton;

	/*allocates button array*/
	thebuttonarray=(LPTBBUTTON) LMemory_Malloc (sizeof(TBBUTTON)*inNumButtons);
	/*fills in the array*/
	for (i=0; i<inNumButtons; i++)
	{
		thebuttonarray[i].iBitmap=(int) (inButtonArray[i].mBitmapIndex);
		thebuttonarray[i].idCommand=inButtonArray[i].mItemID;
		thebuttonarray[i].iString=0;

		if (inButtonArray[i].mStyle==LToolbar_BUTTON)
			thebuttonarray[i].fsStyle=TBSTYLE_BUTTON;
		if (inButtonArray[i].mStyle==LToolbar_SEPARATOR)
			thebuttonarray[i].fsStyle=TBSTYLE_SEP;

		if (inButtonArray[i].mState==LToolbar_ENABLED)
			thebuttonarray[i].fsState=TBSTATE_ENABLED;
		if (inButtonArray[i].mState==LToolbar_DISABLED)
			thebuttonarray[i].fsState=TBSTATE_INDETERMINATE;

	}

	/*retrieves display's own dc*/
	thehdc=GetDC (NULL);
	/*creates two memory dc's*/
	thehdcsrc=CreateCompatibleDC (NULL);
	thehdcdest=CreateCompatibleDC (NULL);
	/*creates a display compatible bitmap...*/
	thehbitmap=CreateCompatibleBitmap (thehdc, inBitmap->mDX, inBitmap->mDY);
	/*selects the bitmaps*/
	thebackup0=SelectObject (thehdcsrc, inBitmap->mHbitmap);
	thebackup1=SelectObject (thehdcdest, thehbitmap);
	/*blits src onto dest...*/
	BitBlt (thehdcdest, 0, 0, inBitmap->mDX, inBitmap->mDY, thehdcsrc, 0, 0, SRCCOPY);
	/*releases the bitmaps...*/
	SelectObject (thehdcsrc, thebackup0);
	SelectObject (thehdcdest, thebackup1);
	/*deletes the mem dc's*/
	DeleteDC (thehdcsrc);
	DeleteDC (thehdcdest);
	/*releases the common dc*/
	ReleaseDC (NULL, thehdc);

	thebar.mHWND=CreateToolbarEx (_LApplication_gGlobal.hwndFrame, TBSTYLE_TOOLTIPS|WS_BORDER|WS_CHILDWINDOW|WS_VISIBLE|WS_CLIPSIBLINGS,
					_LApplication_gGlobal.clientcreate.idFirstChild, (int)inNumButtons,
					NULL, (UINT)thehbitmap, thebuttonarray, (int)inNumButtons, 0, 0,
					16, 16, sizeof (TBBUTTON));

	

	thebar.mBitmap=thehbitmap;

	SetParent (thebar.mHWND, _LApplication_gGlobal.hwndFrame);

	_LApplication_gGlobal.clientcreate.idFirstChild=_LApplication_gGlobal.clientcreate.idFirstChild+1;
	
	/*deletes button array*/
	LMemory_Free (&thebuttonarray);

	/* check for call failure */
	if (thebar.mHWND==NULL) Throw(LWindow_CANT_CREATE);

	thebarptr=LMemory_NewObject (LToolbar, thebar);

	_LApplication_gGlobal.Toolbar=(void*) thebarptr;

	LApplication_GetWorkspaceSize (&thedx, &thedy);
	thelparam=MAKELPARAM( (ui2)thedx, (ui2)thedy);
	SendMessage (_LApplication_gGlobal.hwndFrame, WM_SIZE, SIZE_RESTORED, (LPARAM)thelparam);

	return thebarptr;
}

/*-------------------------------------------
// Destroy ()
//-------------------------------------------*/
void LToolbar_Destroy (LToolbar* inToolbar)
{
	ui4 thedx, thedy;
	LPARAM thelparam;

	if (inToolbar==(LToolbar *)_LApplication_gGlobal.Toolbar)
		_LApplication_gGlobal.Toolbar=NULL;

	DestroyWindow (inToolbar->mHWND);
	DeleteObject (inToolbar->mBitmap);
	LMemory_Free (&(inToolbar->mTButtonArray));
	LMemory_DeleteObject (&inToolbar);

	LApplication_GetWorkspaceSize (&thedx, &thedy);
	thelparam=MAKELPARAM( (ui2)thedx, (ui2)thedy);
	SendMessage (_LApplication_gGlobal.hwndFrame, WM_SIZE, SIZE_RESTORED, (LPARAM)thelparam);
}

/*--------------------------------------------
// SetButtonState ()
//------------------------------------------*/
void LToolbar_SetButtonState (LToolbar* inToolbar, ui4 inItemID, Bool inEnabled)
{
	if (inEnabled)
	{
		SendMessage (inToolbar->mHWND, TB_INDETERMINATE, (WPARAM) inItemID, (LPARAM) FALSE);
		SendMessage (inToolbar->mHWND, TB_ENABLEBUTTON, (WPARAM) inItemID, (LPARAM) TRUE); 
	}
	else
	{
		SendMessage (inToolbar->mHWND, TB_ENABLEBUTTON, (WPARAM) inItemID, (LPARAM) FALSE);
		SendMessage (inToolbar->mHWND, TB_INDETERMINATE, (WPARAM) inItemID, (LPARAM) TRUE);
	}
}

#endif

/* Copyright (C) 2003 Andrea Ribichini

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

