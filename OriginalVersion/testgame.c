/*************************************************************************\
 * testgame.c
 * This file includes basic input and output text only routines to test
 * the valuation functions.
 * In main() you can adjust some variables for the two players. This
 * part is enclosed in %%%%%%%%%%%% characters. We think that you need
 * only alter that part of this file.
\*************************************************************************/
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "compile.h"
#include "rand.h"
#include "game.h"
#include "valuat.h"


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

/* these two values must be set like this. We defined them like this
 * because they provide a better readability to the source code, and
 * we rely on these values to be set like this (we refer to them directly)!!!
 */
 
	PlayerInfo[0].Color = RED;
	PlayerInfo[1].Color = BLUE;

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * here you can adjust the two players. Player[0] is RED, Player[1] is
 * BLUE (this is a remnant from the time when we started to write this
 * program. Later we found out that the original game is coloured red
 * and yellow, but we kept these variable names.)

 * Player[].Mind determines whether the player will be played by the computer
 * or by you. Valid values are COMPUTER and HUMAN.

 * Player[].CalcDepth determines how many half moves the computer will
 * calculate. It must be between 1 and 10.

 * Player[].ValFunc determines the valuation function the computer will
 * use. Here you can check how your function plays against you or against
 * a different valuation function.

 * The function SetPlayer() determines which player will begin. If
 * the function's argument is RED, Player[0] will begin, if it is
 * BLUE, Player[1] will begin.

 * The function InitBoard() allocates memory for the game board and
 * sets some variables. You can change the two parameters to change
 * the board size. The first parameter indicates the number of columns,
 * the second one the number of rows.
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	 	
	Player[0].Mind = COMPUTER;
	Player[0].CalcDepth = 1;
	Player[0].ValFunc = RalfsValuation;
	Player[1].Mind = HUMAN;
	Player[1].CalcDepth = 5;
	Player[1].ValFunc = NullValuation;
	SetPlayer( RED );

	if ( InitBoard( 7, 6) )
	 	return 1;

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */	
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


