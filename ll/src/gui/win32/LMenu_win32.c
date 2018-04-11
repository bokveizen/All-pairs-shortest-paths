/* ============================================================================
 *  LMenu_win32.c
 * ============================================================================

 *  Author:         (c) 2001 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Dec 20, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/


#include "LMenu.h"
#include "LArray.h"
#include "LWindow.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

#include "LMenu_win32.h"

/*GLOBAL FUNCTIONS*/
void _addaccels (LMenu* inMenu);

/*--------------------------------------------------
// Create
//--------------------------------------------------*/
LMenu* LMenu_Create (i1* inMenuName)
{
    LMenu theaux;
    LMenu* theauxptr;
	LApplication_TMenuRef theref;

    theaux.mHmenu=CreatePopupMenu ();

	/* check for call failure */
	if (theaux.mHmenu==NULL) Throw(LMenu_CANT_CREATE);

    theaux.mMenuName=inMenuName;
	theaux.mInitHandler=NULL;

	/*creates the accelerators larray...*/
	theaux.mAccelerators=LArray_New (sizeof (LMenuItem));

    theauxptr=LMemory_NewObject (LMenu, theaux);
    
	/*providing mutual reference...*/
	theref.hMenuHandle=theauxptr->mHmenu;
	theref.mMenu=theauxptr;
	LArray_AppendItem (_LApplication_gGlobal.MenuRef, &theref);

    return theauxptr;
}

/*--------------------------------------------------
// AppendMenuItem
//--------------------------------------------------*/
void LMenu_AppendMenuItem (LMenu* inMenu, i1* inItemName, ui4 inItemID, i1* inShortcut, LMenu_THandler inItemHandler, void* inParam)
{
    i1 thebuffer[1024];
	LMenuItem theitem;

    LString_Copy (inItemName, thebuffer);
    if (inShortcut!=NULL)
    {
        LString_Append ("\tCtrl+", thebuffer);
        LString_Append (inShortcut, thebuffer);
    }

	AppendMenu (inMenu->mHmenu, MF_STRING, inItemID, thebuffer);
	_LApplication_gGlobal.MenuItemHandler[inItemID]=inItemHandler;
	_LApplication_gGlobal.MenuItemParam[inItemID]=inParam;

    if (inShortcut!=NULL) /*a menu shortcut has been specified and will have to be installed*/
    {
		theitem.mItemID=inItemID;
		theitem.mShortCut=(ui2) *inShortcut;
		theitem.mSubMenu=NULL;

		LArray_AppendItem (inMenu->mAccelerators, &theitem);
    }
}

/*--------------------------------------------------
// AppendSubMenu
//--------------------------------------------------*/
void LMenu_AppendSubMenu (LMenu* inMenu, LMenu* inSubMenu)
{
	LMenuItem theitem;

    AppendMenu(inMenu->mHmenu, MF_STRING | MF_POPUP,
             (UINT) inSubMenu->mHmenu, inSubMenu->mMenuName);

	theitem.mSubMenu=inSubMenu;

	LArray_AppendItem (inMenu->mAccelerators, &theitem);
}

/*--------------------------------------------------
// AppendMenuToMenuBar
//--------------------------------------------------*/
void LMenu_AppendMenuToMenuBar (LMenu* inMenu)
{
    AppendMenu (_LApplication_gGlobal.MenuBar, MF_STRING|MF_POPUP,
        (UINT) inMenu->mHmenu, inMenu->mMenuName);
}

/*--------------------------------------------------
// ShowMenuBar
//--------------------------------------------------*/
void LMenu_ShowMenuBar ()
{
	int themenucount;
	int i;
	HMENU themenu;
	ui4 thecount;
	ui4 j;
	LApplication_TMenuRef theref;
	//HACCEL thehaccel;

    SetMenu(_LApplication_gGlobal.hwndFrame, _LApplication_gGlobal.MenuBar);

	/*we should build the accelerator table here*/

	/*no entries yet...*/
	_LApplication_gGlobal.MenuShortcutIndex=0;

	/*number of items in menubar (all menus)*/
	themenucount=GetMenuItemCount (_LApplication_gGlobal.MenuBar);

	/*for all menus*/
	for (i=0; i<themenucount; i++)
	{
		/*get the handle*/
		themenu=GetSubMenu (_LApplication_gGlobal.MenuBar, i);
		/*finds the corresponding LMenu*...*/
		thecount=LArray_GetItemsCount (_LApplication_gGlobal.MenuRef);
		for (j=0; j<thecount; j++)
		{
			LArray_FetchItemAt (_LApplication_gGlobal.MenuRef, j, &theref);
			if (theref.hMenuHandle==themenu) /*found it!*/
			{
				/*adds its accels to global array...*/
				_addaccels (theref.mMenu);
			}
		}
	}

	/*creates the new accel table*/
	if (_LApplication_gGlobal.MenuShortcutIndex!=0) /*at least one accelerator...*/
        _LApplication_gGlobal.haccel=CreateAcceleratorTable ((LPACCEL) _LApplication_gGlobal.accel, (int) _LApplication_gGlobal.MenuShortcutIndex);

}

