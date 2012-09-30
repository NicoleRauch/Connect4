/*************************************************************************\
 * game.c
 * Set tabs to 3 to get a readable source.
 * In this source file you will find the MinMax algorithm. This routine 
 * is responsible for the computer decisions.
 * You will also find functions which do some basic tasks like allocating
 * memory for the board.
 * Most of the functions appear twice - ones for other program parts to
 * allow easy game handling and the ones for internal calculation if it's
 * the computers task to find a column to insert a coin.
 * This is done to allow multitasking applications to use the board
 * status while the computers finds the next move.
\*************************************************************************/

#include <malloc.h>
#include <memory.h>
#include <time.h>
#include "compile.h"
#include "rand.h"
#include "misc.h"

#ifdef DUMP_DISPLAY
#include <stdio.h>
#include <assert.h>
#else
#include "errmsgs.h"
#endif

#define INITIALIZE
#include "game.h"


PRIVATE unsigned int maxdepth;	/* maximal depth of recursion in minmax */

PRIVATE unsigned int depth;		/* current depth of recursion in minmax */

/* function prototypes */


/* functions */

/*************************************************************************\
 * ToggleColor:
 * Toggles color between RED and BLUE
\*************************************************************************/
PUBLIC inline byte ToggleColor( const byte oldcolor )
{
	assert( oldcolor == RED | oldcolor == BLUE );
	return oldcolor ^ (RED | BLUE);
}

/*************************************************************************\
 * TogglePlayer:
 * toggles between the two players and sets all relevant data for the
 * current player.
\*************************************************************************/
PUBLIC inline void TogglePlayer()
{
	Player = ( Player == PlayerInfo ) ? ++Player : PlayerInfo;
}

/*************************************************************************\
 * function SetPlayer
 * set the player to the specified color
 * 
\*************************************************************************/
PUBLIC void SetPlayer( const byte color )
{
	assert( color == RED || color == BLUE );
	Player = ( color == RED ) ? PlayerInfo : PlayerInfo + 1;
}

/*************************************************************************\
 * InsertCoin:
 * Inserts a coin in column col with color color
\*************************************************************************/
PUBLIC void InsertCoin( const byte col, const byte color )
{
	assert( GameBoard.movecount < GameBoard.maxmoves );
	GameBoard.moves[++GameBoard.movecount] = col;
	assert( GameBoard.hgt[col] < GameBoard.sizey );
	GameBoard.field[++GameBoard.hgt[col]][col] = color;
}

PUBLIC void DeleteCoin( const int col )
/*************************************************************************\
 * Deletes the last coin in the board in the column col.
 * Note: The variable board.moves isn't changed. This has to done
 * by the calling routine if neccesary.
\*************************************************************************/
{
	assert( GameBoard.movecount > 0 );
	assert( GameBoard.hgt[col] > 0 );
	GameBoard.movecount--;
	GameBoard.field[GameBoard.hgt[col]--][col] = EMPTY;
}

/*************************************************************************\
 * function CInsertCoin
 * Inserts a coin in column col with color color. This function acts like
 * InsertCoin but doesn't increase the move counter and doesn't save
 * the move made. It is used during the calculation of the computer choice
 * where the information just mentioned isn't needed.
\*************************************************************************/
PRIVATE inline void CInsertCoin( const byte col, const byte color )
{
	assert( GameBoard.chgt[col] < GameBoard.sizey );
	GameBoard.cfield[++GameBoard.chgt[col]][col] = color;
}

PRIVATE inline void CDeleteCoin( const byte col )
/*************************************************************************\
 * function CDeleteCoin
 * Deletes the last coin in the board in the column col.
 * 
\*************************************************************************/
{
	assert( GameBoard.chgt[col] > 0 );
	GameBoard.cfield[GameBoard.chgt[col]--][col] = EMPTY;
}

/*************************************************************************\
 * function FreeBoard
 * Release all memory allocate by our game board
\*************************************************************************/
PUBLIC void FreeBoard( )
{
   byte line;

   free( GameBoard.moves );
   free( GameBoard.hgt );
   free( GameBoard.chgt );
   for( line = 0; line < GameBoard.sizey + 2; line++ ) {
      free( GameBoard.field[line] );
      free( GameBoard.cfield[line] );
   }
   free( GameBoard.field );
   free( GameBoard.cfield );
   free( GameBoard.lastval );
}


