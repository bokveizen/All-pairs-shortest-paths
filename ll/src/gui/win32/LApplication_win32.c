/* ==============================================================================
 *  LApplication_win32.c
 * ==============================================================================

 *  Author:         (c) 2001 Andrea Ribichini
 *  License:        See the end of this file for license information
 *  Created:        Dec 20, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2003/05/19 15:19:08 $
 *  Changed by:     $Author: ribbi $
 *  Revision:       $Revision: 1.2 $
*/

#include "LApplication.h"
#include "LWindow_win32.h"
#include "LPushButton_win32.h"
#include "LDraw_win32.h"
#include "LListBox_win32.h"
#include "LScrollBar_win32.h"
#include "LRadioButton_win32.h"
#include "LCheckBox_win32.h"
#include "LMenu_win32.h"
#include "LBitmap_win32.h"
#include "LToolbar_win32.h"

/*for debugging purposes only...*/
#include <stdio.h>

#if __LSL_OS_GUI__ == __LSL_Win32__

/* GLOBAL VARIABLES & FUNCTION PROTOTYPES*/
LRESULT CALLBACK _FrameWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK _ChildWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK _DrawWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK _CloseEnumProc (HWND inhwnd, LPARAM inlparam);
BOOL CALLBACK _WidgetsEnumProc (HWND inhwnd, LPARAM inlparam);

static int sCmdShow;
static MSG sMsg;

/*---------------------------------------------------------------------------------
// WinMain
//---------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, 
                   LPSTR lpszArgs, int nWinMode)
{
    /*for debugging purposes only...*/
    ui1 debug[2048];

    _LApplication_gGlobal.inInstance=hThisInst;
    _LApplication_gGlobal.inArgs=(i1*) lpszArgs;
    sCmdShow=nWinMode;

    _LApplication_InitWindowManager();
    LApplication_Init();
    _LApplication_Run();

    /*for debugging purposes only...*/
    sprintf ((char *) debug, "%d", LMemory_GetBlocksCount());
	if (_LApplication_gGlobal.checkblockscount)
		MessageBox (0, (char *) debug, "LMemory_GetBlocksCount() says:", MB_OK|MB_ICONINFORMATION);

    return sMsg.wParam;
}

