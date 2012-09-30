/*************************************************************************\
 * testgame.c
 * This file includes basic input and output text only routines to test
 * the four functions. 
\*************************************************************************/
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "compile.h"
#include "rand.h"
#include "game.h"


PRIVATE void ShowGameBoard()
/*************************************************************************\
\*************************************************************************/
{
	unsigned char **line;
	int i;

	assert( GameBoard.hgt );
	assert( GameBoard.field );
	for( line = &GameBoard.field[GameBoard.sizey + 1]; line >= GameBoard.field; line-- ) {
		for( i = 0; i < GameBoard.sizex + 2; i++ ) {
			switch ((*line)[i]) {
				case EMPTY:
					printf( " - " );
					break;
				case RED:
					printf( " 0 " );
					break;
				case BLUE:
					printf( " + " );
					break;
				case BORDER:
					printf( " # " );
					break;
				default:
					printf( " ? " );
			} /* end switch */
		} /* end for i */
		putchar( '\n' );
	} /* end for line */
	printf( "   " );
	for( i = 0; i < GameBoard.sizex; i++ )
		printf( " %d ", i + 1 );
	putchar( '\n' );
} /* end function ShowGameBoard */


int main( int argc, char *argv[] )
/**************************** - main  - **********************************\
\*************************************************************************/
{
 	int col = 0;
 	int i;
 	int valid;
	int totaltime = 0;
	byte *deadly;
	byte *pc;
 	
 	/* Initialize random number generator */
#ifdef PRINT_STATS
	/* for comparing it is better to get always the same random numbers */
	srand( 0 );
#else	
 	srand( time( NULL ) );
#endif 	
	if ( InitDefaultGame() )
	 	return 1;
	Player[0].Mind = COMPUTER;
	Player[0].CalcDepth = 1;
	Player[1].CalcDepth = 5;
	SetPlayer( RED );
	ShowGameBoard();
	while ( col >= 0 ) {
		if ( Player->Mind == HUMAN ) {
			printf( "enter column to place next coin for %c: ", 
					  (Player->Color == RED) ? '0': '+' );
			do {
				scanf( "%d", &col );
				if (col < 0 ) {
					printf( "Exiting program\n" );
					break;
				} /* end if */
				valid = col > 0 && col <= GameBoard.sizex && GameBoard.hgt[col] < GameBoard.sizey;
				if ( !valid )
					printf( "This column is invalid - select another: " );
			} while ( !valid ); /* end do */
		if ( col < 0 ) break;
		} else { /* Mind == COMPUTER */
#ifdef PRINT_STATS
			int t;
			t = time( NULL );
#endif			
			col = GetComputerMove();
#ifdef PRINT_STATS
			t = time( NULL ) - t;
			totaltime+= t;
			printf( "Computer valuations:" );
			for (i = 1; i <= GameBoard.sizex; i++ )
			  printf( " %d: %d ", i, GameBoard.lastval[i] );
			printf( "\nComputation time: %d sec. ", t );
#endif			
			
			printf( "Inserted coin %c in column: %d \n",
					  (Player->Color == RED) ? '0': '+',
					  col );
		} /* end if */				
		InsertCoin( col, Player->Color );
		ShowGameBoard();
		switch( TestEnd() ) {
			case 0:
				break;
			case 1:
				printf( "The winner is %s\n", Player->Color == RED? "red (0)" : "blue (+)" );
				col = -1;
				deadly = GetDeadlyFields();
				for ( pc = deadly; *pc; ) {
					printf( "(%d, %d)\n", *pc++, *pc++ );
				}
				break;
			case 2:
				printf( "This was an equal match\n" );
				col = -1;
				break;
			default:
				assert( 0 );
		} /* end switch */
		TogglePlayer();
	}
#ifdef PRINT_STATS
	printf( "Total computation time: %d\n", totaltime );
#endif	
	FreeBoard();
	return 0;
}


