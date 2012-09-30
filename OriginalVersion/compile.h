/*************************************************************************\
 * compile.h
 *
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#ifndef __COMPILE_H

#define __COMPILE_H
#define EXTERN extern
#define PUBLIC extern
#define PRIVATE static

#ifndef __GNUC__			/* disable GNU c language extensions */
#define __attribute__( dummy )
#define __inline__
#define __const__
#endif

#ifdef __OPTIMIZE__
#define NDEBUG
#endif

#else /* !__COMPILE_H */

#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

#endif /* __COMPILE_H */
