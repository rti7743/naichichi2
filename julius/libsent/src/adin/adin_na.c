/**
 * @file   adin_na.c
 *
 * <JA>
 * @brief  NetAudio入力用のサブルーチン
 *
 * adin_netaudio.c 用のサブ関数が定義されています．
 *
 * NetAudio のライブラリの dat_types.h が
 * libsent/include/sent/stddefs.h での定義と一部衝突するため，
 * このようにサブルーチン部分を分離しています．
 * </JA>
 * <EN>
 * @brief  Sub routines for NetAudio input
 *
 * This file defines sub functions for NetAudio input in adin_netaudio.c
 *
 * These functions are separated from adin_netaudio.c because some definitions
 * in NetAudio header "dat_types.h" conflicts with the 
 * include header "sent/stddefs.h".
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Sun Feb 13 19:40:56 2005
 *
 * $Revision: 1.3 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <sent/config.h>

#include <stdio.h>
#include <stdlib.h>

/* sound header */
#include <netaudio.h>
#include <defaults.h>
#define TRUE 1			///< Should be the same definition in stddefs.h
#define FALSE 0			///< Should be the same definition in stddefs.h
typedef short SP16;		///< Should be the same definition in stddefs.h

struct adinopt_na {
	NAport *port;		///< NetAudio port
	int need_swap;	///< Incoming data is always BIG ENDIAN
};
typedef struct adinopt_na ADINOPT;

/** 
 * Initialize NetAudio device.
 * 
 * @param sfreq [in] sampling frequency
 * @param server_devname [in] server host name
 * 
 * @return 1 on success, 0 on failure.
 */
void*
NA_standby(int sfreq, char *server_devname)
{
  NAinfo info;
  char *buf;
  int cnt;

  ADINOPT * adinopt = (ADINOPT*) mymalloc(sizeof(ADINOPT));
  memset(adinopt,0,sizeof(ADINOPT));
  /* endian check --- incoming data is BE */
#ifdef WORDS_BIGENDIAN
  adinopt->need_swap = FALSE;
#else  /* LITTLE ENDIAN */
  adinopt->need_swap = TRUE;
#endif /* WORDS_BIGENDIAN */

  /* Initialize '.datlinkrc' processing */
  /*InitDefaults(argv[0]);*/

  /* Open connection to DAT-Link server on server_devname */
  /* if NULL, env AUDIO_DEVICE is used instead. */
  /* if AUDIO_DEVICE not specified, local port is used */
  adinopt->port = NAOpen(server_devname);
  if (adinopt->port == NULL) {
    jlog("Error: adin_na: failed to open netaudio server on %s\n", server_devname);
    return(NULL);
  }

  /* setup parameters */
  NAGetDefaultInfo(&info);
  info.source            = DL_ISRC_ALL; /* input source: all */
  info.record.sampleRate = sfreq; /* DAT(48kHz)->some freq */
  info.record.precision  = 16;	/* bits per sample */
  info.record.encoding   = NA_ENCODING_LINEAR;
  info.record.channels   = NA_CHANNELS_LEFT; /* mono */
  NASetInfo(adinopt->port, &info);

  /* open a data connection for recording */
  if (NAOpenData(adinopt->port, NA_RECORD) == -1) {
    jlog("Error: adin_na: failed to open data connection\n");
    return(NULL);
  }

  jlog("Stat: adin_na: connected to netaudio server on %s\n", server_devname);
  return adinopt;
}

/** 
 * Close port. (actually never used, just for reference...)
 * 
 */
static void
NA_close(void * adinoptTHIS)
{
  ADINOPT* adinopt = (ADINOPT*)adinoptTHIS;

  /* Flush (delete) any buffered data for recording */
  NAFlush(adinopt->port, NA_RECORD);

  /* Close the data connection */
  NACloseData(adinopt->port, 0);

  /* Close connection */
  NAClose(adinopt->port);

}  

/** 
 * Begin recording.
 * 
 */
void
NA_start(void * adinoptTHIS)
{
  ADINOPT* adinopt = (ADINOPT*)adinoptTHIS;
  NABegin(adinopt->port, NA_RECORD);
}

/** 
 * Pause the recording.
 * 
 */
void
NA_stop(void * adinoptTHIS)
{
  ADINOPT* adinopt = (ADINOPT*)adinoptTHIS;
  NAPause(adinopt->port, NA_RECORD, 1);
}

/**
 * @brief  Read samples from NetAudio port.
 * 
 * Try to read @a sampnum samples and returns actual number of recorded
 * samples currently available.  This function will block until
 * at least some samples are obtained.
 * 
 * @param buf [out] samples obtained in this function
 * @param sampnum [in] wanted number of samples to be read
 * 
 * @return actural number of read samples, -1 if an error occured.
 */
int
NA_read(void * adinoptTHIS,SP16 *buf, int sampnum)
{
  ADINOPT* adinopt = (ADINOPT*)adinoptTHIS;
  int cnt;
  cnt = NARead(adinopt->port, (char *)buf, sampnum * sizeof(SP16)) / sizeof(SP16);
  if (adinopt->need_swap) swap_sample_bytes(buf, cnt);
  return(cnt);
}