/*--------------------------------------------------
// AppendMenuSeparator
//--------------------------------------------------*/
void LMenu_AppendMenuSeparator (LMenu* inMenu)
{
    AppendMenu(inMenu->mHmenu, MF_SEPARATOR, 0, NULL);
}

/*--------------------------------------------------
// EnableMenuItem
//--------------------------------------------------*/
void LMenu_EnableMenuItem (LMenu* inMenu, ui4 inItemID)
{
    EnableMenuItem (inMenu->mHmenu, inItemID, MF_ENABLED);
}

/*--------------------------------------------------
// DisableMenuItem
//--------------------------------------------------*/
void LMenu_DisableMenuItem (LMenu* inMenu, ui4 inItemID)
{
    EnableMenuItem (inMenu->mHmenu, inItemID, MF_GRAYED);
}

/*--------------------------------------------------
// CheckMenuItem
//--------------------------------------------------*/
void LMenu_CheckMenuItem (LMenu* inMenu, ui4 inItemID, ui2 inCheckedUnchecked)
{
    UINT theCheck;

    if (inCheckedUnchecked==LMenu_CHECKED)
        theCheck=MF_CHECKED;
    else
        theCheck=MF_UNCHECKED;

    CheckMenuItem(inMenu->mHmenu, inItemID, theCheck);
}

/*-------------------------------------------------
// InstallHandler
//-------------------------------------------------*/
void LMenu_InstallHandler (ui4 inItemID, LMenu_THandler inItemHandler, void* inParam)
{
    _LApplication_gGlobal.MenuItemHandler[inItemID]=inItemHandler;
	_LApplication_gGlobal.MenuItemParam[inItemID]=inParam;
}

/*------------------------------------------------
// InstallInitHandler ()
//-----------------------------------------------*/
void LMenu_InstallInitHandler (LMenu* inMenu, LMenu_THandler inInitHandler, void* inParam)
{
    inMenu->mInitHandler=inInitHandler;
	inMenu->mParam=inParam;
}

/*------------------------------------------------
// RedrawMenuBar ()
//------------------------------------------------*/
void LMenu_RedrawMenuBar ()
{
	int themenucount;
	int i;
	HMENU themenu;
	ui4 thecount;
	ui4 j;
	LApplication_TMenuRef theref;
	HACCEL thehaccel;

	/*redraws the menubar to reflect changes...*/
	DrawMenuBar (_LApplication_gGlobal.hwndFrame);

	/*rebuilds accelerator table*/
	
	/*no entries yet...*/
	_LApplication_gGlobal.MenuShortcutIndex=0;

	/*number of items in menubar (all menus)*/
	themenucount=GetMenuItemCount (_LApplication_gGlobal.MenuBar);

	/*for all menus*/
	for (i=0; i<themenucount; i++)
	{
		/*get the handle*/
		themenu=GetSubMenu (_LApplication_gGlobal.MenuBar, i);
		/*finds the corresponding LMenu*...*/
		thecount=LArray_GetItemsCount (_LApplication_gGlobal.MenuRef);
		for (j=0; j<thecount; j++)
		{
			LArray_FetchItemAt (_LApplication_gGlobal.MenuRef, j, &theref);
			if (theref.hMenuHandle==themenu) /*found it!*/
			{
				/*adds its accels to global array...*/
				_addaccels (theref.mMenu);
			}
		}
	}

	/*creates the new accel table and replaces it*/
	thehaccel=_LApplication_gGlobal.haccel; /*saves the old one...*/
	if (_LApplication_gGlobal.MenuShortcutIndex!=0) /*at least one accelerator...*/
        _LApplication_gGlobal.haccel=CreateAcceleratorTable ((LPACCEL) _LApplication_gGlobal.accel, (int) _LApplication_gGlobal.MenuShortcutIndex);
	else
		_LApplication_gGlobal.haccel=NULL; /*no accelerators have been defined*/ 

	/*destroys the old (if present)*/
	if (thehaccel!=NULL)
		DestroyAcceleratorTable (thehaccel);
}

