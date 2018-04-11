/* ============================================================================
 *  LFile_qt.c
 * ============================================================================

 *  Author:         (c) 2003 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Feb 01, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:21:35 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/



#if __LSL_OS_CORE__ == __LSL_QT__

#include <qfileinfo.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qregexp.h>

#include "LFile.h"
#include "LGlobals.h"
#include "LConfig.h"

extern "C" {

#include "LString.h"
#include "LMemory.h"

}

/* MEMBER VARIABLES */
struct LFile_SearchHandle {
	
	QStringList *mSearchHandle;
	i1 mFilter[50];

};

/*----------------------------------------------------
// FindFirst 
//--------------------------------------------------*/
LFile_SearchHandle* LFile_FindFirst (const i1* inPath, i1* outFileName)
{
//QDir::QDir ( const QString & path, const QString & nameFilter = QString::null, int sortSpec = Name | IgnoreCase, int filterSpec = All )

	QStringList theFileList;

	QStringList *theFilteredList = new QStringList();

	QString theString(outFileName);

	const i1 * theStringPtr;
#ifdef PRINT_DEBUG
	qDebug( "\n\r outFileName %s",outFileName);
	//qDebug( "\n\r theSearchHandler->mFilter %p", theSearchHandlermFilter);

#endif


	LFile_SearchHandle *theSearchHandler;
	LFile_SearchHandle theSearch;

	

	
	strcpy( theSearch.mFilter, outFileName );





	// The string is not case sensitive
	QRegExp theRegExp( theString, FALSE, TRUE );

	//QDir theDir( inPath );
	QDir theDir( "/home/lsl/bin" );
	theFileList = theDir.entryList();
#ifdef PRINT_DEBUG
		qDebug( "\n\r inPath %s",inPath);

		//qDebug( "\n\r theFileList.begin() %s",theFileList.begin());


	for ( QStringList::Iterator it = theFileList.begin(); it != theFileList.end(); ++it ) {
        cout << *it << " : ";
    }
#endif


	*theFilteredList = theFileList.grep( theRegExp );

	theString = theFilteredList->first();
	//theString = theFilteredList->last();

	
	if ( theString.latin1() == NULL )
		return FALSE;
	theStringPtr = theString.latin1();
#ifdef PRINT_DEBUG
	qDebug( "\n\r theStringPtr %s",theStringPtr);
#endif
	strcpy( outFileName, theString.latin1() );

#ifdef PRINT_DEBUG
	qDebug( "\n\r outFileName %s",outFileName);
#endif

	theFilteredList->remove( theFilteredList->begin() );


#ifdef PRINT_DEBUG
	/*for ( QStringList::Iterator it = theFileList.begin(); it != theFileList.end(); ++it ) {
        cout << *it << " : ";
    }*/
#endif

	theSearch.mSearchHandle = theFilteredList;
	//theSearchHandler->mPath = inPath;
	theSearchHandler = ( LFile_SearchHandle* )LMemory_NewObject(LFile_SearchHandle, theSearch);



	return theSearchHandler;



	//for ( QStringList::Iterator it = fonts.begin(); it != fonts.end(); ++it ) {
    //    cout << *it << ":";
   // }


//QStringList QDir::entryList ( int filterSpec = DefaultFilter, int sortSpec = DefaultSort ) const [virtual]

/* You can sort the list with sort(), and extract a new list which contains only those strings which contain a particular substring (or match a particular regular expression) using the grep() functions, e.g.

fonts.sort();
    cout << fonts.join( ", " ) << endl;
    // Output:
    //  Courier, Courier New, Helvetica [Adobe], Helvetica [Cronyx], Times

    QStringList helveticas = fonts.grep( "Helvetica" );
    cout << helveticas.join( ", " ) << endl;
    // Output:
    //  Helvetica [Adobe], Helvetica [Cronyx]

QStringList QStringList::grep ( const QRegExp & expr ) const

*/


}