/*---------------------------------------------------------------------------------
// InitWindowManager
//---------------------------------------------------------------------------------*/
void _LApplication_InitWindowManager()
{
    int i;

    WNDCLASS thewndclass;   
    HWND theDesktophwnd;
    HDC thehdc;
    RECT theDesktop;

	LFile* theiconfile;
	ui4 theiconfilesize;
	void* theiconfiledata;
	LBitmap* theiconbitmap;
	ICONINFO theiconinfo;
	HBITMAP theiconmask;
	void* themaskdata;

	/*resets CheckBlocksCount() enable flag...*/
	_LApplication_gGlobal.checkblockscount=FALSE;

	/*loads application icon...*/
	/*does file exist?*/
	if (LFile_Exists ("appicon.bmp"))
	{
		/*opens it*/
		theiconfile=LFile_Open ("appicon.bmp", LFile_READ);
		/*gets size*/
		theiconfilesize=LFile_GetSize (theiconfile);
		/*allocates memory*/
		theiconfiledata=LMemory_Malloc (theiconfilesize);
		/*reads file*/
		LFile_Read (theiconfile, theiconfiledata, theiconfilesize);
		/*creates the bitmap*/
		theiconbitmap=LBitmap_CreateFromBMP (theiconfiledata, theiconfilesize);
		/*closes file*/
		LFile_Close (&theiconfile);
		/*deallocates memory*/
		LMemory_Free (&theiconfiledata);

		/*allocates memory for mask data...*/
		themaskdata=LMemory_Malloc (128);
		/*sets bits to 1...*/
		LMemory_Set (themaskdata, (i1) 0, 128);
		/*creates the icon mask...*/
		theiconmask=CreateBitmap (32, 32, 1, 1, themaskdata);
		/*fills ICONINFO struct...*/
		theiconinfo.fIcon=TRUE;
		theiconinfo.hbmMask=theiconmask;
		theiconinfo.hbmColor=theiconbitmap->mHbitmap;
		/*...and creates the icon*/
		_LApplication_gGlobal.appicon=CreateIconIndirect (&theiconinfo);

		/*destroys the bitmap...*/
		LBitmap_Destroy (theiconbitmap);
		/*and its mask*/
		DeleteObject (theiconmask);
		/*deallocates mask data*/
		LMemory_Free (&themaskdata);
	}
	else
		_LApplication_gGlobal.appicon=NULL;


/*cretes the frame window class*/
    thewndclass.style=CS_OWNDC;
    thewndclass.lpfnWndProc=_FrameWndProc;
    thewndclass.cbClsExtra=0;
    thewndclass.cbWndExtra=0;
    thewndclass.hInstance=_LApplication_gGlobal.inInstance;
	thewndclass.hIcon=_LApplication_gGlobal.appicon;
    /*thewndclass.hIcon=LoadIcon (NULL, IDI_APPLICATION);*/
	/*thewndclass.hIcon=LoadIcon (_LApplication_gGlobal.inInstance, MAKEINTRESOURCE (101));*/
    thewndclass.hCursor=LoadCursor (NULL, IDC_ARROW);
    thewndclass.hbrBackground=(HBRUSH) (COLOR_APPWORKSPACE+1);
    thewndclass.lpszMenuName=NULL;
    thewndclass.lpszClassName="FrameWindow";

    RegisterClass (&thewndclass);

/*creates the child window class*/
    thewndclass.style=CS_OWNDC;
    thewndclass.lpfnWndProc=_ChildWndProc;
    thewndclass.cbClsExtra=0;
    thewndclass.cbWndExtra=40;
    thewndclass.hInstance=_LApplication_gGlobal.inInstance;
	thewndclass.hIcon=_LApplication_gGlobal.appicon;
    /*thewndclass.hIcon=LoadIcon (NULL, IDI_APPLICATION);*/
    thewndclass.hCursor=NULL;/*LoadCursor (NULL, IDC_ARROW);*/
    /*thewndclass.hbrBackground=(HBRUSH) (COLOR_APPWORKSPACE-1);*/
	thewndclass.hbrBackground=(HBRUSH) (COLOR_BTNFACE+1);
    thewndclass.lpszMenuName=NULL;
    thewndclass.lpszClassName="ChildWindow";

    RegisterClass (&thewndclass);

/*creates the draw window class*/
    thewndclass.style=CS_OWNDC;
    thewndclass.lpfnWndProc=_DrawWndProc;
    thewndclass.cbClsExtra=0;
    thewndclass.cbWndExtra=40;
    thewndclass.hInstance=_LApplication_gGlobal.inInstance;
    thewndclass.hIcon=LoadIcon (NULL, IDI_APPLICATION);
    thewndclass.hCursor=NULL;/*LoadCursor (NULL, IDC_ARROW);*/
    thewndclass.hbrBackground=(HBRUSH) (COLOR_APPWORKSPACE-1);
    thewndclass.lpszMenuName=NULL;
    thewndclass.lpszClassName="DrawWindow";

    RegisterClass (&thewndclass);

/*creates the frame window*/
    theDesktophwnd = GetDesktopWindow();
    GetClientRect (theDesktophwnd, &theDesktop);
   
	/*required by frame win proc*/ 
	_LApplication_gGlobal.hwndClient=NULL;

    _LApplication_gGlobal.hwndFrame=CreateWindow ("FrameWindow", "LL Main Window",
        WS_OVERLAPPEDWINDOW,
        0, 0,
        theDesktop.right, theDesktop.bottom-28,
        NULL, NULL, _LApplication_gGlobal.inInstance, NULL);

	/*required by LApplication_GetFocus()*/
	SetWindowLong (_LApplication_gGlobal.hwndFrame, GWL_USERDATA, (LONG) NULL);

/*creates the mdiclient window*/
    _LApplication_gGlobal.clientcreate.hWindowMenu=NULL;
    _LApplication_gGlobal.clientcreate.idFirstChild=2;

    _LApplication_gGlobal.hwndClient=CreateWindow ("MDICLIENT", NULL,
        WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE|MDIS_ALLCHILDSTYLES|WS_CLIPSIBLINGS,
        0, 0, theDesktop.right, theDesktop.bottom-28, _LApplication_gGlobal.hwndFrame, (HMENU) 1, _LApplication_gGlobal.inInstance,
        (LPVOID) &_LApplication_gGlobal.clientcreate);

	/*required by LApplication_GetFocus()*/
	SetWindowLong (_LApplication_gGlobal.hwndClient, GWL_USERDATA, (LONG) NULL);

/*creates the menubar...*/
    _LApplication_gGlobal.MenuBar=CreateMenu ();
    /*initializes menu item handler array*/
    for (i=0; i<LApplication_MAX_MENU_ITEMS; i++)
        _LApplication_gGlobal.MenuItemHandler[i]=NULL;

/*initializes accelerator table...*/
    _LApplication_gGlobal.haccel=NULL;

/*initializes mutual reference array - required by menu init handlers & auto clean up*/
    _LApplication_gGlobal.MenuRef=LArray_New (sizeof (LApplication_TMenuRef));

/*initializes common controls...*/
    InitCommonControls ();

/*initializes the global buffer used by LDraw...*/
    thehdc=GetDC (theDesktophwnd);
    _LApplication_gGlobal.gHBitmap=CreateCompatibleBitmap (thehdc, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN));
    ReleaseDC (theDesktophwnd, thehdc);

/*initializes global timer handler and drag'n'drop handler...*/
    _LApplication_gGlobal.TimerHandler=NULL;
    _LApplication_gGlobal.DropHandler=NULL;

/*initializes color picker...*/
    (_LApplication_gGlobal.cc).rgbResult=0x00f08080;

/*initializes toolbar...*/
	_LApplication_gGlobal.Toolbar=NULL;

}

/*---------------------------------------------------------------------------------
// Run
//---------------------------------------------------------------------------------*/
void _LApplication_Run() 
{
/*displays the frame window*/
    ShowWindow (_LApplication_gGlobal.hwndFrame, sCmdShow);
    UpdateWindow (_LApplication_gGlobal.hwndFrame);

/*standard message loop...plus MDI accelerators...plus menu accelerators!*/
    while (GetMessage (&sMsg, NULL, 0,0))
    {
        if (!TranslateMDISysAccel (_LApplication_gGlobal.hwndClient, &sMsg) &&
            !TranslateAccelerator (_LApplication_gGlobal.hwndFrame, _LApplication_gGlobal.haccel, &sMsg))
        {
            TranslateMessage (&sMsg);
            DispatchMessage (&sMsg);
        }
    }
	/*destroy accelerator table (if present)...*/
    DestroyAcceleratorTable (_LApplication_gGlobal.haccel);
}

