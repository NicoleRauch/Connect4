/*************************************************************************\
 * compile.h
 
\*************************************************************************/

#ifndef COMPILE_H

#define COMPILE_H
#define PUBLIC extern
#define PRIVATE static

#ifndef __GNUC__
#define __attribute__( dummy )
#endif

#ifdef __OPTIMIZE__
#define NDEBUG
#endif

#endif
