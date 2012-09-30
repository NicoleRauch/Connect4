/*************************************************************************\
 *  defs.h
 *  Description:  This file contains the definitions for the PM event 
 *                handling and resource routines.
 *
 * (c) 1994 by Ralf Seidel
 * Set tabs to 3 to get a readable source.
\*************************************************************************/

/* General defintions */

#define IDR_MAIN           100
#define IDR_RDOWNPTR			101
#define IDR_BDOWNPTR			102
#define IDR_BMP				103

/* Menu definitions */
 
#define IDM_FILE           200
#define IDM_FILELOAD       201
#define IDM_FILESAVE       202
#define IDM_FILESAVEAS     203
#define IDM_FILEEXIT       204 
#define IDM_FILEDEINST		205

#define IDM_GAME           210
#define IDM_GAMESTART		211
#define IDM_GAMEUNDO       212    
#define IDM_GAMEHINT       213    
#define IDM_GAMENEW        214

#define IDM_OPTS				220
#define IDM_GAMEOPTS	   	221
#define IDM_PROGOPTS			222
#define IDM_SAVEOPTS			223

#define IDM_HELP           290
#define IDM_HELPINDEX      291      
#define IDM_HELPGENERAL    292    
#define IDM_HELPPRODUCTINFO 293 


/* definitons for the options dialogs */

#define IDR_GAMEOPTDLG		300
#define IDC_HUMAN1			301
#define IDC_COMPUTER1		302
#define IDC_SKILL1			303
#define IDC_GROUP1			304
#define IDC_SKILLVAL1		305

#define IDC_HUMAN2			311
#define IDC_COMPUTER2		312
#define IDC_SKILL2			313
#define IDC_GROUP2			314
#define IDC_SKILLVAL2		315

#define IDC_STARTRED			321
#define IDC_STARTBLUE		322

#define IDR_PROGOPTDLG		400
#define IDC_SOUND				401
#define IDC_SAVESETTINGS	402
#define IDC_ANIMATE			403
#define IDC_CHGPOINTER		404

/* definitions relation the Product Information dialog */

#define IDR_PRODINFODLG		500
#define IDR_PRODINFOICON	501

/* string definitions */
#define IDS_APPNAME        1000
#define IDS_VERSION			1001

/* sound definitions */
#define SND_INIT				2000
#define SND_INSERT			2001
#define SND_HINT				2002
#define SND_WIN				2003
#define SND_LOOSE				2004
#define SND_DRAW				2005
#define SND_UNDO				2006
#define SND_NEW				2007
#define SND_EXIT				2008
#define SND_LAUGH				2010


/* help system definitions */

#define HELP_TABLE			5000
#define SUBTABLE_MAIN		5001
#define SUBTABLE_GAMEOPT	5002
#define SUBTABLE_PROGOPT	5003
#define SUBTABLE_PRODUCTINFO 5004

/* Don't use any tabs here!!!!
 * We need this definitions for the help file connect4.ipf. Because we
 * want to use the preprocessor of gcc to use the following symbols instead
 * of integers and because IPFC doesn't like tabulator characters we have
 * to avoid them here. */
#define PANEL_MAIN 6000
#define PANEL_FILE 6001
#define PANEL_FILELOAD 6002
#define PANEL_FILESAVE 6003
#define PANEL_FILEEXIT 6004
#define PANEL_GAME 6010
#define PANEL_GAMESTART 6011
#define PANEL_GAMEUNDO 6012
#define PANEL_GAMEHINT 6013
#define PANEL_GAMENEW 6014
#define PANEL_HELP 6020
#define PANEL_HELPINDEX 6021
#define PANEL_HELPGENERAL 6022
#define PANEL_HELPABOUT 6023
#define PANEL_OPTS 6030
#define PANEL_GAMEOPTS 6031
#define PANEL_PROGOPTS 6032
#define PANEL_SAVEOPTS 6033
#define PANEL_MENU 6040

#define PANEL_GAMEOPT 6100

#define PANEL_PROGOPT 6200

#define PANEL_PRODUCTINFO 6300

#define PANEL_AUTHORS 6501
#define PANEL_SOURCE 6502
#define PANEL_TERMS 6503
#define PANEL_FUTURE 6504
#define PANEL_CONTEST 6505
