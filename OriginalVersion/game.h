/*************************************************************************\
 * game.h
 *
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

typedef int VALFUNC();

typedef unsigned char byte;

/* public constants */
/* I prefer using constants instead of definitions. Although it in most
 * cases it makes no difference, I want to use the better typechecking. */

/* valid entries for the game board: */
enum { EMPTY = 0, RED = 1, BLUE = 2, BORDER = 127 };

/* Minds */
enum { HUMAN = 0, COMPUTER = 1 };

enum { inf = 10000,					/* infinitiy: return if board is won */
		 maxcoins = 4,					/* number of coins needed to win the game */
		 MAXDEPTH = 10 };				/* The end of most computers abilities to do
											 * the minmax recursion in a resonable time */

struct {									/* The board */
	byte sizex;
	byte sizey;
	byte dummy1;						/* Fill to next dword address */
	byte dummy2;
	int movecount;						/* number of moves done so far */
	int maxmoves;						/* an alias for sizex * sizey */
	byte *moves;						/* row of the last inserted coins */
	byte *hgt;							/* number of coins in each column */
	byte **field;						/* game board */
	byte *chgt;							/* copy of hgt and field used during */
	byte **cfield;						/* calculations of a computer choice */
	int *lastval;						/* saves the last valuation of 
											 * GetComputerMove. */
} GameBoard; 

PUBLIC struct {
	byte Color;
	byte Mind;
	byte CalcDepth;
	byte dummy1;
	VALFUNC *ValFunc;
} PlayerInfo[2], *Player;


/* public functions */

PUBLIC int InitBoard( const byte sizex, const byte sizey );

PUBLIC void FreeBoard();

PUBLIC void ClearBoard();

PUBLIC int InitDefaultGame();

PUBLIC inline void InsertCoin( const byte col, const byte color );

PUBLIC inline void DeleteCoin( const int col );

PUBLIC inline byte ToggleColor( const byte oldcolor ) __attribute__ ((const));

PUBLIC void TogglePlayer();

PUBLIC void SetPlayer( const byte color );

PUBLIC inline int TestEnd();

PUBLIC byte GetComputerMove();

PUBLIC byte *GetDeadlyFields();

