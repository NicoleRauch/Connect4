/*************************************************************************\
 * pmgame.c
 * 
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#define INCL_GPI
#define INCL_WIN 
#define INCL_DOSPROCESS
#include <os2.h>
#include <malloc.h>
#include <stdio.h>
#include "compile.h"
#include "defs.h"
#include "pmassert.h"
#include "game.h"
#include "connect4.h"

#undef PUBLIC
#define PUBLIC
#include "pmgame.h"


/***********************************************************************\
 * Returns the Center of the actual coin in POINTL Center
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
 * Calculates the the collumn number from the physical coordinate x.
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
 * returns the lenght of each side of a rectangle in pWidth and phgt
\*************************************************************************/
inline VOID GetRectSize( const RECTL Rect, PSIZEL pSize )
{
	pSize->cx = Rect.xRight - Rect.xLeft;
	pSize->cy = Rect.yTop - Rect.yBottom;
}

/*************************************************************************\
 * function SetBoardRect
 * This function has to adjust the parameter pRect to that 
 * 
\*************************************************************************/
PUBLIC VOID SetBoardRect( SHORT Width, SHORT hgt )
{
	LONG lx1;
	LONG ly1;
	LONG Hshift;
	LONG Vshift;

	lx1 = (LONG)Width / (GameBoard.sizex + 2);
	ly1 = (LONG)hgt / (GameBoard.sizey + 2);

	if ( lx1 < ly1 ) {
		Hshift = (Width - lx1 * (GameBoard.sizex + 2)) / 2;
		Vshift = (hgt - lx1 * (GameBoard.sizey + 2)) / 2;
		WinSetRect( hab, &BoardMetrics.Rect,
						lx1 + Hshift,
						lx1 + Vshift,
						(GameBoard.sizex + 1) * lx1 + Hshift,
						(GameBoard.sizey + 1) * lx1 + Vshift );
		BoardMetrics.d = lx1;
		BoardMetrics.FirstCoin.x = lx1 * 3 / 2 + Hshift;
		BoardMetrics.FirstCoin.y = lx1 * 3 / 2 + Vshift;
	} else {
		Hshift = (Width - ly1 * (GameBoard.sizex + 2)) / 2;
		Vshift = (hgt - ly1 * (GameBoard.sizey + 2)) / 2;
		WinSetRect( hab, &BoardMetrics.Rect,
						ly1 + Hshift,
						ly1 + Vshift,
						(GameBoard.sizex + 1) * ly1 + Hshift,
						(GameBoard.sizey + 1) * ly1 + Vshift );
		BoardMetrics.d = ly1;
		BoardMetrics.FirstCoin.x = ly1 * 3 / 2 + Hshift;
		BoardMetrics.FirstCoin.y = ly1 * 3 / 2 + Vshift;
	}
	BoardMetrics.r = BoardMetrics.d / 3;
}

/*************************************************************************\
 * function to draw the board;
 * The board is drawn centered inside the coordinates specified by Rect.
 * Each field is made round.
\*************************************************************************/
PUBLIC VOID DrawBoard( const HPS hps )
{
	SIZEL RectSize;
	POINTL BmpCorner;
	POINTL Center;
	INT i;
	INT j;
	const RECTL Rect = BoardMetrics.Rect;
	const LONG delta = BoardMetrics.d;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
	HBITMAP hbm;
	
	GetRectSize( Rect, &RectSize );
	assert( RectSize.cx > 2 * GameBoard.sizex );
	assert( RectSize.cy > 2 * GameBoard.sizey );
	BmpCorner.x = Rect.xLeft;
	BmpCorner.y = Rect.yBottom;
	hbm = GpiLoadBitmap( hps, (HMODULE)NULL, IDR_BMP, delta,
		delta ); /* here the bitmap is loaded that contains the picture of	*
					 * one field of the playing board									*/
	for( ; BmpCorner.x < Rect.xRight; BmpCorner.x += delta )
		for( BmpCorner.y = Rect.yBottom; BmpCorner.y < Rect.yTop; BmpCorner.y += delta )
			WinDrawBitmap( hps, hbm, NULL, &BmpCorner, 0, 0, DBM_IMAGEATTRS );
			/* The bitmap is drawn repeatedly to fill the playing board 	*/
	Center.x = BoardMetrics.FirstCoin.x;	/* the x-coord. of the lower left	*
														 * coin in the board						*/
	GpiSetArcParams( hps, &arcparams );
	for( i = 1; i <= GameBoard.sizex; i++, Center.x+= delta ) {
		Center.y = BoardMetrics.FirstCoin.y;
		for( j = 1; j <= GameBoard.sizey; j++, Center.y+= delta ) {
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
 * function to draw one coin 
 * 
\*************************************************************************/
PUBLIC VOID DrawLastCoin( const HPS hps )
{
   const LONG delta = BoardMetrics.d;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
   const INT col = GameBoard.moves[GameBoard.movecount];
	INT row = GameBoard.hgt[col];
	const LONG lArcColor = (GameBoard.field[row][col] == RED) ? CLR_RED : CLR_YELLOW;
	const FIXED fxMult = MAKEFIXED( 1, 0 );
   POINTL Center;
                   

	GpiSetArcParams( hps, &arcparams );                   
	if ( ProgInfo.fAnimate ) {
   	Center.x = BoardMetrics.FirstCoin.x + delta * (col - 1);
      Center.y = BoardMetrics.FirstCoin.y + delta * GameBoard.sizey;
		for ( ; row <= GameBoard.sizey; row++, Center.y-= delta ) {
			GpiMove( hps, &Center );
			GpiSetColor( hps, lArcColor );
			GpiFullArc( hps, DRO_FILL, fxMult );
			
			DosSleep( 25 );
			GpiSetColor( hps, SYSCLR_WINDOW );
			GpiFullArc( hps, DRO_FILL, fxMult );
		}
	} else {
		Center.x = BoardMetrics.FirstCoin.x + delta * (col - 1);
		Center.y = BoardMetrics.FirstCoin.y + delta * (row - 1);
	}
		
	GpiSetColor( hps, lArcColor );
	GpiMove( hps, &Center );
	GpiFullArc( hps, DRO_FILL, fxMult );
}
	
/*************************************************************************\
 * function DeleteCoinPM
 * 
\*************************************************************************/
PUBLIC VOID DeleteCoinPM( const HPS hps, const INT row, const INT col )
{
	POINTL Center;
	ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };

	GetCoinCenter( &Center, row, col );
	GpiSetArcParams( hps, &arcparams );
	GpiMove( hps, &Center );
	GpiSetColor( hps, SYSCLR_WINDOW );
	GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
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
		for( i = 1; i <= GameBoard.sizey + 1; i++ ) /* counts the number of "falls" */
		{
			Center.x = BoardMetrics.FirstCoin.x;
			for( j = 1; j <= GameBoard.sizex; j++, Center.x += delta ) /* indicates the column being processed */
			{
				Center.y = BoardMetrics.FirstCoin.y - delta;
				for( k = i; k <= GameBoard.sizey + 1 /*GameBoard.field[j][i+k-1]*/ ;
						 k++, Center.y += delta )	/* offset from lowest row */
				{
		         GpiMove( hps, &Center );
  		 	      GpiSetColor( hps, 
  		   	   	( GameBoard.field[k][j] == RED ) ? CLR_RED : 
						(( GameBoard.field[k][j] == BLUE ) ? CLR_YELLOW : SYSCLR_WINDOW) ); 
					GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0 ) );
				}
			}
		}
		ClearBoardPM();
	} else {
		ClearBoardPM();
		DrawBoard( hps );
	}
}	
		
