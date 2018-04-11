/* ============================================================================
 *  LMenu_qt.c
 * ============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Apr 16, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:12:30 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qmainwindow.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qframe.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qmenudata.h>
#include <qevent.h>
#include <qpoint.h>

#include "LMenu.h"
#include "LApplication.h"
#include "LMenu_qt.h"
#include "LApplication_qt.h"
#include "LTextEditor_qt.h"

extern int gMenuiHandler;

extern QLeoTextEdit *_gTextEditorFocused;

/*--------------------------------------------------
// Create
//--------------------------------------------------*/
LMenu* LMenu_Create (i1* inMenuName)
{
	
	LMenu theMenu;
	LMenu *theMenuPtr;
	
	theMenu.mHmenu = new QLeoMenu();
	
#ifdef PRINT_DEBUG
	qDebug( "\n\r theMenu.mHmenu 0x%x", theMenu.mHmenu);
#endif

	/* check for call failure */
	if (theMenu.mHmenu == NULL)
		Throw(LMenu_CANT_CREATE);

	theMenu.mMenuName = inMenuName;

	theMenu.mHmenu->setCheckable( TRUE );

	QObject::connect(theMenu.mHmenu,SIGNAL(aboutToShow()),theMenu.mHmenu,SLOT(slotMenuAboutToShow()));
	QObject::connect(theMenu.mHmenu,SIGNAL(activated(int)),_LApplication_gGlobal.mMainWindow,SLOT(slotMenuPressed(int)));

	theMenuPtr =  (LMenu*)LMemory_NewObject(LMenu, theMenu);

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Menu");
#endif
	
	QMenuDestroyList* theMenuListItemPtr = new QMenuDestroyList();

	theMenuListItemPtr->mMenuListPtr = theMenuPtr;

	_LApplication_gGlobal.mMainWindow->cMenuList.append(theMenuListItemPtr);

	return  theMenuPtr;
}



/*--------------------------------------------------
// AppendMenuItem
//--------------------------------------------------*/
void LMenu_AppendMenuItem (LMenu* inMenu, i1* inItemName, ui4 inItemID, i1* inShortcut, LMenu_THandler inItemHandler, void* inParam)
{

     ui4 theMenuId /*= inItemID*/;

     theMenuId = inMenu->mHmenu->insertItem(inItemName, inItemID/*,_LApplication_gGlobal.mMainWindow,NULL*//*slotMenuPressed*//*,inShortcut, 1*/);

     if (inShortcut != NULL) {
     	inMenu->mHmenu->setAccel(Qt::CTRL + inShortcut[0], theMenuId);
     }


     LMenuHandlerStruct* theMenuHandlerStructPtr;
     LMenuHandlerStruct  theMenuHandlerStruct;

     theMenuHandlerStructPtr = (LMenuHandlerStruct* )LMemory_NewObject(LMenuHandlerStruct,theMenuHandlerStruct);

#ifdef PRINT_DEBUG
	gMenuiHandler++;
	qDebug( "\n\r LMemory_NewObject : MenuHandlerStruct %d   0x%x",gMenuiHandler, theMenuHandlerStructPtr);

#endif


     if (_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct == NULL) {
		_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct = theMenuHandlerStructPtr;
		theMenuHandlerStructPtr->mNext = NULL;
     }
     else {
		theMenuHandlerStructPtr->mNext = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;
		_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct = theMenuHandlerStructPtr;
     }

     theMenuHandlerStructPtr->mMenuId = theMenuId;
     theMenuHandlerStructPtr->mMenuHandler = (LMenu_THandler)inItemHandler;
     theMenuHandlerStructPtr->mMenuHandlerParam = (void*) inParam;


   // return theMenuId;
}

/*--------------------------------------------------
// AppendMenuToMenuBar
//--------------------------------------------------*/
void LMenu_AppendMenuToMenuBar(LMenu* inMenu)
{
	_LApplication_gGlobal.mMainWindow->menuBar()->insertItem(inMenu->mMenuName, inMenu->mHmenu);
}

/*--------------------------------------------------
// ShowMenuBar
//--------------------------------------------------*/
void LMenu_ShowMenuBar ()
{
}

/*--------------------------------------------------
// AppendMenuSeparator
//--------------------------------------------------*/
void LMenu_AppendMenuSeparator(LMenu* inMenu)
{
	inMenu->mHmenu->insertSeparator();
}

