/*************************************************************************\
 * connect4.c
 * This is the "main" program source.  You will find the initialization
 * routines and the event handling.
 * Set tabs to 3 to get a readable source.
\*************************************************************************/

/* include files, macros and definitions */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS
#define INCL_GPI
#include <os2.h>
#include "compile.h"
#include "errmsgs.h"
#include "misc.h"
#include "game.h"
#include "pmgame.h"
#include "profile.h"
#include "defs.h"
#include "mmsound.h"

#define INITIALIZE
#include "connect4.h"

#define BEEP_WARN_FREQ 	440U
#define BEEP_WARN_DUR 	100U


/* prototypen of the functions defined by the program */

MRESULT EXPENTRY WndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY GameOptsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY ProgOptsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY ProdInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

PRIVATE PSZ LoadString( USHORT IdStr );

PRIVATE VOID SoundThread( PVOID pssnd );
PRIVATE VOID StartSoundThread( ULONG sound );
PRIVATE VOID NewThread( PVOID phwnd );
PRIVATE VOID StartNewThread( HWND hwnd );

/* local variables */

HELPINIT HelpInit;

PRIVATE TID ComputerThreadID = -1;
PRIVATE TID SoundThreadID = -1;
PRIVATE TID NewThreadID = -1;
PRIVATE TID InsertThreadID = -1;
PRIVATE TID ShowHintThreadID = -1;
PRIVATE TID ShowWinThreadID = -1;
PRIVATE TID UndoThreadID = -1;

PRIVATE HPOINTER hptrRDown;			/* Handle of the red down pointer 		*/
PRIVATE HPOINTER hptrBDown;			/* Handle of the blue down pointer		*/
PRIVATE HPOINTER hptrSysArrow;		/* Handle of the system default ptr		*/
PRIVATE HPOINTER hptrSysWait;			/* Handle of the system wait ptr			*/
PRIVATE HPOINTER hptrSysIllegal;		/* Handle of the system illegal ptr		*/

PRIVATE HEV hevDrawInProgress;		/* Handle of the event semaphore which
												 * indicates the dropping of a coin. 	*/
PRIVATE HEV hevSoundInProgress;												 