/*************************************************************************\
 * function ShowHintPM
\*************************************************************************/
PUBLIC VOID ShowHintPM( const HPS hps )
{
	POINTL Center;
	ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
	ULONG color = (GameInfo.HintColor == RED) ? CLR_RED : CLR_YELLOW;
	const INT col = GameInfo.HintCol;
	const INT row = (INT)GameBoard.hgt[col] + 1;

	assert( col > 0 && col <= GameBoard.sizex );
	assert( row > 0 && row <= GameBoard.sizey );
	GetCoinCenter( &Center, row, col );
	GpiSetArcParams( hps, &arcparams );
	GpiMove( hps, &Center );
	while ( GameInfo.FlashingDisplay ) {
		GpiSetColor( hps, color );
		GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		DosSleep( 300 );
		GpiSetColor( hps, CLR_BLACK );
		GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		DosSleep( 150 );
	}	
		GpiSetColor( hps, SYSCLR_WINDOW );
		GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
}	


/*************************************************************************\
 * function ShowWinPM  bis jetzt tut's noch nichts, frag mich nicht warum
\*************************************************************************/
PUBLIC VOID ShowWinPM( const HPS hps )
{
	byte *result;
	POINTL Center[maxcoins];
	INT i, j;
	const ARCPARAMS arcparams = { BoardMetrics.r, BoardMetrics.r, 0, 0 };
	LONG color;

	result = (byte*)malloc(4 * maxcoins * sizeof( byte ));
	result = GetDeadlyFields();
	/*Center = (PPOINTL)malloc( maxcoins * sizeof(POINTL));*/
	color = (GameBoard.field[result[0]][result[1]] == RED) ? CLR_RED : CLR_YELLOW;
	for( i = 0, j = 0; i < maxcoins; i++) { 
		GetCoinCenter( &Center[i], result[j], result[j+1] );
		j += 2;
	}
	GpiSetArcParams( hps, &arcparams );
	while ( GameInfo.FlashingDisplay ) {
		GpiSetColor( hps, CLR_BLACK );
		for (i=0; i < maxcoins; i++ ) {
			GpiMove( hps, &Center[i] );
			GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		}
		DosSleep( 150 );
		GpiSetColor( hps, color );
		for (i=0; i < maxcoins; i++ ) {
			GpiMove( hps, &Center[i] );
			GpiFullArc( hps, DRO_FILL, MAKEFIXED( 1, 0) );
		}
		DosSleep( 300 );
	} 
}	
	
/*************************************************************************\
 * function ClearBoardPM
\*************************************************************************/
PUBLIC VOID ClearBoardPM( )
{
	SetPlayer( ProgInfo.StartingPlayer );
	ClearBoard();
	GameInfo.UndoCount = 0;
	GameInfo.InputAllowed = Player->Opponent == HUMAN;
}
		
/*************************************************************************\
 * function InitDefaultGamePM 
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
	