PUBLIC void ClearBoard()
/*************************************************************************\
 * function ClearBoard
 * Delete the contents of the board.
\*************************************************************************/
{
   int line;

   GameBoard.movecount = 0;
   memset( GameBoard.moves, 0, sizeof( byte ) * (GameBoard.maxmoves + 1) );
   memset( GameBoard.hgt, 0, sizeof( byte ) * (GameBoard.sizex + 2) );
   memset( GameBoard.chgt, 0, sizeof( byte ) * (GameBoard.sizex + 2) );
   memset( GameBoard.lastval, 0 , sizeof( int ) * (GameBoard.sizex + 1) );
   memset( GameBoard.field[0], BORDER, sizeof( byte ) * (GameBoard.sizex + 2) );
   memset( GameBoard.cfield[0], BORDER, sizeof( byte ) * (GameBoard.sizex + 2) );
   for( line = 1; line <= GameBoard.sizey; line++ ) {
      memset( GameBoard.field[line], EMPTY, sizeof( byte ) * (GameBoard.sizex + 1));
      memset( GameBoard.cfield[line], EMPTY, sizeof( byte ) * (GameBoard.sizex + 1));
      GameBoard.field[line][0] = BORDER;
      GameBoard.field[line][GameBoard.sizex + 1] = BORDER;
      GameBoard.cfield[line][0] = BORDER;
      GameBoard.cfield[line][GameBoard.sizex + 1] = BORDER;
   }
   memset( GameBoard.field[GameBoard.sizey + 1], BORDER, sizeof( byte ) * (GameBoard.sizex + 2) );
   memset( GameBoard.cfield[GameBoard.sizey + 1], BORDER, sizeof( byte ) * (GameBoard.sizex + 2) );
}

/*************************************************************************\
 * function InitBoard
 * Allocate memory, clear all fields and set a border 
 * returns 1 if everything went allright - otherwise 0 is returned.
\*************************************************************************/
PUBLIC int InitBoard( const byte wdth, const byte hgt )
{
	byte line;

	PlayerInfo[0].Color = RED;
	PlayerInfo[0].ValFunc = RalfsValuation;
	PlayerInfo[1].Color = BLUE;
	PlayerInfo[1].ValFunc = RalfsValuation;

   GameBoard.sizex = wdth;
   GameBoard.sizey = hgt;
   GameBoard.maxmoves = wdth * hgt;
   if ( !(GameBoard.moves = (byte*)malloc( (GameBoard.maxmoves + 1) * sizeof( byte ))) )
      return 1;
   if ( !(GameBoard.hgt = (byte*)malloc( (wdth + 2) * sizeof( byte )) ) )
      return 1;
   if ( !(GameBoard.field = (byte**)malloc( (hgt + 2) * sizeof( byte*)) ) )
      return 1;
   if ( !(GameBoard.chgt = (byte*)malloc( (wdth + 2) * sizeof( byte )) ) )
      return 1;
   if ( !(GameBoard.cfield = (byte**)malloc( (hgt + 2) * sizeof( byte*)) ) )
      return 1;
   for( line = 0; line < GameBoard.sizey + 2; line++ ) {
      if ( !(GameBoard.field[line] = (byte*)malloc( (wdth + 2) * sizeof( byte ))) )
         return 1;
      if ( !(GameBoard.cfield[line] = (byte*)malloc( (wdth + 2) * sizeof( byte ))) )
         return 1;
   }
   if( !(GameBoard.lastval = (int*)malloc( (wdth + 1) * sizeof( int ))) )
   	return 1;
   ClearBoard();
   return 0;
}

/*************************************************************************\
 * function InitDefaultGame
\*************************************************************************/
PUBLIC int InitDefaultGame()
{
	PlayerInfo[0].Color = RED;
	PlayerInfo[0].Mind = HUMAN;
	PlayerInfo[0].CalcDepth = 5;
	PlayerInfo[0].ValFunc = RalfsValuation;
	
	PlayerInfo[1].Color = BLUE;
	PlayerInfo[1].Mind = COMPUTER;
	PlayerInfo[1].CalcDepth = 5;
	PlayerInfo[1].ValFunc = RalfsValuation;
	SetPlayer( RED );
   return InitBoard( 7, 6 );
}

