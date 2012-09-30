/*
 * An easy but acceptable random function
 */
#include "compile.h"
#undef PUBLIC
#define PUBLIC
#include "rand.h"
 
PRIVATE unsigned next_rand = 10;

PUBLIC void srand( const unsigned seed )
{
    next_rand = seed;
}

PUBLIC unsigned rand()
{
    next_rand = next_rand * 1103515245U + 12345;
    return (unsigned)(next_rand / 65536) % 32768;
}

PUBLIC unsigned random( const unsigned max )
{
	return rand() % (max + 1);
}
