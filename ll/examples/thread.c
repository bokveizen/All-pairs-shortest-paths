/* ============================================================================
 *  File: menu.c
 *  LL Sample Program - Topic: Menus
 * ============================================================================
*/

/*includes LL's own function prototypes and types definitions...*/
#include "LApplication.h"

/*menus...*/
LMenu *file, *help;

/*menu item IDs...*/
ui4 idexit=0, idabout=1;

/*menu item handlers...*/
void myExitMenuHandler (void* inparam);
void myAboutMenuHandler (void* inparam);

/*threads...*/
LWindow *tw0;
LDraw *td0;
LPushButton *tp0, *tp1;
LThread *t0, *t1;

Bool myThreadCloseHandler (LWindow* inWindow);
void th0 (void *inParam);
void myThreadSuspend (LPushButton *inButton);
void myThreadResume (LPushButton *inButton);

/*initialization function (LL-based applications' entry point)...*/ 
void LApplication_Init ()
{
	/*creates menus*/
	file=LMenu_Create ("&File");
	help=LMenu_Create ("&Help");
	
	/*appends some items*/
	LMenu_AppendMenuItem (file, "E&xit", idexit, "Q", myExitMenuHandler, NULL);
	LMenu_AppendMenuItem (help, "&About...", idabout, NULL, myAboutMenuHandler, NULL);

	/*appends menus to menubar*/
	LMenu_AppendMenuToMenuBar (file);
	LMenu_AppendMenuToMenuBar (help);

	/*makes the menubar visible*/
	LMenu_ShowMenuBar ();

	/*sets main window's title*/
	LApplication_SetTitle ("LL Sample Program - Topic: Menus");

	/*creates a window*/
	tw0=LWindow_Create (LWindow_REGULAR, LWindow_Caption);
	LWindow_SetTitle (tw0, "New Thread");
	LWindow_Resize (tw0, 300, 200);
	LWindow_Move (tw0, 100, 100);
	LWindow_Finalize (tw0);
	/*creates an ldraw object*/
	td0=LDraw_CreateLocal (tw0, 100, 0, 200, 200, 50, 50, TRUE);
	/*installs a "close" event handler*/
	LWindow_InstallEvent (tw0, LWindow_CLOSE, (LWindow_THandler) myThreadCloseHandler);
	/*creates "suspend" and "resume" buttons*/
	tp0=LPushButton_Create (tw0, 20, 10, 60, 20, "Suspend", NULL, myThreadSuspend);
	LPushButton_SetFont (tp0, LPushButton_ANSIVAR);
	tp1=LPushButton_Create (tw0, 20, 70, 60, 20, "Resume", NULL, myThreadResume);
	LPushButton_SetFont (tp1, LPushButton_ANSIVAR);
	
	/*finally launches the new thread*/
	t0=LThread_New ((LThread_THandler)th0, NULL);
}

/*"exit" handler*/
void myExitMenuHandler (void* inparam)
{
	/*terminates the application*/
	LApplication_Quit ();
}

/*"about" handler*/
void myAboutMenuHandler (void* inparam)
{
	/*displays a message box*/
	LDialog_DisplayMessageBox (NULL, "LL Sample Program - Topic: Menus", "About...",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

/*thread handlers...*/

/*"close" window event handler*/
Bool myThreadCloseHandler (LWindow* inWindow)
{
	/*terminates the thread*/
	LThread_Delete (t0);
	/*default handling (i.e. close window)*/
	return FALSE;
}

/*thread function - displays a flashing square*/
void th0 (void *inParam)
{
	ui4 color=0;
	
	while (1)
	{
		if (color>0x00ffffff)
			color=0;
		LDraw_SetBrush (td0, color);
		LDraw_SaturateBuffer (td0);
		LDraw_Update (td0);
		color=color+10;
	}
}

/*suspends the thread*/
void myThreadSuspend (LPushButton *inButton)
{
	LThread_Suspend (t0);
}

/*resumes the thread*/
void myThreadResume (LPushButton *inButton)
{
	LThread_Resume (t0);
}