/*----------------------------------------------------------------------
// SetTitle
//----------------------------------------------------------------------*/
void    LApplication_SetTitle(const i1* inTitle)
{
    SetWindowText (_LApplication_gGlobal.hwndFrame, inTitle);
}

/*----------------------------------------------------------------------
// GetTitle
//----------------------------------------------------------------------*/
void LApplication_GetTitle(i1* outTitle)
{
    GetWindowText (_LApplication_gGlobal.hwndFrame, outTitle, LApplication_MAX_TITLE); 
}

/*----------------------------------------------------------------------
// GetWorkspaceSize
//----------------------------------------------------------------------*/
void LApplication_GetWorkspaceSize(ui4 *outWidth, ui4 *outHeight)
{
    RECT theaux;
    
    GetClientRect (_LApplication_gGlobal.hwndFrame, &theaux);

    *outWidth=theaux.right;
    *outHeight=theaux.bottom;   
}

/*----------------------------------------------------------------------
// Quit
//----------------------------------------------------------------------*/
void LApplication_Quit()
{
    SendMessage (_LApplication_gGlobal.hwndFrame, WM_CLOSE, 0, 0);
}

/*----------------------------------------------------------------------
// WindowLayout
//----------------------------------------------------------------------*/
void LApplication_WindowLayout (LApplication_TLayout inLayout)
{
    switch (inLayout)
    {
    case LApplication_CASCADE:
        SendMessage (_LApplication_gGlobal.hwndClient, WM_MDICASCADE, 0, 0);
        break;
    case LApplication_TILE_VERTICALLY:
        SendMessage (_LApplication_gGlobal.hwndClient, WM_MDITILE, MDITILE_VERTICAL, 0);
        break;
    case LApplication_TILE_HORIZONTALLY:
        SendMessage (_LApplication_gGlobal.hwndClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
        break;
    }
}

/*-------------------------------------------------------------------
// SetTimer 
//-----------------------------------------------------------------*/
void LApplication_SetTimer (ui4 inTicks, LApplication_THandler inHandler)
{
    if (inHandler==NULL) /*we are uninstalling the timer*/ 
        if (_LApplication_gGlobal.TimerHandler==NULL) /*there is no previously installed timer!*/
            return; /*nothing to do...*/
        else /*there is a timer installed...*/ 
        {
            KillTimer (_LApplication_gGlobal.hwndFrame, 1); /*kills the timer*/
            _LApplication_gGlobal.TimerHandler=inHandler; /*uninstalls the handler*/
            return; /*we are done*/
        }

    /*if we get here we are installing or reinstalling a timer!*/

    if (_LApplication_gGlobal.TimerHandler==NULL) /*no timer is currently installed*/
    {
        _LApplication_gGlobal.TimerHandler=inHandler;
        SetTimer (_LApplication_gGlobal.hwndFrame, 1, inTicks, NULL);
    }
    else /*there's a timer already installed*/
    {
        KillTimer (_LApplication_gGlobal.hwndFrame, 1);
        _LApplication_gGlobal.TimerHandler=inHandler;
        SetTimer (_LApplication_gGlobal.hwndFrame, 1, inTicks, NULL);
    }   
}

/*-----------------------------------
// GetArgs
//----------------------------------*/
i1* LApplication_GetArgs ()
{
    return _LApplication_gGlobal.inArgs;
}

/*-----------------------------------
// SetDropHandler
//----------------------------------*/
void LApplication_SetDropHandler (LApplication_TDropHandler inHandler)
{
	if (inHandler==NULL)
	{
		DragAcceptFiles (_LApplication_gGlobal.hwndFrame, 0);/*disables drag'n'drop*/
		_LApplication_gGlobal.DropHandler=NULL;
	}
	else
	{
		_LApplication_gGlobal.DropHandler=inHandler;
		DragAcceptFiles (_LApplication_gGlobal.hwndFrame, 1);/*enables drag'n'drop*/
	}
}

/*----------------------------------------------------------------
// GetFocus
//---------------------------------------------------------------*/
Bool LApplication_GetFocus (ui2* outObjID, void** outObj)
{
	HWND thehwnd;
	LWindow* theaux;/*just a dummy...any widget would do*/

	thehwnd=GetFocus();

	if (thehwnd==NULL) /*current thread's message queue doesn't have the focus*/
		return FALSE;

	/*retrieves mutual reference pointer*/
	theaux=(LWindow*) GetWindowLong (thehwnd, GWL_USERDATA);

	/*either the frame or the mdiclient has the focus*/
	if (theaux==NULL)
		return FALSE;

	/*at this point it's either a window or a widget*/
	/*returns objID & pointer*/
	*outObjID=(ui2) theaux->mID;
	*outObj=(void *) theaux;
	return TRUE;
}

/*-----------------------------------------------------
// SetCheckBlocksCount ()
//----------------------------------------------------*/
void LApplication_SetCheckBlocksCount (Bool inStatus)
{
	_LApplication_gGlobal.checkblockscount=inStatus;
}

/*----------------------------------------------------
// GetSystemInfo ()
//---------------------------------------------------*/
void LApplication_GetSystemInfo (i1* outBuffer, ui4 inBufSize)
{
	ui4 i;
	ui4 thelength;
	i1 thebuf[65536];
	i1 theaux[65536];
	OSVERSIONINFO theosverinfo;

	theosverinfo.dwOSVersionInfoSize=sizeof (OSVERSIONINFO);
	GetVersionEx (&theosverinfo);

	if (theosverinfo.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		LString_Copy ("Windows NT ", thebuf);
		sprintf (theaux, "%d", theosverinfo.dwMajorVersion);
		LString_Append (theaux, thebuf);
		LString_Append (".", thebuf);
		sprintf (theaux, "%d", theosverinfo.dwMinorVersion);
		LString_Append (theaux, thebuf);
		LString_Append (".", thebuf);
		sprintf (theaux, "%d", theosverinfo.dwBuildNumber);
		LString_Append (theaux, thebuf);
		/*LString_Append (" ", thebuf);
		LString_Append (theosverinfo.szCSDVersion, thebuf);*/
	}
	else
	{
		LString_Copy ("Windows 9x ", thebuf);
		sprintf (theaux, "%d", theosverinfo.dwMajorVersion);
		LString_Append (theaux, thebuf);
		LString_Append (".", thebuf);
		sprintf (theaux, "%d", theosverinfo.dwMinorVersion);
		LString_Append (theaux, thebuf);
		LString_Append (".", thebuf);
		sprintf (theaux, "%d", LOWORD(theosverinfo.dwBuildNumber));
		LString_Append (theaux, thebuf);
		/*LString_Append (" ", thebuf);
		LString_Append (theosverinfo.szCSDVersion, thebuf);*/
	}

	thelength=LString_Len (thebuf);

	if (thelength<inBufSize)
		for (i=0; i<=thelength; i++)
			outBuffer[i]=thebuf[i];
	else
	{
		for (i=0; i<inBufSize-1; i++)
			outBuffer[i]=thebuf[i];

		outBuffer[inBufSize]='\0';
	}
}

/*----------------------------------------------------------------------
// _FrameWndProc
//----------------------------------------------------------------------*/
LRESULT CALLBACK _FrameWndProc(HWND inHwnd, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    ui4 theMenuItem;
    LMenu_THandler theMenuItemHandler;
    LMenu_THandler theMenuInitHandler;
    LApplication_THandler thetimerhandler;
    LApplication_TDropHandler thedrophandler;
    HMENU thehmenu;
    ui4 i;
	ui4 thecount;
	LApplication_TMenuRef theref;
	LMenu* themenuptr;

	LPTOOLTIPTEXT thetooltip;

    switch (inMessage)
    {
	case WM_NOTIFY:
		thetooltip=(LPTOOLTIPTEXT) inLParam;
		if (thetooltip->hdr.code==TTN_NEEDTEXT)
		{
			/*starts searching for a matching item id...*/
			i=0;
			while (((LToolbar*)(_LApplication_gGlobal.Toolbar))->mTButtonArray[i].mItemID!=thetooltip->hdr.idFrom)
				i++;
			/*sets tooltip text*/
			thetooltip->lpszText=((LToolbar*)(_LApplication_gGlobal.Toolbar))->mTButtonArray[i].mTooltipText;
		}
		break;
	case WM_SIZE:
		/*checks whether mdi client already exists...*/
		if (_LApplication_gGlobal.hwndClient!=NULL)
		{
			/*takes the toolbar (if present) into account...*/
			if (_LApplication_gGlobal.Toolbar!=NULL)
			{
				/*resizes the toolbar!!!*/
				SendMessage (((LToolbar*)(_LApplication_gGlobal.Toolbar))->mHWND, TB_AUTOSIZE, 0, 0);
				/*resizes the mdi client...*/
				MoveWindow (_LApplication_gGlobal.hwndClient, 0, 28, LOWORD(inLParam), 
					HIWORD(inLParam)-28, TRUE);
			}
			else
				/*resizes the mdi client...*/
				MoveWindow (_LApplication_gGlobal.hwndClient, 0, 0, LOWORD(inLParam), 
					HIWORD(inLParam), TRUE);
		}
		return 0;
    case WM_DROPFILES: /*cannot happen if handler is not installed...*/
    	DragQueryFile ((HDROP) inWParam, 0, _LApplication_gGlobal.DroppedFile, LApplication_MAX_PATH);
    	DragQueryPoint ((HDROP) inWParam, &_LApplication_gGlobal.DroppedPt);
    	thedrophandler=_LApplication_gGlobal.DropHandler;
    	thedrophandler (_LApplication_gGlobal.DroppedFile, (ui4) (_LApplication_gGlobal.DroppedPt).x, (ui4) (_LApplication_gGlobal.DroppedPt).y);
    	DragFinish ((HDROP) inWParam);
    	break;
    case WM_INITMENUPOPUP:
        /*gets number of menus*/
		thecount=LArray_GetItemsCount (_LApplication_gGlobal.MenuRef);
		/*gets win32 menu handle*/
        thehmenu=(HMENU) inWParam;

        for (i=0; i<thecount; i++)
        {
			/*gets current item*/
			LArray_FetchItemAt (_LApplication_gGlobal.MenuRef, i, &theref);		
			if (theref.hMenuHandle==thehmenu)
                {
					themenuptr=(LMenu*) theref.mMenu;
                    theMenuInitHandler=themenuptr->mInitHandler;
                    if (theMenuInitHandler!=NULL)
                        theMenuInitHandler (themenuptr->mParam);
                    return 0;
                }
        }
        break;
    case WM_TIMER:
        if (_LApplication_gGlobal.TimerHandler!=NULL)
        {
            thetimerhandler=_LApplication_gGlobal.TimerHandler;
            thetimerhandler ();
        }
        break;
    case WM_COMMAND:
            theMenuItem=LOWORD (inWParam);
            
/*these lines correct a very nasty bug...*/
            if (theMenuItem<0)
                break;
            if (theMenuItem>LApplication_MAX_MENU_ITEMS-1)
                break;

            if (_LApplication_gGlobal.MenuItemHandler[theMenuItem]==NULL)
                MessageBeep (MB_ICONASTERISK); /*open to change...*/
            else
            {
                /*as ugly as it may look...still it works!!!!!*/
                theMenuItemHandler=_LApplication_gGlobal.MenuItemHandler[theMenuItem];
                theMenuItemHandler (_LApplication_gGlobal.MenuItemParam[theMenuItem]);
            }
            return 0;
    case WM_QUERYENDSESSION:
    case WM_CLOSE:
        /*attempts to shut down all child windows*/
        EnumChildWindows (_LApplication_gGlobal.hwndClient, (WNDENUMPROC) _CloseEnumProc, 0);
        /*if there are still open children do nothing...*/
        if (NULL!=GetWindow (_LApplication_gGlobal.hwndClient, GW_CHILD))
            return 0;
        /*...otherwise call defframeproc...*/
        break;
    case WM_DESTROY:
        /*some clean up first...*/

        /*removes the timer, if present*/
        if (_LApplication_gGlobal.TimerHandler!=NULL)
            KillTimer (_LApplication_gGlobal.hwndFrame, 1);

        /*deletes global LDraw buffer...*/
        DeleteObject (_LApplication_gGlobal.gHBitmap);

        /*deletes the menus (if present)*/
		thecount=LArray_GetItemsCount (_LApplication_gGlobal.MenuRef);
		for (i=0; i<thecount; i++)
        {
			/*gets current item*/
			LArray_FetchItemAt (_LApplication_gGlobal.MenuRef, i, &theref);	
			/*destroys the menu*/
			DestroyMenu (theref.hMenuHandle);
			/*destroys the accelerators larray*/
			themenuptr=(LMenu*) theref.mMenu;
			LArray_Delete (&(themenuptr->mAccelerators));
			/*deletes the LMenu object*/
			LMemory_DeleteObject (&themenuptr);
        }
		/*deletes mutual reference table*/
		LArray_Delete (&_LApplication_gGlobal.MenuRef);

		/*destroys app icon, if present...*/
		if (_LApplication_gGlobal.appicon!=NULL)
			DestroyIcon (_LApplication_gGlobal.appicon);

		/*destroys toolbar (and associated bitmap), if present...*/
		if (_LApplication_gGlobal.Toolbar!=NULL)
		{
			LToolbar_Destroy ((LToolbar*) _LApplication_gGlobal.Toolbar);
		}
        /*terminates the application*/
        PostQuitMessage (0);
        return 0;
    }

    return DefFrameProc (inHwnd, _LApplication_gGlobal.hwndClient, inMessage, inWParam, inLParam);
}

/*----------------------------------------------------------------------
// _ChildWndProc
//----------------------------------------------------------------------*/
LRESULT CALLBACK _ChildWndProc (HWND inHwnd, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    LWindow* thelwaux;
    BOOL thedecider;

    LPushButton* thelpbaux;
	LCheckBox* thelcbaux;
	LRadioButton* thelrbaux;

    /*LPushButton_THandler thebhandler;*/
    LListBox* thelbaux;
    LScrollBar* thescrollbaux;
    HWND thehwndaux;
    LScrollBar_THandler thesbhandler;

	LPMINMAXINFO theminmax;

	HCURSOR thehc;

    switch (inMessage)
    {
	case WM_GETMINMAXINFO:
		/*retrieves mutual reference pointer*/
		thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
		/*if mutual reference doesn't exist yet...*/
		if (thelwaux==NULL)
			break; /*we are done...*/
		/*if min max disabled...*/
		if (thelwaux->mMinMaxEnable==FALSE)
			break; /*we are done...*/
		/*otherwise sets paramenters...*/
		theminmax=(LPMINMAXINFO) inLParam;
		theminmax->ptMinTrackSize.x=thelwaux->mMinX;
		theminmax->ptMinTrackSize.y=thelwaux->mMinY;
		theminmax->ptMaxTrackSize.x=thelwaux->mMaxX;
		theminmax->ptMaxTrackSize.y=thelwaux->mMaxY;
		break;
    case WM_VSCROLL:
    case WM_HSCROLL:
        thehwndaux=(HWND) inLParam;
        thescrollbaux=(LScrollBar*) GetWindowLong (thehwndaux, GWL_USERDATA);
        switch ((int) LOWORD(inWParam))
        {
        case SB_LINEUP:
            if (thescrollbaux->mHandler!=NULL)
            {
                thesbhandler=(LScrollBar_THandler) (thescrollbaux->mHandler);
                thesbhandler (thescrollbaux, LScrollBar_LINEUP, 0);
            }
            break;
        case SB_LINEDOWN:
            if (thescrollbaux->mHandler!=NULL)
            {
                thesbhandler=(LScrollBar_THandler) (thescrollbaux->mHandler);
                thesbhandler (thescrollbaux, LScrollBar_LINEDOWN, 0);
            }
            break;
        case SB_PAGEUP:
            if (thescrollbaux->mHandler!=NULL)
            {
                thesbhandler=(LScrollBar_THandler) (thescrollbaux->mHandler);
                thesbhandler (thescrollbaux, LScrollBar_PAGEUP, 0);
            }
            break;
        case SB_PAGEDOWN:
            if (thescrollbaux->mHandler!=NULL)
            {
                thesbhandler=(LScrollBar_THandler) (thescrollbaux->mHandler);
                thesbhandler (thescrollbaux, LScrollBar_PAGEDOWN, 0);
            }
            break;
        case SB_THUMBPOSITION:
            if (thescrollbaux->mHandler!=NULL)
            {
                thesbhandler=(LScrollBar_THandler) (thescrollbaux->mHandler);
                thesbhandler (thescrollbaux, LScrollBar_THUMBPOSITION, (ui2) ((short int) HIWORD(inWParam)));
            }
            break;
        case SB_THUMBTRACK:
            if (thescrollbaux->mHandler!=NULL)
            {
                thesbhandler=(LScrollBar_THandler) (thescrollbaux->mHandler);
                thesbhandler (thescrollbaux, LScrollBar_THUMBTRACK, (ui2) ((short int) HIWORD(inWParam)));
            }
            break;
        }
        break;

    case WM_CHILDACTIVATE:
        thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
        if (thelwaux->mActivateHandler==NULL)
            break;
        thedecider=thelwaux->mActivateHandler (thelwaux, LOWORD (inLParam), HIWORD (inLParam));
        if (thedecider==TRUE)
            return 0;
        else
            break;
    case WM_SETFOCUS:
        thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
        if (thelwaux->mSetFocusHandler==NULL)
        {
        /*things to do to give focus to the right widget...*/
            if (thelwaux->mWidgetPointer==NULL)
                return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);

            switch (thelwaux->mWidgetType)
            {
            case LPushButton_ID:
                LPushButton_SetFocus ((LPushButton*) (thelwaux->mWidgetPointer));
                return 0;
            case LTextEditor_ID:
                LTextEditor_SetFocus ((LTextEditor*) (thelwaux->mWidgetPointer));
                return 0;
            case LCheckBox_ID:
                LCheckBox_SetFocus ((LCheckBox*) (thelwaux->mWidgetPointer));
                return 0;
            case LListBox_ID:
                LListBox_SetFocus ((LListBox*) (thelwaux->mWidgetPointer));
                return 0;
            case LRadioButton_ID:
                LRadioButton_SetFocus ((LRadioButton*) (thelwaux->mWidgetPointer));
                return 0;
            case LTextField_ID:
                LTextField_SetFocus ((LTextField*) (thelwaux->mWidgetPointer));
                return 0;
            }
        }

        thedecider=thelwaux->mSetFocusHandler (thelwaux, LOWORD (inLParam), HIWORD (inLParam));
        if (thedecider==TRUE)
            return 0;
        else
        {
            if (thelwaux->mWidgetPointer==NULL)
                return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);

            switch (thelwaux->mWidgetType)
            {
            case LPushButton_ID:
                LPushButton_SetFocus ((LPushButton*) (thelwaux->mWidgetPointer));
                return 0;
            case LTextEditor_ID:
                LTextEditor_SetFocus ((LTextEditor*) (thelwaux->mWidgetPointer));
                return 0;
            case LCheckBox_ID:
                LCheckBox_SetFocus ((LCheckBox*) (thelwaux->mWidgetPointer));
                return 0;
            case LListBox_ID:
                LListBox_SetFocus ((LListBox*) (thelwaux->mWidgetPointer));
                return 0;
            case LRadioButton_ID:
                LRadioButton_SetFocus ((LRadioButton*) (thelwaux->mWidgetPointer));
                return 0;
            case LTextField_ID:
                LTextField_SetFocus ((LTextField*) (thelwaux->mWidgetPointer));
                return 0;
            }
        }
    case WM_SIZE:
        thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
        if (thelwaux!=NULL)
        {
            if (thelwaux->mStatusBarHandle!=NULL)
            {
                SendMessage (thelwaux->mStatusBarHandle, WM_SIZE, inWParam, inLParam);
            }

            if (thelwaux->mResizeHandler==NULL)
                return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);
            thedecider=thelwaux->mResizeHandler (thelwaux, LOWORD (inLParam), HIWORD (inLParam));
            if (thedecider==TRUE)
                return 0;
            else
                return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);
        }
        else
            break;
    case WM_LBUTTONDOWN:
        thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
        if (thelwaux->mMouseDownHandler==NULL)
            break;
        thedecider=thelwaux->mMouseDownHandler (thelwaux, LOWORD (inLParam), HIWORD (inLParam));
        if (thedecider==TRUE)
            return 0;
        else
            break;
    case WM_MOUSEMOVE:
		/*gets mutual reference...*/
		thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
		/*if handler installed...*/
        if (thelwaux->mMouseMoveHandler!=NULL)
			thelwaux->mMouseMoveHandler (thelwaux, LOWORD (inLParam), HIWORD (inLParam));
		
		/*sets cursor shape*/
		switch (thelwaux->mCursorShape)
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
		return 0;
        /*thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
        if (thelwaux->mMouseMoveHandler==NULL)
        {
            SetCursor (LoadCursor (NULL, IDC_ARROW));
            return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);
        }
        thedecider=thelwaux->mMouseMoveHandler (thelwaux, LOWORD (inLParam), HIWORD (inLParam));
        if (thedecider==TRUE)
            return 0;
        else
        {
            SetCursor (LoadCursor (NULL, IDC_ARROW));
            return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);
        }*/
        case WM_COMMAND:
            /*listbox...*/
            if (HIWORD (inWParam)==LBN_DBLCLK)
            {
                thelbaux=(LListBox *) GetWindowLong ((HWND) inLParam, GWL_USERDATA);
                if (thelbaux->mHandler!=NULL)
                    thelbaux->mHandler (thelbaux);
                return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);
            }
            if (HIWORD (inWParam)==LBN_ERRSPACE)
                break;
            if (HIWORD (inWParam)==LBN_KILLFOCUS)
                break;
            if (HIWORD (inWParam)==LBN_SELCANCEL)
                break;
            if (HIWORD (inWParam)==LBN_SELCHANGE)
                break;
            if (HIWORD (inWParam)==LBN_SETFOCUS)
            {
                thelbaux=(LListBox *) GetWindowLong ((HWND) inLParam, GWL_USERDATA);
                thelwaux=LListBox_GetParentWindow (thelbaux);
                thelwaux->mWidgetPointer=(void *) thelbaux;
                thelwaux->mWidgetType=LListBox_ID;
                return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);
            }
            /*pushbutton, radiobutton, checkbox, textedit...*/
            /*beware: all structs should have mHandler at same offset!*/
            thelpbaux=(LPushButton*) GetWindowLong ((HWND) inLParam, GWL_USERDATA);

            /*store focus information in LWindow...*/
            thelwaux=LPushButton_GetParentWindow (thelpbaux);
            thelwaux->mWidgetPointer=(void *) thelpbaux;
            thelwaux->mWidgetType=thelpbaux->mID;

            if (thelpbaux==NULL)
                break;
            if (thelpbaux->mHandler==NULL) /*this includes the textedit widget*/
                break;
            else
            {
				switch (thelpbaux->mID)
				{
				case LCheckBox_ID:
					thelcbaux=(LCheckBox*) thelpbaux;
					thelcbaux->mHandler (thelcbaux);
					break;
				case LPushButton_ID:
					thelpbaux->mHandler (thelpbaux);
					break;
				case LRadioButton_ID:
					thelrbaux=(LRadioButton*) thelpbaux;
					thelrbaux->mHandler (thelrbaux);
					break;
				}
            }
            return 0;
    case WM_QUERYENDSESSION:
    case WM_CLOSE:
        thelwaux=(LWindow*) GetWindowLong (inHwnd, 0);
