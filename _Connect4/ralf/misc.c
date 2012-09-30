/*************************************************************************\
 * misc.c
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#include "compile.h"
#include <os2.h>

#undef PUBLIC
#define PUBLIC
#include "misc.h"

PUBLIC VOID Beep()
{
	DosBeep( 440, 100 );
}