/*************************************************************************\
 * function TestEnd
 * Test if an end condition is true
 * returns:
 *    0: the game isn't finished
 *    1: the last inserted coin was the winning one
 *    2: the board is full and noone has won
 * This function examines the contents of GameBoard.field and GameBoard.hgt
\*************************************************************************/
PUBLIC int TestEnd()
{
   byte *pc;
	byte c;
	byte r;
   byte lastcolor;
   byte lastcol, lastrow;
   byte same_left;			/* number of coins in same color to the left, */
   byte same_right;		/* same as above to the right                 */
   byte same_down;
   /* same_right, same_rd (right down), same_ru (right up) and same_down
    * are initialized with one because we have to count one coin more in
    * one direction. */
   
   lastcol = GameBoard.moves[GameBoard.movecount];
   lastrow = GameBoard.hgt[lastcol];
   lastcolor = GameBoard.field[lastrow][lastcol];

   /* Count all the coins of the same color in any direction begining at
    * the position of the last coin inserted. */
   /* Count all the coins of the same color in any direction begining at
    * the position of the last coin inserted. */
   /* Begin with the horizontal line */
   same_left = 0;
   same_right = 1;
   /* same_right is initialized with one because we have to count one coin
	 * more in one direction. */
   for ( pc = &GameBoard.field[lastrow][lastcol - 1];
         *pc == lastcolor;
         pc--, same_left++ );
   for ( pc = &GameBoard.field[lastrow][lastcol + 1];
         *pc == lastcolor;
         pc++, same_right++ );
   if ( same_left + same_right >= maxcoins ) return 1;

   /* Now check the ascending diagonal */
   same_left = 0;
   same_right = 1;
   for ( c = lastcol - 1, r = lastrow - 1;
         GameBoard.field[r][c] == lastcolor;
         c--, r--, same_left++ );
   for ( c = lastcol + 1, r = lastrow + 1;
         GameBoard.field[r][c] == lastcolor;
         c++, r++, same_right++ );
   if ( same_left + same_right >= maxcoins ) return 1;

   /* Then the descending diagonal */
   same_left = 0;
   same_right = 1;
   for ( c = lastcol - 1, r = lastrow + 1;
         GameBoard.field[r][c] == lastcolor;
         c--, r++, same_left++ );
   for ( c = lastcol + 1, r = lastrow - 1;
         GameBoard.field[r][c] == lastcolor;
         c++, r--, same_right++ );
   if ( same_left + same_right >= maxcoins ) return 1;

   /* And at last check vertical line */
   same_down = 1;
   for ( c = lastcol, r = lastrow - 1;
         GameBoard.field[r][c] == lastcolor;
         r--, same_down++ );
   if ( same_down >= maxcoins ) return 1;

   /* No winner --> return 2 if board is full otherwise 0 */
   return ( GameBoard.movecount == GameBoard.maxmoves ) ? 2 : 0;
}

/*************************************************************************\
 * function CTestEnd
 * Test if an end condition is true in board Game
 * the parameter lastcol has to specify the last column where a coin was
 * inserted.
 * returns:
 *    0: the game isn't finished
 *    1: the last inserted coin was the winning one
\*************************************************************************/
PRIVATE inline int CTestEnd( const byte col, const byte color )
{
   byte *pc;
   const byte row = GameBoard.chgt[col];
	byte c;
	byte r;
   byte same_left; 	 	/* number of coins in same color to the left, */
   byte same_right;		/* same as above to the right                 */
   byte same_down;
   /* same_right, same_rd (right down), same_ru (right up) and same_down
    * are initialized with one because we have to count one coin more in
    * one direction. */
   

   /* Count all the coins of the same color in any direction begining at
    * the position of the last coin inserted. */
   /* Begin with the horizontal line */
   same_left = 0;
   same_right = 1;
   /* same_right is initialized with one because we have to count one coin
	 * more in one direction. */
   for ( pc = &GameBoard.cfield[row][col - 1];
         *pc == color;
         pc--, same_left++ );
   for ( pc = &GameBoard.cfield[row][col + 1];
         *pc == color;
         pc++, same_right++ );
   if ( same_left + same_right >= maxcoins ) return 1;

   /* Now check the ascending diagonal */
   same_left = 0;
   same_right = 1;
   for ( c = col - 1, r = row - 1;
         GameBoard.cfield[r][c] == color;
         c--, r--, same_left++ );
   for ( c = col + 1, r = row + 1;
         GameBoard.cfield[r][c] == color;
         c++, r++, same_right++ );
   if ( same_left + same_right >= maxcoins ) return 1;

   /* Then the descending diagonal */
   same_left = 0;
   same_right = 1;
   for ( c = col - 1, r = row + 1;
         GameBoard.cfield[r][c] == color;
         c--, r++, same_left++ );
   for ( c = col + 1, r = row - 1;
         GameBoard.cfield[r][c] == color;
         c++, r--, same_right++ );
   if ( same_left + same_right >= maxcoins ) return 1;

   /* And at last check vertical line */
   same_down = 1;
   for ( c = col, r = row - 1;
         GameBoard.cfield[r][c] == color;
         r--, same_down++ );
   if ( same_down >= maxcoins ) return 1;

   /* No winner --> return 0 */
   return 0;
}


