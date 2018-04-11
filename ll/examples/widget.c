/* ============================================================================
 *  File: widget.c
 *  LL Sample Program - Topic: Widgets
 * ============================================================================
*/

/*includes LL's own function prototypes and types definitions...*/
#include "LApplication.h"

/*menus...*/
LMenu *file, *help;

/*windows...*/
LWindow *mywin;

/*textfield*/
LTextField *ltf;
LGroupBox *gb5;
LPushButton *pb7;

/*statusbar*/
LStatusBar *stbar;
i4 stbararray[2];

/*pushbutton*/
LPushButton *pb0, *pb1;
LGroupBox *gb0;

/*checkbox*/
LCheckBox *cb0;
LGroupBox *gb1;

/*radiobutton*/
LRadioButton *rb0, *rb1, *rb2;
LGroupBox *gb2;

/*progressbar*/
LProgressBar *progbar;
LPushButton *pb3, *pb4;
LGroupBox *gb3;
int lastinc=0;

/*listbox*/
LListBox *llist0;
LPushButton *pb5, *pb6;
LGroupBox *gb4;

/*menu item IDs...*/
ui4 idexit=0, idabout=1;

/*menu item handlers...*/
void myExitMenuHandler (void* inparam);
void myAboutMenuHandler (void* inparam);

/*widget handlers*/
Bool myEnterHandler (LTextField* inEditor);
void myTFHandler0 (LPushButton* inButton);

void myFirstCBHandler (LCheckBox* inBox);

void myFirstRBHandler (LRadioButton* inRadio);
void mySecondRBHandler (LRadioButton* inRadio);
void myThirdRBHandler (LRadioButton* inRadio);

void myPlusHandler (LPushButton* inButton);
void myZeroHandler (LPushButton* inButton);

