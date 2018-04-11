/* =============================================================================
 *  LBitmap_win32.c
 * =============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        May 17, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LBitmap.h"
#include "LBitmap_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__


/*--------------------------------
// CreateFromBMP ()
//-------------------------------*/
LBitmap* LBitmap_CreateFromBMP (void* inBMPFileImage, ui4 inFileImageSize)
{
    LBitmap thebitmap;
    LBitmap* thebitmapptr;
    BITMAPFILEHEADER *thebmfh;
    HDC thehdc;
    BITMAPINFOHEADER *thebmih;

    thehdc=GetDC (NULL);
    thebmfh=inBMPFileImage;
    

    thebitmap.mHbitmap=CreateDIBitmap (thehdc, (BITMAPINFOHEADER *)(thebmfh+1),
        CBM_INIT, (BYTE *)thebmfh+thebmfh->bfOffBits, (BITMAPINFO *)(thebmfh+1),
        DIB_RGB_COLORS);

		/* check for call failure */
	if (thebitmap.mHbitmap==NULL) Throw(LBitmap_CANT_CREATE);

    thebmih=(BITMAPINFOHEADER *)(thebmfh+1);
    thebitmap.mDX=thebmih->biWidth;
    thebitmap.mDY=thebmih->biHeight;
    
    ReleaseDC (NULL, thehdc);

    thebitmapptr=LMemory_NewObject (LBitmap, thebitmap);
    return thebitmapptr;
}

/*-----------------------------
// Destroy ()
//---------------------------*/
void LBitmap_Destroy (LBitmap* inBitmap)
{
    DeleteObject (inBitmap->mHbitmap);
    LMemory_DeleteObject (&inBitmap);
}

/*-----------------------------
// ConvertToBMP ()
//----------------------------*/
void* LBitmap_ConvertToBMP (LBitmap* inBitmap, ui4* outSize)
{
    HDC thehdc;
    BITMAPFILEHEADER *thefileheader;
    BITMAPCOREHEADER *thecoreheader;
    ui4 theRowLength;
    ui4 theDataSize;
    void *theData;
    void *theoutBMPFileImage;

    /*computes data size*/
    theRowLength=4*((inBitmap->mDX*24+31)/32);
    theDataSize=theRowLength*inBitmap->mDY;
    /*retrieves display compatible DC*/
    thehdc=GetDC (NULL);
    /*allocates memory for file header*/
    thefileheader=(BITMAPFILEHEADER *)LMemory_Malloc (sizeof (BITMAPFILEHEADER));
    /*performs initialization*/
    thefileheader->bfType=0x4D42; /*BM*/
    thefileheader->bfSize=sizeof (BITMAPFILEHEADER)+sizeof (BITMAPCOREHEADER)+theDataSize;
    thefileheader->bfReserved1=0;
    thefileheader->bfReserved2=0;
    thefileheader->bfOffBits=sizeof (BITMAPCOREHEADER)+sizeof (BITMAPFILEHEADER);
    /*allocates memory for core header*/
    thecoreheader=LMemory_Malloc (sizeof (BITMAPCOREHEADER));
    /*performs required init*/
    thecoreheader->bcSize=12;
    thecoreheader->bcWidth=(WORD)inBitmap->mDX;
    thecoreheader->bcHeight=(WORD)inBitmap->mDY;
    thecoreheader->bcPlanes=1;
    thecoreheader->bcBitCount=24; /*this way a color palette is NOT required*/
    /*allocates memory for data*/
    theData=LMemory_Malloc (theDataSize);
    /*calls GetDIBits()*/
    GetDIBits (thehdc, inBitmap->mHbitmap, 0, inBitmap->mDY, theData, (LPBITMAPINFO)thecoreheader, DIB_RGB_COLORS);
    /*allocates memory for file image*/
    theoutBMPFileImage=LMemory_Malloc (thefileheader->bfSize);
    LMemory_Copy (thefileheader, theoutBMPFileImage, sizeof (BITMAPFILEHEADER));
    LMemory_Copy (thecoreheader, (ui1*)(theoutBMPFileImage)+sizeof (BITMAPFILEHEADER), sizeof (BITMAPCOREHEADER));
    LMemory_Copy (theData, (ui1*)(theoutBMPFileImage)+sizeof (BITMAPFILEHEADER)+sizeof (BITMAPCOREHEADER), theDataSize);
    /*frees aux memory*/
    LMemory_Free (&thefileheader);
    LMemory_Free (&thecoreheader);
    LMemory_Free (&theData);
    /*releases the DC*/
    ReleaseDC (NULL, thehdc);
    *outSize=sizeof (BITMAPFILEHEADER)+sizeof (BITMAPCOREHEADER)+theDataSize;
    return theoutBMPFileImage;
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
