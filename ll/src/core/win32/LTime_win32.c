/* ============================================================================
 *  LTime_win32.c
 * ============================================================================

 *  Author:         (c) 2003 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Jan 25, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:04 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LConfig.h"

#if __LSL_OS_CORE__ == __LSL_WIN32__

#include "LTime.h"
#include "LGlobals.h"
#include <time.h>


/* ----------------------------------------------------------------------------
 *  GetCalendarTime
 * ----------------------------------------------------------------------------
*/
LTime LTime_GetCalendarTime() 
{ 
	LTime thetime;
	SYSTEMTIME thesystime;

	GetSystemTime (&thesystime);

	thetime.mYear=(ui2) thesystime.wYear;
	thetime.mMonth=(ui1) thesystime.wMonth;
	thetime.mDay=(ui1) thesystime.wDay;
	thetime.mHour=(ui1) thesystime.wHour;
	thetime.mMin=(ui1) thesystime.wMinute;
	thetime.mSec=(ui1) thesystime.wSecond;
	thetime.mMsec=(ui2) thesystime.wMilliseconds;

	return thetime;
}

/* ----------------------------------------------------------------------------
 *  Compare
 * ----------------------------------------------------------------------------
*/
i1 LTime_Compare (LTime t1, LTime t2)
{
	if (t1.mYear<t2.mYear)
		return -1;
	if (t1.mYear>t2.mYear)
		return 1;

	if (t1.mMonth<t2.mMonth)
		return -1;
	if (t1.mMonth>t2.mMonth)
		return 1;

	if (t1.mDay<t2.mDay)
		return -1;
	if (t1.mDay>t2.mDay)
		return 1;

	if (t1.mHour<t2.mHour)
		return -1;
	if (t1.mHour>t2.mHour)
		return 1;

	if (t1.mMin<t2.mMin)
		return -1;
	if (t1.mMin>t2.mMin)
		return 1;

	if (t1.mSec<t2.mSec)
		return -1;
	if (t1.mSec>t2.mSec)
		return 1;

	if (t1.mMsec<t2.mMsec)
		return -1;
	if (t1.mMsec>t2.mMsec)
		return 1;

	return 0;
}

/* ----------------------------------------------------------------------------
 *  GetUserTime
 * ----------------------------------------------------------------------------
*/
f8 LTime_GetUserTime()
{
	HANDLE theprocess;
    FILETIME thecreation, theexit, thekernel, theuser;
	f8 thekernelt, theusert, thetotalt;
	DWORD theversion;

	theversion=GetVersion ();
	if (theversion<0x80000000) /*should be NT/2000/XP class operating system...*/
	{
	theprocess=GetCurrentProcess (); /*unsupported on 9x/ME systems...*/

	GetProcessTimes (theprocess, &thecreation, &theexit, &thekernel, &theuser);

	thekernelt=((f8)thekernel.dwHighDateTime*(f8)0xffffffff)+(f8)thekernel.dwLowDateTime;
	theusert=((f8)theuser.dwHighDateTime*(f8)0xffffffff)+(f8)theuser.dwLowDateTime;
	thetotalt=thekernelt+theusert;
	thetotalt=thetotalt/(f8)10000000;

	return thetotalt;
	}
	else /* 9x/ME... */
	{
		return (f8)clock()/CLOCKS_PER_SEC; /*standard C api*/
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
