/* ============================================================================
 *  LDialog_qt.c
 * ============================================================================

 *  Author:         (c) 2002 Gianni Bevacqua
 *  License:        See the end of this file for license information
 *  Created:        Jun 25, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/06/16 23:57:27 $
 *  Changed by:     $Author: elbevacq $
 *  Revision:       $Revision: 1.4 $
*/


#if __LSL_OS_GUI__ == __LSL_QT__


#include <qfiledialog.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qframe.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qmainwindow.h>
#include <qcolordialog.h>

#include <string.h>
#include <stdlib.h>

#include "LDialog.h"
#include "LApplication_qt.h"

#define MAX_FILTER_CHARACTERS   100

/*extern QLeoMainWindow* _gMainWindow;*/

/*-------------------------------------------
// DisplayMessageBox ()
//------------------------------------------*/
LDialog_TMBReturnID LDialog_DisplayMessageBox (LWindow* inOwner, i1* inText, i1* inTitle,
		LDialog_TMBButtons inButtons, LDialog_TMBIcon inIcon)
{
	
	ui1 theReturnID;
	i1 theString[200];
	i1* theNewText;

	ui2 theTitleLength = strlen(inTitle);
	ui2 theTextLength = strlen(inText);
	
#ifdef PRINT_DEBUG
	//qDebug( "\n\r LDialog_DisplayMessageBox ");
	//qDebug( "\n\r inText *%s*   textLength %d", inText, theTextLength);
	//qDebug( "\n\r inTitle *%s*  titleLength %d", inTitle, theTitleLength);
#endif

	if ( theTitleLength > theTextLength ) {
#ifdef PRINT_DEBUG
	//qDebug( "\n\r titleLength %d  >  textLength %d ", theTitleLength, theTextLength);
#endif

		memset(theString, ' ', theTitleLength - theTextLength + 24);
		theString[theTitleLength - theTextLength + 24] = '\0';
#ifdef PRINT_DEBUG
	//qDebug( "\n\r theString %s|  ,  theTitleLength - theTextLength + 24 %d ", theString, theTitleLength - theTextLength + 24);
	//qDebug( "\n\r strlen(inText)+strlen(theString) = %d ", strlen(inText)+strlen(theString));
	//qDebug( "\n\r sizeof(*inText) %d ", sizeof(*inText));
#endif

		//if ( sizeof(*inText) < strlen(inText)+strlen(theString) )
		theNewText = (i1*)malloc(200);
		strcpy(theNewText, inText);
		strncat(theNewText, theString, strlen(inText)+strlen(theString));
		/*strcat(inText, "                           ");*/
		//memcpy(inText, theString, strlen(inText)+strlen(theString));

#ifdef PRINT_DEBUG

	//qDebug( "\n\r inText+theString  *%s%s*", inText, theString);

#endif

	}
	else{

#ifdef PRINT_DEBUG

	//qDebug( "\n\r theNewText  *%s ---  inText%s*", theNewText, inText);

#endif
		theNewText = (i1*)malloc(200);
		strcpy(theNewText, inText);

#ifdef PRINT_DEBUG

	//qDebug( "\n\r theNewText  *%s ---  inText%s*", theNewText, inText);

#endif

	}

	switch (inButtons) {

		case LDialog_MBABORTRETRYIGNORE:
		
		 	switch (inIcon) {
		
		case LDialog_MBICONEXCLAMATION:

			theReturnID = QMessageBox::warning(0,inTitle, theNewText,
						            QMessageBox::Abort,
						            QMessageBox::Retry | QMessageBox::Default,
						            QMessageBox::Ignore | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONINFORMATION:
			
			theReturnID = QMessageBox::information(0,inTitle, theNewText,
						            QMessageBox::Abort,
						            QMessageBox::Retry | QMessageBox::Default,
						            QMessageBox::Ignore | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONQUESTION:
		
		          theReturnID = QMessageBox::warning(0,inTitle, theNewText,
						            QMessageBox::Abort,
						            QMessageBox::Retry | QMessageBox::Default,
						            QMessageBox::Ignore | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONSTOP:
			
			theReturnID = QMessageBox::critical(0,inTitle, theNewText,
						            QMessageBox::Abort,
						            QMessageBox::Retry | QMessageBox::Default,
						            QMessageBox::Ignore | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
			

		break;
		
		default :
		

			return LDialog_MBIDABORT;
			
		
		break;
		
			}
		
		
		break;
		
		case LDialog_MBOK:
		
#ifndef MODIFICA_DEL_07072002			
			switch (inIcon) {

		case LDialog_MBICONEXCLAMATION:
			
			theReturnID = QMessageBox::warning(0,inTitle, theNewText,
						            QMessageBox::Ok | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape);
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONINFORMATION:
			
			theReturnID = QMessageBox::information(0,inTitle, theNewText,
			                                     QMessageBox::Ok | QMessageBox::Default,
							     QMessageBox::NoButton,
							     QMessageBox::NoButton);
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONQUESTION:
		
		          theReturnID = QMessageBox::warning(0,inTitle, theNewText,
		                                            QMessageBox::Ok | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape);
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
						
			

		break;
		
		case LDialog_MBICONSTOP:
			
			theReturnID = QMessageBox::critical(0,inTitle, theNewText,
			                                    QMessageBox::Ok | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape);
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;	
			
			
			
		
		
		
		default :
		

			return LDialog_MBIDABORT;
			
		
		break;
		
			}
			
			
		break;
#endif /* MODIFICA_DEL_07072002	*/		
		case LDialog_MBOKCANCEL:
		
		      switch (inIcon) {
		
		       case LDialog_MBICONEXCLAMATION:
			
			theReturnID = QMessageBox::warning(0,inTitle, theNewText,
								QMessageBox::Ok | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
			

		break;
		
		case LDialog_MBICONINFORMATION:
			
			theReturnID = QMessageBox::information(0,inTitle, theNewText,
			                                    QMessageBox::Ok | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONQUESTION:
		
		          theReturnID = QMessageBox::warning(0,inTitle, theNewText,
		                                             QMessageBox::Ok | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
						

		break;
		
		case LDialog_MBICONSTOP:
			
			theReturnID = QMessageBox::critical(0,inTitle, theNewText,
			                                    QMessageBox::Ok | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
						

		break;
		
		
		
		
		
		
	
		default :

			return LDialog_MBIDABORT;

		break;
		
			}
			

					
		break;

		case LDialog_MBRETRYCANCEL:
		
		
		          switch (inIcon) {
		
		       case LDialog_MBICONEXCLAMATION:
			
			theReturnID = QMessageBox::warning(0,inTitle, theNewText,
								QMessageBox::Retry | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
			
		
		break;
		
		case LDialog_MBICONINFORMATION:
			
			theReturnID = QMessageBox::information(0,inTitle, theNewText,
			                                    QMessageBox::Retry | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONQUESTION:
		
		          theReturnID = QMessageBox::warning(0,inTitle, theNewText,
		                                             QMessageBox::Retry | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;
		
		case LDialog_MBICONSTOP:
			
			theReturnID = QMessageBox::critical(0,inTitle, theNewText,
			                                    QMessageBox::Retry | QMessageBox::Default,
						                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
						
			
		break;
		
		
		default :
		
			
			return LDialog_MBIDABORT;
			
		
		break;
		
			}
		
		break;
		
		case LDialog_MBYESNO:
		
		
		switch (inIcon) {
		
		       case LDialog_MBICONEXCLAMATION:
			
			theReturnID = QMessageBox::warning(0,inTitle, theNewText,
								QMessageBox::Yes | QMessageBox::Default,
						                QMessageBox::No  );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
			

		break;
		
		case LDialog_MBICONINFORMATION:
			
			theReturnID = QMessageBox::information(0,inTitle, theNewText,
			                                     QMessageBox::Yes | QMessageBox::Default,
						                QMessageBox::No  );
								
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
			
			


		break;
		
		case LDialog_MBICONQUESTION:
		
		          theReturnID = QMessageBox::warning(0,inTitle, theNewText,
		                                              QMessageBox::Yes | QMessageBox::Default,
						                QMessageBox::No  );
								

			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
		


		break;
		
		case LDialog_MBICONSTOP:
			
			theReturnID = QMessageBox::critical(0,inTitle, theNewText,
			                                        QMessageBox::Yes | QMessageBox::Default,
						                QMessageBox::No  );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
			

		break;
		

		default :
		

			return LDialog_MBIDABORT;

		
		break;
		
			}
						
		break;
		
		case LDialog_MBYESNOCANCEL:
		
		
		    switch (inIcon) {
		
		       case LDialog_MBICONEXCLAMATION:
			
			theReturnID = QMessageBox::warning(0,inTitle, theNewText,
								QMessageBox::Yes | QMessageBox::Default,
						                QMessageBox::No,
			                   	                QMessageBox::Cancel | QMessageBox::Escape );
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);

		break;
		
		case LDialog_MBICONINFORMATION:
			
			theReturnID = QMessageBox::information(0,inTitle, theNewText,
			                                     QMessageBox::Yes | QMessageBox::Default,
			                                      QMessageBox::No,
			                   	                QMessageBox::Cancel | QMessageBox::Escape );
						
			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);
			


		break;
		
		case LDialog_MBICONQUESTION:
		
		          theReturnID = QMessageBox::warning(0,inTitle, theNewText,
		                                              QMessageBox::Yes | QMessageBox::Default,
		                                                QMessageBox::No,
			                   	                QMessageBox::Cancel | QMessageBox::Escape );
						
    			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);

		break;
		
		case LDialog_MBICONSTOP:
			
			theReturnID = QMessageBox::critical(0,inTitle, theNewText,
			                                        QMessageBox::Yes | QMessageBox::Default,
			                                         QMessageBox::No,
			                   	                QMessageBox::Cancel | QMessageBox::Escape );

			/* check for call failure */
			if (theReturnID == 0)
				Throw(LDialog_CANT_CREATE);


		break;


		
		default :
		
			return LDialog_MBIDABORT;

		break;
		
			}
			
		
		break;
		
		default :
		
			return LDialog_MBIDABORT;
			
		break;
	}


	
	switch (theReturnID) {
	
		case QMessageBox::Abort:
			return LDialog_MBIDABORT;
		break;	
			
		case QMessageBox::Cancel :
			return LDialog_MBIDCANCEL;
		break;	
			
		case QMessageBox::Ignore:
			return LDialog_MBIDIGNORE;
		break;	
			
		case QMessageBox::No:
			return LDialog_MBIDNO;
		break;	
			
		case QMessageBox::Ok :
			return LDialog_MBIDOK;
		break;	
			
		case QMessageBox::Retry:
			return LDialog_MBIDRETRY;
		break;	
		
		case QMessageBox::Yes :
			return LDialog_MBIDYES;
		break;	
		
		default :
			return LDialog_MBIDABORT;
		break;

	}
	
	free(theNewText);
}




/*-----------------------------------------
// GetSaveFileName ()
//----------------------------------------*/
Bool LDialog_GetSaveFileName (LWindow* inOwner, i1* inFilters[], ui1 inNumFilters, i1* outBuffer)
{
	QString *theStringPtr = new QString();
	
	i1 theFilterString[inNumFilters][MAX_FILTER_CHARACTERS];
	i1 theCompleteFilterString[MAX_FILTER_CHARACTERS*inNumFilters];
	
	theCompleteFilterString[0] = 0;


	for (ui1 theIndex = 0; theIndex < inNumFilters; theIndex++) {

		snprintf(theFilterString[theIndex], MAX_FILTER_CHARACTERS, "%s(%s);;", inFilters[2*theIndex], inFilters[2*(theIndex+1)-1]);
#ifdef PRINT_DEBUG
	qDebug("\n\r theFilterString : %s", theFilterString[theIndex]);

#endif

		strncat(theCompleteFilterString, theFilterString[theIndex], MAX_FILTER_CHARACTERS);

#ifdef PRINT_DEBUG
	qDebug("\n\r theCompleteFilterString : %s", theCompleteFilterString);

#endif


	}


	
	
	*theStringPtr = QFileDialog::getSaveFileName(QString::null, theCompleteFilterString/* "*.c"*//*inFilters[]*/, _LApplication_gGlobal.mMainWindow, "Save", "Choose a file");

#ifdef PRINT_DEBUG
	qDebug("\n\r theStringPtr : 0x%x", theStringPtr);
#endif




	if (theStringPtr->length() == 0)
		return (FALSE);
	
	strcpy(outBuffer, (i1*)theStringPtr->latin1());

#ifdef PRINT_DEBUG
	qDebug("\n\r theString : %s",outBuffer);
#endif

	delete theStringPtr;

	return ( (outBuffer != NULL) ? TRUE : FALSE );

}

/*-----------------------------------------
// GetOpenFileName ()
//----------------------------------------*/
Bool LDialog_GetOpenFileName (LWindow* inOwner, i1* inFilters[], ui1 inNumFilters, i1* outBuffer)
{
	QString* theStringPtr = new QString();

	i1 theFilterString[inNumFilters][MAX_FILTER_CHARACTERS];
	i1 theCompleteFilterString[MAX_FILTER_CHARACTERS*inNumFilters];

	theCompleteFilterString[0] = 0;

	for (ui1 theIndex = 0; theIndex < inNumFilters; theIndex++) {

		snprintf(theFilterString[theIndex], MAX_FILTER_CHARACTERS, "%s(%s);;", inFilters[2*theIndex], inFilters[2*(theIndex+1)-1]);
#ifdef PRINT_DEBUG
	qDebug("\n\r theFilterString[%d] : %s", theIndex, theFilterString[theIndex]);

#endif

		strncat(theCompleteFilterString, theFilterString[theIndex], MAX_FILTER_CHARACTERS);

#ifdef PRINT_DEBUG
	qDebug("\n\r theCompleteFilterString : %s", theCompleteFilterString);

#endif

	}


	*theStringPtr = QFileDialog::getOpenFileName(QString::null, theCompleteFilterString/*"*.c"*//*inFilters[]*/, _LApplication_gGlobal.mMainWindow, "Open", "Choose a file");

#ifdef PRINT_DEBUG
	qDebug("\n\r theStringPtr : 0x%x", theStringPtr);
#endif

	if (theStringPtr->length() == 0)
		return (FALSE);

	strcpy(outBuffer, (i1*)theStringPtr->latin1());

	
#ifdef PRINT_DEBUG
	qDebug("\n\r theString : %s",outBuffer);
#endif
	delete theStringPtr;

	return ( (outBuffer != NULL) ? TRUE : FALSE );

}

/*--------------------------------------------
// ColorPicker ()
//-------------------------------------------*/
ui4 LDialog_ColorPicker(LWindow* inOwner)
{
	QColor theColor;
	ui4  thePixelColor = 0;
	
	theColor = QColorDialog::getColor();
	
	thePixelColor = theColor.red();
	thePixelColor |= (theColor.green() << 8);
	thePixelColor |= (theColor.blue() << 16);
	
	return thePixelColor;
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


