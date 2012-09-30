/***************************************************************************\
 * connect4.h
 *
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\***************************************************************************/
#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

/* definitions */

#define WM_FATAL 			WM_USER
#define WM_INSERT 		WM_USER + 1
#define WM_THREADEND 	WM_USER + 2
#define MYM_ENDINSERT	WM_USER + 3

/* global variables */

PUBLIC HAB  hab;                    /* process anchor block 					*/
PUBLIC HMQ  hmq;                    /* handle of the message queue			*/
PUBLIC HWND hwndMain;					/* Handle of the main window				*/
PUBLIC HWND hwndClient;					/* Handle of the client window (the
												 * window where our action takes place)*/
PUBLIC HWND hwndHelp;					/* Help window handle.						*/

PUBLIC PSZ pszProgPath;					/* the pathname where the program is
												 * located 										*/
PUBLIC PSZ pszProgName;					/* the program file name					*/

PUBLIC PSZ pszAppName;
												/* Used for profile (connect4.ini)		*/
												/* handling.									*/
#ifdef INITIALIZE
PUBLIC PCSZ pszVersion = "1.0.0";	/* Version number								*/
#else
PUBLIC PCSZ pszVersion;					/* Version number								*/
#endif												

PUBLIC struct {
	BOOL fSaveSettings;
	BOOL fSound;
	BOOL fAnimate;
	BOOL fAdjustSize;
	BOOL fChgPointer;
	BYTE StartingPlayer;	
} ProgInfo;


/* function prototypes */

PUBLIC inline VOID EnableMenu( const USHORT usId );

PUBLIC inline VOID DisableMenu( const USHORT usId );

PUBLIC inline VOID SetMenu( const USHORT usId, const BOOL fEnable );
