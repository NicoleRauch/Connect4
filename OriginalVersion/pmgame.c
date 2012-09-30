/*************************************************************************\
 * pmgame.c
 *
 * In this file you find the functions that have anything to do with
 * the graphical display of the game.
 * (c) 1994 by Nicole Greiber and Ralf Seidel
 *
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#define INCL_GPI
#define INCL_WIN 
#define INCL_DOSPROCESS
#define INCL_DOSDATETIME
#define INCL_DOSSEMAPHORES
#include <os2.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "compile.h"
#include "defs.h"
#include "errmsgs.h"
#include "game.h"
#include "misc.h"
#include "connect4.h"

#undef PUBLIC
#define PUBLIC
#include "pmgame.h"

/***********************************************************************\
 * function GetCoinCenter
 * Returns the Center of the coin that is in (row, col) in POINTL Center
\***********************************************************************/
PUBLIC VOID GetCoinCenter( PPOINTL Center, const INT row, const INT col )
{
	assert( row > 0 && row <= GameBoard.sizey );
	assert( col > 0 && col <= GameBoard.sizex );
	Center->x =  BoardMetrics.FirstCoin.x + (col - 1) * BoardMetrics.d;
	Center->y =  BoardMetrics.FirstCoin.y + (row - 1) * BoardMetrics.d;
}


/*************************************************************************\
 * function GetBoardCol
 * Calculates the the column number from the physical coordinate x.
 * returns 0 if x lies outside of the GameBoard.
\*************************************************************************/
PUBLIC INT GetBoardCol( const LONG x )
{
	if( x < BoardMetrics.Rect.xLeft || x >= BoardMetrics.Rect.xRight )
		return 0;
	else 
		return ( x - BoardMetrics.Rect.xLeft ) / BoardMetrics.d + 1;
}

/*************************************************************************\
 * function GetBoardRow
 * Same as GetBoardCol.
\*************************************************************************/
PUBLIC INT GetBoardRow( const LONG y )
{
	if( y < BoardMetrics.Rect.yBottom || y >= BoardMetrics.Rect.yTop )
		return 0;
	else 
		return ( y - BoardMetrics.Rect.yBottom ) / BoardMetrics.d + 1;
}


/*************************************************************************\
 * function RectSize
 * returns the length of each side of the rectangle Rect
 * in pSize.cx and pSize.cy
\*************************************************************************/
/*inline VOID GetRectSize( const RECTL Rect, PSIZEL pSize )
{
	pSize->cx = Rect.xRight - Rect.xLeft;
	pSize->cy = Rect.yTop - Rect.yBottom;
}
*/
/*************************************************************************\
 * function SetBoardRect
 * This function gets the width and height of the new gameboard and
 * adjusts all necessary variables to this size. Necessary variables
 * are the members of the struct BoardMetrics.
 * 
\*************************************************************************/
PUBLIC VOID SetBoardRect( SHORT Width, SHORT hgt )
{
	LONG len;
	LONG Hshift;
	LONG Vshift;

	len = min( (LONG)Width / (GameBoard.sizex + 2),
				  (LONG)hgt / (GameBoard.sizey + 2) );

	Hshift = (Width - len * (GameBoard.sizex + 2)) / 2;
	Vshift = (hgt - len * (GameBoard.sizey + 2)) / 2;
	/* Hshift and Vshift contain the horizontal and vertical offset*
	 * that is necessary to center the board in the window			*/
	WinSetRect( hab, &BoardMetrics.Rect,
					len + Hshift,
					len + Vshift,
					(GameBoard.sizex + 1) * len + Hshift,
					(GameBoard.sizey + 1) * len + Vshift );
					/* this function returns the rectangle which describes		*
					 * the place where the gameboard is located in the window.	*
					 * Around the board there is a white border which is at		*
					 * least as wide as one of the board's fields, that's why	*	
					 * the rectangle's lower left corner is at						*
					 *	(len + Hshift, len + Vshift).										*/
	BoardMetrics.d = len;	/* the width and height of one of the board's	*
									 * fields (yes, they are square!)					*/
	BoardMetrics.FirstCoin.x = len * 3 / 2 + Hshift;
	BoardMetrics.FirstCoin.y = len * 3 / 2 + Vshift;
	/* the x- and y-coordinate of the lowest and leftmost coin in the board	*/
	BoardMetrics.r = BoardMetrics.d / 3;	/* the radius of the coins	*/
}

