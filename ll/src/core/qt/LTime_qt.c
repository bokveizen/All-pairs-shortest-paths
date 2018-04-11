/* ============================================================================
 *  LTime_qt.c
 * ============================================================================

 *  Author:         (c) 2003 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Feb 01, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/19 00:21:35 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.3 $
*/

#if __LSL_OS_CORE__ == __LSL_QT__

#include <qdatetime.h>

#include "LTime.h"
#include "LGlobals.h"
#include "LConfig.h"

/* ----------------------------------------------------------------------------
 *  GetCalendarTime
 * ----------------------------------------------------------------------------*/
LTime LTime_GetCalendarTime() 
{
	LTime theTime;
	
	QDate theCurrentDate = QDate::currentDate(Qt::LocalTime);
	QTime theCurrentTime = QTime::currentTime(Qt::LocalTime);

 	theTime.mYear = (ui2) theCurrentDate.year(); /* Returns the year (1752..8000) of this date */
	theTime.mMonth = (ui1) theCurrentDate.month();
	theTime.mDay = (ui1) theCurrentDate.day();
	theTime.mHour = (ui1) theCurrentTime.hour(); /* Returns the hour part (0..23) */
	theTime.mMin = (ui1) theCurrentTime.minute();
	theTime.mSec = (ui1) theCurrentTime.second();
	theTime.mMsec = (ui2) theCurrentTime.msec();

	return theTime;

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
