/* =============================================================================
 *  LBitmap_qt.c
 * =============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Jun 26, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/21 19:07:41 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/

#if __LSL_OS_GUI__ == __LSL_QT__

#include <qpixmap.h>

//extern "C" {

#include "LMemory.h"

//}

#include "LBitmap.h"
#include "LFile.h"
#include "LBitmap_qt.h"
#include "LWindow_qt.h"
#include "LApplication_qt.h"

/*--------------------------------
// CreateFromBMP
//-------------------------------*/
LBitmap *LBitmap_CreateFromBMP( void *inBMPFileImage, ui4 inbmpfilesize )
{
	LBitmap		theBitMap;
	LBitmap		*theBitMapPtr;

	theBitMap.mHbitmap = new QPixmap();

	/* check for call failure */
	if ( theBitMap.mHbitmap == NULL )
		Throw(LBitmap_CANT_CREATE);

	theBitMap.mHbitmap->loadFromData( (uchar *)inBMPFileImage, inbmpfilesize, 0, QPixmap::Auto );

	theBitMap.mDX = theBitMap.mHbitmap->width();
	theBitMap.mDY = theBitMap.mHbitmap->height();

#ifdef PRINT_DEBUG
	qDebug( "\n\r LMemory_NewObject : Bitmap");
#endif

	theBitMapPtr = ( LBitmap* )LMemory_NewObject( LBitmap, theBitMap );

	return theBitMapPtr;

}

/*-----------------------------
// Destroy ()
//---------------------------*/
void LBitmap_Destroy (LBitmap* inBitmap)
{
	delete inBitmap->mHbitmap;
	LMemory_DeleteObject(&inBitmap);
}

/*-----------------------------
// ConvertToBMP ()
//----------------------------*/
void* LBitmap_ConvertToBMP (LBitmap* inBitmap, ui4* outSize)
{
	LFile *mybmpfile;
	ui4 mybmpfilesize;
	void *mydata;

	inBitmap->mHbitmap->save( "tmp.bmp", "BMP" );
	
	/* open file */
	mybmpfile = LFile_Open ("tmp.bmp", LFile_READ);
	
	/* get file size */
	mybmpfilesize = LFile_GetSize (mybmpfile);
	
	/* allocate memory */
	mydata = LMemory_Malloc( mybmpfilesize );
	
	/* read file */
	LFile_Read (mybmpfile, mydata, mybmpfilesize);
	
	/* close file */
	LFile_Close (&mybmpfile);


	*outSize = mybmpfilesize;
	
	LFile_Remove("tmp.bmp");

	return mydata;
	
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


