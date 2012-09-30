/*************************************************************************\
 * profile.c
 *	Any profile (connect4.ini) related stuff is located here.
 * Set tabs to 3 to get a readable source code.
\*************************************************************************/
#define INCL_WINSHELLDATA
#include <os2.h>
#include <string.h>

#include "compile.h"
#include "game.h"
#include "misc.h"
#include "connect4.h"
#include "pmgame.h"
#define INITIALIZE
#include "profile.h"

#define MaxVersionLen 8

PRIVATE struct {
	PCSZ pszVersion;
	PCSZ pszProg;
	PCSZ pszPlayer;
	PCSZ pszBoardSize;
	PCSZ pszWinPos;
	PCSZ pszSavedSize;
	PCSZ pszSavedMoves;
} PrfKeys = {
	"Version",
	"ProgSettings",
	"PlayerSettings",
	"BoardSize",
	"WinPos",
	"SavedBoardSize",
	"SavedBoardData" };


PRIVATE PCSZ pszIniName = "connect4.ini";

typedef struct {
	BYTE Mind;
	BYTE CalcDepth;
} PRFPLAYERINFO;

/*************************************************************************\
 * function StoreWindowPos
 * This function is meant as a replacement for the os/2 library function
 * WinStoreWindowPos. It stores the current size and postion of the window
 * hwnd (window handle) to the profile hini (profile handle).
 * I use my own function because the original always stores it's
 * information in os2.ini which slows down system performace. 
 * Note: The function WinStoreWindowPos does a bit more. It also saves
 * the presentation parameters (i.e. color, fonts etc) of the window.
 * Because I don't need this it's not included here. (It would be a lot of
 * stupid work.)
 * Like the original this function returns TRUE if it was successful and
 * FALSE otherwise.
\*************************************************************************/
BOOL StoreWindowPos(	const HINI hini, const PSZ pszAppName,
							const PCSZ pszKeyName, const HWND hwnd )
{
	SWP swp;				/* structure to save window information returned
							 * by WinQueryWindowPos 									*/
	LONG lWinPos[4];
	
	
	if ( !WinQueryWindowPos( hwnd, &swp ) ) return FALSE;
	lWinPos[0] = swp.x;
	lWinPos[1] = swp.y;
	lWinPos[2] = swp.cx;
	lWinPos[3] = swp.cy;
	/* write the values of swp.cy, swp.cx, swp.y and swp.x only */
	if ( !PrfWriteProfileData( hini, pszAppName, pszKeyName,
										lWinPos, 4 * sizeof( LONG ) )
		) return FALSE;
	return TRUE;
}

/*************************************************************************\
 * function RestoreWindowPos
 * Analogue to StoreWindowPos but the contratry action.
\*************************************************************************/
PUBLIC BOOL RestoreWindowPos( const HINI hini, const PSZ pszAppName,
										const PCSZ pszKeyName, const HWND hwnd )
{
	LONG lWinPos[4];
	ULONG ulDataLen = 4 * sizeof( LONG );

	/* Overwrite the values of swp.cy, swp.cx, swp.y and swp.x. */
	if ( !PrfQueryProfileData( hini, pszAppName, pszKeyName,
										lWinPos, &ulDataLen )
		) return FALSE;
	/* Set new position */
	if ( !WinSetWindowPos( hwnd, NULLHANDLE,
								  lWinPos[0], lWinPos[1], lWinPos[2], lWinPos[3],
								  SWP_SIZE | SWP_MOVE )
		) return FALSE;
	return TRUE;
}

/*************************************************************************\
 * function ReadProfile()
 * Tries to open the file "connect4.ini" and sets game specific options taken
 * from the profile data. If the file isn't found or the data is invalid
 * FALSE is returned otherwise the function returns TRUE.
\*************************************************************************/
PUBLIC BOOL ReadProfile()
{
	CHAR pszVersionFound[MaxVersionLen] = "";
	LONG lNumRead;
	HINI hini;
	PSZ pszIniNameCopy;
	PRFPLAYERINFO PrfPlayerInfo[2];

	pszIniNameCopy = (PSZ)alloca( strlen( pszIniName ) + 1);
	strcpy( pszIniNameCopy, pszIniName );
	if ( !(hini = PrfOpenProfile( hab, pszIniNameCopy )) ) goto Error;
	PrfQueryProfileString( hini, pszAppName, PrfKeys.pszVersion, NULL,
								  pszVersionFound, MaxVersionLen );
	if ( strcmp( pszVersion, pszVersionFound ) != 0 )
		goto Error;
	/* retrieve game settings */
	lNumRead = sizeof( ProgInfo );
	if ( !PrfQueryProfileData( hini, pszAppName, PrfKeys.pszProg, &ProgInfo,
										&lNumRead ) 
		) goto Error;
	/* retrieve board size */
	lNumRead = 2;
	if ( !PrfQueryProfileData( hini, pszAppName, PrfKeys.pszBoardSize,
										&GameBoard, &lNumRead ) 
		) goto Error;
	/* retrieve player settings */
	lNumRead = 2 * sizeof( PRFPLAYERINFO );
	if ( !PrfQueryProfileData( hini, pszAppName, PrfKeys.pszPlayer,
										&PrfPlayerInfo, &lNumRead )
		) goto Error;
	PlayerInfo[0].Mind = PrfPlayerInfo[0].Mind;
	PlayerInfo[0].CalcDepth = PrfPlayerInfo[0].CalcDepth;
	PlayerInfo[1].Mind = PrfPlayerInfo[1].Mind;
	PlayerInfo[1].CalcDepth = PrfPlayerInfo[1].CalcDepth;
	/* Restore window position and size */ 
	if ( !RestoreWindowPos( hini, pszAppName, PrfKeys.pszWinPos, hwndMain )
		) goto Error;
	PrfCloseProfile( hini );
	return TRUE;

Error:
	PrfCloseProfile( hini );
	return FALSE;
}

