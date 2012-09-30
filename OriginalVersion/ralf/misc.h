/*************************************************************************\
 * misc.h
 *
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

#include <errno.h>

#define ERRNO ( *_errno() )

PUBLIC inline int min( const int a, const int b ) __attribute__ ((const));
PUBLIC inline int min( const int a, const int b )
{
	return a < b ? a : b;
}

PUBLIC inline int max( const int a, const int b ) __attribute__ ((const));
PUBLIC inline int max( const int a, const int b ) 
{
	return a > b ? a : b;
}

PUBLIC void Beep();