/************************************************************************\
 * The main program
 * It initializes the Anchor block handle, the different mousepointers
 * and the message queue. Reads the profile data stored in connect4.ini
 * and initializes the online help facilities.
 * Registers the window and starts the message
 * loop. When the program is exited, it cleans up by destroying all handles.
\************************************************************************/
INT main( INT argc, CHAR *argv[], CHAR *envp[] )
{
	QMSG qmsg;							/* message-queue of our program			*/
	ULONG fWndCtrlData;				/* window style for the created win		*/
	BOOL fRegistered; 				/* return value by various functions	*/
	RECTL rectlWinPos;				/* needed when resizing window			*/
	LONG cxScreen;
	LONG cyScreen;
	INT rc;
	
	static CHAR szClientClass[] = "4wins.child";

	pszProgName = argv[0];			/* Get the full qualified program name	*/

	if ( (hab = WinInitialize(0)) == NULLHANDLE ) {
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR );
		return 1;
	}
	/* WinInitialize: 0 is the only allowed parameter. Returns the			*/
	/* Anchor-block handle in case of success, otherwise the Nullhandle.	*/

	if ( (hmq = WinCreateMsgQueue( hab, 0 )) == NULLHANDLE ) {
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR );
		WinTerminate( hab );
		return 1;
	}
	/* WinCreateMsgQueue: first parameter is the input Anchor-block handle.*/
	/* second parameter determines the maximum Message-queue size, 		*/
	/* 0 causes the use of the system default queue size. 					*/
	/* Returns the Message-queue handle in case of success. 					*/
	/* WinTerminate terminates (surprise!) the application associated		*/
	/* with the handle hab and returns TRUE  in case of success.			*/

	/* Retrieve some resource Information */
	if( !(pszAppName = LoadString( IDS_APPNAME )) ) {
		Fatal( "Error reading resource data" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}

	hptrRDown =	WinLoadPointer( HWND_DESKTOP, NULLHANDLE, IDR_RDOWNPTR );
	if ( hptrRDown == NULLHANDLE ) {
		Fatal( "Error reading resource data" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}
	hptrBDown = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, IDR_BDOWNPTR );
	if ( hptrBDown == NULLHANDLE ) {
		Fatal( "Error reading resource data" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}
	
	hptrSysArrow = WinQuerySysPointer( HWND_DESKTOP, SPTR_ARROW, FALSE );
	if ( hptrSysArrow == NULLHANDLE ) {
		Fatal( "Error using WinQuerySysPointer" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}

	hptrSysWait = WinQuerySysPointer( HWND_DESKTOP, SPTR_WAIT, FALSE );
	if ( hptrSysWait == NULLHANDLE ) {
		Fatal( "Error using WinQuerySysPointer" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}

	hptrSysIllegal = WinQuerySysPointer( HWND_DESKTOP, SPTR_ILLEGAL, FALSE );
	if ( hptrSysIllegal == NULLHANDLE ) {
		Fatal( "Error using WinQuerySysPointer" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}

	if ( DosCreateEventSem( NULL, &hevDrawInProgress, 0L, TRUE ) ) {
		Fatal( "Error while trying to create a event semaphore" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}
	
	fRegistered =
	WinRegisterClass(
			hab,
			szClientClass,
			WndProc,
			CS_SIZEREDRAW | CS_MOVENOTIFY,
			0 );
	/* WinRegisterClass: registers the window. Returns TRUE if successful.	*/
	/* Parameters: Anchor-block handle,													*/
	/* input window-class name (application-specified), input window-procedure */
	/* identifier, input Default-window style, input number of bytes to		*/
	/* reserved for this window. 	*/
	if ( !fRegistered ) {
		rc = WinGetLastError( hab );
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}
	/* same as above, additional WinDestroyMsgQueue (guess what it does!)	*/

	/* definition of main window, determination of size							*/
	fWndCtrlData = FCF_MINBUTTON 		| FCF_MAXBUTTON	|
						FCF_ACCELTABLE		| FCF_TASKLIST		|
						FCF_SYSMENU 		|
						FCF_TITLEBAR 		| FCF_SIZEBORDER	|
						FCF_MENU				| FCF_ICON			|
						FCF_AUTOICON;

	hwndMain =
	WinCreateStdWindow(
		HWND_DESKTOP, 	 				/* Parent-window 		  								*/
		WS_ANIMATE,						/* don't make window visible yet					*/
		&fWndCtrlData,					/* window parameters, defined above 			*/
		szClientClass,					/* window class, here: "4wins.child"			*/
		NULL,								/* Title Bar text										*/
		0,									/* client-window style								*/
		0,				 					/* resources are bound within 4wins.exe		*/
		IDR_MAIN,						/* Frame-window identifier (see connect4.rc) */
		&hwndClient );					/* output client-window handle					*/
		/* returns Frame-window handle if successful, otherwise NULLHANDLE			*/
	/* check if hwndMain was created */
	if ( hwndMain == NULLHANDLE) {
		Fatal( "Error creating main window" );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	} /* endif */

	/* Initialize help facilities */
	HelpInit.cb = sizeof(HELPINIT);
	HelpInit.ulReturnCode = 0L;
	HelpInit.pszTutorialName = NULL;
	HelpInit.phtHelpTable = (PHELPTABLE)MAKELONG(HELP_TABLE, 0xFFFF);
	HelpInit.hmodHelpTableModule = NULLHANDLE;
	HelpInit.hmodAccelActionBarModule = NULLHANDLE;
	HelpInit.idAccelTable = 0;
	HelpInit.idActionBar = 0;
	HelpInit.pszHelpWindowTitle = "Help 4 Connect";
	HelpInit.fShowPanelId = CMIC_HIDE_PANEL_ID;
	HelpInit.pszHelpLibraryName = "connect4.hlp";

	hwndHelp = WinCreateHelpInstance( hab, &HelpInit );
	if ( !hwndHelp || HelpInit.ulReturnCode  ) {
		/* We couldn't initialize the help system. This is no fatal error
		 * but help will be disabled --> Tell the user about it. */
		WinMessageBox( HWND_DESKTOP, hwndMain,
			"Couldn't create help instance.\nHelp will be disabled.\n" \
			"To use the online help the file connect4.hlp has to be located " \
			"either in the current working directory or in one of the " \
			"directorys your HELP environment variable points to.",
			"Error when using WinCreateHelpInstance",
			0,
			MB_OK | MB_INFORMATION );
	} else if ( !WinAssociateHelpInstance( hwndHelp, hwndMain ) )
		WinMessageBox( hwndClient, hwndMain,
			"Couldn't associate help instance.\nHelp will be disabled.",
	      "Error when using WinAssociateHelpInstance",
	      0,
	      MB_OK | MB_INFORMATION );
	                                                                  
	if ( ReadProfile() ) {
		if ( InitBoard( GameBoard.sizex, GameBoard.sizey ) ) 
			Fatal( "Error when trying to allocate memory for the game board!" );
		else 
			ClearBoardPM();
	} else {
		/* Initalize the game board with default values */
		if( InitDefaultGamePM() ) 
			Fatal( "Error when trying to initialize default game." );
		cxScreen = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
		cyScreen = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );
      WinSetRect( hab, &rectlWinPos,
				0, 0,
				(GameBoard.sizex + 2) * 32,
				(GameBoard.sizey + 2) * 32 );
		WinCalcFrameRect( hwndMain, &rectlWinPos, FALSE );
		WinSetWindowPos( hwndMain, NULLHANDLE,
				(cxScreen - (rectlWinPos.xRight - rectlWinPos.xLeft)) / 2,
				cyScreen - (rectlWinPos.yTop - rectlWinPos.yBottom) - 50,
				rectlWinPos.xRight - rectlWinPos.xLeft,
				rectlWinPos.yTop - rectlWinPos.yBottom,
				SWP_SIZE | SWP_MOVE
		);
	}
   StartSoundThread( SND_INIT ); /* the startup sound	*/
	/* adjust some variables necessary for the game	*/
	GameInfo.InputAllowed = Player->Mind == HUMAN;
	DisableMenu( IDM_GAMEUNDO );
	SetMenu( IDM_GAMESTART, Player->Mind == COMPUTER );
	SetMenu( IDM_GAMEHINT, Player->Mind == HUMAN );

	/* Now we can make the window visible */
	if ( !WinShowWindow( hwndMain, TRUE ) )
		Fatal( "Couldn't make the main window visible!" );

	/* I don't mind if the next operation (making hwndMain the active
	 * foreground window) is not successful. */
	WinSetActiveWindow( HWND_DESKTOP, hwndMain );

	/* get / dispatch message loop */
	while( WinGetMsg( hab, (PQMSG)&qmsg, NULLHANDLE, 0L, 0L ))
		 WinDispatchMsg( hab, (PQMSG)&qmsg );
	/* WinGetMsg is the function that gets all the messages concerning the	*/
	/* application specified by hab, and WinDispatchMsg dispatches				*/
	/* (i. e. posts) them.																	*/
	/* params: Anchor-block handle, output message structure, input			*/
	/* window filter, input First and Last message identity.						*/
	/* Returns TRUE as long as the message is unequal WM_QUIT.					*/

	/* clean up */

	DosCloseEventSem( hevDrawInProgress );
	WinDestroyWindow( hwndMain );
	WinDestroyMsgQueue( hmq );
	WinTerminate( hab );
	return 0;
} /* end of main	 */

/************************************************************************\
 * function SoundThread
 * calls the function PlaySound 
\************************************************************************/
PRIVATE VOID SoundThread( PVOID pssnd )
{
	PlaySound( *(PULONG)pssnd );
	DosPostEventSem( hevSoundInProgress );
	InsertThreadID = -1;
}


/************************************************************************\
 * function StartSoundThread
 * starts the function SoundThread
\************************************************************************/
PRIVATE VOID StartSoundThread( ULONG sound )
{
	static ULONG ssnd;
	LONG lDummy;

	DosResetEventSem( hevSoundInProgress, &lDummy );
	ssnd = sound;
	SoundThreadID = _beginthread( &SoundThread, NULL, 0x8000, (VOID*)&ssnd);
	if ( SoundThreadID == -1 )
		Fatal( "Error creating thread" );
}

/************************************************************************\
 * function UndoThread
 * calls the function InsertCoinPM and adjusts the draw semaphore
\************************************************************************/
PRIVATE VOID UndoThread( PVOID phwnd )
{
	DeleteCoinPM( *(PHWND)phwnd );
	DosCloseEventSem( hevStopInsert );
	DosPostEventSem( hevDrawInProgress );
	UndoThreadID = -1;
}

/************************************************************************\
 * function StartUndoThread
 * starts the function UndoThread as a thread
\************************************************************************/
PRIVATE VOID StartUndoThread( HWND hwnd )
{
	static HWND shwnd;
	LONG lDummy;

	DosResetEventSem( hevDrawInProgress, &lDummy );
	if ( DosCreateEventSem( NULL, &hevStopInsert, 0, FALSE ) ) {
		Fatal( "Error while trying to create an event semaphore" );
		return;
	}
	shwnd = hwnd;
	UndoThreadID = _beginthread( &UndoThread, NULL, 0x8000, (VOID*)&shwnd);
	if ( UndoThreadID == -1 )
		Fatal( "Error creating thread" );
}

/************************************************************************\
 * function InsertThread
 * calls the function InsertCoinPM and adjusts the draw semaphore
\************************************************************************/
PRIVATE VOID InsertThread( PVOID phwnd )
{
	InsertCoinPM( *(PHWND)phwnd );
	DosCloseEventSem( hevStopInsert );
	DosPostEventSem( hevDrawInProgress );
	InsertThreadID = -1;
}

/************************************************************************\
 * function StartInsertThread
 * starts the function InsertThread as a thread
\************************************************************************/
PRIVATE VOID StartInsertThread( HWND hwnd )
{
	static HWND shwnd;
	LONG lDummy;

	DosResetEventSem( hevDrawInProgress, &lDummy );
	if ( DosCreateEventSem( NULL, &hevStopInsert, 0, FALSE ) ) {
		Fatal( "Error while trying to create an event semaphore" );
		return;
	}
	shwnd = hwnd;
	InsertThreadID = _beginthread( &InsertThread, NULL, 0x8000, (VOID*)&shwnd);
	if ( InsertThreadID == -1 )
		Fatal( "Error creating thread" );
}

/************************************************************************\
 * function StopInsertThread (also stops the undo thread)
 * This functions stops the thread only if it is active.
\************************************************************************/
PRIVATE VOID StopInsertThread()
{
	if ( InsertThreadID != -1 || UndoThreadID != -1 ) {
		DosPostEventSem( hevStopInsert );
	}
}

/************************************************************************\
 * function NewThread
 * Thread that draws after the Game New option
\************************************************************************/
PRIVATE VOID NewThread( PVOID phwnd )
{
	HPS hps;

	hps = WinGetPS( *(PHWND)phwnd );
	DropCoinsPM( hps );
   WinReleasePS( hps );
	DosPostEventSem( hevDrawInProgress );
	NewThreadID = -1;
}


/************************************************************************\
 * function StartNewThread
 * starts the function NewThread as a thread
\************************************************************************/
PRIVATE VOID StartNewThread( HWND hwnd )
{
	static HWND shwnd;
	LONG lDummy;

	shwnd = hwnd;
	DosResetEventSem( hevDrawInProgress, &lDummy );
	NewThreadID = _beginthread( &NewThread, NULL, 0x8000, (VOID*)&shwnd);
	if ( NewThreadID == -1 )
		Fatal( "Error creating thread" );
}
/************************************************************************\
 * function ShowHintThread
 * Thread that draws after the Game ShowHint option
\************************************************************************/
PRIVATE VOID ShowHintThread( PVOID phwnd )
{
	HPS hps;

	hps = WinGetPS( *(PHWND)phwnd );
	ShowHintPM( hps );
   if( !WinReleasePS( hps ))
		Fatal( "Error using WinReleasePS" );
	DosPostEventSem( hevDrawInProgress );
	DosCloseEventSem( hevStopFlash );
	ShowHintThreadID = -1;
}


/************************************************************************\
 * function StartShowHintThread
 * starts the function ShowHintThread as a thread
\************************************************************************/
PRIVATE VOID StartShowHintThread( HWND hwnd )
{
	static HWND shwnd;
	LONG lDummy;

	assert( ShowHintThreadID == -1 );
	shwnd = hwnd;
	DosResetEventSem( hevDrawInProgress, &lDummy );
	if ( DosCreateEventSem( NULL, &hevStopFlash, 0, FALSE ) ) {
		Fatal( "Error while trying to create an event semaphore" );
		return;
	}
	ShowHintThreadID = _beginthread( &ShowHintThread, NULL, 0x8000, (VOID*)&shwnd);
	if ( ShowHintThreadID == -1 )
		Fatal( "Error creating thread" );
}

/************************************************************************\
 * function ShowWinThread
 * Thread that draws after the Game ShowWin option
\************************************************************************/
PRIVATE VOID ShowWinThread( PVOID phwnd )
{
	HPS hps;

	hps = WinGetPS( *(PHWND)phwnd );
	ShowWinPM( hps );
   if( !WinReleasePS( hps ))
		Fatal( "Error using WinReleasePS" );
	DosCloseEventSem( hevStopFlash );
	DosPostEventSem( hevDrawInProgress );
	ShowWinThreadID = -1;
}


/************************************************************************\
 * function StartShowWinThread
\************************************************************************/
PRIVATE VOID StartShowWinThread( HWND hwnd )
{
	static HWND shwnd;
	ULONG ulPostCt;

	shwnd = hwnd;
	DosWaitEventSem( hevDrawInProgress, -1 );
	DosResetEventSem( hevDrawInProgress, &ulPostCt );
	if ( DosCreateEventSem( NULL, &hevStopFlash, 0, FALSE ) ) {
		Fatal( "Error while trying to create an event semaphore" );
		return;
	}
	ShowWinThreadID = _beginthread( &ShowWinThread, NULL, 0x8000, (VOID*)&shwnd);
	if ( ShowWinThreadID == -1 )
		Fatal( "Error creating thread" );
}

PRIVATE VOID StopFlashThreads( VOID )
/************************************************************************\
 * function StopFlashThreads
 * This functions stops the threads... 
\************************************************************************/
{
	if ( ShowWinThreadID != -1 || ShowHintThreadID != -1 ) {
		DosPostEventSem( hevStopFlash ); /* this semaphore is checked in the	*
													 * drawing routines						*/
	}
}

/************************************************************************\
 * function ComputerThread
 * Thread that calculates the computer's move
\************************************************************************/
PRIVATE VOID ComputerThread( VOID* dummy )
{
	BYTE col;
	/* Set a lower priority for this thread so that the application
	 * reacts faster on user inputs */
   DosSetPriority( PRTYS_THREAD, PRTYC_NOCHANGE, -4, 0 );
   /* Get the computers choice */
   col = GetComputerMove();	
   /* Wait until any drawing has finished. We wait here because we don't
	 * want to disable the message queue. */
   DosWaitEventSem( hevDrawInProgress, -1 );
   /* Send selected column */
	WinPostMsg( hwndMain, WM_THREADEND, MPFROMCHAR(col), 0 );
}

/************************************************************************\
 * function StartComputerThread
 * This functions starts the thread which calculates the computer's choice
 * for the next move.
\************************************************************************/
PRIVATE VOID StartComputerThread()
{
	ComputerThreadID = _beginthread( &ComputerThread, NULL, 0x8000, NULL);
	if ( ComputerThreadID == -1 )
		Fatal( "Error creating thread" );
}

/************************************************************************\
 * function StopComputerThread
 * This functions stops the thread only if it is active.
\************************************************************************/
PRIVATE VOID StopComputerThread()
{
	if ( ComputerThreadID != - 1) {
		if ( DosKillThread( ComputerThreadID ) )
			Fatal( "Couldn't stop computer calculation." );
		else
		   ComputerThreadID = -1;
	}
}

/************************************************************************\
 * function SuspendComputerThread
 * Suspend calculation thread if it is active.
\************************************************************************/
PRIVATE VOID SuspendComputerThread()
{
	if ( ComputerThreadID != - 1) 
		if ( DosSuspendThread( ComputerThreadID ) )
			Fatal( "Couldn't suspend computer calculation." );
}
/************************************************************************\
 * function ResumeComputerThread
\************************************************************************/
PRIVATE VOID ResumeComputerThread()
{
	if ( ComputerThreadID != - 1) 
		if ( DosResumeThread( ComputerThreadID ) )
			Fatal( "Couldn't resume computer calculation." );
}


/*************************************************************************\
 * function LoadString
 * Load a string from the resource.
 * If an error occured NULL is returned
\*************************************************************************/
PRIVATE PSZ LoadString( USHORT IdStr )
{
	PSZ pszBuffer = (PSZ)malloc( 256 );
	LONG lLen;

	if ( !(lLen = WinLoadString( hab, NULLHANDLE, IdStr, 256, pszBuffer )) )
		return NULL;
	return realloc( pszBuffer, lLen + 1);
}
	
	
/*************************************************************************\
 * Enable menu items
\*************************************************************************/
PUBLIC inline VOID EnableMenu( const USHORT usId )
{
	const HWND hwndMenu = WinWindowFromID( hwndMain, FID_MENU );
	(VOID)WinEnableMenuItem( hwndMenu, usId, TRUE );
}

/*************************************************************************\
 * Disable menu items
\*************************************************************************/
PUBLIC inline VOID DisableMenu( const USHORT usId )
{
	const HWND hwndMenu = WinWindowFromID( hwndMain, FID_MENU );
	(VOID)WinEnableMenuItem( hwndMenu, usId, FALSE );
}

/*************************************************************************\
 * Enable/Disable menu items
\*************************************************************************/
PUBLIC inline VOID SetMenu( const USHORT usId, const BOOL fEnable )
{
	const HWND hwndMenu = WinWindowFromID( hwndMain, FID_MENU );
	(VOID)WinEnableMenuItem( hwndMenu, usId, fEnable );
}

/*************************************************************************\
 * function SetPointer
 * Set the pointer icon. The icon used depends on the current position
 * and the game status.
\*************************************************************************/
PRIVATE inline HPOINTER GetPointer( const POINTL ptl )
{
	if ( ProgInfo.fChgPointer ) {
		const INT col = GetBoardCol( ptl.x );
		const INT row = GetBoardRow( ptl.y );
	
		if( col && row ) {
			if ( GameInfo.InputAllowed ){
				if ( GameBoard.hgt[col] < GameBoard.sizey ) {
					return (Player->Color == RED) ? hptrRDown : hptrBDown;
				} else {
					return hptrSysIllegal;
				}
			} else {
				if (ComputerThreadID != -1)
					return hptrSysWait;
			}
		}
	}
	return hptrSysArrow;	/* If any condition isn't true
								 * --> return default system pointer 	*/
}

/*************************************************************************\
 * Client-Window-procedure of the main window
 * Here all the message handling takes place.
\*************************************************************************/
MRESULT EXPENTRY WndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	/* some variables used during message processing */
	HPS hps; 						/* Presentation Space Handle		 */
	RECTL rectl;					/* rectangle for WM_PAINT			 */
	POINTL ptl;						/* Mouse pointer position			 */
	INT col;
	INT row;
	INT OldMind;
	BOOL fResponse;
	
	switch (msg) {
		case WM_MOUSEMOVE:
			ptl.x = (LONG)SHORT1FROMMP( mp1 );
			ptl.y = (LONG)SHORT2FROMMP( mp1 );
			WinSetPointer( HWND_DESKTOP, GetPointer( ptl ) );
			return (MRESULT)0;
		case WM_PAINT:
			hps = WinBeginPaint( hwnd, 0, &rectl );
			WinFillRect( hps, &rectl, SYSCLR_WINDOW );
			/* clear background of the rectangle that has been invalidated	*/
			DrawBoard( hps, rectl );
			WinEndPaint( hps );
        	return (MRESULT)0;
		case WM_BUTTON1CLICK:
			if ( GameInfo.InputAllowed ) {
				ptl.x = (LONG)SHORT1FROMMP( mp1 );
				ptl.y = (LONG)SHORT2FROMMP( mp1 );
				col = GetBoardCol( ptl.x );
				row = GetBoardRow( ptl.y );
				
				if ( col != 0 && row != 0 &&
					  GameBoard.hgt[col] < GameBoard.sizey ) {
					StopComputerThread();		/* stop hint thread if active */
					StopFlashThreads();
					/* Don't wait until the threads are finished - this is done	*
					 * during the processing of the WM_INSERT message 				*/
					WinPostMsg( hwnd, WM_INSERT, (MPARAM)col, (MPARAM)0 );
					EnableMenu( IDM_GAMEUNDO );
				} else {
					WinAlarm( HWND_DESKTOP, WA_NOTE );
				}
			} else {
				WinAlarm( HWND_DESKTOP, WA_NOTE );
			}
			return (MRESULT)0;
		case WM_COMMAND:
         switch ( SHORT1FROMMP( mp1 )) {
         	case IDM_FILELOAD:
         		if ( LoadGame() )
         			WinInvalidateRect( hwnd, NULL, FALSE );
         		return (MRESULT)0;
         	case IDM_FILESAVE:
         		if ( !SaveGame() )
						NonFatal( "Error when trying to save the current game" );
         		return (MRESULT)0;
				case IDM_FILEEXIT:
					StopComputerThread();
					StopFlashThreads();
					StopInsertThread();
            	WinSendMsg( hwnd, WM_CLOSE, NULL, NULL );
              	return (MRESULT)0;
            case IDM_GAMENEW:
					StopComputerThread();	/* Stop background calculation	*/
					StopFlashThreads();	/* Stop showing hints or winning coins	*/
					StopInsertThread();		/* Stop dropping coins.				*/
					DosWaitEventSem( hevDrawInProgress, -1 ); 
		        	if ( GameBoard.movecount > 0 ) {
						StartNewThread( hwnd );
						PlaySound( SND_NEW );
						DisableMenu( IDM_GAMEUNDO );
						SetMenu( IDM_GAMESTART, Player->Mind == COMPUTER );
						SetMenu( IDM_GAMEHINT, Player->Mind == HUMAN );
					}
            	return (MRESULT)0;
            case IDM_GAMESTART:
            	StartComputerThread();
		        	return (MRESULT)0;
				case IDM_GAMEUNDO:
            	assert( GameBoard.movecount > 0 );
					StopComputerThread();
            	StopFlashThreads();
					DosWaitEventSem( hevDrawInProgress, -1 );
           		StartSoundThread( SND_UNDO );
           		StartUndoThread( hwnd );
					DosWaitEventSem( hevDrawInProgress, -1 );
           		DeleteCoin( GameBoard.moves[GameBoard.movecount] );
           		if ( GameInfo.UndoCount == 2 ) {
	            	assert( GameBoard.movecount > 0 );
	            	StartUndoThread( hwnd );
						DosWaitEventSem( hevDrawInProgress, -1 );
	           		DeleteCoin( GameBoard.moves[GameBoard.movecount] );
            		if( GameInfo.HasLaughed ) GameInfo.HasLaughed--; 
					} else {
						TogglePlayer();
					}
					if ( GameBoard.movecount ==
						( ( PlayerInfo[0].Mind == HUMAN ) ? 0 : 1 ) ) {
						DisableMenu( IDM_GAMEUNDO );
					}
					GameInfo.InputAllowed = TRUE;
					EnableMenu( IDM_GAMEHINT );
					WinQueryPointerPos( HWND_DESKTOP, &ptl );
					WinMapWindowPoints( HWND_DESKTOP, hwnd, &ptl, 1 );
					WinSetPointer( HWND_DESKTOP, GetPointer( ptl ) );
					return (MRESULT)0;
				case IDM_GAMEHINT:
					assert( GameInfo.InputAllowed );
					DisableMenu( IDM_GAMEHINT );
					StartComputerThread();
					return (MRESULT)0;
				case IDM_GAMEOPTS: 
					OldMind = Player->Mind;
					SuspendComputerThread();
					fResponse = 
					 	WinDlgBox(
							HWND_DESKTOP, 		/* Place anywhere on desktop		*/
							hwndMain,			/* Owned by main	 					*/
							GameOptsDlgProc,	/* Address of dialog procedure	*/
							(HMODULE)0,			/* Module handle					 	*/
							IDR_GAMEOPTDLG,	/* Dialog identifier in resource */
							NULL );	 			/* Initialization data				*/
					if ( GameBoard.movecount == 0 ) {
						SetPlayer( ProgInfo.StartingPlayer );
						if ( Player->Mind == COMPUTER ) {
							EnableMenu( IDM_GAMESTART );
							GameInfo.InputAllowed = FALSE;
							DisableMenu( IDM_GAMEHINT );
						} else {
							GameInfo.InputAllowed = TRUE;
							EnableMenu( IDM_GAMEHINT );
							DisableMenu( IDM_GAMESTART );
						}
						WinQueryPointerPos( HWND_DESKTOP, &ptl );
						WinMapWindowPoints( HWND_DESKTOP, hwnd, &ptl, 1 );
						WinSetPointer( HWND_DESKTOP, GetPointer( ptl ) );
					}
					if ( ( GameBoard.movecount == 0 || !TestEnd() )
							 && OldMind != Player->Mind ) {
						/* stop hint or move thread if active */
						StopComputerThread();	
						if ( Player->Mind == COMPUTER ) {
							if ( GameBoard.movecount > 0 ) {
 								/* Begin calculation of next move immediatly */
 								StartComputerThread();
							}
							GameInfo.InputAllowed = FALSE;
							DisableMenu( IDM_GAMEHINT );
						} else {
							GameInfo.InputAllowed = TRUE;
							EnableMenu( IDM_GAMEHINT );
							DisableMenu( IDM_GAMESTART );
							WinQueryPointerPos( HWND_DESKTOP, &ptl );
							WinMapWindowPoints( HWND_DESKTOP, hwnd, &ptl, 1 );
							WinSetPointer( HWND_DESKTOP, GetPointer( ptl ) );
						}
					} else {
						ResumeComputerThread();
					}
					return (MRESULT)0;
				case IDM_PROGOPTS: 
					fResponse = 
					 	WinDlgBox(
							HWND_DESKTOP, 		/* Place anywhere on desktop		*/
							hwndMain,			/* Owned by main	 					*/
							ProgOptsDlgProc,	/* Address of dialog procedure	*/
							(HMODULE)0,			/* Module handle					 	*/
							IDR_PROGOPTDLG,	/* Dialog identifier in resource */
							NULL );	 			/* Initialization data				*/
					return (MRESULT)0;

				/* The messages IDM_HELPINDEX and IDM_HELPGENERAL are			
				 * handled automaticly because of the definitions you find
				 * in the file help.rc. */
				case IDM_HELPPRODUCTINFO:
					fResponse = 
					 	WinDlgBox(
							HWND_DESKTOP, 		/* Place anywhere on desktop		*/
							hwndMain,			/* Owned by main	 					*/
							ProdInfoDlgProc,	/* Address of dialog procedure	*/
							(HMODULE)0,			/* Module handle					 	*/
							IDR_PRODINFODLG,	/* Dialog identifier in resource */
							NULL );	 			/* Initialization data				*/
					return (MRESULT)0;
				case IDM_SAVEOPTS:
					if ( !WriteProfile() )
						NonFatal( "Error writing initialization file." );
					return (MRESULT)0;
				default:
					return (MRESULT)WinDefWindowProc( hwnd, msg, mp1, mp2 );
         } /* end switch mp1 */
			break;
		case WM_INSERT:
			DosWaitEventSem( hevDrawInProgress, -1 );
			InsertCoin( (byte)CHAR1FROMMP(mp1), Player->Color ); 
			StartInsertThread( hwnd );
			PlaySound( SND_INSERT );
			if ( Player->Mind == HUMAN ) 
				GameInfo.UndoCount = 1;
			else {
 				GameInfo.UndoCount++;
 				if( GameBoard.lastval[0] >= inf - MAXDEPTH
					 && Player->CalcDepth > 4){
	 				if( !GameInfo.HasLaughed ) 
						PlaySound( SND_LAUGH );
						/* Computer laughs when he knows he will win	*/
					GameInfo.HasLaughed++;
				}	
 			}	
			switch ( TestEnd() ) {
				case 0:	/* nobody has won	*/
					DisableMenu( IDM_GAMESTART );
					TogglePlayer();
					GameInfo.InputAllowed = Player->Mind == HUMAN;
					WinQueryPointerPos( HWND_DESKTOP, &ptl );
					WinMapWindowPoints( HWND_DESKTOP, hwnd, &ptl, 1 );
					if ( Player->Mind == COMPUTER ) {
						GameInfo.InputAllowed = FALSE;
						DisableMenu( IDM_GAMEHINT );
						StartComputerThread();
					} else {
						GameInfo.InputAllowed = TRUE;
						EnableMenu( IDM_GAMEHINT );
					}
					WinSetPointer( HWND_DESKTOP, GetPointer( ptl ) );
					return (MRESULT)0;
   			case 1:	/* one of the players has won	*/
   				GameInfo.InputAllowed = FALSE;
					DisableMenu( IDM_GAMEHINT );
					WinSetPointer(	HWND_DESKTOP, hptrSysArrow );
					if ( Player->Mind == HUMAN ) {
						StartShowWinThread( hwnd );
						PlaySound( SND_WIN );
					} else {
						StartShowWinThread( hwnd );
						PlaySound( SND_LOOSE );
					}
					TogglePlayer(); 		/* Need this if undo is selected */
					return (MRESULT)0;
				case 2:	/* the game board is full and no one has won	*/
					PlaySound( SND_DRAW );   
   				GameInfo.InputAllowed = FALSE;
					DisableMenu( IDM_GAMEHINT );
					WinSetPointer(	HWND_DESKTOP, hptrSysArrow );
					DosWaitEventSem( hevDrawInProgress, -1 );
					TogglePlayer();
					return (MRESULT)0;
			} /* end switch TestEnd() */
			return (MRESULT)0;
		case WM_THREADEND:
			ComputerThreadID = -1;
			if (Player->Mind == COMPUTER) {	/* computer has made a move	*/
				WinPostMsg( hwnd, WM_INSERT, mp1, mp2 );
			} else {	/* computer has calculated a hint	*/
				GameInfo.HintCol = (INT)CHAR1FROMMP(mp1);
				GameInfo.HintColor = Player->Color;
				StartShowHintThread( hwnd );
				PlaySound( SND_HINT );
			}
			return (MRESULT) 0;
		case WM_CONTROL:
			break;
		case WM_CREATE:
			/* Proceed with the default processing of this message */
			break;
		case WM_QUIT:
			break;
		case WM_CLOSE:	
			StopComputerThread();		/* Stop all active threads */
			StopFlashThreads();
			StopInsertThread();
			DosWaitEventSem( hevSoundInProgress, -1 );
			StartSoundThread( SND_EXIT );			
			/* Save game settings if this option is enabled */
			if ( ProgInfo.fSaveSettings )
				WriteProfile( );
			break;
		case WM_FATAL:
			WinPostMsg( 0, WM_QUIT, (MPARAM)0, (MPARAM)0 );
			return (MRESULT)0;
		case WM_MOVE:
			StopInsertThread();
			StopFlashThreads();
			DosWaitEventSem( hevDrawInProgress, -1 );
			return (MRESULT)0;
		case WM_SIZE:
			StopInsertThread();
			StopFlashThreads();
			DosWaitEventSem( hevDrawInProgress, -1 );
			SetBoardRect( SHORT1FROMMP(mp2), SHORT2FROMMP (mp2) );
			break;
		case HM_HELPSUBITEM_NOT_FOUND:
			return (MRESULT)FALSE;
		default:
			return (MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2);
	} /* end switch msg */
	return (MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2);
}

/*************************************************************************\
 * Set the value of a slider 
\*************************************************************************/
PRIVATE inline VOID SetSliderValue(
	const HWND hwndDlg, const USHORT usId, const ULONG ulValue )
{
	WinSendDlgItemMsg(
		hwndDlg, usId,
		SLM_SETSLIDERINFO,
		MPFROM2SHORT( SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE ),
		(MPARAM)ulValue );
}

/*************************************************************************\
 * Get the value of a slider 
\*************************************************************************/
PRIVATE inline ULONG GetSliderValue(
	const HWND hwndDlg, const USHORT usId )
{
	return (ULONG)WinSendDlgItemMsg(
				hwndDlg, usId,
				SLM_QUERYSLIDERINFO,
				MPFROM2SHORT( SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE ),
				(MPARAM)0 );
}

/*************************************************************************\
 * Set the value of a spinbutton
\*************************************************************************/
PRIVATE inline VOID SetSpinValue(
	const HWND hwndDlg, const USHORT usId, const ULONG ulValue )
{
	WinSendDlgItemMsg( hwndDlg, usId, SPBM_SETCURRENTVALUE,
							 (MPARAM)ulValue, (MPARAM)0 );
}
	
/*************************************************************************\
 * function GameOptsDlgProc
 * Client-Window-procedure of the options dialog window;
\*************************************************************************/
MRESULT EXPENTRY GameOptsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	BOOL fHuman;

	switch( msg )
	{
		case WM_INITDLG: 	/* the dialog window is opened	*/
			fHuman = PlayerInfo[0].Mind == HUMAN;
			(VOID)WinCheckButton( hwnd, (USHORT)IDC_HUMAN1, fHuman );
			(VOID)WinCheckButton( hwnd, (USHORT)IDC_COMPUTER1, !fHuman );
			WinSendDlgItemMsg( hwnd, IDC_SKILL1, SLM_SETTICKSIZE,
								    MPFROM2SHORT(SMA_SETALLTICKS, 5),
									 (MPARAM)0 );
			SetSliderValue( hwnd, IDC_SKILL1, PlayerInfo[0].CalcDepth - 1);
			WinSendDlgItemMsg( hwnd, IDC_SKILLVAL1, SPBM_SETLIMITS,
								    MPFROMLONG(MAXDEPTH), MPFROMLONG(1));
			SetSpinValue( hwnd, IDC_SKILLVAL1, PlayerInfo[0].CalcDepth );
			fHuman = PlayerInfo[1].Mind == HUMAN;
			(VOID)WinCheckButton( hwnd, (USHORT)IDC_HUMAN2, fHuman );
			WinSendDlgItemMsg( hwnd, IDC_SKILLVAL2, SPBM_SETLIMITS,
								    MPFROMLONG(MAXDEPTH), MPFROMLONG(1));
			(VOID)WinCheckButton( hwnd, (USHORT)IDC_COMPUTER2, !fHuman );
			WinSendDlgItemMsg( hwnd, IDC_SKILL2, SLM_SETTICKSIZE,
								    MPFROM2SHORT(SMA_SETALLTICKS, 5),
									 (MPARAM)0 );
			SetSliderValue( hwnd, IDC_SKILL2, PlayerInfo[1].CalcDepth - 1);
			SetSpinValue( hwnd, IDC_SKILLVAL2, PlayerInfo[1].CalcDepth );
			fHuman = ProgInfo.StartingPlayer == RED;	/* okay, fHuman doesn't		*
																	 * suit in this case, but	*
																	 * it was an existing var	*/
			(VOID)WinCheckButton( hwnd, IDC_STARTRED, fHuman );
			(VOID)WinCheckButton( hwnd, IDC_STARTBLUE, !fHuman );														
			break;
		case WM_COMMAND:
			switch( SHORT1FROMMP( mp1 ) )
			{
				case DID_OK:	/* the okay button is pressed	*/
					if( WinQueryButtonCheckstate( hwnd, IDC_HUMAN1 ) == 1 ) {
						PlayerInfo[0].Mind = HUMAN;
					} else {
						PlayerInfo[0].Mind = COMPUTER;
					}
					PlayerInfo[0].CalcDepth = GetSliderValue( hwnd, IDC_SKILL1 ) + 1;
					if( WinQueryButtonCheckstate( hwnd, IDC_HUMAN2 ) == 1 ) {
						PlayerInfo[1].Mind = HUMAN;
					} else {
						PlayerInfo[1].Mind = COMPUTER;
					}
					PlayerInfo[1].CalcDepth = GetSliderValue( hwnd, IDC_SKILL2 ) + 1;
					if( WinQueryButtonCheckstate( hwnd, IDC_STARTRED ) == 1 ) 
						ProgInfo.StartingPlayer = RED;
					else
						ProgInfo.StartingPlayer = BLUE;	
					WinDismissDlg( hwnd, TRUE );
					return (MRESULT)0;
				case DID_CANCEL:	/* the cancel button is pressed	*/
					WinDismissDlg( hwnd, FALSE );
					return (MRESULT)0;
				default:
					return WinDefDlgProc( hwnd, msg, mp1, mp2 );
					break;
			} /* end switch ( mp1 ) */
		case WM_CONTROL:	/* a slider is moved	*/
			switch( SHORT1FROMMP( mp1 ) ) {
				case IDC_SKILL1:
					if ( SHORT2FROMMP( mp1 ) == SLN_CHANGE )
						SetSpinValue( hwnd, IDC_SKILLVAL1,
										  GetSliderValue( hwnd, IDC_SKILL1 ) + 1 );
					break;
				case IDC_SKILL2:
					if ( SHORT2FROMMP( mp1 ) == SLN_CHANGE )
						SetSpinValue( hwnd, IDC_SKILLVAL2,
										  GetSliderValue( hwnd, IDC_SKILL2 ) + 1 );
					break;
			default:
				return WinDefDlgProc( hwnd, msg, mp1, mp2 );
			} /* end switch mp1.short1 */
		default:
			return WinDefDlgProc( hwnd, msg, mp1, mp2 );
	}
	return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/*************************************************************************\
 * function ProgOptsDlgProc
 * Client-Window-procedure of the options/program dialog window;
\*************************************************************************/
MRESULT EXPENTRY ProgOptsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	BOOL sndstate = ProgInfo.fSound;
	switch( msg )
	{
		case WM_INITDLG:
#ifdef USEMMSOUND
			/* when the program is compiled without sound, the "Enable Sound"	*
			 * checkbutton will be disabled during the whole game					*/
			(VOID)WinCheckButton( hwnd, (USHORT)IDC_SOUND, ProgInfo.fSound );
#else
			WinEnableControl( hwnd, IDC_SOUND, FALSE );
			(VOID)WinCheckButton( hwnd, (USHORT)IDC_SOUND, FALSE );
#endif /* USEMMSOUND */
			(VOID)WinCheckButton(
				hwnd, (USHORT)IDC_SAVESETTINGS, ProgInfo.fSaveSettings );
			(VOID)WinCheckButton(
				hwnd, (USHORT)IDC_ANIMATE, ProgInfo.fAnimate );
			(VOID)WinCheckButton(
				hwnd, (USHORT)IDC_CHGPOINTER, ProgInfo.fChgPointer );
			break;
		case WM_COMMAND:
			switch( SHORT1FROMMP( mp1 ) )
			{
				case DID_OK:
					ProgInfo.fSound =
						(BOOL)WinQueryButtonCheckstate( hwnd, IDC_SOUND );
					if( sndstate != ProgInfo.fSound ) {
						if ( ProgInfo.fSound ) StartSoundThread( SND_INIT );
						else {
							ProgInfo.fSound = TRUE;
							PlaySound( SND_EXIT );
							ProgInfo.fSound = FALSE;
							/* this is done because otherwise the  Exit-sound	*
							 * can't be played (the sound flag is already disabled)*/
						}
					}	
					ProgInfo.fSaveSettings =
						(BOOL)WinQueryButtonCheckstate( hwnd, IDC_SAVESETTINGS );
					ProgInfo.fAnimate =
						(BOOL)WinQueryButtonCheckstate( hwnd, IDC_ANIMATE );
					ProgInfo.fChgPointer =
						(BOOL)WinQueryButtonCheckstate( hwnd, IDC_CHGPOINTER );
					WinDismissDlg( hwnd, TRUE );
					return (MRESULT)0;
				case DID_CANCEL:
					WinDismissDlg( hwnd, FALSE );
					return (MRESULT)0;
				default:
					return WinDefDlgProc( hwnd, msg, mp1, mp2 );
					break;
			} /* end switch ( mp1 ) */
		case WM_CONTROL:	/* was ist das eigentlich hier ??? */
			switch( SHORT1FROMMP( mp1 ) ) {
				case IDC_HUMAN1:
					if (SHORT2FROMMP( mp1 ) == BN_CLICKED ) {
						WinEnableControl( hwnd, IDC_SKILL1, FALSE );
					}
					break;
				case IDC_COMPUTER1:
					if (SHORT2FROMMP( mp1 ) == BN_CLICKED ) {
						WinEnableControl( hwnd, IDC_SKILL1, TRUE );
					}
					break;
				case IDC_HUMAN2:
					if (SHORT2FROMMP( mp1 ) == BN_CLICKED ) {
						WinEnableControl( hwnd, IDC_SKILL2, FALSE );
					}
					break;
				case IDC_COMPUTER2:
					if (SHORT2FROMMP( mp1 ) == BN_CLICKED ) {
						WinEnableControl( hwnd, IDC_SKILL2, TRUE );
					}
					break;
			default:
				return WinDefDlgProc( hwnd, msg, mp1, mp2 );
			} /* end switch mp1.short1 */
		default:
			return WinDefDlgProc( hwnd, msg, mp1, mp2 );
	}
	return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/*************************************************************************\
 * Client-Window-procedure of the product info window
\*************************************************************************/
MRESULT EXPENTRY ProdInfoDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	switch(msg)
	{
		case WM_INITDLG:
			return (MRESULT)0;
		case WM_COMMAND:	/* No matter what the command, close the dialog	*/
			WinDismissDlg(hwnd, TRUE);
			return (MRESULT)0;
		default:
			return WinDefDlgProc( hwnd, msg, mp1, mp2 );
			break;
	} /* end switch msg */
	return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

