/* ============================================================================
 *  File: window.c
 *  LL Sample Program - Topic: Windows
 * ============================================================================
*/

/*includes LL's own function prototypes and types definitions...*/
#include "LApplication.h"

/*menus...*/
LMenu *file, *help;

/*menu item IDs...*/
ui4 idexit=0, idabout=1, idnewwin=2;

/*menu item handlers...*/
void myNewWinHandler (void* inparam);
void myExitMenuHandler (void* inparam);
void myAboutMenuHandler (void* inparam);

/*initialization function (LL-based applications' entry point)...*/ 
void LApplication_Init ()
{
	/*creates menus*/
	file=LMenu_Create ("&File");
	help=LMenu_Create ("&Help");
	
	/*appends some items*/
	LMenu_AppendMenuItem (file, "&New Window", idnewwin, "N", myNewWinHandler, NULL);
	LMenu_AppendMenuSeparator (file);
	LMenu_AppendMenuItem (file, "E&xit", idexit, "Q", myExitMenuHandler, NULL);
	LMenu_AppendMenuItem (help, "&About...", idabout, NULL, myAboutMenuHandler, NULL);

	/*appends menus to menubar*/
	LMenu_AppendMenuToMenuBar (file);
	LMenu_AppendMenuToMenuBar (help);

	/*makes the menubar visible*/
	LMenu_ShowMenuBar ();

	/*sets main window's title*/
	LApplication_SetTitle ("LL Sample Program - Topic: Windows");
}

/*"new window" handler*/
void myNewWinHandler (void* inparam)
{
	LWindow *thew, *thew2;
	ui4 thex, they;

	/*creates a new window...*/
	thew=LWindow_Create (LWindow_REGULAR, LWindow_Resizeable|LWindow_Caption|LWindow_MinMaxBox);
	
	/*...and places it relatively to front window:*/
	/*gets front window, if any...*/
	thew2=LWindow_GetFrontWindow ();
	/*if front window exists...*/
	if (thew2!=NULL) 
	{
		/*...gets its position...*/
		LWindow_GetPosition (thew2, &thex, &they);
		/*...and places newly created window relatively to it*/
		LWindow_Move (thew, thex+20, they+20);
	}

	/*brings it to front...*/
	LWindow_BringToFront (thew);
	/*...and finally shows it*/
	LWindow_Finalize (thew);	
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
	LDialog_DisplayMessageBox (NULL, "LL Sample Program - Topic: Windows", "About...",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}