/*************************************************************************\
 * function DrawBoard
 * This function paints the gameboard, but only the part that is inside
 * the rectangle PaintRect.
 * Each field is made round.
\*************************************************************************/
PUBLIC VOID DrawBoard( const HPS hps , RECTL PaintRect  )
{
	POINTL BmpCorner;
	POINTL Center;
	INT i;
	INT j;
	const RECTL BoardRect = BoardMetrics.Rect;
	const LONG delta = BoardMetrics.d;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
	HBITMAP hbm;
	
	PaintRect.xLeft = max(PaintRect.xLeft, BoardRect.xLeft );
	PaintRect.xRight = min( PaintRect.xRight, BoardRect.xRight );
	PaintRect.yBottom = max( PaintRect.yBottom, BoardRect.yBottom );
	PaintRect.yTop = min( PaintRect.yTop, BoardRect.yTop );
	/* reduce PaintRect if it was bigger than BoardRect	*/

	PaintRect.xLeft -= (PaintRect.xLeft- BoardRect.xLeft) % delta;
/*	PaintRect.xRight += (BoardRect.xRight - PaintRect.xRight) % delta;	*/
	PaintRect.yBottom -= (PaintRect.yBottom - BoardRect.yBottom) % delta;
/*	PaintRect.yTop +=	(BoardRect.yTop - PaintRect.yTop) % delta;	*/
	/* adjust the left and bottom coordinates so that they are identical	*
	 * with the point where the first bitmap will be painted. The right	*
	 * and top coordinates need not be adjusted exactly						*/
	
	hbm = GpiLoadBitmap( hps, (HMODULE)NULL, IDR_BMP, delta,
		delta ); /* here the bitmap is loaded. It contains the picture of		*
					 * one field of the playing board									*/
	for( BmpCorner.x = PaintRect.xLeft; BmpCorner.x < PaintRect.xRight;
		  BmpCorner.x += delta )
		for( BmpCorner.y = PaintRect.yBottom; BmpCorner.y < PaintRect.yTop;
			  BmpCorner.y += delta )
			WinDrawBitmap( hps, hbm, NULL, &BmpCorner, 0, 0, DBM_IMAGEATTRS );
			/* The bitmap is drawn repeatedly to fill the part of the			*
			 * gameboard that has been invalidated										*/

	PaintRect.xRight += delta / 2;
	PaintRect.yTop += delta / 2;
		/* we have to push these coordinates a little because now we want	*
		 * to find out whether the center of a field is in the newly 		*
		 * painted area (before we regarded the lower left corner)			*/
	Center.x = PaintRect.xLeft + delta / 2;	/* the x-coord. of the lower	*
														 * left coin that has to be painted */
	GpiSetArcParams( hps, &arcparams );
	for( i = (PaintRect.xLeft - BoardRect.xLeft) / delta + 1; /* first column */
		  Center.x < PaintRect.xRight; i++, Center.x+= delta ) {
		Center.y = PaintRect.yBottom + delta / 2;
		for( j = (PaintRect.yBottom - BoardRect.yBottom) / delta + 1; /* first row */
			  Center.y < PaintRect.yTop; j++, Center.y+= delta ) {
         GpiMove( hps, &Center );
         GpiSetColor( hps, 
         	( GameBoard.field[j][i] == RED ) ? CLR_RED : 
				(( GameBoard.field[j][i] == BLUE ) ? CLR_YELLOW : SYSCLR_WINDOW) ); 
					/* check in which colour the coin is drawn. SYSCLR_WINDOW is	*
					 * the background colour and indicates that no coin has been	*
					 * inserted at that position (you can still look through the 	*
					 * board)																		*/
			GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0 ) );
      } /* end for j */
   } /* end for i */
   GpiDeleteBitmap( hbm );	/* releases the bitmap-handle	*/
}
   