/*---------------------------------------------------
// FindNext ()
//--------------------------------------------------*/
Bool LFile_FindNext (LFile_SearchHandle* inHandle, i1* outFileName)
{
	//QStringList theFileList;

	//QStringList *theFilteredList = new QStringList();

	QString theString( inHandle->mFilter );

	const i1 * theStringPtr;
#ifdef PRINT_DEBUG
	qDebug( "\n\r outFileName %s",outFileName);
#endif



	// The string is not case sensitive
	QRegExp theRegExp( theString, FALSE, TRUE );



	*(inHandle->mSearchHandle) = inHandle->mSearchHandle->grep( theRegExp );

	theString = inHandle->mSearchHandle->first();
	//theString = theFilteredList->last();

	theStringPtr = theString.latin1();
	
	if ( theString.latin1() == NULL )
		return FALSE;
#ifdef PRINT_DEBUG
	qDebug( "\n\r theStringPtr %s",theStringPtr);
#endif
	strcpy( outFileName, theString.latin1() );

#ifdef PRINT_DEBUG
	qDebug( "\n\r outFileName %s",outFileName);
#endif

	inHandle->mSearchHandle->remove( inHandle->mSearchHandle->begin() );


#ifdef PRINT_DEBUG
	/*for ( QStringList::Iterator it = theFileList.begin(); it != theFileList.end(); ++it ) {
        cout << *it << " : ";
    }*/
#endif


	return TRUE;




}

/*--------------------------------------------------
// FindClose ()
//------------------------------------------------*/
void LFile_FindClose (LFile_SearchHandle** ThisA)
{
	delete (*ThisA)->mSearchHandle;
	LMemory_DeleteObject( ThisA );
}

/*-------------------------------------------------
// GetFileInfo ()
//-----------------------------------------------*/
Bool LFile_GetFileInfo (const i1* inFileName, LFile_Info* outInfo)
{
	/*Bool  mIsDir;
	LTime mCreated;
	LTime mLastMod;*/
	Bool theResult;

	/*The file can also include an absolute or relative path.*/
	QFileInfo theFileInfo(inFileName);
	
	QDateTime theDateTime;

	/*bool QFileInfo::isDir () const*/

	theResult = theFileInfo.isDir();
	outInfo->mIsDir = TRUE;
	
	if (!theResult) {
		theResult = theFileInfo.isFile();
		outInfo->mIsDir = FALSE;

	}

	if (theResult != FALSE) {
			
			/*creation date & time...*/
			/*FileTimeToSystemTime (&(thedata.ftCreationTime), &thesystime);*/
			theDateTime = theFileInfo.created();
			
			QDate theDate = theDateTime.date();
			QTime theTime = theDateTime.time();
			
			outInfo->mCreated.mYear = (ui2) theDate.year();
			outInfo->mCreated.mMonth = (ui1) theDate.month();
			outInfo->mCreated.mDay = (ui1) theDate.day();
			outInfo->mCreated.mHour = (ui1) theTime.hour();
			outInfo->mCreated.mMin = (ui1) theTime.minute();
			outInfo->mCreated.mSec = (ui1) theTime.second();
			outInfo->mCreated.mMsec = (ui2) theTime.msec();
			
			/*last modification date & time...*/
			/*FileTimeToSystemTime (&(thedata.ftLastWriteTime), &thesystime);*/
			theDateTime = theFileInfo.lastModified();
			
			theDate = theDateTime.date();
			theTime = theDateTime.time();

			outInfo->mLastMod.mYear = (ui2) theDate.year();
			outInfo->mLastMod.mMonth = (ui1) theDate.month();
			outInfo->mLastMod.mDay = (ui1) theDate.day();
			outInfo->mLastMod.mHour = (ui1) theTime.hour();
			outInfo->mLastMod.mMin = (ui1) theTime.minute();
			outInfo->mLastMod.mSec = (ui1) theTime.second();
			outInfo->mLastMod.mMsec = (ui2) theTime.msec();
	}

	return theResult;
}

/*-------------------------------------------------
// CreateDir ()
//------------------------------------------------*/
Bool LFile_CreateDir (const i1* inCompletePath)
{
	QDir theDir;

	return (theDir.mkdir(inCompletePath/*const QString & dirName*//*, bool acceptAbsPath = TRUE */));
	
	/* If acceptAbsPath is TRUE a path starting with a separator ('/') will remove the absolute directory;
	 if acceptAbsPath is FALSE any number of separators at the beginning of dirName will be removed. */
}

/*-------------------------------------------------
// RemoveDir ()
//-----------------------------------------------*/
Bool LFile_RemoveDir (const i1* inCompletePath)
{
	/* The directory must be empty for rmdir() to succeed. */
	QDir theDir;

	return (theDir.rmdir(inCompletePath/*const QString & dirName*//*, bool acceptAbsPath = TRUE */));
	
	/* If acceptAbsPath is TRUE a path starting with a separator ('/') will remove the absolute directory;
	 if acceptAbsPath is FALSE any number of separators at the beginning of dirName will be removed. */
}


#endif /* __LSL_OS_CORE__ == __LSL_QT__ */


/* Copyright (C) 2003 Gianni Bevacqua

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
