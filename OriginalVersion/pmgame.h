/*************************************************************************\
 * pmgame.h
 * 
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/
#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif

PUBLIC struct {
	RECTL Rect;
	POINTL FirstCoin;	/* center of the left bottom field.						*/
	LONG d;				/* offset between two fields in x-direction 			*/
	LONG r;				/* radius of the circles to be drawn for each coin	*/
} BoardMetrics;

PUBLIC struct {
	BOOL InputAllowed;
	INT UndoCount;
	INT HintCol;
	INT HintColor;
	BOOL FlashingDisplay;
	INT HasLaughed;
} GameInfo;

HEV hevStopInsert;
HEV hevStopFlash;

BOOL fStopInsertThread;

PUBLIC VOID DrawBoard( const HPS hps, RECTL rectl );

PUBLIC VOID SetBoardRect( SHORT Width, SHORT Height );

PUBLIC INT GetBoardCol( const LONG x );

PUBLIC INT GetBoardRow( const LONG y );

PUBLIC VOID ClearBoardPM();

PUBLIC VOID InsertCoinPM( HWND hwnd );

PUBLIC VOID DeleteCoinPM( const HWND hwnd );

PUBLIC VOID ShowHintPM( const HPS hps );

PUBLIC VOID ShowWinPM( const HPS hps );

PUBLIC BOOL InitDefaultGamePM();

PUBLIC VOID DropCoinsPM( const HPS hps );