/*if no handler has been installed, let the default kick in*/
        if (thelwaux->mCloseHandler==NULL)
            break;
/*calls the handler*/
        thedecider=thelwaux->mCloseHandler (thelwaux);
/*determines whether to pass on to default handler or return*/
        if (thedecider==TRUE)
            return 0;
        else
            break;
    case WM_DESTROY:
        /*destroys all children (i.e. widgets) present...*/
        EnumChildWindows (inHwnd, (WNDENUMPROC) _WidgetsEnumProc, 0);
/*frees memory that has been allocated upon creation of window*/
        thelwaux=(LWindow*) GetWindowLong (inHwnd, 0); /*mutual reference in slot 0...*/
        LMemory_DeleteObject (&thelwaux);
        break; /*i.e. call defmdichildproc*/
    }

    return DefMDIChildProc (inHwnd, inMessage, inWParam, inLParam);
}

/*-------------------------------------------------------------
// _WidgetsEnumProc
//------------------------------------------------------------*/
BOOL CALLBACK _WidgetsEnumProc (HWND inhwnd, LPARAM inlparam)
{
    LPushButton *theauxw;

    theauxw=(LPushButton *) GetWindowLong (inhwnd, GWL_USERDATA);

    switch (theauxw->mID)
    {
    case LCheckBox_ID:
        LCheckBox_Destroy ((LCheckBox*) theauxw);
        break;
    case LDraw_ID:
        LDraw_Destroy ((LDraw*) theauxw);
        break;
    case LGroupBox_ID:
        LGroupBox_Destroy ((LGroupBox*) theauxw);
        break;
    case LListBox_ID:
        LListBox_Destroy ((LListBox*) theauxw);
        break;
    case LProgressBar_ID:
        LProgressBar_Destroy ((LProgressBar*) theauxw);
        break;
    case LPushButton_ID:
        LPushButton_Destroy ((LPushButton*) theauxw);
        break;
    case LRadioButton_ID:
        LRadioButton_Destroy ((LRadioButton*) theauxw);
        break;
    case LStaticText_ID:
        LStaticText_Destroy ((LStaticText*) theauxw);
        break;
    case LStatusBar_ID:
        LStatusBar_Destroy ((LStatusBar*) theauxw);
        break;
    case LTextEditor_ID:
        LTextEditor_Destroy ((LTextEditor*) theauxw);
        break;
    case LTextField_ID:
        LTextField_Destroy ((LTextField*) theauxw);
        break;
    case LScrollBar_ID:
        LScrollBar_Destroy ((LScrollBar*) theauxw);
        break;
    case LSizeBox_ID:
        LSizeBox_Destroy ((LSizeBox*) theauxw);
        break;
    }

    return TRUE;
}

