/***************************************************************************\
 * mmsound.c
 *
 * (c) 1994 by Nicole Greiber
 * Set tabs to 3 to get a readable source.
\***************************************************************************/
#include <os2.h>
#ifdef USEMMSOUND
#define  INCL_OS2MM
#include <os2me.h>

#include <stdlib.h> 
#include "compile.h"
#include "connect4.h" 
#include "defs.h"
#define INITIALIZE
#include "mmsound.h"


typedef struct {
    ULONG operation;
    ULONG operand1;
    ULONG operand2;
    ULONG operand3;
} PLAYLIST;


typedef struct {
	PSZ bptr;
	ULONG bsize;
	ULONG SamplesPerSec;
	USHORT BitsPerSample;
	USHORT Channels;
	char* name;
	PLAYLIST pl[2];
} SOUND;

SOUND insert, create, win, loose, draw, undo, neww, exitt, laugh, hint;

/***********************************************************************\
 * function LoadFile
 * opens a waveaudio file, gets some information about the sampling
 * rate etc., closes the file again and sets up a playlist for this
 * file (this list only contains the file and an exit command)
\***********************************************************************/
VOID LoadFile(SOUND* file)
{
    HMMIO  hmmio;
    MMAUDIOHEADER mmah;
	 ULONG BytesRead = 0;
	 char *p = (char*)&mmah;
	 int i = 0;

	 /* initialize the header */
	 for( ; i < sizeof(mmah); i++ ) p[i] = 0;
    /* open the file */
    hmmio = mmioOpen(file->name, 0, MMIO_READ);
    /* get header */	 
    mmioGetHeader(hmmio, &mmah, sizeof(mmah), &BytesRead, 0, 0);
    /* get some infos about the waveaudio file  */
    file->SamplesPerSec = mmah.mmXWAVHeader.WAVEHeader.ulSamplesPerSec;
    file->BitsPerSample = mmah.mmXWAVHeader.WAVEHeader.usBitsPerSample;
    file->Channels = mmah.mmXWAVHeader.WAVEHeader.usChannels;
    file->bsize = mmah.mmXWAVHeader.XWAVHeaderInfo.ulAudioLengthInBytes;
    file->bptr  = calloc(file->bsize, sizeof(char));
    /* read file */
    mmioRead(hmmio, file->bptr, file->bsize);
    /* close file */
    mmioClose(hmmio, 0);

	/* setup playlist */
    file->pl[0].operation = DATA_OPERATION; 
    file->pl[0].operand1  = (ULONG)file->bptr;
    file->pl[0].operand2  = file->bsize;
    file->pl[0].operand3  = 0;

    file->pl[1].operation = EXIT_OPERATION;
    file->pl[1].operand1  = 0;
    file->pl[1].operand2  = 0;
    file->pl[1].operand3  = 0;
}

/***********************************************************************\
 * function WaveAudio
 * opens the sound device, plays the playlist belonging to the file
 * and closes the sound device 
\***********************************************************************/
VOID WaveAudio(SOUND file)
{
    MCI_OPEN_PARMS mop;
    MCI_WAVE_SET_PARMS wsp;
    MCI_PLAY_PARMS mpp;
    MCI_GENERIC_PARMS mgp;
    
    /* open device  */
    mop.hwndCallback   = 0;
    mop.usDeviceID     = 0;
    mop.pszDeviceType  = MCI_DEVTYPE_WAVEFORM_AUDIO_NAME;
    mop.pszElementName = (PSZ)"insert.wav";

    mciSendCommand(0,
                              MCI_OPEN,                        /* open message */
                              MCI_WAIT | MCI_OPEN_SHAREABLE |  /* message flags */
                              MCI_OPEN_ELEMENT,
                              &mop,                            /* parameters */
                              0);

    
    /* set device parameters */
    wsp.hwndCallback    = 0;
    wsp.ulSamplesPerSec = file.SamplesPerSec;
    wsp.usBitsPerSample = file.BitsPerSample;
    wsp.usChannels = file.Channels;

    mciSendCommand(mop.usDeviceID, MCI_SET,
			MCI_WAIT | MCI_WAVE_SET_SAMPLESPERSEC,
         &wsp, 0);

    mciSendCommand(mop.usDeviceID, MCI_SET,
			MCI_WAIT | MCI_WAVE_SET_BITSPERSAMPLE,
         &wsp, 0);

    mciSendCommand(mop.usDeviceID, MCI_SET,
			MCI_WAIT | MCI_WAVE_SET_CHANNELS,
         &wsp, 0);

    /* play the playlist */
    mpp.hwndCallback = 0;
    mciSendCommand(mop.usDeviceID, MCI_PLAY, MCI_WAIT, &mpp, 0);

    /* close device */
    mgp.hwndCallback = 0;
    mciSendCommand(mop.usDeviceID, MCI_CLOSE, MCI_WAIT, &mgp, 0);
}

/***********************************************************************\
 * function FreeMem
 * frees the memory that has been allocated
 * This function is only interesting when the Sound-option is disabled
 * but the game is not finished.
\***********************************************************************/
VOID FreeMem(SOUND file)
{
	free(file.bptr);
	free(file.pl);
}

/***********************************************************************\
 * function PlaySound
 * this is the heart of this file. Only this function is called from
 * outside. It gets a parameter that indicates which sound to play,
 * and then does its work. On startup it also initializes the sound
 * variables, and on exit it frees the memory occupied by these variables
\***********************************************************************/
PUBLIC VOID PlaySound(ULONG sound )
{
	if (! ProgInfo.fSound) return;
	switch (sound) {
		case SND_INIT:
/*			create.name = "create.wav";
			LoadFile(&create);
		 	WaveAudio(create);*/	/* play the create-sound as early as possible */
		   insert.name = "insert.wav";
			win.name = "win.wav";
			loose.name = "loose.wav";
			draw.name = "draw.wav";
			undo.name = "undo.wav";
			neww.name = "new.wav";
			exitt.name = "exit.wav";
			laugh.name = "laugh.wav";
			hint.name = "hint.wav";
			LoadFile(&insert);
			LoadFile(&win);
			LoadFile(&loose);
			LoadFile(&draw);
			LoadFile(&undo);
			LoadFile(&neww);
			LoadFile(&exitt);
			LoadFile(&laugh);
			LoadFile(&hint);
			break;
		case SND_INSERT:
	   	if( ProgInfo.fAnimate) WaveAudio(insert);
	   	break;
		case SND_WIN:
			WaveAudio(win);
			break;
		case SND_LOOSE:
			WaveAudio(loose);
			break;
		case SND_DRAW:
			WaveAudio(draw);
			break;
		case SND_HINT:
			WaveAudio(hint);
			break;
		case SND_UNDO:
			WaveAudio(undo);
			break;
		case SND_NEW:
			if( ProgInfo.fAnimate) WaveAudio(neww);
			break;
		case SND_LAUGH:
			WaveAudio(laugh);
			break;
		case SND_EXIT:
			WaveAudio(exitt);
	 		FreeMem(insert);
/*			FreeMem(create);	*/
			FreeMem(win);
			FreeMem(loose);
			FreeMem(draw);
			FreeMem(undo);
			FreeMem(neww);
			FreeMem(exitt);
			FreeMem(laugh);
			FreeMem(hint);
			break;
	} /* end switch (sound) */
}
#else /* No sound available --> Define a dummy playsound function */
#define INITIALIZE
#include "mmsound.h"

PUBLIC VOID PlaySound( ULONG sound )
{
	return;
}

#endif /* USEMMSOUND */