/*--------------------------------------------------
// EnableMenuItem
//--------------------------------------------------*/
void LMenu_EnableMenuItem (LMenu* inMenu, ui4 inItemID)
{
#ifdef PRINT_DEBUG
	qDebug("\n\r b LMenu_EnableMenuItem inMenu 0x%x", inMenu);
#endif /* PRINT_DEBUG */
	
	inMenu->mHmenu->setItemEnabled(inItemID, true);
	
#ifdef PRINT_DEBUG
	qDebug("\n\r a LMenu_EnableMenuItem inMenu 0x%x", inMenu);
#endif /* PRINT_DEBUG */

}

/*--------------------------------------------------
// DisableMenuItem
//--------------------------------------------------*/
void LMenu_DisableMenuItem (LMenu* inMenu, ui4 inItemID)
{
#ifdef PRINT_DEBUG
	qDebug("\n\r b LMenu_DisableMenuItem inMenu 0x%x", inMenu);
#endif /* PRINT_DEBUG */
	
	_LApplication_gGlobal.mMainWindow->menuBar()->setItemEnabled(int(inItemID), false);
	
#ifdef PRINT_DEBUG
	qDebug("\n\r a LMenu_DisableMenuItem inMenu 0x%x", inMenu);
#endif /* PRINT_DEBUG */

}

/*--------------------------------------------------
// CheckMenuItem
//--------------------------------------------------*/
void LMenu_CheckMenuItem(LMenu* inMenu, ui4 inItemID, ui2 inCheckedUnchecked)
{

	 inMenu->mHmenu->setItemChecked(int(inItemID), bool(!inCheckedUnchecked));

}

/*-------------------------------------------------
// InstallHandler
//-------------------------------------------------*/
void LMenu_InstallHandler (ui4 inItemID, LMenu_THandler inItemHandler)
{

	
	struct tagMenuHandlerStruct* theMenuHandlerStructPtr;

	theMenuHandlerStructPtr = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;

	while(theMenuHandlerStructPtr != NULL)
		if (theMenuHandlerStructPtr->mMenuId != inItemID)
			theMenuHandlerStructPtr = theMenuHandlerStructPtr->mNext;
		else {
			
			theMenuHandlerStructPtr->mMenuHandler = (LMenu_THandler)inItemHandler;

		}
	

}

/*--------------------------------------------------
// AppendSubMenu
//--------------------------------------------------*/
void LMenu_AppendSubMenu (LMenu* inMenu, LMenu* inSubMenu)
{
 	inMenu->mHmenu->insertItem(inSubMenu->mMenuName, inSubMenu->mHmenu);

}

/*------------------------------------------------
// InstallInitHandler ()
//-----------------------------------------------*/
void LMenu_InstallInitHandler (LMenu* inMenu, LMenu_THandler inInitHandler, void* inParam)
{

	inMenu->mHmenu->cMenuInitHandler = inInitHandler;

#ifdef PRINT_DEBUG
	qDebug( "\n\r cMenuInitHandler 0x%x", inMenu->mHmenu->cMenuInitHandler);
#endif


}

/*------------------------------------------------
// RedrawMenuBar ()
//------------------------------------------------*/
void LMenu_RedrawMenuBar ()
{

}

/*------------------------------------------------
// InsertMenuInMenuBar ()
//----------------------------------------------*/
void LMenu_InsertMenuInMenuBar (LMenu* inMenu, ui4 inPos)
{
	_LApplication_gGlobal.mMainWindow->menuBar()->insertItem(inMenu->mMenuName, inMenu->mHmenu, -1, (int)inPos);
}

/*----------------------------------------------
// RemoveMenuFromMenuBar ()
//---------------------------------------------*/
void LMenu_RemoveMenuFromMenuBar (ui4 inPos)
{

	_LApplication_gGlobal.mMainWindow->menuBar()->removeItemAt( int(inPos) );

}

/*---------------------------------------------
// Destroy ()
//-------------------------------------------*/
void LMenu_Destroy (LMenu* inMenu)
{
	delete inMenu->mHmenu;
	LMemory_DeleteObject(&inMenu);
	/* Eliminare il menu dalla lista dei menu */
}

