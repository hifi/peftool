/*------------------------------------------------------------------------------
#
#	Macintosh Developer Technical Support
#
#	Simple Color QuickDraw Animation Sample Application
#
#	TubeTest
#
#	TubeTest.c	-	C Source
#
#	Copyright � 1988, 1994-95 Apple Computer, Inc.
#	All rights reserved.
#
#	Versions:	1.0					8/88
#				1.01				3/94	Updated for Universal Interfaces
#
#	Components:	TubeTest.c			August 1, 1988
#				TubeTest.r			August 1, 1988
#				TubeTest.make		August 1, 1988
#
#	The TubeTest program is a simple demonstration of how to use the Palette 
#	Manager in a color program.  It has a special color palette that is associated
#	with the main window.  The colors are animated using the Palette Manager 
#	to give a flowing tube effect.  The program is very simple, and the Palette
#	Manager and drawing parts are put in separate subroutines to make it easier
#	to figure out what is happening.
#	
#	The program is still a complete Macintosh application with a Main Event Loop,
#	so there is the extra code to run the MEL.  
#	
#	There is a resource file that is necessary as well, to define the Menus, Window,
#	Dialog, and Palette resources used in the program.  
#
#	See Sample and TESample for the general structure and MultiFinder techniques that
#	we recommend that you use when building a new application.
#
------------------------------------------------------------------------------*/

 /*
	File TubeTest.c

	Version 1.0:  6/2/88
				 7/20/88	DJB	Converted to C (MPW 3.0 ONLY)
			1.01 9/16/92	GAB Replaced GetNextEvent call with WaitNextEvent
	
	4/19/88:
	TubeTest -- A small sample application written by Bo3b Johnson.
	The idea is to draw two circles in varying colors in the window, then
	animate the colors when the menu is chosen.	This is a complete program
	with event loop and everything.	It is intended to be a simple example of
	how to use the Palette Manager to do some minor color animation, and
	how to use the PM to set up the colors desired in a window.
	
	Also see the resource file that goes with this to see how the Palette 
	itself is layed out.
	
	Could be built with something like this:
		rez TubeTest.r -o TubeTest
		C -r TubeTest.C
		Link TubeTest.c.o �
			"{Libraries}"Interface.o �
			"{Libraries}"Runtime.o �
			-o TubeTest
		TubeTest
 */
 
/* Where does it fit:
	This is a series of sample programs for those doing development
	using Color QuickDraw.  Since the whole color problem depends
	upon the exact effect desired, there are a number of answers
	to how to use colors, from the simple to the radically complex.
	These programs try to cover the gamut, so you should use 
	which ever seems appropriate.  In most cases, use the simplest
	one that will give the desired results.  The compatibility
	rating is from 0..9 where low is better.  The more known risks 
	there are the higher the rating.
	
	
	The programs (in order of compatibility):
	
		SillyBalls:
			This is the simplest use of Color QuickDraw, and does
			not use the Palette Manager.  It draws randomly colored
			balls in a color window.  This is intended to give you
			the absolute minimum required to get color on the screen.
			Written in straight Pascal code.
			Compatibility rating = 0, no known risks.
		
		FracAppPalette:
			This is a version of FracApp that uses only the Palette
			Manager.  It does not support color table animation
			since that part of the Palette Manager is not sufficient.
			The program demonstrates a full color palette that is
			used to display the Mandelbrot set.  It uses an offscreen
			gDevice w/ Port to handle the data, using CopyBits to
			draw to the window.  The Palette is automatically 
			associated with each window.  The PICT files are read
			and written using the bottlenecks, to save on memory
			useage.
			Written in MacApp Object Pascal code.
			Compatibility rating = 0, no known risks.
		
		TubeTest:	(***)
			This is a small demo program that demonstrates using the
			Palette Manager for color table animation.  It uses a 
			color palette with animating entries, and draws using the
			Palette Manager.  There are two circles of animating colors
			which gives a flowing tube effect.  This is a valid case
			for using the animating colors aspect of the Palette Manager,
			since the image is being drawn directly.
			Written in straight Pascal code.
			Compatibility rating = 0, no known risks.
		
		FracApp:
			This is the �commercial quality� version of FracApp.  This
			version supports color table animation, using an offscreen
			gDevice w/ Port, and handles multiple documents.  The
			CopyBits updates to the screen are as fast as possible.  The
			program does not use the Palette Manager, except to
			provide for the system palette, or color modes with less than
			255 colors.  For color table animation using an offscreen
			gDevice w/ Port, it uses the Color Manager and handles the
			colors itself.  Strict compatibility was relaxed to allow for
			a higher performance program.  This is the most �real� of the
			sample programs.
			Written in MacApp Object Pascal code.
			Compatibility rating = 2.  (nothing will break, but it may not
				always look correct.)
		
		FracApp300:
			This doesn't support colors, but demonstrates how to create and
			use a 300 dpi bitmap w/ Port.  The bitmap is printed at full
			resolution on LaserWriters, and clipped on other printers (but
			they still print).  It demonstrates how to use a high resolution
			image as a PICT file, and how to print them out.
			Written in MacApp Object Pascal code.
			Compatibility rating = 1.  (The use of PrGeneral is slightly 
				out of the ordinary, although supported.)
*/


