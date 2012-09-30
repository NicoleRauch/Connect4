/*************************************************************************\
 * errmsgs.c
 * Some error message functions and assert for Connect Four.
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#define INCL_WINDIALOGS
#define INCL_WINMESSAGEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compile.h"
#include "defs.h"
#include "misc.h"
#include "connect4.h"

#define INITIALIZE
#undef NDEBUG
#include "errmsgs.h"

/************************************************************************\
 * function ErrorMsg
 * Display some error information 
\************************************************************************/
PUBLIC VOID _ErrorMsg( const char *str, const char *fname, const unsigned line )
{
	static CHAR szErrMes[256];
	static PSZ szTitle = "Error";

	sprintf( szErrMes, "%s\nfile: %s, line: %ul\n" \
				"WinGetLastError reports error code: %X\n" \
				"Runtime library reports error: %s", 
				str, fname, line, (USHORT)WinGetLastError( hab ),
				strerror( ERRNO ) );
	WinMessageBox( HWND_DESKTOP, hwndMain, szErrMes, szTitle, (USHORT)0,
						MB_OK | MB_CUACRITICAL | MB_APPLMODAL | MB_MOVEABLE );
}


