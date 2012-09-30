/*************************************************************************\
 * errmsgs.h
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

PUBLIC void _ErrorMsg( const char *str, const char* fname, const unsigned line );

#define Fatal( errstr ) ( _ErrorMsg( (errstr), __FILE__, __LINE__ ), \
		  WinPostMsg( hwndMain, WM_QUIT, (MPARAM)0, (MPARAM)0 ) )

#define NonFatal( errstr ) _ErrorMsg( (errstr), __FILE__, __LINE__ )

#ifndef assert
#ifdef NDEBUG
#define assert( dummy ) (void)0
#else
PUBLIC void _assert( const char *str, const char *fname,
							const unsigned int line );
#define assert( exp ) ((exp) ? (void)0 : _assert( #exp, __FILE__, __LINE__ ))
#endif
#endif