/*************************************************************************\
 * valuate the current board 
\*************************************************************************/
   /***********************************************************************\
    * 
   \***********************************************************************/
   PRIVATE inline byte EvalLR(
      const byte same,
      const byte lempty,
      const byte rempty ) __attribute__ ((const));
   PRIVATE inline byte EvalLR(
      const byte same,
      const byte lempty,
      const byte rempty )
   {
      const byte empty = lempty + rempty;
   
      if (same + empty >= maxcoins ) {
         if ( lempty != 0 && rempty != 0 ) 
            return 4 * same + empty + 1;
         else 
            return 4 * same + empty;
      } else {
         return 0;
      }
   }
   /***********************************************************************/

/*************************************************************************\
 *
\*************************************************************************/
PUBLIC int RalfsValuation()
{
   byte color;
   byte same;                   
   byte lempty;
   byte rempty;
   byte x, y;
   byte ymin;
   byte cx, cy;
   int result;
   int add;
   byte *pf;
   const byte maxx = GameBoard.sizex;
   const byte maxy = GameBoard.sizey;

   result = 0;
   for ( x = 1; x <= maxx; x++ ) {
      y = GameBoard.chgt[x]; /* [cy][cx] -> last field in this col */
      if ( y > 0 ) {
         color = GameBoard.cfield[y][x];
         for ( same = 1, cy = y - 1;
               GameBoard.cfield[cy][x] == color;
               cy--, same++ ) ;
         /* same is now the number of equal coins below the
          * current field. The higher same is the better is
          * the chance to win with this column for player 
          * but only if there are enougth empty fields above. */
         if ( same + maxy - y  >= maxcoins )
            result+= 4 * (( color == Player->Color ) ? same : -same );
			ymin = max(1, min( GameBoard.chgt[x - 1], GameBoard.chgt[ x + 1] ));
			for (; y >= ymin; y-- ) {
	         /* Now valuate the horizontal line */
	         /* Left side: */
	         color = GameBoard.cfield[y][x];
	         same = 1;
	         lempty = rempty = 0;
	         for ( pf = &GameBoard.cfield[y][x - 1]; *pf == color; same++, pf-- );
	         for ( ; *pf == EMPTY; pf--, lempty++ );
	         /* Right side: */
	         for ( pf = &GameBoard.cfield[y][x + 1]; *pf == color; same++, pf++ );
	         for ( ; *pf == EMPTY; pf++, rempty++ );

	         add = EvalLR( same, lempty, rempty );
	
	         /* Now valuate the ascending diagonal */
	
	         /* Left side: */
	         same = 1;
	         lempty = rempty = 0;
	         for ( cx = x - 1, cy = y - 1;
	               GameBoard.cfield[cy][cx] == color;
	               cx--, cy--, same++ );
	         for ( ; GameBoard.cfield[cy][cx] == EMPTY; cx--, cy--, lempty++ );
	
	         /* Right side: */
	         for ( cx = x + 1, cy = y + 1;
	               GameBoard.cfield[cy][cx] == color;
	               cx++, cy++, same++ );
	         for ( ; GameBoard.cfield[cy][cx] == EMPTY; cx++, cy++, rempty++ );
	            
	         add+= EvalLR( same, lempty, rempty );
	
	         /* Now valuate the descending diagonal */
	
	         /* Left side: */
	         same = 1;
	         lempty = rempty = 0;
	         for ( cx = x - 1, cy = y + 1;
	               GameBoard.cfield[cy][cx] == color;
	               cx--, cy++, same++ );
	         for ( ; GameBoard.cfield[cy][cx] == EMPTY; cx--, cy++, lempty++ );
	         /* Right side: */
	         for ( cx = x + 1, cy = y - 1;
	               GameBoard.cfield[cy][cx] == color;
	               cx++, cy--, same++ );
	         for ( ; GameBoard.cfield[cy][cx] == EMPTY; cx++, cy--, rempty++ );
	
	         add+= EvalLR( same, lempty, rempty );

	         result+= (color == Player->Color ) ? add : -add;
			}
      } /* end if y > 0 */
   } /* end for x */
   return result;
}