/*-------------------------------------------
// InsertMenuItem ()
//------------------------------------------*/
void LMenu_InsertMenuItem (LMenu* inMenu, ui4 inPos, i1* inItemName, ui4 inItemID, i1* inShortcut, LMenu_THandler inItemHandler, void* inParam)
{
	ui4 theMenuId /*= inItemID*/;

     theMenuId = inMenu->mHmenu->insertItem(inItemName, inItemID, (int)inPos /*,_LApplication_gGlobal.mMainWindow,NULL*//*slotMenuPressed*//*,inShortcut, 1*/);

     if (inShortcut != NULL) {
     	inMenu->mHmenu->setAccel(Qt::CTRL + inShortcut[0], theMenuId);
     }


     LMenuHandlerStruct* theMenuHandlerStructPtr;
     LMenuHandlerStruct  theMenuHandlerStruct;

     theMenuHandlerStructPtr = (LMenuHandlerStruct* )LMemory_NewObject(LMenuHandlerStruct,theMenuHandlerStruct);

#ifdef PRINT_DEBUG
	gMenuiHandler++;
	qDebug( "\n\r LMemory_NewObject : MenuHandlerStruct %d   0x%x",gMenuiHandler, theMenuHandlerStructPtr);

#endif


     if (_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct == NULL) {
		_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct = theMenuHandlerStructPtr;
		theMenuHandlerStructPtr->mNext = NULL;
     }
     else {
		theMenuHandlerStructPtr->mNext = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;
		_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct = theMenuHandlerStructPtr;
     }

     theMenuHandlerStructPtr->mMenuId = theMenuId;
     theMenuHandlerStructPtr->mMenuHandler = (LMenu_THandler)inItemHandler;
     theMenuHandlerStructPtr->mMenuHandlerParam = (void*) inParam;
}

/*-------------------------------------------
// RemoveMenuItemByID ()
//------------------------------------------*/
void LMenu_RemoveMenuItemByID (LMenu* inMenu, ui4 inItemID)
{
	inMenu->mHmenu->removeItem( int(inItemID) );
//#ifndef PROVA
	LMenuHandlerStruct  *theMenuHandlerStructPtr;
	LMenuHandlerStruct  *theOldMenuHandlerStructPtr;
#ifdef PRINT_DEBUG
	qDebug( "\n\r LMenu_RemoveMenuItemByID");

#endif
	theMenuHandlerStructPtr = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;
	theOldMenuHandlerStructPtr = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;

	if (theMenuHandlerStructPtr != NULL)
		if (theMenuHandlerStructPtr->mMenuId == inItemID ) {

			_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct = theMenuHandlerStructPtr->mNext;
			LMemory_DeleteObject (&theMenuHandlerStructPtr);

		}
		else {
			while(theMenuHandlerStructPtr != NULL)
				if (theMenuHandlerStructPtr->mMenuId != inItemID ) {
#ifdef PRINT_DEBUG
	qDebug( "\n\r theMenuHandlerStructPtr->mMenuId != inItemID  %d", theMenuHandlerStructPtr->mMenuId);

#endif
					theOldMenuHandlerStructPtr = theMenuHandlerStructPtr;

					theMenuHandlerStructPtr = theMenuHandlerStructPtr->mNext;
				}
				else {
#ifdef PRINT_DEBUG
	qDebug( "\n\r theMenuHandlerStructPtr->mNext %p",  theMenuHandlerStructPtr->mNext);

#endif
					theOldMenuHandlerStructPtr->mNext = theMenuHandlerStructPtr->mNext;

					LMemory_DeleteObject (&theMenuHandlerStructPtr);
					theMenuHandlerStructPtr = NULL;
				}


		}

//#endif

}

