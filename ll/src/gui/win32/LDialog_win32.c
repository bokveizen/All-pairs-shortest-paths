/* ============================================================================
 *  LDialog_win32.c
 * ============================================================================

 *  Author:         (c) 2002 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Feb 16, 2002
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/15 15:21:06 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.1.1.1 $
*/

#include "LDialog.h"
#include "LWindow_win32.h"

#if __LSL_OS_GUI__ == __LSL_Win32__

/*-------------------------------------------
// DisplayMessageBox ()
//------------------------------------------*/
LDialog_TMBReturnID LDialog_DisplayMessageBox (LWindow* inOwner, i1* inText, i1* inTitle,
        LDialog_TMBButtons inButtons, LDialog_TMBIcon inIcon)
{
    HWND theHwnd;
    UINT theStyle=0;
    UINT theStyle2=0;
    int theReturnID;

    if (inOwner==NULL)
        theHwnd=_LApplication_gGlobal.hwndFrame;
    else
        theHwnd=inOwner->mHwnd;

    switch (inButtons)
    {
    case LDialog_MBABORTRETRYIGNORE:
        theStyle=MB_ABORTRETRYIGNORE;
        break;
    case LDialog_MBOK:
        theStyle=MB_OK;
        break;
    case LDialog_MBOKCANCEL:
        theStyle=MB_OKCANCEL;
        break;
    case LDialog_MBRETRYCANCEL:
        theStyle=MB_RETRYCANCEL;
        break;
    case LDialog_MBYESNO:
        theStyle=MB_YESNO;
        break;
    case LDialog_MBYESNOCANCEL:
        theStyle=MB_YESNOCANCEL;
        break;
    }

    switch (inIcon)
    {
    case LDialog_MBICONEXCLAMATION:
        theStyle2=MB_ICONEXCLAMATION;
        break;
    case LDialog_MBICONINFORMATION:
        theStyle2=MB_ICONINFORMATION;
        break;
    case LDialog_MBICONQUESTION:
        theStyle2=MB_ICONQUESTION;
        break;
    case LDialog_MBICONSTOP:
        theStyle2=MB_ICONSTOP;
        break;
    }

    theReturnID=MessageBox (theHwnd, (LPCTSTR) inText, (LPCTSTR) inTitle, theStyle|theStyle2); 

	/* check for call failure */
	if (theReturnID==0) Throw(LDialog_CANT_CREATE);

    switch (theReturnID)
    {
    case IDABORT:
        return LDialog_MBIDABORT;
    case IDCANCEL:
        return LDialog_MBIDCANCEL;
    case IDIGNORE:
        return LDialog_MBIDIGNORE;
    case IDNO:
        return LDialog_MBIDNO;
    case IDOK:
        return LDialog_MBIDOK;
    case IDRETRY:
        return LDialog_MBIDRETRY;
    case IDYES:
        return LDialog_MBIDYES;
    }
}

/*-----------------------------------------
// GetSaveFileName ()
//----------------------------------------*/
Bool LDialog_GetSaveFileName (LWindow* inOwner, i1* inFilters[], ui1 inNumFilters, i1 inBuffer[LFile_MAX_PATHNAME_LEN])
{
    OPENFILENAME theOpen;
	Bool theaux;
	DWORD thecode;

	/*required by filter string parsing routine*/
	int i;
	ui4 j,k;
	i1 thebuf[65536];


    theOpen.lStructSize = sizeof(OPENFILENAME);

    if (inOwner==NULL)
        theOpen.hwndOwner=_LApplication_gGlobal.hwndFrame;
    else
        theOpen.hwndOwner=inOwner->mHwnd;

    theOpen.hInstance = 0;

	/*we should parse the filter string here...*/
	if (inFilters==NULL)
		theOpen.lpstrFilter = NULL;
	else
	{
		/*initializes position in buffer and array*/
		k=0;
		i=0;
		while (i<=2*inNumFilters-2)
		{
			/*gets description length*/
			j=LString_Len (inFilters[i]);
			/*copies it*/
			LString_Copy (inFilters[i], &thebuf[k]);
			/*increments pos in buffer (takes the terminator into account) and array*/
			k=k+j+1;
			i++;
			/*gets filter length*/
			j=LString_Len (inFilters[i]);
			/*copies it*/
			LString_Copy (inFilters[i], &thebuf[k]);
			/*increments pos in buffer (takes the terminator into account) and array*/
			k=k+j+1;
			i++;
		}
		/*adds final terminator*/
		thebuf[k]='\0';

		theOpen.lpstrFilter = thebuf;
	}

    /*theOpen.lpstrFilter = inFilters;*/
    theOpen.lpstrCustomFilter = NULL;
    theOpen.nMaxCustFilter = 0;
    theOpen.nFilterIndex = 0;
  
    theOpen.lpstrFile = inBuffer;
    theOpen.nMaxFile = LFile_MAX_PATHNAME_LEN;
  
    theOpen.lpstrFileTitle = NULL;
    theOpen.nMaxFileTitle = 512;
    theOpen.lpstrInitialDir = NULL; 
    theOpen.lpstrTitle = NULL;
    theOpen.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
    theOpen.nFileOffset = 0;
    theOpen.nFileExtension = 0;
    theOpen.lCustData = 0;
    theOpen.lpfnHook = NULL;
    theOpen.lpTemplateName = NULL;
    theOpen.lpstrDefExt=NULL;

	theaux=(Bool) GetSaveFileName (&theOpen);
	
	if (theaux==TRUE)
		return theaux;

	thecode=CommDlgExtendedError ();
	if (thecode!=0)
			Throw(LDialog_CANT_CREATE);

	return theaux;
    /*return (Bool) GetSaveFileName (&theOpen);*/
}