/*************************************************************************\
 * function NicolesValuation
 * this function can only be used when the gameboard is of the size
 * 7 columns and 6 rows. Otherwise it will act like the NullValuation,
 * that means it will always return 0.
 *
\*************************************************************************/
PUBLIC int NicolesValuation()
{
   byte erg = 0,
   rtg [8]; /* 1..7 */
   register byte i, j;

   byte InternalValuation(byte anz)
   {  
   byte akt, add, erg = 0, k; 

		for(k=1; k <= anz; k++) {
			akt = rtg[k];
      	if(akt) {
         	akt = ToggleColor(akt);	/* to test <> this color */
         	if((k==1) || (k==anz))
            	add = 3;     			/* border stone */
	         else add = 4;  			/* no border stone */
   	      if ((k > 1) && (rtg[k-1] != akt)) add+= 4; 
      		    /* previous coin not of different color */
         	if ((k < anz) && (rtg[k+1] != akt)) add += 4;
         		 /* succeeding coin not of different color */
	         if (akt == Player->Color)  /* == computer color */
   	          add *= -1; /* because of the color toggling */
      	   erg += add;
	      }   /* if */
   	}     /* for */
	   return erg;
   }   /* function InternalValuation() */

/* function NicolesValuation() */

	if( GameBoard.sizex != 7 || GameBoard.sizey != 6)
		return 0;	/* check if the gameboard has the required size	*/

   for(j = 1; j<8; j++) {
      for(i = 1; i<7; i++)
         rtg[i] = GameBoard.cfield[i][j];          /* vertical */
      erg += InternalValuation(6);
   }

   for(i = 1; i<7; i++) {
      for(j = 1; j<8; j++)
         rtg[j] = GameBoard.cfield[i][j];         /* horizontal */
      erg += InternalValuation(7);
   }

   for(i = 1; i<5; i++)
      rtg[i] = GameBoard.cfield[i][5 - i];  /* lowest ascending diag. */
   erg += InternalValuation(4);

   for(i = 1; i<6; i++)
      rtg[i] = GameBoard.cfield[i][6 - i];  /* second ascending diag. */
   erg += InternalValuation(5);

   for(j = 1; j<3; j++) {
      for(i = 1; i<7; i++)
         rtg[i] = GameBoard.cfield[i][(6+j) - i]; /* middle ascending diag. */
      erg += InternalValuation(6);
   }

   for(i = 1; i<6; i++)
      rtg[i] = GameBoard.cfield[1 + i][8 - i]; /* fifth ascending diag. */
   erg += InternalValuation(5);

   for(i = 1; i<5; i++)
      rtg[i] = GameBoard.cfield[2 + i][8 - i]; /* uppermost ascending diag. */
   erg += InternalValuation(4);

   for(i = 1; i<5; i++)
      rtg[i] = GameBoard.cfield[i][3 + i];  /* lowest descending diag. */
   erg += InternalValuation(4);
   for(i = 1; i<6; i++)
      rtg[i] = GameBoard.cfield[i][2 + i];  /* second descending diag. */
   erg += InternalValuation(5);


   for(j = 1; j<3; j++) {
      for(i = 1; i<7; i++)
         rtg[i] = GameBoard.cfield[i][(2-j) + i]; /* middle descending diag. */
      erg += InternalValuation(6);
   }

   for(i = 1; i<6; i++)
      rtg[i] = GameBoard.cfield[1 + i][i];  /* fifth descending diag. */
   erg += InternalValuation(5);

   for(i = 1; i<5; i++)
      rtg[i] = GameBoard.cfield[2 + i][i];  /* uppermost descending diag. */
   erg += InternalValuation(4);

   return erg;
} /* function NicolesValuation() */

/*************************************************************************\
 * Make no differences
\*************************************************************************/
PUBLIC int NullValuation()
{
   return 0;
}

