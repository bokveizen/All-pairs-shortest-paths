/* ============================================================================
 *  LFile_win32.c
 * ============================================================================

 *  Author:         (c) 2003 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jan 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:04 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LConfig.h"

#if __LSL_OS_CORE__ == __LSL_WIN32__

#include "LFile.h"
#include "LString.h"
#include "LMemory.h"
#include "LGLobals.h"


/* MEMBER VARIABLES */
struct LFile_SearchHandle {
	HANDLE mSH;
};

/*----------------------------------------------------
// FindFirst ()
//--------------------------------------------------*/
LFile_SearchHandle* LFile_FindFirst (const i1* inPath, i1* outFileName)
{
	LFile_SearchHandle thehandle;
	WIN32_FIND_DATA thestruct;
	
	thehandle.mSH=FindFirstFile (inPath, &thestruct);
	
	/*no files or subdirs...*/
	if (thehandle.mSH==INVALID_HANDLE_VALUE)
		return NULL;
		
	LString_Copy (thestruct.cFileName, outFileName);
	
	return LMemory_NewObject (LFile_SearchHandle, thehandle);
}

/*---------------------------------------------------
// FindNext ()
//--------------------------------------------------*/
Bool LFile_FindNext (LFile_SearchHandle* inHandle, i1* outFileName)
{
	Bool theresult;
	WIN32_FIND_DATA thestruct;
	
	theresult=FindNextFile (inHandle->mSH, &thestruct);
	
	if (theresult!=FALSE)
		LString_Copy (thestruct.cFileName, outFileName);
	
	return theresult;
}

/*--------------------------------------------------
// FindClose ()
//------------------------------------------------*/
void LFile_FindClose (LFile_SearchHandle** ThisA)
{
	FindClose ((*ThisA)->mSH);
	LMemory_DeleteObject (ThisA);
}

/*-------------------------------------------------
// GetFileInfo ()
//-----------------------------------------------*/
Bool LFile_GetFileInfo (const i1* inFileName, LFile_Info* outInfo)
{
	Bool theresult;
	WIN32_FILE_ATTRIBUTE_DATA thedata;
	SYSTEMTIME thesystime;
	
	theresult=GetFileAttributesEx (inFileName, GetFileExInfoStandard, &thedata);
	
	/*file or dir exists...*/
	if (theresult!=FALSE)
	{
			/*is directory?*/
			if ((thedata.dwFileAttributes)&FILE_ATTRIBUTE_DIRECTORY)
				outInfo->mIsDir=TRUE;
			else
				outInfo->mIsDir=FALSE;	
			/*creation date & time...*/
			FileTimeToSystemTime (&(thedata.ftCreationTime), &thesystime);
			outInfo->mCreated.mYear=(ui2) thesystime.wYear;
			outInfo->mCreated.mMonth=(ui1) thesystime.wMonth;
			outInfo->mCreated.mDay=(ui1) thesystime.wDay;
			outInfo->mCreated.mHour=(ui1) thesystime.wHour;
			outInfo->mCreated.mMin=(ui1) thesystime.wMinute;
			outInfo->mCreated.mSec=(ui1) thesystime.wSecond;
			outInfo->mCreated.mMsec=(ui2) thesystime.wMilliseconds;
			/*last modification date & time...*/
			FileTimeToSystemTime (&(thedata.ftLastWriteTime), &thesystime);
			outInfo->mLastMod.mYear=(ui2) thesystime.wYear;
			outInfo->mLastMod.mMonth=(ui1) thesystime.wMonth;
			outInfo->mLastMod.mDay=(ui1) thesystime.wDay;
			outInfo->mLastMod.mHour=(ui1) thesystime.wHour;
			outInfo->mLastMod.mMin=(ui1) thesystime.wMinute;
			outInfo->mLastMod.mSec=(ui1) thesystime.wSecond;
			outInfo->mLastMod.mMsec=(ui2) thesystime.wMilliseconds;
	}
	
	return theresult;
}

/*-------------------------------------------------
// CreateDir ()
//------------------------------------------------*/
Bool LFile_CreateDir (const i1* inCompletePath)
{
	return CreateDirectory (inCompletePath, NULL);
}

/*-------------------------------------------------
// RemoveDir ()
//-----------------------------------------------*/
Bool LFile_RemoveDir (const i1* inCompletePath)
{
	return RemoveDirectory (inCompletePath);
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
