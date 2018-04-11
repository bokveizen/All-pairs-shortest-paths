/* =============================================================================
 *  LCursor_qt.c
 * =============================================================================

 *  Author:         (c) Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Aug 13, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/21 19:07:41 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qcursor.h>
#include <qapplication.h>
#include <qpoint.h>

#include "LCursor.h"
#include "LCursor_qt.h"

/*-----------------------------
// _LCursor_GetShape ()
//----------------------------*/
int _LCursor_GetShape( LCursor_TShape inCursorShape )
{
	int theShape;

	switch ( inCursorShape ) {

		case LCursor_APPSTARTING:
			theShape = Qt::WaitCursor;
		break;

		case LCursor_ARROW:
			theShape = Qt::ArrowCursor;
		break;

		case LCursor_CROSS:
			theShape = Qt::CrossCursor;
		break;

		case LCursor_IBEAM:
			theShape = Qt::IbeamCursor;
		break;

		case LCursor_NO:
			theShape = Qt::BlankCursor;
		break;

		case LCursor_SIZEALL:
			theShape = Qt::SizeAllCursor;
		break;

		case LCursor_SIZENESW:
			theShape = Qt::SizeBDiagCursor;
		break;

		case LCursor_SIZENS:
			theShape = Qt::SizeVerCursor;
		break;

		case LCursor_SIZENWSE:
			theShape = Qt::SizeFDiagCursor;
		break;

		case LCursor_SIZEWE:
			theShape = Qt::SizeHorCursor;
		break;

		case LCursor_UPARROW:
			theShape = Qt::UpArrowCursor;
		break;

		case LCursor_WAIT:
			theShape = Qt::WaitCursor;
		break;

		default :
			theShape = Qt::ArrowCursor;
		break;

	}

	return theShape;
}


/*-----------------------------
// SetShape ()
//----------------------------*/
/*void LCursor_SetShape (LCursor_TShape inShape)
{
	int theShape;
	QCursor theCursor;

	switch (inShape) {
		case LCursor_APPSTARTING:
			theShape = LoadCursor (NULL, IDC_APPSTARTING);
			break;
		case LCursor_ARROW:
			theShape = Qt::ArrowCursor;
			break;
		case LCursor_CROSS:
			theShape = Qt::CrossCursor;
			break;
		case LCursor_IBEAM:
			theShape = Qt::IbeamCursor;
			break;
		case LCursor_NO:
			theShape = Qt::BlankCursor;
			break;
		case LCursor_SIZEALL:
			theShape = Qt::SizeAllCursor;
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
			theShape = Qt::UpArrowCursor;
			break;
		case LCursor_WAIT:
			theShape = Qt::WaitCursor;
			break;
		default :
			theShape = Qt::ArrowCursor;
		break;
	}

	theCursor.setShape(theShape);

	QApplication::setOverrideCursor(theCursor);




}
*/

/*----------------------------------------
// GetPosition ()
//---------------------------------------*/
void LCursor_GetPosition (ui4 *outX, ui4 *outY)
{
	QPoint thePoint;

	thePoint = QCursor::pos();

	*outX = ui4(thePoint.x());
	*outY = ui4(thePoint.y());
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

