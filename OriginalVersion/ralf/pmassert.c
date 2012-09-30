/*************************************************************************\
 * pmassert.c
 * assert for PM aplications.
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#include <stdio.h>
#define INCL_WINDIALOGS
#define INCL_WINMESSAGEMGR
#include <os2.h>

#define PUBLIC
#undef NDEBUG
#include "pmassert.h"

PUBLIC void _assert( const char *str, const char *fname, const unsigned int line )
{
	CHAR szErrMes[256];
	PSZ szTitle = "Assertion failure";

	sprintf( szErrMes, "Assertion %s failed:\nfile %s, line %u.",
				str, fname, line );
	WinMessageBox( HWND_DESKTOP, NULLHANDLE, szErrMes, szTitle, (USHORT)0,
						MB_OK | MB_ICONEXCLAMATION );
	exit(1);
}
	
