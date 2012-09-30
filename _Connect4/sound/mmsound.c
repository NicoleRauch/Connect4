/*
 rhythm - waveaudio playlist sample

 using emx 0.8h, mm4emx 1.0


 Marc E.E. van Woerkom, 2/94
*/


#include <os2.h>

#define  INCL_OS2MM
#include <os2me.h>
#include <stdio.h>
#include <stdlib.h>
#include "compile.h"
#include "connect4.h"
#include "defs.h"

#undef PUBLIC
#define PUBLIC

/* prototypes */

void mci_err(ULONG, PCH);
void mmio_err(ULONG);

/*
// ple: a playlist entry
*/

typedef struct {
    ULONG operation;
    ULONG operand1;
    ULONG operand2;
    ULONG operand3;
} ple;


typedef struct {
	PSZ bptr;
	ULONG bsize;
	ULONG SamplesPerSec;
	USHORT BitsPerSample;
	USHORT Channels;
	char* name;
	ple pl[2];
} memwav;

memwav start, insert, create, win, loose, draw, undo, neww, exitt, laugh;

/*
// mem_wav: a waveaudio file loaded into memory
*/


VOID mem_wav(memwav* file)
{
    HMMIO  hmmio;
    MMAUDIOHEADER mmah;
    ULONG rc;
	 ULONG BytesRead = 0;
	 char *p = (char*)&mmah;
	 int i = 0;

	 /* initialize the header */
	 for( ; i < sizeof(mmah); i++ ) p[i] = 0;
    /* open the file */
    hmmio = mmioOpen(file->name, 0, MMIO_READ);
    /* get header */	 
    rc = mmioGetHeader(hmmio, &mmah, sizeof(mmah),
                             &BytesRead, 0, 0);

    if (rc != MMIO_SUCCESS) mmio_err(rc);


    /* get some infos about the waveaudio file  */

    file->SamplesPerSec = mmah.mmXWAVHeader.WAVEHeader.ulSamplesPerSec;
    file->BitsPerSample = mmah.mmXWAVHeader.WAVEHeader.usBitsPerSample;
    file->Channels = mmah.mmXWAVHeader.WAVEHeader.usChannels;
    file->bsize = mmah.mmXWAVHeader.XWAVHeaderInfo.ulAudioLengthInBytes;
    file->bptr  = calloc(file->bsize, sizeof(char));
    printf("%ld Samples per Sec and %d Channel(s) in file %s\n", file->SamplesPerSec, file->Channels, file->name);

    /* read file */

    rc = mmioRead(hmmio, file->bptr, file->bsize);
 
    if (rc == MMIO_ERROR) mmio_err(rc);



    /* close file */

    rc = mmioClose(hmmio, 0);
    if (rc != MMIO_SUCCESS) mmio_err(rc);

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


/*
// waveaudio: a waveaudio device
*/

VOID waveaudio(memwav file)
{
    MCI_OPEN_PARMS mop;
    ULONG rc;
    MCI_WAVE_SET_PARMS wsp;
    MCI_PLAY_PARMS mpp;
    MCI_GENERIC_PARMS mgp;
    
    /* open device  */

    mop.hwndCallback   = 0;
    mop.usDeviceID     = 0;
    mop.pszDeviceType  = MCI_DEVTYPE_WAVEFORM_AUDIO_NAME;
    mop.pszElementName = (PSZ)file.pl;

    rc = mciSendCommand(0,
                        MCI_OPEN,                        /* open message */
                        MCI_WAIT | MCI_OPEN_SHAREABLE |  /* message flags */
                        MCI_OPEN_PLAYLIST,
                        &mop,                            /* parameters */
                        0);

    if (rc != MCIERR_SUCCESS)	mci_err(rc, " in open dev");

    /* set device parameters */
    wsp.hwndCallback    = 0;
    wsp.ulSamplesPerSec = file.SamplesPerSec;
    wsp.usBitsPerSample = file.BitsPerSample;
    wsp.usChannels = file.Channels;

   rc = mciSendCommand(mop.usDeviceID, MCI_SET,
			MCI_WAIT | MCI_WAVE_SET_SAMPLESPERSEC,
         &wsp, 0);
    if (rc != MCIERR_SUCCESS) mci_err(rc, " in set samples per sec");

    rc = mciSendCommand(mop.usDeviceID, MCI_SET,
			MCI_WAIT | MCI_WAVE_SET_BITSPERSAMPLE,
         &wsp, 0);
    if (rc != MCIERR_SUCCESS) mci_err(rc, " in set bits per sample");

    rc = mciSendCommand(mop.usDeviceID, MCI_SET,
			MCI_WAIT | MCI_WAVE_SET_CHANNELS,
         &wsp, 0);

    if (rc != MCIERR_SUCCESS) mci_err(rc, " in set channels");

    /* play the playlist */

    mpp.hwndCallback = 0;

    rc = mciSendCommand(mop.usDeviceID,
                              MCI_PLAY,
                              MCI_WAIT,
                              &mpp,
                              0);

    if (rc != MCIERR_SUCCESS) mci_err(rc, " in play");

    /* close device */

    mgp.hwndCallback = 0;

    rc = mciSendCommand(mop.usDeviceID,
                              MCI_CLOSE,
                              MCI_WAIT,
                              &mgp,
                              0);

    if (rc != MCIERR_SUCCESS) mci_err(rc, " in close");
}


/*
// mci_err: translate the MCI return code into an error string
*/

void mci_err(ULONG rc, PCH str)
{
    const INT rsize = 128;
    char rbuff[rsize];
    ULONG rc2;

    rc2 = mciGetErrorString(rc,      /* error code */
                                  rbuff,   /* return buffer */
                                  rsize);  /* rbuff size */

    if (rc2 == MCIERR_SUCCESS)
    	printf("MCI error %s : %ld %s\n", str, rc, rbuff);
    
/*        cerr << "MCI error" << str << ": " << rbuff << "\n\n"; */
    else
    	printf("error # %ld %s has occured! \n", rc, str);
/*        cerr << "error #" << rc << str << " has occured!\n\n"; */
}



/*
// mmio_err: translate MMIO error code into a string
*/

void mmio_err(ULONG rc)
{
    char* s;

    switch (rc) {
    case MMIO_SUCCESS:
        s = "SUCCESS (huh?)";
        break;
    case MMIOERR_UNBUFFERED:
        s = "UNBUFFERD";
        break;
    case MMIOERR_INVALID_HANDLE:
        s = "INVALID HANDLE";
        break;
    case MMIOERR_INVALID_PARAMETER:
        s = "INVALID PARAMETER";
        break;
    case MMIOERR_READ_ONLY_FILE:
        s = "READ ONLY FILE";
        break;
    case MMIOERR_WRITE_ONLY_FILE:
        s = "WRITE ONLY FILE";
        break;
    case MMIOERR_WRITE_FAILED:
        s = "WRITE FAILED";
        break;
    case MMIOERR_READ_FAILED:
        s = "READ FAILED";
        break;
    case MMIOERR_SEEK_FAILED:
        s = "SEEK FAILED";
        break;
    case MMIOERR_NO_FLUSH_NEEDED:
        s = "NO FLUSH NEEDED";
        break;
    case MMIOERR_OUTOFMEMORY:
        s = "OUT OF MEMORY";
        break;
    case MMIOERR_CANNOTEXPAND:
        s = "CANNOT EXPAND";
        break;
    case MMIOERR_FREE_FAILED:
        s = "FREE FAILED";
        break;
    case MMIOERR_CHUNKNOTFOUND:
        s = "CHUNK NOT FOUND";
        break;
    case MMIO_ERROR:
        s = "ERROR";
        break;
    case MMIO_WARNING:
        s = "WARNING";
        break;
    case MMIO_CF_FAILURE:
        s = "CF FAILURE";
        break;
    default:
        s = " (hmm...)";
    }
	printf("MMIO error %s\n", s);
}



PUBLIC VOID PlaySound(ULONG sound )
{
	switch (sound){
	case SND_INIT:
	   insert.name = "ein2.wav";
		create.name = "criver.wav";
		win.name = "applause.wav";
		loose.name = "boo.wav";
		draw.name = "cuckoo.wav";
		undo.name = "wooeep.wav";
	/*	start( */
		neww.name = "newboard.wav";
		exitt.name = "doorcls.wav";
		laugh.name = "laughing2.wav";

		mem_wav(&insert);
		mem_wav(&create);
		mem_wav(&win);
		mem_wav(&loose);
		mem_wav(&draw);
		mem_wav(&undo);
		mem_wav(&neww);
		mem_wav(&exitt);
		mem_wav(&laugh);

		break;
	case SND_INSERT:
    	waveaudio(insert);
    	break;
	case SND_CREATE:
	 	waveaudio(create);
	 	break;
	case SND_WIN:
	 	waveaudio(win);
	 	break;
	case SND_LOOSE:
	 	waveaudio(loose);
	 	break;
	case SND_DRAW:
	 	waveaudio(draw);
	 	break;
	case SND_UNDO:
	 	waveaudio(undo);
	 	break;
	case SND_NEW:
	 	waveaudio(neww);
	 	break;
	case SND_EXIT:
	 	waveaudio(exitt);
	 	break;
	case SND_LAUGH:
	 	waveaudio(laugh);
	 	break; 

	} /* end switch (sound) */
}

int main()
{
	PlaySound(SND_INIT);
   PlaySound(SND_INSERT);
   PlaySound(SND_CREATE);
   PlaySound(SND_WIN);
	PlaySound(SND_LOOSE);
	PlaySound(SND_DRAW);
	PlaySound(SND_UNDO);
	PlaySound(SND_NEW);
	PlaySound(SND_EXIT);
	PlaySound(SND_LAUGH);

	return 0;
}
