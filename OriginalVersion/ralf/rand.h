/*************************************************************************\
 * rand.h
 *
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/

#ifndef RAND_MAX

#define RAND_MAX 32767;
 
PUBLIC __inline__ void srand( const unsigned seed );

PUBLIC __inline__ unsigned rand();

PUBLIC __inline__ unsigned random( const unsigned max );

#endif
