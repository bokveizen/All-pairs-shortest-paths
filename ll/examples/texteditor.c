/* ============================================================================
 *  File: texteditor.c
 *  LL Sample Program - Topic: TextEditor
 * ============================================================================
*/

/*includes LL's own function prototypes and types definitions...*/
#include "LApplication.h"

/*menus...*/
LMenu *file, *edit, *help;

/*menu item IDs...*/
ui4 idexit=0, idabout=1, idnew=2, idopen=3, idsave=4, idundo=5;
ui4 idcut=6, idcopy=7, idpaste=8, idselectall=9;
 

/*menu item handlers...*/
void myNewTextEditHandler (void* inparam);
void mySaveHandler (void* inparam);
void myOpenHandler (void* inparam);
void myExitMenuHandler (void* inparam);

void myCutHandler (void* inparam);
void myCopyHandler (void* inparam);
void myPasteHandler (void* inparam);
void myUndoHandler (void* inparam);
void mySelectAllHandler (void* inparam);

void myAboutMenuHandler (void* inparam);

/*menu init handlers*/ 
void myInitEditHandler (void* inparam);
void myInitFileHandler (void* inparam);

/*window event handlers*/
Bool myResizeHandler (LWindow* inWindow, ui4 inX, ui4 inY);

/*initialization function (LL-based applications' entry point)...*/ 
void LApplication_Init ()
{
	/*creates menus*/
	file=LMenu_Create ("&File");
	edit=LMenu_Create ("&Edit");
	help=LMenu_Create ("&Help");
	
	/*appends some items*/
	LMenu_AppendMenuItem (file, "&New Editor", idnew, "N", myNewTextEditHandler, NULL);
	LMenu_AppendMenuItem (file, "&Open...", idopen, "O", myOpenHandler, NULL);
	LMenu_AppendMenuItem (file, "&Save As...", idsave, "S", mySaveHandler, NULL);
	LMenu_AppendMenuSeparator (file);
	LMenu_AppendMenuItem (file, "E&xit", idexit, "Q", myExitMenuHandler, NULL);

	LMenu_AppendMenuItem (edit, "&Undo", idundo, "Z", myUndoHandler, NULL);
	LMenu_AppendMenuSeparator (edit);
	LMenu_AppendMenuItem (edit, "Cu&t", idcut, "X", myCutHandler, NULL);
	LMenu_AppendMenuItem (edit, "&Copy", idcopy, "C", myCopyHandler, NULL);
	LMenu_AppendMenuItem (edit, "&Paste", idpaste, "V", myPasteHandler, NULL);
	LMenu_AppendMenuSeparator (edit);
	LMenu_AppendMenuItem (edit, "Select A&ll", idselectall, "A", mySelectAllHandler, NULL);

	LMenu_AppendMenuItem (help, "&About...", idabout, NULL, myAboutMenuHandler, NULL);

	/*appends menus to menubar*/
	LMenu_AppendMenuToMenuBar (file);
	LMenu_AppendMenuToMenuBar (edit);
	LMenu_AppendMenuToMenuBar (help);

	/*installs menuinit handlers*/
	LMenu_InstallInitHandler (edit, myInitEditHandler, NULL);
	LMenu_InstallInitHandler (file, myInitFileHandler, NULL);

	/*makes the menubar visible*/
	LMenu_ShowMenuBar ();

	/*sets main window's title*/
	LApplication_SetTitle ("LL Sample Program - Topic: TextEditor");
}