/*************************************************************************\
 * function InsertCoinPM
 * A thread to insert a coin.
 * This function draws the last coin that has been inserted. If
 * ProgInfo.fAnimate is TRUE (i. e. if "Animation" has been selected
 * in the Options/Program menu), the coin will "fall" into the gameboard
 *
\*************************************************************************/
PUBLIC VOID InsertCoinPM( HWND hwnd )
{
   HPS hps;					/* presentation space associated with hwnd 	*/

   const LONG delta = BoardMetrics.d;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
   const INT col = GameBoard.moves[GameBoard.movecount];
	INT row = GameBoard.hgt[col];
	const LONG lArcColor = (GameBoard.field[row][col] == RED) ? CLR_RED : CLR_YELLOW;
	const FIXED fxMult = MAKEFIXED( 1, 0 );
   POINTL Center;
   ULONG ulPostCt;

   DosSetPriority( PRTYS_THREAD, PRTYC_NOCHANGE, -5, 0 );
	hps = WinGetPS( hwnd );
	GpiSetArcParams( hps, &arcparams );                   

	if ( ProgInfo.fAnimate ) {
   	Center.x = BoardMetrics.FirstCoin.x + delta * (col - 1);
      Center.y = BoardMetrics.FirstCoin.y + delta * GameBoard.sizey;
      	/* the x-coord contains the column where the coin is inserted	*
      	 * while the y-coord describes the area above the gameboard.	*
      	 * in the next for-loop the coin is painted in its color and	*
      	 * then, after a short DosSleep-while, in the window's back-	*
      	 * ground color. Then the y-coord is decreased and the coin		*
      	 * is painted one row lower than before. 								*/
		/* draw the first coin above the board. */
		GpiMove( hps, &Center );
		GpiSetColor( hps, lArcColor );
		GpiFullArc( hps, DRO_FILL, fxMult );
		do {
			/* Wait a until hevStopInsert is posted but no longer than
			 * 30 msec */
			DosWaitEventSem( hevStopInsert, 30 );
			DosQueryEventSem( hevStopInsert, &ulPostCt );
			/* if ulPostCt is > 0 the thread has to be stoped now */
			if ( ulPostCt ) {
				/* Thread should be stopped --> delete last coin */
				GpiSetColor( hps, SYSCLR_WINDOW );
				GpiFullArc( hps, DRO_FILL, fxMult );
		      Center.y = BoardMetrics.FirstCoin.y + delta * (GameBoard.hgt[col] - 1);
		      row = GameBoard.sizey + 1; /* leave this loop */
		   } else {
				GpiSetColor( hps, SYSCLR_WINDOW );
				GpiFullArc( hps, DRO_FILL, fxMult );
				row++;
				Center.y-= delta;
				GpiMove( hps, &Center );
				GpiSetColor( hps, lArcColor );
				GpiFullArc( hps, DRO_FILL, fxMult );
				DosResetEventSem( hevStopInsert, &ulPostCt );
			} 
		} while ( row <= GameBoard.sizey ) ;
		GpiSetColor( hps, lArcColor );
		GpiMove( hps, &Center );
		GpiFullArc( hps, DRO_FILL, fxMult );
	} else {
		/* No animation --> Just draw the coin and exit immediatly */
		Center.x = BoardMetrics.FirstCoin.x + delta * (col - 1);
		Center.y = BoardMetrics.FirstCoin.y + delta * (row - 1);
		GpiSetColor( hps, lArcColor );
		GpiMove( hps, &Center );
		GpiFullArc( hps, DRO_FILL, fxMult );
	}
	WinReleasePS( hps );
}
	