/*************************************************************************\
 * function WriteProfile()
 * Trys to open the file "profile.ini" and saves game specific options 
 * from the profile data. 
\*************************************************************************/
PUBLIC BOOL WriteProfile()
{
	HINI hini;
	PSZ pszIniNameCopy;
	PRFPLAYERINFO PrfPlayerInfo[2];

	pszIniNameCopy = (PSZ)alloca( strlen( pszIniName ) + 1);
	strcpy( pszIniNameCopy, pszIniName );
	if ( !(hini = PrfOpenProfile( hab, pszIniNameCopy )) ) goto Error;
	/* write version number */
	if( !PrfWriteProfileData( hini, pszAppName,
									  PrfKeys.pszBoardSize, &GameBoard, 2 ) )
		goto Error;
	PrfPlayerInfo[0].Mind = PlayerInfo[0].Mind;
	PrfPlayerInfo[0].CalcDepth = PlayerInfo[0].CalcDepth;
	PrfPlayerInfo[1].Mind = PlayerInfo[1].Mind;
	PrfPlayerInfo[1].CalcDepth = PlayerInfo[1].CalcDepth;
	if( !PrfWriteProfileData( hini, pszAppName,
									  PrfKeys.pszPlayer,
									  &PrfPlayerInfo, 2 * sizeof(PRFPLAYERINFO) ) )
		goto Error;
	if( !PrfWriteProfileData( hini, pszAppName,
									  PrfKeys.pszProg,
									  &ProgInfo, sizeof( ProgInfo ) ) )
		goto Error;
	if( !PrfWriteProfileString( hini, pszAppName,
										 PrfKeys.pszVersion, pszVersion ) )
		goto Error;
	if( !StoreWindowPos( hini, pszAppName, PrfKeys.pszWinPos, hwndMain ) )
		goto Error;
	PrfCloseProfile( hini );
	return TRUE;
Error:
	PrfCloseProfile( hini );
	return FALSE;
}

/*************************************************************************\
 * function SaveGame()
 * Trys to open the file "profile.ini" and saves the current game .
\*************************************************************************/
PUBLIC BOOL SaveGame()
{
	HINI hini;
	PBYTE pbBuffer;
	PSZ pszIniNameCopy;

	pszIniNameCopy = (PSZ)alloca( strlen( pszIniName ) + 1);
	strcpy( pszIniNameCopy, pszIniName );
	if ( !(hini = PrfOpenProfile( hab, pszIniNameCopy )) ) goto Error;
	/* write version number */
	if( !PrfWriteProfileData( hini, pszAppName,
									  PrfKeys.pszSavedSize, &GameBoard, 2 ) )
		goto Error;
	pbBuffer = (PBYTE)alloca( GameBoard.movecount + 1);
	memcpy( pbBuffer, &GameBoard.moves[1], GameBoard.movecount * sizeof( BYTE ));
	pbBuffer[GameBoard.movecount] = 0;
	if( !
	PrfWriteProfileData( hini,
			pszAppName, PrfKeys.pszSavedMoves,
			pbBuffer, GameBoard.movecount + 1 )
	) goto Error;
	PrfCloseProfile( hini );
	return TRUE;

Error:
	PrfCloseProfile( hini );
	return FALSE;
}

/*************************************************************************\
 * function LoadGame()
 * Trys to open the file "profile.ini" and looks for a saved gamed.
\*************************************************************************/
PUBLIC BOOL LoadGame()
{
	HINI hini;
	PBYTE pbBuffer;
	PBYTE pbMove;
	struct {
		BYTE Width;
		BYTE Height;
	} SavedSize;
	LONG lNumRead;
	LONG lMovesSaved;
	PSZ pszIniNameCopy;

	pszIniNameCopy = (PSZ)alloca( strlen( pszIniName ) + 1);
	strcpy( pszIniNameCopy, pszIniName );
	if ( !(hini = PrfOpenProfile( hab, pszIniNameCopy )) ) goto Error;
	lNumRead = 2;
	if( !
	PrfQueryProfileData( hini,
			pszAppName, PrfKeys.pszSavedSize,
			&SavedSize, &lNumRead )
	) goto Error;
	if ( lNumRead != 2 ) goto Error;
	if( !
	PrfQueryProfileSize( hini, pszAppName, PrfKeys.pszSavedMoves, &lMovesSaved )
	) goto Error;
	lNumRead = lMovesSaved;
	pbBuffer = (PBYTE)alloca( lMovesSaved );
	if( !
	PrfQueryProfileData( hini,
			pszAppName, PrfKeys.pszSavedMoves,
			pbBuffer, &lNumRead )
	) goto Error;
	PrfCloseProfile( hini );
	FreeBoard();
	InitBoard( SavedSize.Width, SavedSize.Height );
	ClearBoardPM();
	pbMove = pbBuffer;
	for ( ; *pbMove; pbMove++ ) {
		InsertCoin( *pbMove, Player->Color );
		TogglePlayer();
	}
	return TRUE;

Error:
	PrfCloseProfile( hini );
	return FALSE;
}