/*----------------------------------------------------------------------
// _CloseEnumProc
//----------------------------------------------------------------------*/  
BOOL CALLBACK _CloseEnumProc (HWND inhwnd, LPARAM inlparam) 
{
    LWindow* thelwaux;

    if (GetWindow (inhwnd, GW_OWNER))
        return TRUE;

    thelwaux=(LWindow*) GetWindowLong (inhwnd, 0);
    /*this way window has to be restored manually during close event...*/
    /*if (thelwaux->mCloseHandler!=NULL)
        SendMessage (GetParent (inhwnd), WM_MDIRESTORE, (WPARAM) inhwnd, 0);*/

    if (!SendMessage (inhwnd, WM_QUERYENDSESSION, 0, 0))
        return FALSE;

    SendMessage (GetParent (inhwnd), WM_MDIDESTROY, (WPARAM) inhwnd, 0);
    return TRUE;
}

/*---------------------------------------------------------------------
// _DrawWndProc
//--------------------------------------------------------------------*/
LRESULT CALLBACK _DrawWndProc (HWND inHwnd, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    LDraw* theldaux;
    BOOL thedecider;
    HDC thehdc;
    HBITMAP thehbit;
    PAINTSTRUCT thepaintstruct;
	HCURSOR thehc;

    switch (inMessage)
    {
    case WM_PAINT:
        theldaux=(LDraw*) GetWindowLong (inHwnd, GWL_USERDATA);
        if (theldaux->mType==LDraw_LOCAL)
        {
            if (theldaux->mRefreshHandler!=NULL)
            {
                thedecider=theldaux->mRefreshHandler (theldaux);
                if (thedecider==TRUE)
                    return DefWindowProc (inHwnd, inMessage, inWParam, inLParam);
            }
            thehdc=BeginPaint (inHwnd, &thepaintstruct);
            BitBlt (thehdc, theldaux->Ox, theldaux->Oy, theldaux->mmaxX, theldaux->mmaxY, theldaux->mMemHdc, 0, 0, SRCCOPY);
            EndPaint (inHwnd, &thepaintstruct);
        }
        if (theldaux->mType==LDraw_GLOBAL)
        {
            thehbit=SelectObject (theldaux->mMemHdc, _LApplication_gGlobal.gHBitmap);
            if (theldaux->mRefreshHandler!=NULL)
            {
                thedecider=theldaux->mRefreshHandler (theldaux);
                if (thedecider==TRUE)
                {
                    SelectObject (theldaux->mMemHdc, thehbit);
                    return DefWindowProc (inHwnd, inMessage, inWParam, inLParam);
                }
            }
            thehdc=BeginPaint (inHwnd, &thepaintstruct);
            BitBlt (thehdc, theldaux->Ox, theldaux->Oy, theldaux->mmaxX, theldaux->mmaxY, theldaux->mMemHdc, 0, 0, SRCCOPY);
            EndPaint (inHwnd, &thepaintstruct);
            SelectObject (theldaux->mMemHdc, thehbit);
        }

        break;
    case WM_LBUTTONDOWN:
        theldaux=(LDraw*) GetWindowLong (inHwnd, GWL_USERDATA);
        if (theldaux->mMouseDownHandler==NULL)
            break;
        thedecider=theldaux->mMouseDownHandler (theldaux, LOWORD (inLParam), HIWORD (inLParam));
        if (thedecider==TRUE)
            return 0;
        else
            break;
    case WM_MOUSEMOVE:
		/*gets mutual reference...*/
		theldaux=(LDraw*) GetWindowLong (inHwnd, GWL_USERDATA);
		/*if handler installed...*/
        if (theldaux->mMouseMoveHandler!=NULL)
			theldaux->mMouseMoveHandler (theldaux, LOWORD (inLParam), HIWORD (inLParam));
		
		/*sets cursor shape*/
		switch (theldaux->mCursorShape)
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
		return 0;

        /*theldaux=(LDraw*) GetWindowLong (inHwnd, GWL_USERDATA);
        if (theldaux->mMouseMoveHandler==NULL)
        {
            SetCursor (LoadCursor (NULL, IDC_ARROW));
            return DefWindowProc (inHwnd, inMessage, inWParam, inLParam);
        }
        thedecider=theldaux->mMouseMoveHandler (theldaux, LOWORD (inLParam), HIWORD (inLParam));
        if (thedecider==TRUE)
            return 0;
        else
        {
            SetCursor (LoadCursor (NULL, IDC_ARROW));
            return DefWindowProc (inHwnd, inMessage, inWParam, inLParam);
        }*/
    case WM_DESTROY:
/*frees memory that has been allocated upon creation of window*/
        theldaux=(LDraw*) GetWindowLong (inHwnd, GWL_USERDATA); /*mutual reference pointer...*/
        DeleteDC (theldaux->mMemHdc);
        if (theldaux->mLocalBuffer!=NULL)
            DeleteObject (theldaux->mLocalBuffer);
        LMemory_DeleteObject (&theldaux);
        break;
    }

    return DefWindowProc (inHwnd, inMessage, inWParam, inLParam);
}

#endif

/* Copyright (C) 2001 Andrea Ribichini

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