/*************************************************************************\
 * Get a random choice 
\*************************************************************************/
PUBLIC int RndValuation()
{
	return random( inf ) - inf / 2;
}

/*************************************************************************\
 * Get the valuation of one leaf.
 * recurses all posible moves and gets the best one for color.
 * Note: this function doesn't save the best move found. It only returns
 * the valuation for a coin inserted in the column inscol.
\*************************************************************************/
PRIVATE int MiniMax( const byte inscol, const byte color,
			 			 	const int alpha, const int beta )
{
	int minmax;							/* best result so far */
	int i;

	CInsertCoin( inscol, color );

	if( !CTestEnd( inscol, color ) )	{
		depth++;
		if ( depth == maxdepth ) {
			minmax = ValFunc();
		} else {
			/* If depth is odd we look for a minimum of all possiblities -
			 * otherwise get the maximum.
			 * Initialize minmax with alpha or beta. */
			if ( depth & 1 ) {
				minmax = beta;
				for ( i = 1; minmax > alpha && i <= GameBoard.sizex; i++ ) {
					if ( GameBoard.chgt[i] < GameBoard.sizey ) {
						minmax = min( minmax, MiniMax( i, ToggleColor( color ),
																 alpha, minmax ) );
					} /* end if */
				} /* end for */
			} else {
				minmax = alpha;
				for ( i = 1; minmax < beta && i <= GameBoard.sizex; i++ )	{
					if ( GameBoard.chgt[i] < GameBoard.sizey ) {
						minmax = max( minmax, MiniMax( i, ToggleColor( color ),
																 minmax, beta ) );
					} /* end if */
				} /* end for */
			} /* end if depth & 1 */
		} /* end if depth == maxdepth */
		depth--;
	} else {
		minmax = (depth & 1) ? (depth - inf) : (inf - depth);
	} /* end if CTestEnd */

	CDeleteCoin( inscol );
	return minmax;
}

/*************************************************************************\
 *
\*************************************************************************/
PUBLIC byte GetComputerMove()
{
	int i;
	int new;
	int minmax = -inf;
	int s;
	byte firstmove;
	byte bestcol = 0;
	byte testcol;

	ValFunc = Player->ValFunc;

	maxdepth = Player->CalcDepth;
	/* If maxdepth is greater than the number of the fields which are
	 * empty we adjust maxdepth so that we don't try to insert more coins
	 * than possible. Doing it this way we don't have to keep an eye on the
	 * end condition if a board is full. So we don't have to adjust the
	 * movecount variable and gain less calculation which result in a faster
	 * minmax algorithm. */
	maxdepth = min( maxdepth, GameBoard.maxmoves - GameBoard.movecount );
	assert( 0 < maxdepth && maxdepth <= MAXDEPTH );
	switch( GameBoard.movecount ) {
		case 0:
			if ( GameBoard.sizex > 4 )
				return (GameBoard.sizex + 1) / 2 + random(2) - 1;
			else 
				return (GameBoard.sizex + 1) / 2;
		case 1:
			firstmove = GameBoard.moves[1];
			if ( 1 < firstmove && firstmove < GameBoard.sizex / 2 ) {
				return firstmove + random(1);
			} else if ( GameBoard.sizex > firstmove && firstmove > GameBoard.sizex / 2 ) {
				return GameBoard.moves[1] - random(1);
			}
			else
				return (GameBoard.sizex + 1) / 2 + random(2) - 1;
		default:
			/* Multitaskting considerations:
			 * To allow calculation of the next move without modifieing field
			 * copy field to cfield */
			for ( i = 1; i <= GameBoard.sizey; i++ )
				memcpy( GameBoard.cfield[i], GameBoard.field[i],
						  (GameBoard.sizex + 1) * sizeof( byte ) );
			/* hgt to chgt */
			memcpy( GameBoard.chgt, GameBoard.hgt,
					  (GameBoard.sizex + 1) * sizeof( byte ) );
			testcol = GameBoard.sizex / 2 + 1;
			depth = 0;
			s = rand() & 1;
			minmax = -inf;
			for ( i = 0; i < GameBoard.sizex; ++i, s= !s, testcol+= s ? i : -i ) {
				if ( GameBoard.chgt[testcol] < GameBoard.sizey ) {
					new = MiniMax( testcol, Player->Color, minmax, inf );
					GameBoard.lastval[testcol] = new;
					if ( new > minmax ) {
						minmax = new;
						bestcol = testcol;
					} 
				} else {
					GameBoard.lastval[testcol] = 0;
				}
			} /* end for */

			GameBoard.lastval[0] = minmax;	/* save best valuation in field 0 */
			assert( 0 < bestcol && bestcol <= GameBoard.sizex );
			assert( depth == 0 );
			return bestcol;
	} /* end switch */
}



