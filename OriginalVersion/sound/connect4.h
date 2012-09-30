/***************************************************************************\
 *  File name  :  connect4.h
 *
 *  Description:  global variables and function prototypes of connect4.c
 *                
 *
 * Set tabs to 3 to get a readable source.
\***************************************************************************/
#define INCL_WINPOINTERS


/* global variables */

PUBLIC HWND hwndMain;					/* Handle of the main window				*/
PUBLIC HAB  hab;                    /* process anchor block 					*/
PUBLIC HMQ  hmq;                    /* handle of the message queue			*/

PUBLIC struct {
	BOOL fSound;
} ProgInfo;


/* function prototypes */

PUBLIC inline VOID EnableMenu( const HWND hwnd, const USHORT usId );

PUBLIC inline VOID DisableMenu( const HWND hwnd, const USHORT usId );

PUBLIC VOID DisplayMessage( PCH str );