/*------------------------------------------------
// InsertMenuInMenuBar ()
//----------------------------------------------*/
void LMenu_InsertMenuInMenuBar (LMenu* inMenu, ui4 inPos)
{
	LWindow* thew;
	ui4 thepos;

	thew=LWindow_GetFrontWindow ();
	if (thew==NULL)
		thepos=inPos;
	else
		if ((LWindow_GetSizeStatus (thew))==LWindow_MAXIMIZED)
			thepos=inPos+1;
		else
			thepos=inPos;

	InsertMenu (_LApplication_gGlobal.MenuBar, thepos, MF_BYPOSITION|MF_POPUP|MF_STRING,
		(UINT) inMenu->mHmenu, inMenu->mMenuName);
}

/*----------------------------------------------
// RemoveMenuFromMenuBar ()
//---------------------------------------------*/
void LMenu_RemoveMenuFromMenuBar (ui4 inPos)
{
	LWindow* thew;
	ui4 thepos;

	thew=LWindow_GetFrontWindow ();
	if (thew==NULL)
		thepos=inPos;
	else
		if ((LWindow_GetSizeStatus (thew))==LWindow_MAXIMIZED)
			thepos=inPos+1;
		else
			thepos=inPos;

	RemoveMenu (_LApplication_gGlobal.MenuBar, thepos, MF_BYPOSITION);
}

/*---------------------------------------------
// Destroy ()
//-------------------------------------------*/
void LMenu_Destroy (LMenu* inMenu)
{
	ui4 i;
	LApplication_TMenuRef theref;
	ui4 thecount;

	/*gets number of items in mutual reference table*/
	thecount=LArray_GetItemsCount (_LApplication_gGlobal.MenuRef);

	/*removes menu from mutual reference table and deletes it...*/
	for (i=0; i<thecount; i++)
	{
		LArray_FetchItemAt (_LApplication_gGlobal.MenuRef, i, &theref);
		if (theref.mMenu==inMenu)
		{
			LArray_RemoveItemAt (_LApplication_gGlobal.MenuRef, i);
			DestroyMenu (inMenu->mHmenu);
			LArray_Delete (&(inMenu->mAccelerators));
			LMemory_DeleteObject (&inMenu);
			return;
		}
	}
}

/*-------------------------------------------
// InsertMenuItem ()
//------------------------------------------*/
void LMenu_InsertMenuItem (LMenu* inMenu, ui4 inPos, i1* inItemName, ui4 inItemID, i1* inShortcut, LMenu_THandler inItemHandler, void* inParam)
{
	i1 thebuffer[1024];
	LMenuItem theitem;

    LString_Copy (inItemName, thebuffer);
    if (inShortcut!=NULL)
    {
        LString_Append ("\tCtrl+", thebuffer);
        LString_Append (inShortcut, thebuffer);
    }

	InsertMenu (inMenu->mHmenu, inPos, MF_BYPOSITION|MF_STRING, inItemID, thebuffer);
	_LApplication_gGlobal.MenuItemHandler[inItemID]=inItemHandler;
	_LApplication_gGlobal.MenuItemParam[inItemID]=inParam;

    if (inShortcut!=NULL) /*a menu shortcut has been specified and will have to be installed*/
    {
		theitem.mItemID=inItemID;
		theitem.mShortCut=(ui2) *inShortcut;
		theitem.mSubMenu=NULL;

		LArray_AppendItem (inMenu->mAccelerators, &theitem);
	}
}

/*-------------------------------------------
// RemoveMenuItemByID ()
//------------------------------------------*/
void LMenu_RemoveMenuItemByID (LMenu* inMenu, ui4 inItemID)
{
	ui4 thecount;
	int i;
	LMenuItem theitem;

	RemoveMenu (inMenu->mHmenu, inItemID, MF_BYCOMMAND);  

	/*we should now remove item from accel larray (if present)*/
	thecount=LArray_GetItemsCount (inMenu->mAccelerators);
	for (i=0; i<(int)thecount; i++)
	{
		LArray_FetchItemAt (inMenu->mAccelerators, i, &theitem);
		if ((theitem.mItemID==inItemID)&&(theitem.mSubMenu==NULL))
		{
			LArray_RemoveItemAt (inMenu->mAccelerators, i);
			return;
		}
	}
}

