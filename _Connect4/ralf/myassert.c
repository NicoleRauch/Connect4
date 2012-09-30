/*************************************************************************\
 * myassert.c
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#define INCL_WINDIALOGS
#define INCL_WINMESSAGEMGR
#include <os2.h>
#include <stdio.h>
#include "compile.h"
#include "connect4.h"

#define INITIALIZE
#include "myassert.h"

PUBLIC VOID _assert( const char *str, const char *fname, const unsigned int line )
{
	CHAR szErrMes[256];
	PSZ szTitle = "Assertion failure";

	sprintf( szErrMes, "Assertion %s failed:\nfile %s, line %u.",
				str, fname, line );
	WinMessageBox( HWND_DESKTOP, hwndMain, szErrMes, szTitle, (USHORT)0,
						MB_OK | MB_CUACRITICAL | MB_APPLMODAL );
	/* Send WM_FATAL message if assert is called inside a thread. */
	WinPostMsg( hwndMain, WM_FATAL, (MPARAM)0, (MPARAM)0 );
	_endthread();
}
	