/*************************************************************************\
 * function DeleteCoinPM
 * deletes a coin from the graphical gameboard
\*************************************************************************/
PUBLIC VOID DeleteCoinPM( const HWND hwnd )
{
	HPS hps;
	POINTL Center;
	ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
   const LONG delta = BoardMetrics.d;
	const INT col = GameBoard.moves[GameBoard.movecount];
	INT row = GameBoard.hgt[col];
	const LONG lArcColor = (GameBoard.field[row][col] == RED) ? CLR_RED : CLR_YELLOW;
	const FIXED fxMult = MAKEFIXED( 1, 0 );
   ULONG ulPostCt;

	hps = WinGetPS( hwnd );
   DosSetPriority( PRTYS_THREAD, PRTYC_NOCHANGE, -5, 0 );
	GpiSetArcParams( hps, &arcparams );                   

	if ( ProgInfo.fAnimate ) {
		GetCoinCenter( &Center, row, col );
		GpiMove( hps, &Center );
		GpiSetColor( hps, SYSCLR_WINDOW );
		GpiFullArc( hps, DRO_FILL, fxMult );
		do {
			row++;
			Center.y+= delta;
			GpiMove( hps, &Center );
			GpiSetColor( hps, lArcColor );
			GpiFullArc( hps, DRO_FILL, fxMult );
			/* Wait until hevStopInsert is posted but no longer than
			 * 30 msec */
			DosWaitEventSem( hevStopInsert, 30 );
			DosQueryEventSem( hevStopInsert, &ulPostCt );
			GpiSetColor( hps, SYSCLR_WINDOW );
			GpiFullArc( hps, DRO_FILL, fxMult );
			/* if ulPostCt is > 0 the thread has to be stopped now */
			if ( ulPostCt ) {
		      row = GameBoard.sizey + 1; /* leave this loop */
		   } 
		} while ( row <= GameBoard.sizey ) ;
	} else {
		/* No animation --> Just delete the coin and exit immediatly */
		GetCoinCenter( &Center, row, col );
		GpiMove( hps, &Center );
		GpiSetColor( hps, SYSCLR_WINDOW );
		GpiFullArc( hps, DRO_FILL, fxMult );
	}
	WinReleasePS( hps );
}	
	
/*************************************************************************\
 * function DropCoinsPM
 * makes all the inserted coins fall out of the board
\*************************************************************************/
PUBLIC VOID DropCoinsPM( const HPS hps )
{
	INT i, j, k;
	const LONG delta = BoardMetrics.d;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
   POINTL Center = { BoardMetrics.FirstCoin.x ,
                     BoardMetrics.FirstCoin.y };

	if( ProgInfo.fAnimate ) {
		GpiSetArcParams( hps, &arcparams );
		for( i = 1; i <= GameBoard.sizey + 1; i++ ) {
			/* indicates the row that is mirrored below the gameboard */
			Center.x = BoardMetrics.FirstCoin.x;
			for( j = 1; j <= GameBoard.sizex; j++, Center.x += delta ) {
				/* indicates the column being processed */
				Center.y = BoardMetrics.FirstCoin.y - delta;
				for( k = i; k <= GameBoard.sizey + 1; k++, Center.y += delta ) {
					/* points to the coin i and all coins that are above */
		         GpiMove( hps, &Center );
  		 	      GpiSetColor( hps, 
  		   	   	( GameBoard.field[k][j] == RED ) ? CLR_RED : 
						(( GameBoard.field[k][j] == BLUE ) ? CLR_YELLOW :
						SYSCLR_WINDOW) ); 
					GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0 ) );
				}	/* end for k */
			}	/* end for j	*/
		}	/*	end for i	*/
		ClearBoardPM();
	} else {
		ClearBoardPM();
		DrawBoard( hps, BoardMetrics.Rect );
	}	
	/* it is necessary to call ClearBoardPM from here because otherwise	*
	 * it would be quicker than this function (which is called as a		*
	 * thread) and would erase the gameboard (the real variable-values)	*
	 * before this function has finished its job (then the falling is		*
	 * interrupted)																		*/
}	
		