/*------------------------------------------
// RemoveMenuItemByPos ()
//-----------------------------------------*/
void LMenu_RemoveMenuItemByPos (LMenu* inMenu, ui4 inPos)
{
	inMenu->mHmenu->removeItemAt( int(inPos) );
	
	LMenuHandlerStruct  *theMenuHandlerStructPtr;
	LMenuHandlerStruct  *theOldMenuHandlerStructPtr;
#ifdef PRINT_DEBUG
	qDebug( "\n\r RemoveMenuItemByPos");

#endif

#ifdef PRINT_DEBUG
	
	
	theMenuHandlerStructPtr = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;
	theOldMenuHandlerStructPtr = _LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct;

	int theItemID = inMenu->mHmenu->idAt( int(inPos) );
	
	if (theMenuHandlerStructPtr != NULL)
		if (theMenuHandlerStructPtr->mMenuId == theItemID ) {

			_LApplication_gGlobal.mMainWindow->cFirstMenuHandlerStruct = theMenuHandlerStructPtr->mNext;
			LMemory_DeleteObject (&theMenuHandlerStructPtr);

		}
		else {
			while(theMenuHandlerStructPtr != NULL)
				if (theMenuHandlerStructPtr->mMenuId != theItemID ) {
#ifdef PRINT_DEBUG
	qDebug( "\n\r theMenuHandlerStructPtr->mMenuId != theItemID");

#endif
					theOldMenuHandlerStructPtr = theMenuHandlerStructPtr;

					theMenuHandlerStructPtr = theMenuHandlerStructPtr->mNext;
				}
				else {
#ifdef PRINT_DEBUG
	qDebug( "\n\r theMenuHandlerStructPtr->mNext %p",  theMenuHandlerStructPtr->mNext);

#endif
					theOldMenuHandlerStructPtr->mNext = theMenuHandlerStructPtr->mNext;

					LMemory_DeleteObject (&theMenuHandlerStructPtr);
					theMenuHandlerStructPtr = NULL;
				}


		}
#endif
}

/*----------------------------------------
// InsertSubMenu ()
//---------------------------------------*/
void LMenu_InsertSubMenu (LMenu* inMenu, LMenu* inSubMenu, ui4 inPos)
{
	inMenu->mHmenu->insertItem(inSubMenu->mMenuName, inSubMenu->mHmenu, -1, (int)inPos);
}

/*----------------------------------------
// InsertMenuSeparator ()
//---------------------------------------*/
void LMenu_InsertMenuSeparator (LMenu* inMenu, ui4 inPos)
{
	inMenu->mHmenu->insertSeparator((int)inPos);
}

/*-------------------------------
// HideMenuBar ()
//-----------------------------*/
void LMenu_HideMenuBar ()
{
	_LApplication_gGlobal.mMainWindow->menuBar()->hide();

}

/*--------------------------------------------
// DisplayContextualMenu ()
//-------------------------------------------*/
void LMenu_DisplayContextualMenu (LMenu* inMenu, ui4 inX, ui4 inY)
{
	QPoint thePoint;

#ifdef PRINT_DEBUG

	qDebug( "\n\r LMenu_DisplayContextualMenu : 0x%x", inMenu);

#endif

	
	thePoint.setX(int(inX));
	thePoint.setY(int(inY));

	inMenu->mHmenu->popup(thePoint, -1);
}

/*------------------------------------------------
// QLeoMenu::QLeoMenu ()
//-----------------------------------------------*/
QLeoMenu::QLeoMenu() : QPopupMenu()
{
	cMenuInitHandler = NULL;
}

/*------------------------------------------------
// QLeoMenu::~QLeoMenu ()
//-----------------------------------------------*/
QLeoMenu::~QLeoMenu()
{

}

/*------------------------------------------------
// QLeoMenu::slotMenuPressed ()
//-----------------------------------------------*/
void QLeoMenu::slotMenuPressed(int inId)
{



}

/*------------------------------------------------
// QLeoMenu::slotMenuAboutToShow ()
//-----------------------------------------------*/
void QLeoMenu::slotMenuAboutToShow()
{

#ifdef PRINT_DEBUG
	qDebug( "\n\r slotMenuAboutToShow 0x%x", cMenuInitHandler);
#endif
#ifdef PRINT_DEBUG
	qDebug( "\n\r QLeoMenu : _LApplication_gGlobal.mApplication->focusWidget() =  0x%x",  _LApplication_gGlobal.mApplication->focusWidget());

#endif

#ifdef PROVA
	if( _LApplication_gGlobal.mApplication->focusWidget() != NULL)
		_gTextEditorFocused = (QLeoTextEdit *)_LApplication_gGlobal.mApplication->focusWidget();
	else
		_gTextEditorFocused = NULL;
#endif

	if (cMenuInitHandler != NULL)
		cMenuInitHandler(NULL); /* ??? */

#ifdef PRINT_DEBUG
	qDebug( "\n\r cMenuInitHandler  0x%x",  cMenuInitHandler);

#endif

}


#endif	/* __LSL_OS_GUI__ == __LSL_QT__ */



/* Copyright (C) 2002 Gianni Bevacqua

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





