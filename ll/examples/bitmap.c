/* ============================================================================
 *  File: bitmap.c
 *  LL Sample Program - Topic: Bitmap
 * ============================================================================
*/

/*includes LL's own function prototypes and types definitions...*/
#include "LApplication.h"

/*menus...*/
LMenu *file, *pen, *help;

/*menu item IDs...*/
ui4 idexit=0, idabout=1, idopen=2, idsave=3, idred=4, idgreen=5, idblue=6;

/*menu item handlers...*/
void myOpenHandler (void* inparam);
void mySaveHandler (void* inparam);
void myExitMenuHandler (void* inparam);

void myRedHandler (void* inparam);
void myGreenHandler (void* inparam);
void myBlueHandler (void* inparam);

void myAboutMenuHandler (void* inparam);

/*ldraw event handlers*/
Bool myMouseDownHandler (LDraw* inDraw, ui4 inX, ui4 inY);

/*windows*/
LWindow *mywin;

/*ldraws*/
LDraw *mydrw;

/*other vars*/
ui4 dx, dy;
ui4 pencolor=0x000000ff;

/*initialization function (LL-based applications' entry point)...*/ 
void LApplication_Init ()
{
	/*creates menus*/
	file=LMenu_Create ("&File");
	pen=LMenu_Create ("&Color");
	help=LMenu_Create ("&Help");
	
	/*appends some items*/
	LMenu_AppendMenuItem (file, "&Open...", idopen, "O", myOpenHandler, NULL);
	LMenu_AppendMenuItem (file, "&Save As...", idsave, "S", mySaveHandler, NULL);
	LMenu_AppendMenuSeparator (file);
	LMenu_AppendMenuItem (file, "E&xit", idexit, "Q", myExitMenuHandler, NULL);

	LMenu_AppendMenuItem (pen, "&Red", idred, "R", myRedHandler, NULL);
	LMenu_AppendMenuItem (pen, "&Green", idgreen, "G", myGreenHandler, NULL);
	LMenu_AppendMenuItem (pen, "&Blue", idblue, "B", myBlueHandler, NULL);
	LMenu_CheckMenuItem (pen, idred, LMenu_CHECKED);

	LMenu_AppendMenuItem (help, "&About...", idabout, NULL, myAboutMenuHandler, NULL);

	/*appends menus to menubar*/
	LMenu_AppendMenuToMenuBar (file);
	LMenu_AppendMenuToMenuBar (pen);
	LMenu_AppendMenuToMenuBar (help);

	/*makes the menubar visible*/
	LMenu_ShowMenuBar ();

	/*sets main window's title*/
	LApplication_SetTitle ("LL Sample Program - Topic: Bitmap");

	/*creates window and sets its title, position, dimensions*/
	mywin=LWindow_Create (LWindow_REGULAR, LWindow_Caption);
	LWindow_SetTitle (mywin, "Draw");
	LWindow_Move (mywin, 320, 200);
	LWindow_Resize (mywin, 350, 250);

	/*gets window's client area size*/
	LWindow_GetClientAreaSize (mywin, &dx, &dy);
	/*creates an ldraw widget*/
	mydrw=LDraw_CreateLocal (mywin, 0, 0, dx, dy, dx, dy, FALSE);
	/*sets its cursor shape*/
	LDraw_SetCursorShape (mydrw, LCursor_CROSS);
	/*installs mouse down event handler*/
	LDraw_InstallEvent (mydrw, LDraw_MOUSE_DOWN, (LDraw_THandler)myMouseDownHandler);

	/*finally shows the window*/
	LWindow_Finalize (mywin);
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
	LDialog_DisplayMessageBox (NULL, "LL Sample Program - Topic: Bitmap", "About...",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

/*"red" handler*/
void myRedHandler (void* inparam)
{
	LMenu_CheckMenuItem (pen, idred, LMenu_CHECKED);
	LMenu_CheckMenuItem (pen, idgreen, LMenu_UNCHECKED);
	LMenu_CheckMenuItem (pen, idblue, LMenu_UNCHECKED);
	pencolor=0x000000ff;
}

/*"green" handler*/
void myGreenHandler (void* inparam)
{
	LMenu_CheckMenuItem (pen, idred, LMenu_UNCHECKED);
	LMenu_CheckMenuItem (pen, idgreen, LMenu_CHECKED);
	LMenu_CheckMenuItem (pen, idblue, LMenu_UNCHECKED);
	pencolor=0x0000ff00;
}

/*"blue" handler*/
void myBlueHandler (void* inparam)
{
	LMenu_CheckMenuItem (pen, idred, LMenu_UNCHECKED);
	LMenu_CheckMenuItem (pen, idgreen, LMenu_UNCHECKED);
	LMenu_CheckMenuItem (pen, idblue, LMenu_CHECKED);
	pencolor=0x00ff0000;
}

/*"open" handler*/
void myOpenHandler (void* inparam)
{
	LFile *thefile;
	ui4 thesize;
	void *thetemp;

	i1* theFilters[4]={"Bitmap Files", "*.BMP", "All Files", "*.*"};
	i1 theBuffer[LDialog_MAX_TITLE]="filename";

	LBitmap* thebitmap;

	/*displays standard dialog*/
	if (LDialog_GetOpenFileName (NULL, theFilters, 2, theBuffer))
	{
		/*if file exists...*/
		if (LFile_Exists (theBuffer))
		{
			/*reads it!*/
			thefile=LFile_Open (theBuffer, LFile_READ);
			thesize=LFile_GetSize (thefile);
			thetemp=LMemory_Malloc (thesize);
			LFile_Read (thefile, thetemp, thesize);
			LFile_Close (&thefile);

			/*creates a bitmap object from a .bmp file image*/
			thebitmap=LBitmap_CreateFromBMP (thetemp, thesize);
			/*blits it onto the ldraw*/
			LDraw_BlitBitmap (mydrw, thebitmap, 0, 0);
			/*updates the display*/
			LDraw_Update (mydrw);
			/*destroys the bitmap*/
			LBitmap_Destroy (thebitmap);
			
			LMemory_Free (&thetemp);
		}
		else
			LDialog_DisplayMessageBox (NULL, "File does not exist!", "Error",
				LDialog_MBOK, LDialog_MBICONEXCLAMATION);
	}
}

/*"save" handler*/
void mySaveHandler (void* inparam)
{
	i1* theFilters[4]={"Bitmap Files", "*.BMP", "All Files", "*.*"};
	i1 theBuffer[LDialog_MAX_TITLE]="filename";

	LBitmap* thebitmap;
	LFile* thefile;
	void *thetemp;
	ui4 thesize;

	/*displays standard dialog*/
	if (LDialog_GetSaveFileName (NULL, theFilters, 2, theBuffer))
	{
		/*gets bitmap object from display*/
		thebitmap=LDraw_GetBitmap (mydrw, 0, 0, dx, dy);
		/*converts it into a .bmp file image*/
		thetemp=LBitmap_ConvertToBMP (thebitmap, &thesize);

		/*writes it to disk!*/
		thefile=LFile_Open (theBuffer, LFile_WRITE);
		LFile_Write (thefile, thetemp, thesize); 
		LFile_Close (&thefile);
		/*destroys the bitmap*/
		LBitmap_Destroy (thebitmap);
	}
}

/*ldraw mouse down event handler*/
Bool myMouseDownHandler (LDraw* inDraw, ui4 inX, ui4 inY)
{
	/*sets brush*/
	LDraw_SetBrush (inDraw, pencolor);
	/*sets pen*/
	LDraw_SetPen (inDraw, 0x0000ffff, 2);
	/*draws the ellipse*/
	LDraw_Ellipse (inDraw, inX-6, inY-6, inX+6, inY+6);
	/*updates the display*/
	LDraw_Update (inDraw);
	
	/*no further handling is required...*/
	return TRUE;
}