/*------------------------------------------
// RemoveMenuItemByPos ()
//-----------------------------------------*/
void LMenu_RemoveMenuItemByPos (LMenu* inMenu, ui4 inPos)
{
	MENUITEMINFO theinfo;
	ui4 thecount;
	int i;
	LMenuItem theitem;

	/*first we should determine whether it's an item, a sep or a submenu*/

	/*initializes the struct*/
	theinfo.cbSize=sizeof (MENUITEMINFO);
	theinfo.fMask=MIIM_TYPE|MIIM_SUBMENU|MIIM_ID;

	/*retrieves the information*/
	GetMenuItemInfo (inMenu->mHmenu, inPos, TRUE, &theinfo);
	
	/*then we go on removing it...*/

	/*if separator, just remove it*/
	if (theinfo.fType==MFT_SEPARATOR)
	{
		RemoveMenu (inMenu->mHmenu, inPos, MF_BYPOSITION);
		return;
	}
	/*if menuitem, we should remove shortcut entry, if present*/
	if (theinfo.fType==MFT_STRING)
	{
		LMenu_RemoveMenuItemByID (inMenu, theinfo.wID); /*why not?*/
		return;
	}
	/*if submenu, we should remove its entry*/
	if (theinfo.hSubMenu!=NULL)
	{
		thecount=LArray_GetItemsCount (inMenu->mAccelerators);
		for (i=0; i<(int)thecount; i++)
		{
		LArray_FetchItemAt (inMenu->mAccelerators, i, &theitem);
		if (theitem.mSubMenu==theinfo.hSubMenu)
			{
				LArray_RemoveItemAt (inMenu->mAccelerators, i);
				RemoveMenu (inMenu->mHmenu, inPos, MF_BYPOSITION);
				return;
			}
		}
	}
}

/*----------------------------------------
// InsertSubMenu ()
//---------------------------------------*/
void LMenu_InsertSubMenu (LMenu* inMenu, LMenu* inSubMenu, ui4 inPos)
{
	LMenuItem theitem;

	InsertMenu (inMenu->mHmenu, inPos, MF_BYPOSITION|MF_POPUP, (UINT) inSubMenu->mHmenu,
		inSubMenu->mMenuName);

	theitem.mSubMenu=inSubMenu;

	LArray_AppendItem (inMenu->mAccelerators, &theitem);
}

/*----------------------------------------
// InsertMenuSeparator ()
//---------------------------------------*/
void LMenu_InsertMenuSeparator (LMenu* inMenu, ui4 inPos)
{
	InsertMenu (inMenu->mHmenu, inPos, MF_BYPOSITION|MF_SEPARATOR, 0, NULL);
}

/*-------------------------------
// HideMenuBar ()
//-----------------------------*/
void LMenu_HideMenuBar ()
{
	HACCEL thehaccel;

	/*we should remove accelerators here...*/
	thehaccel=_LApplication_gGlobal.haccel;
	if (thehaccel!=NULL)
	{
		_LApplication_gGlobal.haccel=NULL;
		DestroyAcceleratorTable (thehaccel);
	}

	/*actually hides the menubar*/
	SetMenu(_LApplication_gGlobal.hwndFrame, NULL);

}

/*--------------------------------------------
// DisplayContextualMenu ()
//-------------------------------------------*/
void LMenu_DisplayContextualMenu (LMenu* inMenu, ui4 inX, ui4 inY)
{
	TrackPopupMenu (inMenu->mHmenu, TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON, 
		inX, inY, 0, _LApplication_gGlobal.hwndFrame, NULL);
}

/*----------------------------------------
// _addaccels ()
//--------------------------------------*/
void _addaccels (LMenu* inMenu)
{
	ui4 thecount;
	ui4 i;
	LMenuItem theitem;

	thecount=LArray_GetItemsCount (inMenu->mAccelerators);

	for (i=0; i<thecount; i++)
	{
		LArray_FetchItemAt (inMenu->mAccelerators, i, &theitem);
		if (theitem.mSubMenu!=NULL) /*submenu!*/
			_addaccels (theitem.mSubMenu); /*recursive call!*/
		else /*add accelerator to array*/
		{
			_LApplication_gGlobal.accel[_LApplication_gGlobal.MenuShortcutIndex].fVirt=FCONTROL|FVIRTKEY;
			_LApplication_gGlobal.accel[_LApplication_gGlobal.MenuShortcutIndex].key=(WORD) theitem.mShortCut;
			_LApplication_gGlobal.accel[_LApplication_gGlobal.MenuShortcutIndex].cmd=(WORD) theitem.mItemID;
			_LApplication_gGlobal.MenuShortcutIndex=_LApplication_gGlobal.MenuShortcutIndex+1;
		}
	}
}

#endif

/* Copyright (C) 2001 Andrea Ribichini

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