#include <Types.h>
#include <Memory.h>
#include <Quickdraw.h>
#include <Palettes.h>
#include <Fonts.h>
#include <Windows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Menus.h>
#include <Devices.h>
#include <Events.h>
#include <Sound.h>
#include <ToolUtils.h>
#include <Processes.h>
	
/* Constants */
#define appleID				1000			/* resource IDs/menu IDs for Apple, */
#define fileID				1001			/* 	File and */
#define editID				1002			/*	Edit menus */
	
#define appleM				0				/* Index for each menu in myMenus (array of menu handles) */
#define fileM				1
#define editM				2

#define menuCount			3				/* Total number of menus */

#define windowID			1000			/* Resource ID for main window */
#define aboutMeDLOG 		1000			/* And Resource ID for About box dialog. */

#define tubularItem			1				/* When checked, animation of colors. */
#define quitItem			3				/* Quit in the menu of course. */

#define aboutMeCommand		1				/* Menu item in apple menu for About TubeTest item */
	
#define totalColors			152				/* use 150 colors in our palette for drawing eyes. */
#define numColors			150				/* to skip black and white. */
	
	
/* Globals */
MenuHandle	myMenus[menuCount];
Rect		dragRect;						/* Rectangle used to mark bounds for dragging window */
Boolean		doneFlag,						/* true if user has chosen Quit command */
			tubeCheck;						/* if true, the menu is checked, and we animate. */
EventRecord	myEvent;
WindowPtr	myWindow,
			whichWindow;
char		theChar;
OSErr		error;
SysEnvRec	theWorld;
QDGlobals	qd;

/* Prototypes */
void DrawEyes();
void SetUpMenus();
void ShiftyColors();
void ShowAboutMeDialog();
void DoCommand(long int mResult);


	
void main(void)
{
	/*
	**	Test the computer to be sure we can do color.  
	**	If not we would crash, which would be bad.  
	**	If we can�t run, just beep and exit.
	*/
	error = SysEnvirons(1, &theWorld);
	if (theWorld.hasColorQD == false) {
		SysBeep (50);
		ExitToShell();							/* If no color QD, we must leave. */
	};

	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();

	SetRect(&dragRect, 4, 24, qd.screenBits.bounds.right - 4, qd.screenBits.bounds.bottom - 4);
	doneFlag = false;							/* flag to detect when Quit command is chosen */
	tubeCheck = false;							/* flag for animating color is initially off. */

	/*
	**	Open the color window.
	*/
	myWindow = GetNewCWindow(windowID, nil, (WindowPtr) -1);
	SetPort(myWindow);

	/*
	**	Set up menus last, since the menu drawing can then use 
	**	the palette we have for our window. 
	**	Makes the Apple look better, in particular.
	*/
	SetUpMenus();
	
	/*
	**	Main Event Loop
	*/
	do {
		SystemTask();

		if (WaitNextEvent(everyEvent, &myEvent, 5L, NULL)) {
			switch (myEvent.what) {				/* case on event type */

				case mouseDown:
					switch (FindWindow(myEvent.where, &whichWindow)) {

						case inSysWindow:		/* desk accessory window: call Desk Manager to handle it */
							SystemClick(&myEvent, whichWindow);
							break;

						case inMenuBar:			/* Menu bar: learn which command, then execute it. */
							DoCommand(MenuSelect(myEvent.where));
							break;

						case inDrag:			/* title bar: call Window Manager to drag */
							DragWindow(whichWindow, myEvent.where, &dragRect);
							break;

						case inContent:			/* body of application window: */
							if (whichWindow != FrontWindow())
								SelectWindow(whichWindow); /* and make it active if not */
							break;
					}
					break;

				case updateEvt:					/* Update the eyes in window. */
					if ((WindowPtr) myEvent.message == myWindow) {
						BeginUpdate((WindowPtr) myEvent.message);
						DrawEyes();
						EndUpdate((WindowPtr) myEvent.message);
					}
					break;
							
				case keyDown:
				case autoKey:					/* key pressed once or held down to repeat */
					if (myWindow == FrontWindow()) {
						theChar = (myEvent.message & charCodeMask); /* get the char */
						/* 
						**	If Command key down, do it as a Menu Command.
						*/
						if (myEvent.modifiers & cmdKey)
							DoCommand(MenuKey(theChar));
					}
					break;

			}
		}

		/*	
		**	If we have menu item checked, go ahead and animate colors.
		*/
		if (tubeCheck) ShiftyColors();
		
	} while (!doneFlag);

	/*
	**	clean up after palette manager, 
	**	so he can chuck the palette in use.
	*/
	DisposeWindow (myWindow);
}