/*************************************************************************\
 * function ShowHintPM
 * makes the hint field flash
\*************************************************************************/
PUBLIC VOID ShowHintPM( const HPS hps )
{
	POINTL Center;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
	const ULONG color = (GameInfo.HintColor == RED) ? CLR_RED : CLR_YELLOW;
	const INT col = GameInfo.HintCol;
	const INT row = (INT)GameBoard.hgt[col] + 1;
	ULONG ulPostCt;

	assert( col > 0 && col <= GameBoard.sizex );
	assert( row > 0 && row <= GameBoard.sizey );
	GetCoinCenter( &Center, row, col );
	GpiSetArcParams( hps, &arcparams );
	GpiMove( hps, &Center );
	do {
		GpiSetColor( hps, color );
		GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		DosWaitEventSem( hevStopFlash, 200 );
		DosQueryEventSem( hevStopFlash, &ulPostCt );
		if ( ulPostCt ) break;
		GpiSetColor( hps, CLR_BLACK );
		GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		DosWaitEventSem( hevStopFlash, 200 );
		/* Test if hevStopFlash has been posted */
		DosQueryEventSem( hevStopFlash, &ulPostCt );
	} while ( !ulPostCt );
	GpiSetColor( hps, SYSCLR_WINDOW );
	GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
}	


/*************************************************************************\
 * function ShowWinPM
 * makes the coins flash that have lead to victory
\*************************************************************************/
PUBLIC VOID ShowWinPM( const HPS hps )
{
	PBYTE result;
	POINTL ptlCenter[7 * maxcoins];
	INT i, j;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
	LONG color;
	ULONG ulPostCt;

	result = GetDeadlyFields();
	color = (GameBoard.field[result[0]][result[1]] == RED) ? CLR_RED : CLR_YELLOW;
	for( j = 0; result[j]; j+= 2 ) { 
		GetCoinCenter( &ptlCenter[j / 2], result[j], result[j+1] );
	}
	GpiSetArcParams( hps, &arcparams );
	do {
		GpiSetColor( hps, CLR_BLACK );
		for( i = 0; result[2 * i]; i++ ) { 
			GpiMove( hps, &ptlCenter[i] );
			GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		}
		DosWaitEventSem( hevStopFlash, 150 );
		DosQueryEventSem( hevStopFlash, &ulPostCt );
		if ( ulPostCt ) break;
		GpiSetColor( hps, color );
		for( i = 0; result[2 * i]; i++ ) { 
			GpiMove( hps, &ptlCenter[i] );
			GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		}
		DosWaitEventSem( hevStopFlash, 300 );
		/* Test if hevStopFlash has been posted */
		DosQueryEventSem( hevStopFlash, &ulPostCt );
	} while ( !ulPostCt );
	GpiSetColor( hps, color );
	for( i = 0; result[2 * i]; i++ ) { 
		GpiMove( hps, &ptlCenter[i] );
		GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
	}
	free( result );
}	
	
/*************************************************************************\
 * function ClearBoardPM
 * adjusts some GameInfo variables and calls the function that
 * resets the gameboard variables
\*************************************************************************/
PUBLIC VOID ClearBoardPM( )
{
	SetPlayer( ProgInfo.StartingPlayer );
	ClearBoard();
	GameInfo.UndoCount = 0;
	GameInfo.InputAllowed = Player->Mind == HUMAN;
	GameInfo.HasLaughed = 0;
}
		
/*************************************************************************\
 * function InitDefaultGamePM
 * If connect4.ini is not found, the game is initialized with these values
\*************************************************************************/
PUBLIC BOOL InitDefaultGamePM()
{
	if ( InitDefaultGame( 7, 6 ))
		return TRUE;
	ProgInfo.StartingPlayer = RED;
	ClearBoardPM();
	ProgInfo.fSound = FALSE;
	ProgInfo.fSaveSettings = FALSE;
	ProgInfo.fAnimate = TRUE;
	ProgInfo.fAdjustSize = FALSE;
	ProgInfo.fChgPointer = TRUE;
	return FALSE;
}
	
