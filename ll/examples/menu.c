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
