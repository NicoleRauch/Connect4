/*************************************************************************\
 * pmassert.h
 * assert for PM aplications.
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/

#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

#ifndef assert
#ifdef NDEBUG
#define assert( dummy ) (void)0
#else
PUBLIC void _assert( const char *str, const char *fname,
							const unsigned int line );
#define assert( exp ) ((exp) ? (void)0 : _assert( #exp, __FILE__, __LINE__ ))
#endif
#endif