/*-----------------------------------------
// GetOpenFileName ()
//----------------------------------------*/
Bool LDialog_GetOpenFileName (LWindow* inOwner, i1* inFilters[], ui1 inNumFilters, i1 inBuffer[LFile_MAX_PATHNAME_LEN])
{
    OPENFILENAME theOpen;
	Bool theaux;
	DWORD thecode;

	/*required by filter string parsing routine*/
	int i;
	ui4 j,k;
	i1 thebuf[65536];


    theOpen.lStructSize = sizeof(OPENFILENAME);

    if (inOwner==NULL)
        theOpen.hwndOwner=_LApplication_gGlobal.hwndFrame;
    else
        theOpen.hwndOwner=inOwner->mHwnd;

    theOpen.hInstance = 0;
    /*we should parse the filter string here...*/
	if (inFilters==NULL)
		theOpen.lpstrFilter = NULL;
	else
	{
		/*initializes position in buffer and array*/
		k=0;
		i=0;
		while (i<=2*inNumFilters-2)
		{
			/*gets description length*/
			j=LString_Len (inFilters[i]);
			/*copies it*/
			LString_Copy (inFilters[i], &thebuf[k]);
			/*increments pos in buffer (takes the terminator into account) and array*/
			k=k+j+1;
			i++;
			/*gets filter length*/
			j=LString_Len (inFilters[i]);
			/*copies it*/
			LString_Copy (inFilters[i], &thebuf[k]);
			/*increments pos in buffer (takes the terminator into account) and array*/
			k=k+j+1;
			i++;
		}
		/*adds final terminator*/
		thebuf[k]='\0';

		theOpen.lpstrFilter = thebuf;
	}

    /*theOpen.lpstrFilter = inFilters;*/
    theOpen.lpstrCustomFilter = NULL;
    theOpen.nMaxCustFilter = 0;
    theOpen.nFilterIndex = 0;
  
    theOpen.lpstrFile = inBuffer;
    theOpen.nMaxFile = LFile_MAX_PATHNAME_LEN;
  
    theOpen.lpstrFileTitle = NULL;
    theOpen.nMaxFileTitle = 512;
    theOpen.lpstrInitialDir = NULL; 
    theOpen.lpstrTitle = NULL;
    theOpen.Flags = OFN_HIDEREADONLY;
    theOpen.nFileOffset = 0;
    theOpen.nFileExtension = 0;
    theOpen.lCustData = 0;
    theOpen.lpfnHook = NULL;
    theOpen.lpTemplateName = NULL;
    theOpen.lpstrDefExt=NULL;

	theaux=(Bool) GetOpenFileName (&theOpen);
	
	if (theaux==TRUE)
		return theaux;

	thecode=CommDlgExtendedError ();
	if (thecode!=0)
			Throw(LDialog_CANT_CREATE);

	return theaux;
    /*return (Bool) GetOpenFileName (&theOpen);*/
}

/*--------------------------------------------
// ColorPicker ()
//-------------------------------------------*/
ui4 LDialog_ColorPicker (LWindow* inOwner)
{
	Bool theaux;
	DWORD thecode;

    (_LApplication_gGlobal.cc).lStructSize=sizeof (CHOOSECOLOR);

    if (inOwner==NULL)
        (_LApplication_gGlobal.cc).hwndOwner=_LApplication_gGlobal.hwndFrame;
    else
        (_LApplication_gGlobal.cc).hwndOwner=inOwner->mHwnd;
    
    (_LApplication_gGlobal.cc).hInstance=NULL;
    (_LApplication_gGlobal.cc).lpCustColors=_LApplication_gGlobal.crCustColors;
    (_LApplication_gGlobal.cc).Flags=CC_RGBINIT|CC_FULLOPEN;
    (_LApplication_gGlobal.cc).lCustData=0;
    (_LApplication_gGlobal.cc).lpfnHook;
    (_LApplication_gGlobal.cc).lpTemplateName=NULL;

	theaux=ChooseColor (&_LApplication_gGlobal.cc);
	if (theaux==TRUE)
		return (ui4) (_LApplication_gGlobal.cc).rgbResult;

	thecode=CommDlgExtendedError ();
	if (thecode!=0)
			Throw(LDialog_CANT_CREATE);

	return 0xffffffff; /*not a valid color!*/

    /*if (ChooseColor (&_LApplication_gGlobal.cc)!=0)
        return (ui4) (_LApplication_gGlobal.cc).rgbResult;
    else
        return 0xffffffff;*/
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

