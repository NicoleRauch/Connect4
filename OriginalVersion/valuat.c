/*************************************************************************\
 * valuat.c
 * Valuation functions.
 * (c) 1994 Nicole Greiber and Ralf Seidel
\*************************************************************************/
#include "compile.h"
#include "game.h"
#include "rand.h"
#include "misc.h"
#define INITIALIZE
#include "valuat.h"

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
 * Valuates one line. This function is used in RalfsValuation().
\*************************************************************************/
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

/*************************************************************************\
 * This function seems to be the best one... so far.
 * Try to find a better one.
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

