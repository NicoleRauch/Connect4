/*************************************************************************\
 *  File name  :  defs.h
 *
 *  Description:  This file contains the definitions for the PM event 
 *                handling and resource routines.
 *
 * Set tabs to 3 to get a readable source.
\*************************************************************************/

/* General defintions */

#define IDR_MAIN           100
#define IDR_RDOWNPTR			101
#define IDR_BDOWNPTR			102
#define IDR_BMP				103

/* Menue definitions */
 
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

#define IDC_HUMAN2			311
#define IDC_COMPUTER2		312
#define IDC_SKILL2			313
#define IDC_GROUP2			314

#define IDR_PROGOPTDLG		400
#define IDC_SOUND				401
#define IDC_SAVESETTINGS	402

#define IDR_PRODINFODLG		500
#define IDR_PRODINFOICON	501

/* string definitions */
#define IDS_APPNAME        1000
#define IDS_VERSION			1001

/* sound definitions */
#define SND_INIT				2000
#define SND_INSERT			2001
#define SND_CREATE			2002
#define SND_WIN				2003
#define SND_LOOSE				2004
#define SND_DRAW				2005
#define SND_UNDO				2006
#define SND_NEW				2007
#define SND_EXIT				2008
#define SND_START				2009
#define SND_LAUGH				2010