/*
**	This routine will update the window when required by update events.	It
**	will draw two circular dudes that are indexed in colors through the colors
**	we are using. 0 and 1 are skipped, since those are white and black in the
**	palette.
*/
void DrawEyes(void)
{
	Rect	tempRect;
	int		i;
	
	SetRect(&tempRect, numColors, numColors, numColors, numColors);
	for (i = 2; i <= totalColors; i++) {
		PmForeColor(i);
		FrameOval (&tempRect);
		InsetRect (&tempRect, -1, -1);
	}
	
	SetRect(&tempRect, numColors*3, numColors, numColors*3, numColors);
	for (i = totalColors; i >= 2; i--) {
		PmForeColor(i);
		FrameOval (&tempRect);
		InsetRect (&tempRect, -1, -1);
	}
}


/*
**	Read menu descriptions from resource file into memory 
**	and store handles in myMenus array.
**	Insert into MenuBar and draw.
*/
void SetUpMenus(void)
{
	int i;

	myMenus[appleM] = GetMenu(appleID); 		/* read Apple menu from resource file */
	AppendResMenu(myMenus[appleM], 'DRVR');		/* add desk accessory names to Apple menu */
	myMenus[fileM] = GetMenu(fileID);			/* read file menu from resource file */
	myMenus[editM] = GetMenu(editID);			/* read edit menu from resource file */

	for (i = 0; i < menuCount; i++) 
		InsertMenu(myMenus[i], 0); 				/* install menus in menu bar */
	
	DrawMenuBar();								/* and draw menu bar */
}


/*
**	Use the Palette currently attached to the main window to animate the colors 
**	in the circular eye shapes.  This will rotate them around to give the flowing 
**	tube effect. We make the palette into a color table so we can move entries 
**	around.	We have to skip the first two entries since those are black and white. 
**	(entries 0 and 1)
*/
void ShiftyColors(void)
{
	
	PaletteHandle	currPalette;
	CTabHandle		destCTab;
	ColorSpec		lastCSpec;
	
	SetPort (myWindow);
	
	currPalette = GetPalette(myWindow);
	destCTab = (CTabHandle) NewHandle(sizeof(ColorTable)+(totalColors*sizeof(ColorSpec)));
	if (destCTab == nil)  return;
	Palette2CTab(currPalette, destCTab);
	
	/*
	**	Move the colors around in the color table, skipping 0 and 1, and moving
	**	all the elements down by one, and copying the element at 2 back to the 
	**	end of the table. The effect is to rotate the colors in the table.
	*/
	lastCSpec = (*destCTab)->ctTable[2];						/* pull first one off. */
	BlockMove (&(*destCTab)->ctTable[3], 
			   &(*destCTab)->ctTable[2], 
			   (numColors) * sizeof(ColorSpec) );				/* copy all one entry down. */
	(*destCTab)->ctTable[totalColors-1] = lastCSpec;			/* put last color back on front. */
		
	AnimatePalette(myWindow, destCTab, 2, 2, numColors);
	
	DisposeHandle ((Handle) destCTab);
}


/*	
**	Display the dialog box in response to the 'About TubeTest' menu item
*/
void ShowAboutMeDialog(void)
{
	DialogPtr	theDialog;
	short		itemHit;

	theDialog = GetNewDialog(aboutMeDLOG, nil, (WindowPtr) -1);
	ModalDialog(nil, &itemHit);
	DisposeDialog(theDialog);
}


/*
**	Execute menu command specified by mResult,
**	the result of MenuSelect
*/
void DoCommand(long int mResult)
{
	short	theItem,							/* menu item number from mResult low-order word */
			theMenu;							/* menu number from mResult high-order word */
	Str255	name;								/* desk accessory name */
	int		temp;
	Boolean	dummy;

	theItem = LoWord(mResult);					/* call Toolbox Utility routines to */
	theMenu = HiWord(mResult);					/* set menu item number and menu */

	switch (theMenu) {							/* switch on menu ID */

		case appleID:
			if (theItem == aboutMeCommand)
				ShowAboutMeDialog();
			else {
					GetMenuItemText(myMenus[appleM], theItem, name);
					temp = OpenDeskAcc(name);
					SetPort(myWindow);
			}
			break;

		case fileID:
			if (theItem == quitItem)
				doneFlag = true;
			else if (theItem == tubularItem) {
					tubeCheck = !tubeCheck;
					CheckItem(myMenus[fileM], tubularItem, tubeCheck);
			}
			break;

		case editID:
			dummy = SystemEdit(theItem - 1);	/* Pass the command on to the Desk Manager. */
			break;
	}

	HiliteMenu(0);								/* Unhighlight menu title */
												/* (highlighted by MenuSelect) */
}