/*************************************************************************\
 * function GetDeadlyFields
 * This function is usefull if an game has ended because one of the
 * players had won. It returns an array of pairs of bytes where each of
 * this pair (row, column) is the coordinate of a board field which was
 * responsible for ending the game. The array ends with a pair of 0.
 * The pointer returned should be release with free after it's usage.
\*************************************************************************/
PUBLIC byte *GetDeadlyFields()
{
   byte *pc;
   byte *result;
	byte c;
	byte r;
   byte same_left;			/* number of coins in same color to the left, */
   byte same_right;			/* same as above to the right                 */
   byte same_down;
   
	const byte lastcol = GameBoard.moves[GameBoard.movecount];
	const byte lastrow = GameBoard.hgt[lastcol];
	const byte lastcolor = GameBoard.field[lastrow][lastcol];

   pc = result = (byte*)malloc( 7 * maxcoins * sizeof( byte ) );
   *pc++ = lastrow;
   *pc++ = lastcol;

   /* check the horizontal */
   same_left = 0;
   same_right = 1;
   r = lastrow;
   for ( c = lastcol - 1;
         GameBoard.field[r][c] == lastcolor;
         c--, same_left++ );
   for ( c = lastcol + 1;
         GameBoard.field[r][c] == lastcolor;
         c++, same_right++ );
   if ( same_left + same_right >= maxcoins ) {
   	/* more than maxcoins (4) found --> insert them in our return array */
	   for ( c = lastcol - 1, r = lastrow;
	         GameBoard.field[r][c] == lastcolor;
	         *pc++ = r, *pc++ = c-- ); 
	   for ( c = lastcol + 1, r = lastrow;
	         GameBoard.field[r][c] == lastcolor;
	         *pc++ = r, *pc++ = c++ ); 
	}
   	
   /* Now check the ascending diagonal */
   same_left = 0;
   same_right = 1;
   for ( c = lastcol - 1, r = lastrow - 1;
         GameBoard.field[r][c] == lastcolor;
         c--, r--, same_left++ );
   for ( c = lastcol + 1, r = lastrow + 1;
         GameBoard.field[r][c] == lastcolor;
         c++, r++, same_right++ );
   if ( same_left + same_right >= maxcoins ) {
   	/* more than maxcoins (4) found --> insert them in our return array */
	   for ( c = lastcol - 1, r = lastrow - 1;
	         GameBoard.field[r][c] == lastcolor;
	         *pc++ = r--, *pc++ = c-- ); 
	   for ( c = lastcol + 1, r = lastrow + 1;
	         GameBoard.field[r][c] == lastcolor;
	         *pc++ = r++, *pc++ = c++ ); 
	}

   /* Then the descending diagonal */
   same_left = 0;
   same_right = 1;
   for ( c = lastcol - 1, r = lastrow + 1;
         GameBoard.field[r][c] == lastcolor;
         c--, r++, same_left++ );
   for ( c = lastcol + 1, r = lastrow - 1;
         GameBoard.field[r][c] == lastcolor;
         c++, r--, same_right++ );
   if ( same_left + same_right >= maxcoins ) {
   	/* more than maxcoins (4) found --> insert them in our return array */
	   for ( c = lastcol - 1, r = lastrow + 1;
	         GameBoard.field[r][c] == lastcolor;
	         *pc++ = r++, *pc++ = c-- ); 
	   for ( c = lastcol + 1, r = lastrow - 1;
	         GameBoard.field[r][c] == lastcolor;
	         *pc++ = r--, *pc++ = c++ );
	}

   /* And at last check the vertical line */
   same_down = 1;
   c = lastcol;
   for ( r = lastrow - 1;
         GameBoard.field[r][c] == lastcolor;
         r--, same_down++ );
   if ( same_down >= maxcoins ) {
	   for ( r = lastrow - 1;
	         GameBoard.field[r][c] == lastcolor;
	         *pc++ = r--, *pc++ = c );
	}
	*pc++ = 0;
	*pc = 0;

   return result;
}