void mylbdcHandler (LListBox* inLBox);
void myQueryStateHandler (LPushButton* inButton);
void myResetSelectionHandler (LPushButton* inButton);

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
	LApplication_SetTitle ("LL Sample Program - Topic: Widgets");

	/*creates a window*/
	mywin=LWindow_Create (LWindow_REGULAR, LWindow_Caption|LWindow_MinMaxBox);
	LWindow_SetTitle (mywin, "Widgets");
	LWindow_Resize (mywin, 450, 400);
	LWindow_Move (mywin, 0, 0);
	LWindow_Finalize (mywin);

	/*textfield*/
	ltf=LTextField_Create (mywin, 40, 230, 150, 20, TRUE);
	LTextField_SetFont (ltf, LTextField_ANSIVAR);
	LTextField_SetTextBuffer (ltf, "This is a TextField !");
	LTextField_InstallReturnHandler (ltf, myEnterHandler);
	gb5=LGroupBox_Create (mywin, 30, 205, 170, 100, "TextField");
	LGroupBox_SetFont (gb5, LGroupBox_ANSIVAR);
	pb7=LPushButton_Create (mywin, 40, 270, 150, 20, "Query State", NULL, myTFHandler0);
	LPushButton_SetFont (pb7, LPushButton_ANSIVAR);

	/*statusbar*/
	stbararray[0]=225;
	stbararray[1]=-1;
	stbar=LStatusBar_Create (mywin, 2);
	LStatusBar_SetText (stbar, 0, "This is a StatusBar");
	LStatusBar_SetText (stbar, 1, "You can display information here");

	/*pushbutton*/
	pb0=LPushButton_Create (mywin, 30, 25, 100, 70, "Enabled", NULL, NULL);
	LPushButton_SetFocus (pb0); 
	LPushButton_SetFont (pb0, LPushButton_ANSIVAR);
	pb1=LPushButton_Create (mywin, 30, 105, 100, 70, "Disabled", NULL, NULL);
	LPushButton_Enable (pb1, FALSE);
	gb0=LGroupBox_Create (mywin, 10, 2, 140, 188, "PushButtons");
	LGroupBox_SetFont (gb0, LGroupBox_ANSIVAR);

	/*checkbox*/
	cb0=LCheckBox_Create (mywin, 175, 35, 80, 20, "Option", myFirstCBHandler);
	LCheckBox_SetFont (cb0, LCheckBox_ANSIVAR);
	gb1=LGroupBox_Create (mywin, 155, 15, 105, 50, "CheckBox");
	LGroupBox_SetFont (gb1, LGroupBox_ANSIVAR);

	/*radiobutton*/
	rb0=LRadioButton_Create (mywin, 180, 95, 50, 20, "Opt1", myFirstRBHandler);
	rb1=LRadioButton_Create (mywin, 180, 120, 60, 20, "Opt2", mySecondRBHandler);
	rb2=LRadioButton_Create (mywin, 180, 145, 50, 20, "Opt3", myThirdRBHandler);
	LRadioButton_SetFont (rb0, LRadioButton_ANSIVAR);
	LRadioButton_SetFont (rb1, LRadioButton_ANSIVAR);
	LRadioButton_SetFont (rb2, LRadioButton_ANSIVAR);
	LRadioButton_SetState (rb0, LRadioButton_CHECKED);
	gb2=LGroupBox_Create (mywin, 160, 75, 85, 100, "RadioButtons");
	LGroupBox_SetFont (gb2, LGroupBox_ANSIVAR);

	/*progressbar*/
	progbar=LProgressBar_Create (mywin, 280, 40, 80, 20);
	pb3=LPushButton_Create (mywin, 370, 40, 20, 20, "+", NULL, myPlusHandler);
	LPushButton_SetFont (pb3, LPushButton_ANSIVAR);
	pb4=LPushButton_Create (mywin, 280, 70, 75, 20, "Reset", NULL, myZeroHandler);
	LPushButton_SetFont (pb4, LPushButton_ANSIVAR);
	gb3=LGroupBox_Create (mywin, 270, 10, 130, 90, "ProgressBar");
	LGroupBox_SetFont (gb3, LGroupBox_ANSIVAR);
	LProgressBar_SetRange (progbar, 0, 140);
	LProgressBar_SetStep (progbar, 14);

	/*listbox*/
	llist0=LListBox_Create (mywin, 280, 150, 100, 100, mylbdcHandler);
	LListBox_SetFont (llist0, LListBox_ANSIVAR);
	gb4=LGroupBox_Create (mywin, 270, 125, 130, 200, "ListBox");
	LGroupBox_SetFont (gb4, LGroupBox_ANSIVAR);
	pb5=LPushButton_Create (mywin, 280, 260, 100, 20, "Query State", NULL, myQueryStateHandler);
	LPushButton_SetFont (pb5, LPushButton_ANSIVAR);
	pb6=LPushButton_Create (mywin, 280, 290, 100, 20, "Reset Selection", NULL, myResetSelectionHandler);
	LPushButton_SetFont (pb6, LPushButton_ANSIVAR);

	LListBox_AddItem (llist0, "Orange");
	LListBox_AddItem (llist0, "Apple");
	LListBox_AddItem (llist0, "Pineapple");
	LListBox_AddItem (llist0, "Avocado");
	LListBox_AddItem (llist0, "Pear");
	LListBox_AddItem (llist0, "Grapes");
	LListBox_AddItem (llist0, "Peach");
	LListBox_AddItem (llist0, "Apricot");
	LListBox_AddItem (llist0, "Grapefruit");
	LListBox_AddItem (llist0, "Banana");
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
	LDialog_DisplayMessageBox (NULL, "LL Sample Program - Topic: Widgets", "About...",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

/*textfield handlers*/
Bool myEnterHandler (LTextField* inEditor)
{
	i1 buffer[65536];
	i4 size;
	
	size=LTextField_GetTextBufferSize (ltf);
	LTextField_GetTextBuffer (ltf, buffer, size);
	if (size==1)
		LDialog_DisplayMessageBox (NULL, "The TextField is empty.", "TextField",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
	else
		LDialog_DisplayMessageBox (NULL, buffer, "The TextField contains:",
			LDialog_MBOK, LDialog_MBICONINFORMATION);

	return TRUE; 
}

void myTFHandler0 (LPushButton* inButton)
{
	i1 buffer[65536];
	i4 size;
	
	size=LTextField_GetTextBufferSize (ltf);
	LTextField_GetTextBuffer (ltf, buffer, size);
	if (size==1)
		LDialog_DisplayMessageBox (NULL, "The TextField is empty.", "TextField",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
	else
		LDialog_DisplayMessageBox (NULL, buffer, "The TextField contains:",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

/*checkbox handler*/
void myFirstCBHandler (LCheckBox* inBox)
{
	ui2 thestate;

	thestate=LCheckBox_QueryState (inBox);
	if (thestate==LCheckBox_CHECKED)
	{
		LCheckBox_SetState (inBox, LCheckBox_UNCHECKED);
	}
	else
	{
		LCheckBox_SetState (inBox, LCheckBox_CHECKED);
	}
}

/*radiobutton handlers*/
void myFirstRBHandler (LRadioButton* inRadio)
{
	LRadioButton_SetState (rb0, LRadioButton_CHECKED);
	LRadioButton_SetState (rb1, LRadioButton_UNCHECKED);
	LRadioButton_SetState (rb2, LRadioButton_UNCHECKED);
}

void mySecondRBHandler (LRadioButton* inRadio)
{
	LRadioButton_SetState (rb0, LRadioButton_UNCHECKED);
	LRadioButton_SetState (rb1, LRadioButton_CHECKED);
	LRadioButton_SetState (rb2, LRadioButton_UNCHECKED);
}

void myThirdRBHandler (LRadioButton* inButton)
{
	LRadioButton_SetState (rb0, LRadioButton_UNCHECKED);
	LRadioButton_SetState (rb1, LRadioButton_UNCHECKED);
	LRadioButton_SetState (rb2, LRadioButton_CHECKED);
}

/*progressbar handlers*/
void myPlusHandler (LPushButton* inButton)
{
	lastinc++;
	if (lastinc<11)
		LProgressBar_StepIt (progbar);
}

void myZeroHandler (LPushButton* inButton)
{
	LProgressBar_SetPosition (progbar, 0);
	lastinc=0;
}

/*listbox handlers*/
void mylbdcHandler (LListBox* inLBox)
{
	i1 buffer[LListBox_MAX_TEXT];

	LListBox_GetCurrentSelection (inLBox, buffer);

	LDialog_DisplayMessageBox (NULL, buffer, "ListBox - You have just double clicked...",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

void myQueryStateHandler (LPushButton* inButton)
{
	i1 buffer[LListBox_MAX_TEXT];

	if (LListBox_GetCurrentSelection (llist0, buffer))
		LDialog_DisplayMessageBox (NULL, buffer, "ListBox - Currently Selected Item",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
	else
		LDialog_DisplayMessageBox (NULL, "No item is currently selected.", "ListBox - Currently Selected Item",
			LDialog_MBOK, LDialog_MBICONINFORMATION);
}

void myResetSelectionHandler (LPushButton* inButton)
{
	LListBox_SetCurrentSelection (llist0, NULL);
}
