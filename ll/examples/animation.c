/* ============================================================================
 *  File: animation.c
 *  LL Sample Program - Topic: Animation
 * ============================================================================
*/

/*includes LL's own function prototypes and types definitions...*/
#include "LApplication.h"

/*menus...*/
LMenu *file, *anim, *help;

/*menu item IDs...*/
ui4 idexit=0, idabout=1, idstart=2, idstop=3;

/*menu item handlers...*/
void myExitMenuHandler (void* inparam);
void myAboutMenuHandler (void* inparam);
void myAnimStartHandler (void* inparam);
void myAnimStopHandler (void* inparam);

/*timer handlers*/
void myAnimRoutine (void);

/*windows*/
LWindow *animw;

/*ldraws*/
LDraw *animdrw;

/*other vars*/
ui4 dx, dy;

/*used by animation routine...*/
i4 animX=50, animY=50, animVX=2, animVY=3;
ui4 animMaxX, animMaxY;
Bool animON=TRUE;

/*initialization function (LL-based applications' entry point)...*/ 
void LApplication_Init ()
{
	/*creates menus*/
	file=LMenu_Create ("&File");
	anim=LMenu_Create ("&Animation");
	help=LMenu_Create ("&Help");
	
	/*appends some items*/
	LMenu_AppendMenuItem (file, "E&xit", idexit, "Q", myExitMenuHandler, NULL);
	LMenu_AppendMenuItem (anim, "S&tart", idstart, "T", myAnimStartHandler, NULL);
	LMenu_AppendMenuItem (anim, "Sto&p", idstop, "P", myAnimStopHandler, NULL);
	LMenu_AppendMenuItem (help, "&About...", idabout, NULL, myAboutMenuHandler, NULL);

	/*appends menus to menubar*/
	LMenu_AppendMenuToMenuBar (file);
	LMenu_AppendMenuToMenuBar (anim);
	LMenu_AppendMenuToMenuBar (help);

	/*makes the menubar visible*/
	/*showmenubar() should be called *before* getworkspacesize()
      in order for the menu bar to be taken into account*/
	LMenu_ShowMenuBar ();

	/*sets main window's title*/
	LApplication_SetTitle ("LL Sample Program - Topic: Animation");

	/*gets workspace (main window) size*/
	LApplication_GetWorkspaceSize (&dx, &dy);

	/*creates the animation window and sets its title, position, dimensions*/
	animw=LWindow_Create (LWindow_REGULAR, LWindow_Caption);
	LWindow_SetTitle (animw, "Animation");
	LWindow_Move (animw, dx/4, dy/4);
	LWindow_Resize (animw, dy/2, dy/2);

	/*gets window's client area size*/
	LWindow_GetClientAreaSize (animw, &animMaxX, &animMaxY);
	/*creates an ldraw widget*/
	animdrw=LDraw_CreateLocal (animw, 0, 0, animMaxX, animMaxY, animMaxX, animMaxY, FALSE);
	/*sets pen and brush*/
	LDraw_SetPen (animdrw, 0x0000ff00, 2);
	LDraw_SetBrush (animdrw, 0x00ff0000);
	/*draws initial square*/
	LDraw_Rectangle (animdrw, animX, animY, animX+20, animY+20);
	/*updates display*/
	LDraw_Update (animdrw);

	/*finally shows the window*/
	LWindow_Finalize (animw);

	/*installs timer handler*/
	LApplication_SetTimer (5, myAnimRoutine);
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
	LDialog_DisplayMessageBox (NULL, "LL Sample Program - Topic: Animation", "About...",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

/*anim "start" handler*/
void myAnimStartHandler (void* inparam)
{
	/*installs timer handler*/
	LApplication_SetTimer (5, myAnimRoutine);
	
	animON=TRUE;
}

/*anim "stop" handler*/
void myAnimStopHandler (void* inparam)
{
	/*uninstalls timer handler*/
	LApplication_SetTimer (0, NULL);
	
	animON=FALSE;
}

/*animation routine*/
void myAnimRoutine ()
{
	/*sets brush to white*/
	LDraw_SetBrush (animdrw, 0x00ffffff);
	/*draws rectangle*/
	LDraw_Rectangle (animdrw, animX, animY, animX+20, animY+20);

	/*updates anim parameters...*/
	animX=animX+animVX;
	animY=animY+animVY;

	if (animX<0)
	{
		animX=0;
		animVX=-animVX;
	}
	if (animY<0)
	{
		animY=0;
		animVY=-animVY;
	}
	if (animX>(i4)(animMaxX-20))
	{
		animX=animMaxX-20;
		animVX=-animVX;
	}
	if (animY>(i4)(animMaxY-20))
	{
		animY=animMaxY-20;
		animVY=-animVY;
	}

	/*sets brush to green*/
	LDraw_SetBrush (animdrw, 0x00ff0000);
	/*draws new rectangle*/
	LDraw_Rectangle (animdrw, animX, animY, animX+20, animY+20);
	/*updates display*/
	LDraw_Update (animdrw);
}
