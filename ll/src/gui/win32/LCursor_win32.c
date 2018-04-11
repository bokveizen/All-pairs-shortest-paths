/* =============================================================================
 *  LCursor_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jul 24, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LCursor.h"
/*#include "LWindow_win32.h"*/

#if __LSL_OS_GUI__ == __LSL_Win32__

/*struct tagLStaticText {
    HWND mHwnd;
    LWindow* mParentWindow;
};*/

/*-----------------------------
// SetShape ()
//----------------------------*/
/*void LCursor_SetShape (LCursor_TShape inShape)
{
    HCURSOR thehc;

    switch (inShape)
    {
    case LCursor_APPSTARTING:
        thehc=LoadCursor (NULL, IDC_APPSTARTING);
        break;
    case LCursor_ARROW:
        thehc=LoadCursor (NULL, IDC_ARROW);
        break;
    case LCursor_CROSS:
        thehc=LoadCursor (NULL, IDC_CROSS);
        break;
    case LCursor_IBEAM:
        thehc=LoadCursor (NULL, IDC_IBEAM);
        break;
    case LCursor_NO:
        thehc=LoadCursor (NULL, IDC_NO);
        break;
    case LCursor_SIZEALL:
        thehc=LoadCursor (NULL, IDC_SIZEALL);
        break;
    case LCursor_SIZENESW:
        thehc=LoadCursor (NULL, IDC_SIZENESW);
        break;
    case LCursor_SIZENS:
        thehc=LoadCursor (NULL, IDC_SIZENS);
        break;
    case LCursor_SIZENWSE:
        thehc=LoadCursor (NULL, IDC_SIZENWSE);
        break;
    case LCursor_SIZEWE:
        thehc=LoadCursor (NULL, IDC_SIZEWE);
        break;
    case LCursor_UPARROW:
        thehc=LoadCursor (NULL, IDC_UPARROW);
        break;
    case LCursor_WAIT:
        thehc=LoadCursor (NULL, IDC_WAIT);
        break;
    }

    SetCursor (thehc);
}*/

/*----------------------------------------
// GetPosition ()
//---------------------------------------*/
void LCursor_GetPosition (ui4 *outX, ui4 *outY)
{
	POINT thept;

	GetCursorPos (&thept);

	*outX=thept.x;
	*outY=thept.y;
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