/*MENU ITEM HANDLERS*/

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
	LDialog_DisplayMessageBox (NULL, "LL Sample Program - Topic: TextEditor", "About...",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

/*"new editor" handler*/
void myNewTextEditHandler (void* inparam)
{
	LWindow *w, *w2;
	LTextEditor *t;
	ui4 x, y;
	ui4 dx, dy;

	/*gets front window (if any)...*/
	w2=LWindow_GetFrontWindow ();
	if (w2!=NULL)
	/*...and its position*/
		LWindow_GetPosition (w2, &x, &y);
	else
	{
		x=0;
		y=0;
	}

	/*creates a new window*/
	w=LWindow_Create (LWindow_REGULAR, LWindow_Caption|LWindow_Resizeable|LWindow_MinMaxBox);
	/*places it relatively to front window*/
	LWindow_Move (w, x+30, y+30);

	/*gets its size*/
	LWindow_GetClientAreaSize (w, &dx, &dy);

	/*creates a text editor widget*/
	t=LTextEditor_Create (LTextEditor_HScrollBar|LTextEditor_VScrollBar, w, 0, 0, dx, dy, FALSE);
	/*saves its pointer for future reference (resize handler)*/
	LWindow_SetUserData (w, t);
	/*sets its font*/
	LTextEditor_SetTextFont (t, LTextEditor_ANSIFIXED);

	/*sets window title*/
	LWindow_SetTitle (w, "Editor");

	/*installs handler for the resize event*/
	LWindow_InstallEvent (w, LWindow_RESIZE, (LWindow_THandler)myResizeHandler);

	/*shows the window*/
	LWindow_Finalize (w);
	/*brings it on top*/
	LWindow_BringToFront (w);
	/*gives the keyboard focus to the editor*/
	LTextEditor_SetFocus (t);
}

void mySaveHandler (void* inparam)
{
	LFile* thefile;
	LTextEditor* theeditor;
	void* theobj;
	ui2 theobjID;
	Bool theres;

	i1* theFilters[4]={"Source and Header Files", "*.C;*.CPP;*.H", "All Files", "*.*"};
	i1 theBuffer[LDialog_MAX_TITLE]="filename";
	i1 theTemp[524288];
	i4 thesize;

	/*gets front editor (there must be one - see menu init handlers...*/
	theres=LApplication_GetFocus (&theobjID, &theobj);
	theeditor=(LTextEditor *)theobj;
	/*displays standard dialog*/
	if (LDialog_GetSaveFileName (NULL, theFilters, 2, theBuffer))
	{
		/*copies editor content onto specified file*/
		thefile=LFile_Open (theBuffer, LFile_WRITE);
		thesize=LTextEditor_GetTextBufferSize (theeditor);
		LTextEditor_GetTextBuffer (theeditor, theTemp);
		LFile_Write (thefile, theTemp, thesize); 
		LFile_Close (&thefile);
	}
	/*gives input focus back to editor*/
	LTextEditor_SetFocus (theeditor);
}

void myOpenHandler (void* inparam)
{
	LFile* thefile;
	LTextEditor* theeditor;
	void *theobj;
	ui2 theobjID;
	Bool theres;

	i1* theFilters[4]={"Source and Header Files", "*.C;*.CPP;*.H", "All Files", "*.*"};
	i1 theBuffer[LDialog_MAX_TITLE]="filename";
	i1 theTemp[262144];
	ui4 thesize;

	/*gets front editor (there must be one - see menu init handlers...*/
	theres=LApplication_GetFocus (&theobjID, &theobj);
	theeditor=(LTextEditor *) theobj;
	/*displays standard dialog*/
	if (LDialog_GetOpenFileName (NULL, theFilters, 2, theBuffer))
	{
		/*if file exists...*/
		if (LFile_Exists (theBuffer))
		{
			thefile=LFile_Open (theBuffer, LFile_READ);
			thesize=LFile_GetSize (thefile);
			/*if it isn't too long...*/
			if (thesize>262144)
			{
				LDialog_DisplayMessageBox (NULL, "File size exceeds 256Kb!", "Error",
				LDialog_MBOK, LDialog_MBICONEXCLAMATION);
				LFile_Close (&thefile);
			}
			else
			{
				/*reads it!*/
				LFile_Read (thefile, theTemp, thesize);
				LFile_Close (&thefile);
				LTextEditor_SetTextBuffer (theeditor, theTemp, thesize);
			}
		}
		else
			LDialog_DisplayMessageBox (NULL, "File does not exist!", "Error",
				LDialog_MBOK, LDialog_MBICONEXCLAMATION);
	}
	/*gives input focus back to editor*/
	LTextEditor_SetFocus (theeditor);
}

void myCutHandler (void* inparam)
{
	void* theobj;
	ui2 theobjID;
	Bool theres;

	theres=LApplication_GetFocus (&theobjID, &theobj);
	/*if an object has the focus...*/
	if (theres!=FALSE)
		/*if it is a text editor...*/
		if (theobjID==LTextEditor_ID)
			/*"cuts" current selection*/
			LTextEditor_Cut ((LTextEditor *) theobj);
}

void myCopyHandler (void* inparam)
{
	void* theobj;
	ui2 theobjID;
	Bool theres;

	theres=LApplication_GetFocus (&theobjID, &theobj);
	/*if an object has the focus...*/
	if (theres!=FALSE)
		/*if it is a text editor...*/
		if (theobjID==LTextEditor_ID)
			/*"copies" the selection*/
			LTextEditor_Copy ((LTextEditor *) theobj);
}

void myPasteHandler (void* inparam)
{
	void* theobj;
	ui2 theobjID;
	Bool theres;

	theres=LApplication_GetFocus (&theobjID, &theobj);
	/*if an object has the focus...*/
	if (theres!=FALSE)
		/*if it is a text editor...*/
		if (theobjID==LTextEditor_ID)
			/*"pastes" the selection*/
			LTextEditor_Paste ((LTextEditor *) theobj);
}

void myUndoHandler (void* inparam)
{
	void* theobj;
	ui2 theobjID;
	Bool theres;

	theres=LApplication_GetFocus (&theobjID, &theobj);
	/*if an object has the focus...*/
	if (theres!=FALSE)
		/*if it is a text editor...*/
		if (theobjID==LTextEditor_ID)
			/*"undoes" the last operation*/
			LTextEditor_Undo ((LTextEditor *) theobj);
}

void mySelectAllHandler (void* inparam)
{
	void* theobj;
	ui2 theobjID;
	Bool theres;

	theres=LApplication_GetFocus (&theobjID, &theobj);
	/*if an object has the focus...*/
	if (theres!=FALSE)
		/*if it is a text editor...*/
		if (theobjID==LTextEditor_ID)
			/*"selects" all*/
			LTextEditor_SelectAll ((LTextEditor *) theobj);
}

/*MENU INIT HANDLERS*/

void myInitFileHandler (void* inparam)
{
	void *theobj;
	ui2 theobjID;
	Bool theres;

	theres=LApplication_GetFocus (&theobjID, &theobj);
	
	/*if no object has the focus*/
	if (theres==FALSE)
	{
		/*disables both open and save items*/
		LMenu_DisableMenuItem (file, idopen);
		LMenu_DisableMenuItem (file, idsave);
	}
	else
	{
		/*if object is a text editor...*/
		if (theobjID==LTextEditor_ID)
		{
			/*enables both open and save items*/
			LMenu_EnableMenuItem (file, idopen);
			LMenu_EnableMenuItem (file, idsave);
		}
		else
		{
			LMenu_DisableMenuItem (file, idopen);
			LMenu_DisableMenuItem (file, idsave);
		}
	}
}

void myInitEditHandler (void* inparam)
{
	void* theobj;
	ui2 theobjID;
	Bool theres;

	LTextEditor *thetexaux;

	theres=LApplication_GetFocus (&theobjID, &theobj);

	/*if no object has the focus...*/
	if (theres==FALSE)
	{
		/*disables all items*/
		LMenu_DisableMenuItem (edit, idundo);
		LMenu_DisableMenuItem (edit, idcut);
		LMenu_DisableMenuItem (edit, idcopy);
		LMenu_DisableMenuItem (edit, idpaste);
		LMenu_DisableMenuItem (edit, idselectall);
		return;
	}
	else
	{
		/*if object is a text editor...*/
		if (theobjID==LTextEditor_ID)
		{
			thetexaux=(LTextEditor *) theobj;
			/*enables select all*/
			LMenu_EnableMenuItem (edit, idselectall);

			/*determines whether undo can be perfomed*/
			if (LTextEditor_CanUndo (thetexaux))
				LMenu_EnableMenuItem (edit, idundo);
			else
				LMenu_DisableMenuItem (edit, idundo);

			/*determines whether text is available in system clipboard*/
			if (LTextEditor_IsTextAvailable ())
				LMenu_EnableMenuItem (edit, idpaste);
			else
				LMenu_DisableMenuItem (edit, idpaste);

			/*determines whether a selection is available*/
			if (LTextEditor_IsSelectionAvailable (thetexaux))
			{
				LMenu_EnableMenuItem (edit, idcut);
				LMenu_EnableMenuItem (edit, idcopy);
			}
			else
			{
				LMenu_DisableMenuItem (edit, idcut);
				LMenu_DisableMenuItem (edit, idcopy);
			}
		}
		else
		{
			LMenu_DisableMenuItem (edit, idundo);
			LMenu_DisableMenuItem (edit, idcut);
			LMenu_DisableMenuItem (edit, idcopy);
			LMenu_DisableMenuItem (edit, idpaste);
			LMenu_DisableMenuItem (edit, idselectall);
			return;
		}
	}
}

/*WINDOW HANDLERS*/

Bool myResizeHandler (LWindow* inWindow, ui4 inX, ui4 inY)
{
	LTextEditor *t;

	/*retrieves pointer to corresponding text editor*/
	t=LWindow_GetUserData (inWindow);
	/*resizes it*/
	LTextEditor_Resize (t, inX, inY);

	/*pass on to default (system) handler*/
	return FALSE;
}
