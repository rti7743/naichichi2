/**
 * @file   realtime-1stpass.c
 * 
 * <JA>
 * @brief  ÂE¥Ñ¥¹¡§¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¡Ê¼Â»ş´Ö½èÍıÈÇ¡Ë
 *
 * ÂE¥Ñ¥¹¤òÆşÎÏ³«»Ï¤ÈÆ±»ş¤Ë¥¹¥¿¡¼¥È¤·¡¤ÆşÎÏ¤ÈÊ¿¹Ô¤·¤ÆÇ§¼±½èÍı¤ò¹Ô¤¦¤¿¤á¤Î
 * ´Ø¿ô¤¬ÄEÁ¤µ¤EÆ¤¤¤E 
 * 
 * ¥Ğ¥Ã¥Á½èÍı¤Î¾Eç¡¤Julius ¤Î²»À¼Ç§¼±½èÍı¤Ï°Ê²¼¤Î¼Eç¤Ç
 * main_recognition_loop() Æâ¤Ç¼Â¹Ô¤µ¤EE 
 *
 *  -# ²»À¼ÆşÎÏ adin_go()  ¢ª ÆşÎÏ²»À¼¤¬ speech[] ¤Ë³ÊÇ¼¤µ¤EE *  -# ÆÃÄ§ÎÌÃEĞ new_wav2mfcc() ¢ªspeech¤«¤éÆÃÄ§¥Ñ¥é¥á¡¼¥¿¤Eparam ¤Ë³ÊÇ¼
 *  -# ÂE¥Ñ¥¹¼Â¹Ô get_back_trellis() ¢ªparam ¤È¥â¥Ç¥E«¤éÃ±¸EÈ¥EE¹¤ÎÀ¸À®
 *  -# ÂE¥Ñ¥¹¼Â¹Ô wchmm_fbs()
 *  -# Ç§¼±·EÌ½ĞÎÏ
 *
 * ÂE¥Ñ¥¹¤òÊ¿¹Ô½èÍı¤¹¤EEç¡¤¾åµ­¤Î 1 ¡Á 3 ¤¬Ê¿¹Ô¤·¤Æ¹Ô¤EEE 
 * Julius ¤Ç¤Ï¡¤¤³¤ÎÊÂ¹Ô½èÍı¤ò¡¤²»À¼ÆşÎÏ¤ÎÃÇÊÒ¤¬ÆÀ¤é¤EE¿¤Ó¤Ë
 * Ç§¼±½èÍı¤ò¤½¤ÎÊ¬¤À¤±Á²¼¡Åª¤Ë¿Ê¤á¤E³¤È¤Ç¼ÂÁõ¤·¤Æ¤¤¤E 
 * 
 *  - ÆÃÄ§ÎÌÃEĞ¤ÈÂE¥Ñ¥¹¼Â¹Ô¤ò¡¤°EÄ¤Ë¤Ş¤È¤á¤Æ¥³¡¼¥EĞ¥Ã¥¯´Ø¿ô¤È¤·¤ÆÄEÁ. 
 *  - ²»À¼ÆşÎÏ´Ø¿Eadin_go() ¤Î¥³¡¼¥EĞ¥Ã¥¯¤È¤·¤Æ¾åµ­¤Î´Ø¿ô¤òÍ¿¤¨¤E *
 * ¶ñÂÎÅª¤Ë¤Ï¡¤¤³¤³¤ÇÄEÁ¤µ¤EÆ¤¤¤ERealTimePipeLine() ¤¬¥³¡¼¥EĞ¥Ã¥¯¤È¤·¤Æ
 * adin_go() ¤ËÍ¿¤¨¤é¤EE adin_go() ¤Ï²»À¼ÆşÎÏ¤¬¥È¥E¬¤¹¤EÈ¤½¤ÎÆÀ¤é¤E¿ÆşÎÏ
 * ÃÇÊÒ¤´¤È¤Ë RealTimePipeLine() ¤ò¸Æ¤Ó½Ğ¤¹. RealTimePipeLine() ¤ÏÆÀ¤é¤E¿
 * ÃÇÊÒÊ¬¤Ë¤Ä¤¤¤ÆÆÃÄ§ÎÌÃEĞ¤ÈÂE¥Ñ¥¹¤Î·×»»¤ò¿Ê¤á¤E 
 *
 * CMN ¤Ë¤Ä¤¤¤ÆÃúÌÕ¤¬É¬Í×¤Ç¤¢¤E CMN ¤ÏÄÌ¾E¯ÏÃÃ±°Ì¤Ç¹Ô¤EEE¬¡¤
 * ¥Ş¥¤¥¯ÆşÎÏ¤ä¥Í¥Ã¥È¥E¼¥¯ÆşÎÏ¤Î¤è¤¦¤Ë¡¤ÂE¥Ñ¥¹¤ÈÊ¿¹Ô¤ËÇ§¼±¤ò¹Ô¤¦
 * ½èÍı»ş¤ÏÈ¯ÏÃÁ´ÂÎ¤Î¥±¥×¥¹¥È¥é¥àÊ¿¶Ñ¤òÆÀ¤E³¤È¤¬¤Ç¤­¤Ê¤¤. ¥Ğ¡¼¥¸¥ç¥E3.5
 * °ÊÁ°¤Ç¤ÏÄ¾Á°¤ÎÈ¯ÏÃ5ÉÃÊ¬(´şµÑ¤µ¤E¿ÆşÎÏ¤ò½E¯)¤Î CMN ¤¬¤½¤Î¤Ş¤Ş¼¡È¯ÏÃ¤Ë
 * Î®ÍÑ¤µ¤EÆ¤¤¤¿¤¬¡¤3.5.1 ¤«¤é¤Ï¡¤¾åµ­¤ÎÄ¾Á°È¯ÏÃ CMN ¤ò½é´EÍ¤È¤·¤Æ
 * È¯ÏÃÆECMN ¤EMAP-CMN ¤ò»ı¤Á¤¤¤Æ·×»»¤¹¤Eè¤¦¤Ë¤Ê¤Ã¤¿. ¤Ê¤ª¡¤
 * ºÇ½é¤ÎÈ¯ÏÃÍÑ¤Î½é´üCMN¤E"-cmnload" ¤ÇÍ¿¤¨¤E³¤È¤â¤Ç¤­¡¤¤Ş¤¿
 * "-cmnnoupdate" ¤ÇÆşÎÏ¤´¤È¤Î CMN ¹¹¿·¤ò¹Ô¤EÊ¤¤¤è¤¦¤Ë¤Ç¤­¤E 
 * "-cmnnoupdate" ¤È "-cmnload" ¤ÈÁÈ¤ß¹ç¤E»¤E³¤È¤Ç, ºÇ½é¤Ë¥°¥ú½¼¥Ğ¥EÊ
 * ¥±¥×¥¹¥È¥é¥àÊ¿¶Ñ¤òÍ¿¤¨¡¤¤½¤Eò¾EË½é´EÍ¤È¤·¤Æ MAP-CMN ¤¹¤E³¤È¤¬¤Ç¤­¤E 
 *
 * ¼çÍ×¤Ê´Ø¿ô¤Ï°Ê²¼¤ÎÄÌ¤ê¤Ç¤¢¤E 
 *
 *  - RealTimeInit() - µ¯Æ°»ş¤Î½é´E½
 *  - RealTimePipeLinePrepare() - ÆşÎÏ¤´¤È¤Î½é´E½
 *  - RealTimePipeLine() - ÂE¥Ñ¥¹Ê¿¹Ô½èÍıÍÑ¥³¡¼¥EĞ¥Ã¥¯¡Ê¾å½Ò¡Ë
 *  - RealTimeResume() - ¥·¥ç¡¼¥È¥İ¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó»ş¤ÎÇ§¼±ÉE¢
 *  - RealTimeParam() - ÆşÎÏ¤´¤È¤ÎÂE¥Ñ¥¹½ªÎ»½èÍı
 *  - RealTimeCMNUpdate() - CMN ¤Î¹¹¿·
 *  
 * </JA>
 * 
 * <EN>
 * @brief  The first pass: frame-synchronous beam search (on-the-fly version)
 *
 * These are functions to perform on-the-fly decoding of the 1st pass
 * (frame-synchronous beam search).  These function can be used
 * instead of new_wav2mfcc() and get_back_trellis().  These functions enable
 * recognition as soon as an input triggers.  The 1st pass processing
 * will be done concurrently with the input.
 *
 * The basic recognition procedure of Julius in main_recognition_loop()
 * is as follows:
 *
 *  -# speech input: (adin_go())  ... buffer `speech' holds the input
 *  -# feature extraction: (new_wav2mfcc()) ... compute feature vector
 *     from `speech' and store the vector sequence to `param'.
 *  -# recognition 1st pass: (get_back_trellis()) ... frame-wise beam decoding
 *     to generate word trellis index from `param' and models.
 *  -# recognition 2nd pass: (wchmm_fbs())
 *  -# Output result.
 *
 * At on-the-fly decoding, procedures from 1 to 3 above will be performed
 * in parallel.  It is implemented by a simple scheme, processing the captured
 * small speech fragments one by one progressively:
 *
 *  - Define a callback function that can do feature extraction and 1st pass
 *    processing progressively.
 *  - The callback will be given to A/D-in function adin_go().
 *
 * Actual procedure is as follows. The function RealTimePipeLine()
 * will be given to adin_go() as callback.  Then adin_go() will watch
 * the input, and if speech input starts, it calls RealTimePipeLine()
 * for every captured input fragments.  RealTimePipeLine() will
 * compute the feature vector of the given fragment and proceed the
 * 1st pass processing for them, and return to the capture function.
 * The current status will be hold to the next call, to perform
 * inter-frame processing (computing delta coef. etc.).
 *
 * Note about CMN: With acoustic models trained with CMN, Julius performs
 * CMN to the input.  On file input, the whole sentence mean will be computed
 * and subtracted.  At the on-the-fly decoding, the ceptral mean will be
 * performed using the cepstral mean of last 5 second input (excluding
 * rejected ones).  This was a behavier earlier than 3.5, and 3.5.1 now
 * applies MAP-CMN at on-the-fly decoding, using the last 5 second cepstrum
 * as initial mean.  Initial cepstral mean at start can be given by option
 * "-cmnload", and you can also prohibit the updates of initial cepstral
 * mean at each input by "-cmnnoupdate".  The last option is useful to always
 * use static global cepstral mean as initial mean for each input.
 *
 * The primary functions in this file are:
 *  - RealTimeInit() - initialization at application startup
 *  - RealTimePipeLinePrepare() - initialization before each input
 *  - RealTimePipeLine() - callback for on-the-fly 1st pass decoding
 *  - RealTimeResume() - recognition resume procedure for short-pause segmentation.
 *  - RealTimeParam() - finalize the on-the-fly 1st pass when input ends.
 *  - RealTimeCMNUpdate() - update CMN data for next input
 * 
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Tue Aug 23 11:44:14 2005
 *
 * $Revision: 1.8 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

#undef RDEBUG			///< Define if you want local debug message

/** 
 * <JA>
 * MFCC·×»»¥¤¥ó¥¹¥¿¥ó¥¹Æâ¤ËÆÃÄ§¥Ñ¥é¥á¡¼¥¿¥Ù¥¯¥È¥EÊÇ¼¥¨¥E¢¤ò½àÈ÷¤¹¤E
 * 
 * mfcc->para ¤Î¾ğÊó¤Ë´ğ¤Å¤¤¤Æ¥Ø¥Ã¥À¾ğÊó¤ò³ÊÇ¼¤·¡¤½é´EÊÇ¼ÎÎ°è¤ò³ÎÊİ¤¹¤E 
 * ³ÊÇ¼ÎÎ°è¤Ï¡¤ÆşÎÏ»ş¤ËÉ¬Í×¤Ë±ş¤¸¤Æ¼«Æ°Åª¤Ë¿­Ä¹¤µ¤EEÎ¤Ç¡¤¤³¤³¤Ç¤Ï
 * ¤½¤Î½àÈ÷¤À¤±¹Ô¤¦. ¤¹¤Ç¤Ë³ÊÇ¼ÎÎ°è¤¬³ÎÊİ¤µ¤EÆ¤¤¤EÈ¤­¤Ï¤½¤Eò¥­¡¼¥×¤¹¤E 
 * 
 * ¤³¤EÏÆşÎÏ/Ç§¼±1²ó¤´¤È¤Ë·«¤EÖ¤·¸Æ¤Ğ¤EE
 * 
 * </JA>
 * <EN>
 * 
 * Prepare parameter holder in MFCC calculation instance to store MFCC
 * vectors.
 *
 * This function will store header information based on the parameters
 * in mfcc->para, and allocate initial buffer for the incoming
 * vectors.  The vector buffer will be expanded as needed while
 * recognition, so at this time only the minimal amount is allocated.
 * If the instance already has a certain length of vector buffer, it
 * will be kept.
 * 
 * This function will be called each time a new input begins.
 * 
 * </EN>
 *
 * @param mfcc [i/o] MFCC calculation instance
 * 
 */
static void
init_param(MFCCCalc *mfcc)
{
  Value *para;

  para = mfcc->para;

  /* ¤³¤E«¤é·×»»¤µ¤EEÑ¥é¥á¡¼¥¿¤Î·¿¤ò¥Ø¥Ã¥À¤ËÀßÄE*/
  /* set header types */
  mfcc->param->header.samptype = F_MFCC;
  if (para->delta) mfcc->param->header.samptype |= F_DELTA;
  if (para->acc) mfcc->param->header.samptype |= F_ACCL;
  if (para->energy) mfcc->param->header.samptype |= F_ENERGY;
  if (para->c0) mfcc->param->header.samptype |= F_ZEROTH;
  if (para->absesup) mfcc->param->header.samptype |= F_ENERGY_SUP;
  if (para->cmn) mfcc->param->header.samptype |= F_CEPNORM;
  
  mfcc->param->header.wshift = para->smp_period * para->frameshift;
  mfcc->param->header.sampsize = para->veclen * sizeof(VECT); /* not compressed */
  mfcc->param->veclen = para->veclen;
  
  /* Ç§¼±½èÍıÃE½ªÎ»¸å¤Ë¥»¥Ã¥È¤µ¤EEÑ¿E
     param->parvec (¥Ñ¥é¥á¡¼¥¿¥Ù¥¯¥È¥EÏÎE
     param->header.samplenum, param->samplenum (Á´¥Õ¥E¼¥à¿E
  */
  /* variables that will be set while/after computation has been done:
     param->parvec (parameter vector sequence)
     param->header.samplenum, param->samplenum (total number of frames)
  */
  /* MAP-CMN ¤Î½é´E½ */
  /* Prepare for MAP-CMN */
  if (mfcc->para->cmn || mfcc->para->cvn) CMN_realtime_prepare(mfcc->cmn.wrk);
}

/** 
 * <JA>
 * @brief  ÂE¥Ñ¥¹Ê¿¹ÔÇ§¼±½èÍı¤Î½é´E½.
 *
 * MFCC·×»»¤Î¥E¼¥¯¥¨¥E¢³ÎÊİ¤ò¹Ô¤¦. ¤Ş¤¿É¬Í×¤Ê¾Eç¤Ï¡¤¥¹¥Ú¥¯¥È¥Eº»»ÍÑ¤Î
 * ¥E¼¥¯¥¨¥E¢½àÈ÷¡¤¥Î¥¤¥º¥¹¥Ú¥¯¥È¥EÎ¥ú½¼¥É¡¤CMNÍÑ¤Î½é´E±¥×¥¹¥È¥é¥E * Ê¿¶Ñ¥Ç¡¼¥¿¤Î¥ú½¼¥É¤Ê¤É¤â¹Ô¤EEE 
 *
 * ¤³¤Î´Ø¿ô¤Ï¡¤¥·¥¹¥Æ¥àµ¯Æ°¸E²ó¤À¤±¸Æ¤Ğ¤EE
 * </JA>
 * <EN>
 * @brief  Initializations for the on-the-fly 1st pass decoding.
 *
 * Work areas for all MFCC caculation instances are allocated.
 * Additionaly,
 * some initialization will be done such as allocating work area
 * for spectral subtraction, loading noise spectrum from file,
 * loading initial ceptral mean data for CMN from file, etc.
 *
 * This will be called only once, on system startup.
 * </EN>
 *
 * @param recog [i/o] engine instance
 *
 * @callgraph
 * @callergraph
 */
boolean
RealTimeInit(Recog *recog)
{
  Value *para;
  Jconf *jconf;
  RealBeam *r;
  MFCCCalc *mfcc;


  jconf = recog->jconf;
  r = &(recog->real);

  /* ºÇÂç¥Õ¥E¼¥àÄ¹¤òºÇÂçÆşÎÏ»ş´Ö¿ô¤«¤é·×»» */
  /* set maximum allowed frame length */
  r->maxframelen = MAXSPEECHLEN / recog->jconf->input.frameshift;

  /* -ssload »ØÄEş, SSÍÑ¤Î¥Î¥¤¥º¥¹¥Ú¥¯¥È¥Eò¥Õ¥¡¥¤¥E«¤éÆÉ¤ß¹ş¤E*/
  /* if "-ssload", load noise spectrum for spectral subtraction from file */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->frontend.ssload_filename && mfcc->frontend.ssbuf == NULL) {
      if ((mfcc->frontend.ssbuf = new_SS_load_from_file(mfcc->frontend.ssload_filename, &(mfcc->frontend.sslen))) == NULL) {
	jlog("ERROR: failed to read \"%s\"\n", mfcc->frontend.ssload_filename);
	return FALSE;
      }
      /* check ssbuf length */
      if (mfcc->frontend.sslen != mfcc->wrk->bflen) {
	jlog("ERROR: noise spectrum length not match\n");
	return FALSE;
      }
      mfcc->wrk->ssbuf = mfcc->frontend.ssbuf;
      mfcc->wrk->ssbuflen = mfcc->frontend.sslen;
      mfcc->wrk->ss_alpha = mfcc->frontend.ss_alpha;
      mfcc->wrk->ss_floor = mfcc->frontend.ss_floor;
    }
  }

  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
  
    para = mfcc->para;

    /* ÂĞ¿ô¥¨¥Í¥E®¡¼Àµµ¬²½¤Î¤¿¤á¤Î½é´EÍ */
    /* set initial value for log energy normalization */
    if (para->energy && para->enormal) energy_max_init(&(mfcc->ewrk));
    /* ¥Ç¥E¿·×»»¤Î¤¿¤á¤Î¥µ¥¤¥¯¥EĞ¥Ã¥Õ¥¡¤òÍÑ°Õ */
    /* initialize cycle buffers for delta and accel coef. computation */
    if (para->delta) mfcc->db = WMP_deltabuf_new(para->baselen, para->delWin);
    if (para->acc) mfcc->ab = WMP_deltabuf_new(para->baselen * 2, para->accWin);
    /* ¥Ç¥E¿·×»»¤Î¤¿¤á¤Î¥E¼¥¯¥¨¥E¢¤ò³ÎÊİ */
    /* allocate work area for the delta computation */
    mfcc->tmpmfcc = (VECT *)mymalloc(sizeof(VECT) * para->vecbuflen);
    /* MAP-CMN ÍÑ¤Î½é´E±¥×¥¹¥È¥é¥àÊ¿¶Ñ¤òÆÉ¤ß¹ş¤ó¤Ç½é´E½¤¹¤E*/
    /* Initialize the initial cepstral mean data from file for MAP-CMN */
    if (para->cmn || para->cvn) mfcc->cmn.wrk = CMN_realtime_new(para, mfcc->cmn.map_weight);
    /* -cmnload »ØÄEş, CMNÍÑ¤Î¥±¥×¥¹¥È¥é¥àÊ¿¶Ñ¤Î½é´EÍ¤ò¥Õ¥¡¥¤¥E«¤éÆÉ¤ß¹ş¤E*/
    /* if "-cmnload", load initial cepstral mean data from file for CMN */
    if (mfcc->cmn.load_filename) {
      if (para->cmn) {
	if ((mfcc->cmn.loaded = CMN_load_from_file(mfcc->cmn.wrk, mfcc->cmn.load_filename))== FALSE) {
	  jlog("WARNING: failed to read initial cepstral mean from \"%s\", do flat start\n", mfcc->cmn.load_filename);
	}
      } else {
	jlog("WARNING: CMN not required on AM, file \"%s\" ignored\n", mfcc->cmn.load_filename);
      }
    }

  }
  /* ÁE¹¤ò¥»¥Ã¥È */
  /* set window length */
  r->windowlen = recog->jconf->input.framesize + 1;
  /* ÁE«¤±ÍÑ¥Ğ¥Ã¥Õ¥¡¤ò³ÎÊİ */
  /* set window buffer */
  r->window = mymalloc(sizeof(SP16) * r->windowlen);

  return TRUE;
}

/** 
 * <EN>
 * Prepare work are a for MFCC calculation.
 * Reset values in work area for starting the next input.
 * Output probability cache for each acoustic model will be also
 * prepared at this function.
 *
 * This function will be called before starting each input (segment).
 * </EN>
 * <JA>
 * MFCC·×»»¤ò½àÈ÷¤¹¤E 
 * ¤¤¤¯¤Ä¤«¤Î¥E¼¥¯¥¨¥E¢¤ò¥E»¥Ã¥È¤·¤ÆÇ§¼±¤ËÈ÷¤¨¤E 
 * ¤Ş¤¿¡¤²»¶Á¥â¥Ç¥E´¤È¤Î½ĞÎÏ³ÎÎ¨·×»»¥­¥ã¥Ã¥·¥å¤ò½àÈ÷¤¹¤E 
 *
 * ¤³¤Î´Ø¿ô¤Ï¡¤¤¢¤EşÎÏ¡Ê¤¢¤E¤¤Ï¥»¥°¥á¥ó¥È¡Ë¤ÎÇ§¼±¤¬
 * »Ï¤Ş¤E°¤ËÉ¬¤º¸Æ¤Ğ¤EE 
 * 
 * </JA>
 * 
 * @param recog [i/o] engine instance
 * 
 * @callgraph
 * @callergraph
 */
void
reset_mfcc(Recog *recog) 
{
  Value *para;
  MFCCCalc *mfcc;
  RealBeam *r;

  r = &(recog->real);

  /* ÆÃÄ§ÃEĞ¥â¥¸¥å¡¼¥Eò½é´E½ */
  /* initialize parameter extraction module */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {

    para = mfcc->para;

    /* ÂĞ¿ô¥¨¥Í¥E®¡¼Àµµ¬²½¤Î¤¿¤á¤Î½é´EÍ¤ò¥»¥Ã¥È */
    /* set initial value for log energy normalization */
    if (para->energy && para->enormal) energy_max_prepare(&(mfcc->ewrk), para);
    /* ¥Ç¥E¿·×»»ÍÑ¥Ğ¥Ã¥Õ¥¡¤ò½àÈE*/
    /* set the delta cycle buffer */
    if (para->delta) WMP_deltabuf_prepare(mfcc->db);
    if (para->acc) WMP_deltabuf_prepare(mfcc->ab);
  }

}

/** 
 * <JA>
 * @brief  ÂE¥Ñ¥¹Ê¿¹ÔÇ§¼±½èÍı¤Î½àÈE *
 * ·×»»ÍÑÊÑ¿ô¤ò¥E»¥Ã¥È¤·¡¤³Æ¼EÇ¡¼¥¿¤ò½àÈ÷¤¹¤E 
 * ¤³¤Î´Ø¿ô¤Ï¡¤¤¢¤EşÎÏ¡Ê¤¢¤E¤¤Ï¥»¥°¥á¥ó¥È¡Ë¤ÎÇ§¼±¤¬
 * »Ï¤Ş¤E°¤Ë¸Æ¤Ğ¤EE 
 * 
 * </JA>
 * <EN>
 * @brief  Preparation for the on-the-fly 1st pass decoding.
 *
 * Variables are reset and data are prepared for the next input recognition.
 *
 * This function will be called before starting each input (segment).
 * 
 * </EN>
 *
 * @param recog [i/o] engine instance
 *
 * @return TRUE on success. FALSE on failure.
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
RealTimePipeLinePrepare(Recog *recog)
{
  RealBeam *r;
  PROCESS_AM *am;
  MFCCCalc *mfcc;
#ifdef SPSEGMENT_NAIST
  RecogProcess *p;
#endif

  r = &(recog->real);

  /* ·×»»ÍÑ¤ÎÊÑ¿ô¤ò½é´E½ */
  /* initialize variables for computation */
  r->windownum = 0;
  /* parameter check */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    /* ¥Ñ¥é¥á¡¼¥¿½é´E½ */
    /* parameter initialization */
    if (recog->jconf->input.speech_input == SP_MFCMODULE) {
      if (mfc_module_set_header(mfcc, recog) == FALSE) return FALSE;
    } else {
      init_param(mfcc);
    }
    /* ¥Õ¥E¼¥à¤´¤È¤Î¥Ñ¥é¥á¡¼¥¿¥Ù¥¯¥È¥EİÂ¸¤ÎÎÎ°è¤ò³ÎÊİ */
    /* ¤¢¤È¤ÇÉ¬Í×¤Ë±ş¤¸¤Æ¿­Ä¹¤µ¤EE*/
    if (param_alloc(mfcc->param, 1, mfcc->param->veclen) == FALSE) {
      j_internal_error("ERROR: segmented: failed to allocate memory for rest param\n");
    }
    /* ¥Õ¥E¼¥à¿ô¤ò¥E»¥Ã¥È */
    /* reset frame count */
    mfcc->f = 0;
  }
  /* ½àÈ÷¤·¤¿ param ¹½Â¤ÂÎ¤Î¥Ç¡¼¥¿¤Î¥Ñ¥é¥á¡¼¥¿·¿¤ò²»¶Á¥â¥Ç¥EÈ¥Á¥§¥Ã¥¯¤¹¤E*/
  /* check type coherence between param and hmminfo here */
  if (recog->jconf->input.paramtype_check_flag) {
    for(am=recog->amlist;am;am=am->next) {
      if (!check_param_coherence(am->hmminfo, am->mfcc->param)) {
	jlog("ERROR: input parameter type does not match AM\n");
	return FALSE;
      }
    }
  }

  /* ·×»»ÍÑ¤Î¥E¼¥¯¥¨¥E¢¤ò½àÈE*/
  /* prepare work area for calculation */
  if (recog->jconf->input.type == INPUT_WAVEFORM) {
    reset_mfcc(recog);
  }
  /* ²»¶ÁÌàÅÙ·×»»ÍÑ¥­¥ã¥Ã¥·¥å¤ò½àÈE*/
  /* prepare cache area for acoustic computation of HMM states and mixtures */
  for(am=recog->amlist;am;am=am->next) {
    outprob_prepare(&(am->hmmwrk), r->maxframelen);
  }

#ifdef BACKEND_VAD
  if (recog->jconf->decodeopt.segment) {
    /* initialize segmentation parameters */
    spsegment_init(recog);
  }
#else
  recog->triggered = FALSE;
#endif

#ifdef DEBUG_VTLN_ALPHA_TEST
  /* store speech */
  recog->speechlen = 0;
#endif

  return TRUE;
}

/** 
 * <JA>
 * @brief  ²»À¼ÇÈ·Á¤«¤é¥Ñ¥é¥á¡¼¥¿¥Ù¥¯¥È¥Eò·×»»¤¹¤E
 * 
 * ÁE±°Ì¤Ç¼è¤EĞ¤µ¤E¿²»À¼ÇÈ·Á¤«¤éMFCC¥Ù¥¯¥È¥Eò·×»»¤¹¤E
 * ·×»»·EÌ¤Ï mfcc->tmpmfcc ¤ËÊİÂ¸¤µ¤EE 
 * 
 * @param mfcc [i/o] MFCC·×»»¥¤¥ó¥¹¥¿¥ó¥¹
 * @param window [in] ÁE±°Ì¤Ç¼è¤EĞ¤µ¤E¿²»À¼ÇÈ·Á¥Ç¡¼¥¿
 * @param windowlen [in] @a window ¤ÎÄ¹¤µ
 * 
 * @return ·×»»À®¸ù»ş¡¤TRUE ¤òÊÖ¤¹. ¥Ç¥E¿·×»»¤Ë¤ª¤¤¤ÆÆşÎÏ¥Õ¥E¼¥à¤¬
 * ¾¯¤Ê¤¤¤Ê¤É¡¤¤Ş¤ÀÆÀ¤é¤EÆ¤¤¤Ê¤¤¾Eç¤Ï FALSE ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * @brief  Compute a parameter vector from a speech window.
 *
 * This function calculates an MFCC vector from speech data windowed from
 * input speech.  The obtained MFCC vector will be stored to mfcc->tmpmfcc.
 * 
 * @param mfcc [i/o] MFCC calculation instance
 * @param window [in] speech input (windowed from input stream)
 * @param windowlen [in] length of @a window
 * 
 * @return TRUE on success (an vector obtained).  Returns FALSE if no
 * parameter vector obtained yet (due to delta delay).
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
RealTimeMFCC(MFCCCalc *mfcc, SP16 *window, int windowlen)
{
  int i;
  boolean ret;
  VECT *tmpmfcc;
  Value *para;

  tmpmfcc = mfcc->tmpmfcc;
  para = mfcc->para;

  /* ²»À¼ÇÈ·Á¤«¤Ebase MFCC ¤ò·×»» (recog->mfccwrk ¤òÍøÍÑ) */
  /* calculate base MFCC from waveform (use recog->mfccwrk) */
  for (i=0; i < windowlen; i++) {
    mfcc->wrk->bf[i+1] = (float) window[i];
  }
  WMP_calc(mfcc->wrk, tmpmfcc, para);

  if (para->energy && para->enormal) {
    /* ÂĞ¿ô¥¨¥Í¥E®¡¼¹à¤òÀµµ¬²½¤¹¤E*/
    /* normalize log energy */
    /* ¥E¢¥E¿¥¤¥àÆşÎÏ¤Ç¤ÏÈ¯ÏÃ¤´¤È¤ÎºÇÂç¥¨¥Í¥E®¡¼¤¬ÆÀ¤é¤EÊ¤¤¤Î¤Ç
       Ä¾Á°¤ÎÈ¯ÏÃ¤Î¥Ñ¥E¼¤ÇÂåÍÑ¤¹¤E*/
    /* Since the maximum power of the whole input utterance cannot be
       obtained at real-time input, the maximum of last input will be
       used to normalize.
    */
    tmpmfcc[para->baselen-1] = energy_max_normalize(&(mfcc->ewrk), tmpmfcc[para->baselen-1], para);
  }

  if (para->delta) {
    /* ¥Ç¥E¿¤ò·×»»¤¹¤E*/
    /* calc delta coefficients */
    ret = WMP_deltabuf_proceed(mfcc->db, tmpmfcc);
#ifdef RDEBUG
    printf("DeltaBuf: ret=%d, status=", ret);
    for(i=0;i<mfcc->db->len;i++) {
      printf("%d", mfcc->db->is_on[i]);
    }
    printf(", nextstore=%d\n", mfcc->db->store);
#endif
    /* ret == FALSE ¤Î¤È¤­¤Ï¤Ş¤À¥Ç¥£¥E¤Ãæ¤Ê¤Î¤ÇÇ§¼±½èÍı¤»¤º¼¡ÆşÎÏ¤Ø */
    /* if ret == FALSE, there is no available frame.  So just wait for
       next input */
    if (! ret) {
      return FALSE;
    }

    /* db->vec ¤Ë¸½ºß¤Î¸µ¥Ç¡¼¥¿¤È¥Ç¥E¿·¸¿ô¤¬Æş¤Ã¤Æ¤¤¤EÎ¤Ç tmpmfcc ¤Ë¥³¥Ô¡¼ */
    /* now db->vec holds the current base and full delta, so copy them to tmpmfcc */
    memcpy(tmpmfcc, mfcc->db->vec, sizeof(VECT) * para->baselen * 2);
  }

  if (para->acc) {
    /* Acceleration¤ò·×»»¤¹¤E*/
    /* calc acceleration coefficients */
    /* base+delta ¤ò¤½¤Î¤Ş¤ŞÆş¤EE*/
    /* send the whole base+delta to the cycle buffer */
    ret = WMP_deltabuf_proceed(mfcc->ab, tmpmfcc);
#ifdef RDEBUG
    printf("AccelBuf: ret=%d, status=", ret);
    for(i=0;i<mfcc->ab->len;i++) {
      printf("%d", mfcc->ab->is_on[i]);
    }
    printf(", nextstore=%d\n", mfcc->ab->store);
#endif
    /* ret == FALSE ¤Î¤È¤­¤Ï¤Ş¤À¥Ç¥£¥E¤Ãæ¤Ê¤Î¤ÇÇ§¼±½èÍı¤»¤º¼¡ÆşÎÏ¤Ø */
    /* if ret == FALSE, there is no available frame.  So just wait for
       next input */
    if (! ret) {
      return FALSE;
    }
    /* ab->vec ¤Ë¤Ï¡¤(base+delta) ¤È¤½¤Îº¹Ê¬·¸¿ô¤¬Æş¤Ã¤Æ¤¤¤E 
       [base] [delta] [delta] [acc] ¤Î½ç¤ÇÆş¤Ã¤Æ¤¤¤EÎ¤Ç,
       [base] [delta] [acc] ¤Etmpmfcc ¤Ë¥³¥Ô¡¼¤¹¤E */
    /* now ab->vec holds the current (base+delta) and their delta coef. 
       it holds a vector in the order of [base] [delta] [delta] [acc], 
       so copy the [base], [delta] and [acc] to tmpmfcc.  */
    memcpy(tmpmfcc, mfcc->ab->vec, sizeof(VECT) * para->baselen * 2);
    memcpy(&(tmpmfcc[para->baselen*2]), &(mfcc->ab->vec[para->baselen*3]), sizeof(VECT) * para->baselen);
  }

#ifdef POWER_REJECT
  if (para->energy || para->c0) {
    mfcc->avg_power += tmpmfcc[para->baselen-1];
  }
#endif

  if (para->delta && (para->energy || para->c0) && para->absesup) {
    /* ÀäÂĞÃÍ¥Ñ¥E¼¤ò½EE*/
    /* suppress absolute power */
    memmove(&(tmpmfcc[para->baselen-1]), &(tmpmfcc[para->baselen]), sizeof(VECT) * (para->vecbuflen - para->baselen));
  }

  /* ¤³¤Î»şÅÀ¤Ç tmpmfcc ¤Ë¸½»şÅÀ¤Ç¤ÎºÇ¿·¤ÎÆÃÄ§¥Ù¥¯¥È¥E¬³ÊÇ¼¤µ¤EÆ¤¤¤E*/
  /* tmpmfcc[] now holds the latest parameter vector */

  /* CMN ¤ò·×»» */
  /* perform CMN */
  if (para->cmn || para->cvn) CMN_realtime(mfcc->cmn.wrk, tmpmfcc);

  return TRUE;
}

static int
proceed_one_frame(Recog *recog)
{
  MFCCCalc *mfcc;
  RealBeam *r;
  int maxf;
  PROCESS_AM *am;
  int rewind_frame;
  boolean reprocess;
  boolean ok_p;

  r = &(recog->real);

  /* call recognition start callback */
  ok_p = FALSE;
  maxf = 0;
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (!mfcc->valid) continue;
    if (maxf < mfcc->f) maxf = mfcc->f;
    if (mfcc->f == 0) {
      ok_p = TRUE;
    }
  }
  if (ok_p && maxf == 0) {
    /* call callback when at least one of MFCC has initial frame */
    if (recog->jconf->decodeopt.segment) {
#ifdef BACKEND_VAD
      /* not exec pass1 begin callback here */
#else
      if (!recog->process_segment) {
//	callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
      }
//      callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
//      callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
      recog->triggered = TRUE;
#endif
    } else {
//      callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
//      callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
      recog->triggered = TRUE;
    }
  }
  /* ³Æ¥¤¥ó¥¹¥¿¥ó¥¹¤Ë¤Ä¤¤¤Æ mfcc->f ¤ÎÇ§¼±½èÍı¤E¥Õ¥E¼¥à¿Ê¤á¤E*/
  switch (decode_proceed(recog)) {
  case -1: /* error */
    return -1;
    break;
  case 0:			/* success */
    break;
  case 1:			/* segmented */
    /* Ç§¼±½èÍı¤Î¥»¥°¥á¥ó¥ÈÍ×µá¤Ç½ª¤EÃ¤¿¤³¤È¤ò¥Õ¥é¥°¤Ë¥»¥Ã¥È */
    /* set flag which indicates that the input has ended with segmentation request */
    r->last_is_segmented = TRUE;
    /* tell the caller to be segmented by this function */
    /* ¸Æ¤Ó½Ğ¤·¸µ¤Ë¡¤¤³¤³¤ÇÆşÎÏ¤òÀÚ¤Eè¤¦ÅÁ¤¨¤E*/
    return 1;
  }
#ifdef BACKEND_VAD
  /* check up trigger in case of VAD segmentation */
  if (recog->jconf->decodeopt.segment) {
    if (recog->triggered == FALSE) {
      if (spsegment_trigger_sync(recog)) {
	if (!recog->process_segment) {
//	  callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
	}
//	callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
//	callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	recog->triggered = TRUE;
      }
    }
  }
#endif
  
  if (spsegment_need_restart(recog, &rewind_frame, &reprocess) == TRUE) {
    /* set total length to the current frame */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      mfcc->param->header.samplenum = mfcc->f + 1;
      mfcc->param->samplenum = mfcc->f + 1;
    }
    /* do rewind for all mfcc here */
    spsegment_restart_mfccs(recog, rewind_frame, reprocess);
    /* also tell adin module to rehash the concurrent audio input */
    recog->adin->rehash = TRUE;
    /* reset outprob cache for all AM */
    for(am=recog->amlist;am;am=am->next) {
      outprob_prepare(&(am->hmmwrk), am->mfcc->param->samplenum);
    }
    if (reprocess) {
      /* process the backstep MFCCs here */
      while(1) {
	ok_p = TRUE;
	for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
	  if (! mfcc->valid) continue;
	  mfcc->f++;
	  if (mfcc->f < mfcc->param->samplenum) {
	    mfcc->valid = TRUE;
	    ok_p = FALSE;
	  } else {
	    mfcc->valid = FALSE;
	  }
	}
	if (ok_p) {
	  /* ¤¹¤Ù¤Æ¤Î MFCC ¤¬½ª¤Eê¤ËÃ£¤·¤¿¤Î¤Ç¥E¼¥×½ªÎ» */
	  /* all MFCC has been processed, end of loop  */
	  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
	    if (! mfcc->valid) continue;
	    mfcc->f--;
	  }
	  break;
	}
	/* ³Æ¥¤¥ó¥¹¥¿¥ó¥¹¤Ë¤Ä¤¤¤Æ mfcc->f ¤ÎÇ§¼±½èÍı¤E¥Õ¥E¼¥à¿Ê¤á¤E*/
	switch (decode_proceed(recog)) {
	case -1: /* error */
	  return -1;
	  break;
	case 0:			/* success */
	  break;
	case 1:			/* segmented */
	  /* ignore segmentation while in the backstep segment */
	  break;
	}
	/* call frame-wise callback */
//	callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);
      }
    }
  }
  /* call frame-wise callback if at least one of MFCC is valid at this frame */
//  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
//    if (mfcc->valid) {
//      callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);
//      break;
//    }
//  }
  
  return 0;
}


/** 
 * <JA>
 * @brief  ÂE¥Ñ¥¹Ê¿¹Ô²»À¼Ç§¼±½èÍı¤Î¥á¥¤¥E *
 * ¤³¤Î´Ø¿ôÆâ¤Ç¤Ï¡¤Á²¼¡Åª¤ÊÆÃÄ§ÎÌÃEĞ¤ª¤è¤ÓÂE¥Ñ¥¹¤ÎÇ§¼±¤¬¹Ô¤EEE 
 * ÆşÎÏ¥Ç¡¼¥¿¤ËÂĞ¤·¤ÆÁEİ¤±¡¦¥·¥Õ¥È¤ò¹Ô¤¤MFCC·×»»¤ò¹Ô¤¤¤Ê¤¬¤é¡¤
 * ²»À¼Ç§¼±¤E¥Õ¥E¼¥à¤º¤ÄÊÂÎó¼Â¹Ô¤¹¤E 
 *
 * Ç§¼±½èÍı¡Êdecode_proceed()¡Ë¤Ë¤ª¤¤¤Æ¡¤²»À¼¶è´Ö½ªÎ»¤¬Í×µá¤µ¤EE * ¤³¤È¤¬¤¢¤E ¤³¤Î¾Eç¡¤Ì¤½èÍı¤Î²»À¼¤òÊİÂ¸¤·¤ÆÂE¥Ñ¥¹¤ò½ªÎ»¤¹¤E * ¤è¤¦¸Æ½Ğ¸µ¤ËÍ×µá¤¹¤E 
 *
 * SPSEGMENT_NAIST ¤¢¤E¤¤Ï GMM_VAD ¤Ê¤É¤Î¥Ğ¥Ã¥¯¥¨¥ó¥ÉVADÄEÁ»ş¤Ï¡¤¥Ç¥³¡¼¥À¥Ù¡¼¥¹¤Î
 * VAD ¡Ê²»À¼¶è´Ö³«»Ï¸¡½Ğ¡Ë¤ËÈ¼¤¦¥Ç¥³¡¼¥Ç¥£¥ó¥°À©¸æ¤¬¹Ô¤EEE 
 * ¥È¥E¬Á°¤Ï¡¤Ç§¼±½èÍı¤¬¸Æ¤Ğ¤EE¬¡¤¼Âºİ¤Ë¤Ï³Æ´Ø¿ôÆâ¤ÇÇ§¼±½èÍı¤Ï
 * ¹Ô¤EEÆ¤¤¤Ê¤¤. ³«»Ï¤ò¸¡½Ğ¤·¤¿»ş¡¤¤³¤Î´Ø¿ô¤Ï¤½¤³¤Ş¤Ç¤ËÆÀ¤é¤E¿
 * MFCCÎó¤ò°EEÕ¥E¼¥àÄ¹Ê¬´¬Ìá¤·¡¤¤½¤Î´¬Ìá¤·Àè¤«¤éÄÌ¾EÎÇ§¼±½èÍı¤E * ºÆ³«¤¹¤E ¤Ê¤ª¡¤Ê£¿ô½èÍı¥¤¥ó¥¹¥¿¥ó¥¹´Ö¤¬¤¢¤EEç¡¤³«»Ï¥È¥E¬¤Ï
 * ¤É¤E«¤Î¥¤¥ó¥¹¥¿¥ó¥¹¤¬¸¡½Ğ¤·¤¿»şÅÀ¤ÇÁ´¤Æ¤Î³«»Ï¤¬Æ±´Eµ¤EE 
 * 
 * ¤³¤Î´Ø¿ô¤Ï¡¤²»À¼ÆşÎÏ¥E¼¥Á¥ó¤Î¥³¡¼¥EĞ¥Ã¥¯¤È¤·¤Æ¸Æ¤Ğ¤EE
 * ²»À¼¥Ç¡¼¥¿¤Î¿ôÀé¥µ¥ó¥×¥E¿²»¤´¤È¤Ë¤³¤Î´Ø¿ô¤¬¸Æ¤Ó½Ğ¤µ¤EE 
 * 
 * @param Speech [in] ²»À¼¥Ç¡¼¥¿¤Ø¤Î¥Ğ¥Ã¥Õ¥¡¤Ø¤Î¥İ¥¤¥ó¥¿
 * @param nowlen [in] ²»À¼¥Ç¡¼¥¿¤ÎÄ¹¤µ
 * @param recog [i/o] engine instance
 * 
 * @return ¥¨¥é¡¼»ş¤Ë -1 ¤ò¡¤Àµ¾Eş¤Ë 0 ¤òÊÖ¤¹. ¤Ş¤¿¡¤ÂE¥Ñ¥¹¤E * ½ªÎ»¤¹¤Eè¤¦¸Æ½Ğ¸µ¤ËÍ×µá¤¹¤EÈ¤­¤Ï 1 ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * @brief  Main function of the on-the-fly 1st pass decoding
 *
 * This function performs sucessive MFCC calculation and 1st pass decoding.
 * The given input data are windowed to a certain length, then converted
 * to MFCC, and decoding for the input frame will be performed in one
 * process cycle.  The loop cycle will continue with window shift, until
 * the whole given input has been processed.
 *
 * In case of input segment request from decoding process (in
 * decode_proceed()), this function keeps the rest un-processed speech
 * to a buffer and tell the caller to stop input and end the 1st pass.
 *
 * When back-end VAD such as SPSEGMENT_NAIST or GMM_VAD is defined,  Decoder-based
 * VAD is enabled and its decoding control will be managed here.
 * In decoder-based VAD mode, the recognition will be processed but
 * no output will be done at the first un-triggering input area.
 * when speech input start is detected, this function will rewind the
 * already obtained MFCC sequence to a certain frames, and re-start
 * normal recognition at that point.  When multiple recognition process
 * instance is running, their segmentation will be synchronized.
 * 
 * This function will be called each time a new speech sample comes as
 * as callback from A/D-in routine.
 * 
 * @param Speech [in] pointer to the speech sample segments
 * @param nowlen [in] length of above
 * @param recog [i/o] engine instance
 * 
 * @return -1 on error (will close stream and terminate recognition),
 * 0 on success (allow caller to call me for the next segment).  It
 * returns 1 when telling the caller to segment now at the middle of
 * input , and 2 when input length overflow is detected.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
int
RealTimePipeLine(SP16 *Speech, int nowlen, Recog *recog) /* Speech[0...nowlen] = input */
{
  int i, now, ret;
  MFCCCalc *mfcc;
  RealBeam *r;

  r = &(recog->real);

#ifdef DEBUG_VTLN_ALPHA_TEST
  /* store speech */
  adin_cut_callback_store_buffer(Speech, nowlen, recog);
#endif

  /* window[0..windownum-1] ¤ÏÁ°²ó¤Î¸Æ¤Ó½Ğ¤·¤Ç»Ä¤Ã¤¿²»À¼¥Ç¡¼¥¿¤¬³ÊÇ¼¤µ¤EÆ¤¤¤E*/
  /* window[0..windownum-1] are speech data left from previous call */

  /* ½èÍıÍÑ¥İ¥¤¥ó¥¿¤ò½é´E½ */
  /* initialize pointer for local processing */
  now = 0;
  
  /* Ç§¼±½èÍı¤¬¥»¥°¥á¥ó¥ÈÍ×µá¤Ç½ª¤EÃ¤¿¤Î¤«¤É¤¦¤«¤Î¥Õ¥é¥°¤ò¥E»¥Ã¥È */
  /* reset flag which indicates whether the input has ended with segmentation request */
  r->last_is_segmented = FALSE;

#ifdef RDEBUG
  printf("got %d samples\n", nowlen);
#endif

  while (now < nowlen) {	/* till whole input is processed */
    /* ÆşÎÏÄ¹¤¬ maxframelen ¤ËÃ£¤·¤¿¤é¤³¤³¤Ç¶¯À©½ªÎ» */
    /* if input length reaches maximum buffer size, terminate 1st pass here */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (mfcc->f >= r->maxframelen) return(1);
    }
    /* ÁEĞ¥Ã¥Õ¥¡¤òËä¤á¤é¤EEÀ¤±Ëä¤á¤E*/
    /* fill window buffer as many as possible */
    for(i = min(r->windowlen - r->windownum, nowlen - now); i > 0 ; i--)
      r->window[r->windownum++] = (float) Speech[now++];
    /* ¤â¤·ÁEĞ¥Ã¥Õ¥¡¤¬Ëä¤Ş¤é¤Ê¤±¤EĞ, ¤³¤Î¥»¥°¥á¥ó¥È¤Î½èÍı¤Ï¤³¤³¤Ç½ª¤EE 
       ½èÍı¤µ¤EÊ¤«¤Ã¤¿¥µ¥ó¥×¥E(window[0..windownum-1]) ¤Ï¼¡²ó¤Ë»ı¤Á±Û¤·. */
    /* if window buffer was not filled, end processing here, keeping the
       rest samples (window[0..windownum-1]) in the window buffer. */
    if (r->windownum < r->windowlen) break;
#ifdef RDEBUG
    /*    printf("%d used, %d rest\n", now, nowlen - now);

	  printf("[f = %d]\n", f);*/
#endif

    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->valid = FALSE;
      /* ÁEâ¤Î²»À¼ÇÈ·Á¤«¤éÆÃÄ§ÎÌ¤ò·×»»¤·¤Æ r->tmpmfcc ¤Ë³ÊÇ¼  */
      /* calculate a parameter vector from current waveform windows
	 and store to r->tmpmfcc */
      if ((*(recog->calc_vector))(mfcc, r->window, r->windowlen)) {
#ifdef ENABLE_PLUGIN
	/* call post-process plugin if exist */
	plugin_exec_vector_postprocess(mfcc->tmpmfcc, mfcc->param->veclen, mfcc->f);
#endif
	/* MFCC´°À®¡¤ÅĞÏ¿ */
  	mfcc->valid = TRUE;
	/* now get the MFCC vector of current frame, now store it to param */
	if (param_alloc(mfcc->param, mfcc->f + 1, mfcc->param->veclen) == FALSE) {
	  jlog("ERROR: failed to allocate memory for incoming MFCC vectors\n");
	  return -1;
	}
	memcpy(mfcc->param->parvec[mfcc->f], mfcc->tmpmfcc, sizeof(VECT) * mfcc->param->veclen);
#ifdef RDEBUG
	printf("DeltaBuf: %02d: got frame %d\n", mfcc->id, mfcc->f);
#endif
      }
    }

    /* ½èÍı¤E¥Õ¥E¼¥à¿Ê¤á¤E*/
    /* proceed one frame */
    ret = proceed_one_frame(recog);

    if (ret == 1 && recog->jconf->decodeopt.segment) {
      /* ¥·¥ç¡¼¥È¥İ¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥E ¥Ğ¥Ã¥Õ¥¡¤Ë»Ä¤Ã¤Æ¤¤¤EÇ¡¼¥¿¤E	 ÊÌ¤ËÊİ»ı¤·¤Æ¡¤¼¡²ó¤ÎºÇ½é¤Ë½èÍı¤¹¤E*/
      /* short pause segmentation: there is some data left in buffer, so
	 we should keep them for next processing */
      r->rest_len = nowlen - now;
      if (r->rest_len > 0) {
	/* copy rest samples to rest_Speech */
	if (r->rest_Speech == NULL) {
	  r->rest_alloc_len = r->rest_len;
	  r->rest_Speech = (SP16 *)mymalloc(sizeof(SP16)*r->rest_alloc_len);
	} else if (r->rest_alloc_len < r->rest_len) {
	  r->rest_alloc_len = r->rest_len;
	  r->rest_Speech = (SP16 *)myrealloc(r->rest_Speech, sizeof(SP16)*r->rest_alloc_len);
	}
	memcpy(r->rest_Speech, &(Speech[now]), sizeof(SP16) * r->rest_len);
      }
    }
    if (ret != 0) return ret;

    /* 1¥Õ¥E¼¥à½èÍı¤¬¿Ê¤ó¤À¤Î¤Ç¥İ¥¤¥ó¥¿¤ò¿Ê¤á¤E*/
    /* proceed frame pointer */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      mfcc->f++;
    }

    /* ÁEĞ¥Ã¥Õ¥¡¤ò½èÍı¤¬½ª¤EÃ¤¿Ê¬¥·¥Õ¥È */
    /* shift window */
    memmove(r->window, &(r->window[recog->jconf->input.frameshift]), sizeof(SP16) * (r->windowlen - recog->jconf->input.frameshift));
    r->windownum -= recog->jconf->input.frameshift;
  }

  /* Í¿¤¨¤é¤E¿²»À¼¥»¥°¥á¥ó¥È¤ËÂĞ¤¹¤E§¼±½èÍı¤¬Á´¤Æ½ªÎ»
     ¸Æ¤Ó½Ğ¤·¸µ¤Ë, ÆşÎÏ¤òÂ³¤±¤Eè¤¦ÅÁ¤¨¤E*/
  /* input segment is fully processed
     tell the caller to continue input */
  return(0);			
}

/** 
 * <JA>
 * @brief  ¥»¥°¥á¥ó¥È¤ÎÇ§¼±ºÆ³«½èÍı
 *
 * ¤³¤Î´Ø¿ô¤Ï¥Ç¥³¡¼¥À¥Ù¡¼¥¹VAD¤ä¥·¥ç¡¼¥È¥İ¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó¤Ë¤è¤Ã¤Æ
 * ÆşÎÏ¤¬¥»¥°¥á¥ó¥È¤ËÀÚ¤é¤E¿¾Eç¤Ë¡¤¤½¤Î¸å¤ÎÇ§¼±¤ÎºÆ³«¤Ë´Ø¤¹¤EèÍı¤ò¹Ô¤¦. 
 * ¶ñÂÎÅª¤Ë¤Ï¡¤ÆşÎÏ¤ÎÇ§¼±¤ò³«»Ï¤¹¤E°¤Ë¡¤Á°²ó¤ÎÆşÎÏ¥»¥°¥á¥ó¥È¤Ë¤ª¤±¤E * ´¬Ìá¤·Ê¬¤ÎMFCCÎó¤«¤éÇ§¼±¤ò³«»Ï¤¹¤E ¤µ¤é¤Ë¡¤Á°²ó¤Î¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó»ş¤Ë
 * Ì¤½èÍı¤À¤Ã¤¿»Ä¤ê¤Î²»À¼¥µ¥ó¥×¥E¬¤¢¤EĞ¤½¤Eâ½èÍı¤¹¤E
 *
 * @param recog [i/o] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
 * 
 * @return ¥¨¥é¡¼»ş -1¡¤Àµ¾Eş 0 ¤òÊÖ¤¹. ¤Ş¤¿¡¤¤³¤ÎÆşÎÏÃÇÊÒ¤Î½èÍıÃæ¤Ë
 * Ê¸¾Ï¤Î¶èÀÚ¤ê¤¬¸«¤Ä¤«¤Ã¤¿¤È¤­¤ÏÂE¥Ñ¥¹¤ò¤³¤³¤ÇÃæÃÇ¤¹¤E¿¤á¤Ë 1 ¤òÊÖ¤¹. 
 * </JA>
 * </JA>
 * <EN>
 * @brief  Resuming recognition for short pause segmentation.
 *
 * This function process overlapped data and remaining speech prior
 * to the next input when input was segmented at last processing.
 *
 * @param recog [i/o] engine instance
 *
 * @return -1 on error (tell caller to terminate), 0 on success (allow caller
 * to call me for the next segment), or 1 when an end-of-sentence detected
 * at this point (in that case caller will stop input and go to 2nd pass)
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
int
RealTimeResume(Recog *recog)
{
  MFCCCalc *mfcc;
  RealBeam *r;
  boolean ok_p;
#ifdef SPSEGMENT_NAIST
  RecogProcess *p;
#endif
  PROCESS_AM *am;

  r = &(recog->real);

  /* ·×»»ÍÑ¤Î¥E¼¥¯¥¨¥E¢¤ò½àÈE*/
  /* prepare work area for calculation */
  if (recog->jconf->input.type == INPUT_WAVEFORM) {
    reset_mfcc(recog);
  }
  /* ²»¶ÁÌàÅÙ·×»»ÍÑ¥­¥ã¥Ã¥·¥å¤ò½àÈE*/
  /* prepare cache area for acoustic computation of HMM states and mixtures */
  for(am=recog->amlist;am;am=am->next) {
    outprob_prepare(&(am->hmmwrk), r->maxframelen);
  }

  /* param ¤Ë¤¢¤E´¥Ñ¥é¥á¡¼¥¿¤ò½èÍı¤¹¤EàÈE*/
  /* prepare to process all data in param */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->param->samplenum == 0) mfcc->valid = FALSE;
    else mfcc->valid = TRUE;
#ifdef RDEBUG
    printf("Resume: %02d: f=%d\n", mfcc->id, mfcc->mfcc->param->samplenum-1);
#endif
    /* ¥Õ¥E¼¥à¿ô¤ò¥E»¥Ã¥È */
    /* reset frame count */
    mfcc->f = 0;
    /* MAP-CMN ¤Î½é´E½ */
    /* Prepare for MAP-CMN */
    if (mfcc->para->cmn || mfcc->para->cvn) CMN_realtime_prepare(mfcc->cmn.wrk);
  }

#ifdef BACKEND_VAD
  if (recog->jconf->decodeopt.segment) {
    spsegment_init(recog);
  }
  /* not exec pass1 begin callback here */
#else
  recog->triggered = FALSE;
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (!mfcc->valid) continue;
//    callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
//    callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
    recog->triggered = TRUE;
    break;
  }
#endif

  /* param Æâ¤ÎÁ´¥Õ¥E¼¥à¤Ë¤Ä¤¤¤ÆÇ§¼±½èÍı¤ò¿Ê¤á¤E*/
  /* proceed recognition for all frames in param */

  while(1) {
    ok_p = TRUE;
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (! mfcc->valid) continue;
      if (mfcc->f < mfcc->param->samplenum) {
	mfcc->valid = TRUE;
	ok_p = FALSE;
      } else {
	mfcc->valid = FALSE;
      }
    }
    if (ok_p) {
      /* ¤¹¤Ù¤Æ¤Î MFCC ¤¬½ª¤Eê¤ËÃ£¤·¤¿¤Î¤Ç¥E¼¥×½ªÎ» */
      /* all MFCC has been processed, end of loop  */
      break;
    }

    /* ³Æ¥¤¥ó¥¹¥¿¥ó¥¹¤Ë¤Ä¤¤¤Æ mfcc->f ¤ÎÇ§¼±½èÍı¤E¥Õ¥E¼¥à¿Ê¤á¤E*/
    switch (decode_proceed(recog)) {
    case -1: /* error */
      return -1;
      break;
    case 0:			/* success */
      break;
    case 1:			/* segmented */
      /* segmented, end procs ([0..f])*/
      r->last_is_segmented = TRUE;
      return 1;		/* segmented by this function */
    }

#ifdef BACKEND_VAD
    /* check up trigger in case of VAD segmentation */
    if (recog->jconf->decodeopt.segment) {
      if (recog->triggered == FALSE) {
	if (spsegment_trigger_sync(recog)) {
//	  callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
//	  callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	  recog->triggered = TRUE;
	}
      }
    }
#endif

    /* call frame-wise callback */
//    callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);

    /* 1¥Õ¥E¼¥à½èÍı¤¬¿Ê¤ó¤À¤Î¤Ç¥İ¥¤¥ó¥¿¤ò¿Ê¤á¤E*/
    /* proceed frame pointer */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      mfcc->f++;
    }

  }
  /* Á°²ó¤Î¥»¥°¥á¥ó¥È»ş¤ËÆşÎÏ¤ò¥·¥Õ¥È¤·¤Æ¤¤¤Ê¤¤Ê¬¤ò¥·¥Õ¥È¤¹¤E*/
  /* do the last shift here */
  if (recog->jconf->input.type == INPUT_WAVEFORM) {
    memmove(r->window, &(r->window[recog->jconf->input.frameshift]), sizeof(SP16) * (r->windowlen - recog->jconf->input.frameshift));
    r->windownum -= recog->jconf->input.frameshift;
    /* ¤³¤EÇºÆ³«¤Î½àÈ÷¤¬À°¤Ã¤¿¤Î¤Ç,¤Ş¤º¤ÏÁ°²ó¤Î½èÍı¤Ç»Ä¤Ã¤Æ¤¤¤¿²»À¼¥Ç¡¼¥¿¤«¤E       ½èÍı¤¹¤E*/
    /* now that the search status has been prepared for the next input, we
       first process the rest unprocessed samples at the last session */
    if (r->rest_len > 0) {
      return(RealTimePipeLine(r->rest_Speech, r->rest_len, recog));
    }
  }

  /* ¿·µ¬¤ÎÆşÎÏ¤ËÂĞ¤·¤ÆÇ§¼±½èÍı¤ÏÂ³¤¯¡Ä */
  /* the recognition process will continue for the newly incoming samples... */
  return 0;

}


/** 
 * <JA>
 * @brief  ÂE¥Ñ¥¹Ê¿¹ÔÇ§¼±½èÍı¤Î½ªÎ»½èÍı¤ò¹Ô¤¦.
 *
 * ¤³¤Î´Ø¿ô¤ÏÂE¥Ñ¥¹½ªÎ»»ş¤Ë¸Æ¤Ğ¤E¤ÆşÎÏÄ¹¤ò³ÎÄê¤·¤¿¤¢¤È¡¤
 * decode_end() ¡Ê¥»¥°¥á¥ó¥È¤Ç½ªÎ»¤·¤¿¤È¤­¤Ï decode_end_segmented()¡Ë¤E * ¸Æ¤Ó½Ğ¤·¤ÆÂE¥Ñ¥¹½ªÎ»½èÍı¤ò¹Ô¤¦. 
 *
 * ¤â¤·²»À¼ÆşÎÏ¥¹¥È¥ê¡¼¥à¤Î½ªÎ»¤Ë¤è¤Ã¤ÆÇ§¼±¤¬½ª¤EÃ¤¿¾Eç¡Ê¥Õ¥¡¥¤¥EşÎÏ¤Ç
 * ½ªÃ¼¤ËÃ£¤·¤¿¾Eç¤Ê¤É¡Ë¤Ï¡¤¥Ç¥E¿¥Ğ¥Ã¥Õ¥¡¤ËÌ¤½èÍı¤ÎÆşÎÏ¤¬»Ä¤Ã¤Æ¤¤¤EÎ¤Ç¡¤
 * ¤½¤Eò¤³¤³¤Ç½èÍı¤¹¤E 
 *
 * @param recog [i/o] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
 * 
 * @return ½èÍıÀ®¸ù»ş TRUE, ¥¨¥é¡¼»ş FALSE ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * @brief  Finalize the 1st pass on-the-fly decoding.
 *
 * This function will be called after the 1st pass processing ends.
 * It fix the input length of parameter vector sequence, call
 * decode_end() (or decode_end_segmented() when last input was ended
 * by segmentation) to finalize the 1st pass.
 *
 * If the last input was ended by end-of-stream (in case input reached
 * EOF in file input etc.), process the rest samples remaining in the
 * delta buffers.
 *
 * @param recog [i/o] engine instance
 * 
 * @return TRUE on success, or FALSE on error.
 * </EN>
 */
boolean
RealTimeParam(Recog *recog)
{
  boolean ret1, ret2;
  RealBeam *r;
  int ret;
  int maxf;
  boolean ok_p;
  MFCCCalc *mfcc;
  Value *para;
#ifdef RDEBUG
  int i;
#endif

  r = &(recog->real);

  if (r->last_is_segmented) {

    /* RealTimePipeLine ¤ÇÇ§¼±½èÍıÂ¦¤ÎÍıÍ³¤Ë¤è¤E§¼±¤¬ÃæÃÇ¤·¤¿¾EE
       ¸½¾õÂÖ¤ÎMFCC·×»»¥Ç¡¼¥¿¤ò¤½¤Î¤Ş¤Ş¼¡²ó¤ØÊİ»ı¤¹¤E¬Í×¤¬¤¢¤EÎ¤Ç,
       MFCC·×»»½ªÎ»½èÍı¤ò¹Ô¤Eº¤ËÂè£±¥Ñ¥¹¤Î·EÌ¤Î¤ß½ĞÎÏ¤·¤Æ½ª¤EE */
    /* When input segmented by recognition process in RealTimePipeLine(),
       we have to keep the whole current status of MFCC computation to the
       next call.  So here we only output the 1st pass result. */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->param->header.samplenum = mfcc->f + 1;/* len = lastid + 1 */
      mfcc->param->samplenum = mfcc->f + 1;
    }
    decode_end_segmented(recog);

    /* ¤³¤Î¶è´Ö¤Î param ¥Ç¡¼¥¿¤òÂè£²¥Ñ¥¹¤Î¤¿¤á¤ËÊÖ¤¹ */
    /* return obtained parameter for 2nd pass */
    return(TRUE);
  }

  if (recog->jconf->input.type == INPUT_VECTOR) {
    /* finalize real-time 1st pass */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->param->header.samplenum = mfcc->f;
      mfcc->param->samplenum = mfcc->f;
    }
    /* ºÇ½ª¥Õ¥E¼¥à½èÍı¤ò¹Ô¤¤¡¤Ç§¼±¤Î·EÌ½ĞÎÏ¤È½ªÎ»½èÍı¤ò¹Ô¤¦ */
    decode_end(recog);
    return TRUE;
  }

  /* MFCC·×»»¤Î½ªÎ»½èÍı¤ò¹Ô¤¦: ºÇ¸å¤ÎÃÙ±ä¥Õ¥E¼¥àÊ¬¤ò½èÍı */
  /* finish MFCC computation for the last delayed frames */
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->para->delta || mfcc->para->acc) {
      mfcc->valid = TRUE;
    } else {
      mfcc->valid = FALSE;
    }
  }

  /* loop until all data has been flushed */
  while (1) {

    /* if all mfcc became invalid, exit loop here */
    ok_p = FALSE;
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (mfcc->valid) {
	ok_p = TRUE;
	break;
      }
    }
    if (!ok_p) break;

    /* try to get 1 frame for all mfcc instances */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      
      para = mfcc->para;
      
      if (! mfcc->valid) continue;
      
      /* check if there is data in cycle buffer of delta */
      ret1 = WMP_deltabuf_flush(mfcc->db);
#ifdef RDEBUG
      printf("DeltaBufLast: ret=%d, status=", ret1);
      for(i=0;i<mfcc->db->len;i++) {
	printf("%d", mfcc->db->is_on[i]);
      }
      printf(", nextstore=%d\n", mfcc->db->store);
#endif
      if (ret1) {
	/* uncomputed delta has flushed, compute it with tmpmfcc */
	if (para->energy && para->absesup) {
	  memcpy(mfcc->tmpmfcc, mfcc->db->vec, sizeof(VECT) * (para->baselen - 1));
	  memcpy(&(mfcc->tmpmfcc[para->baselen-1]), &(mfcc->db->vec[para->baselen]), sizeof(VECT) * para->baselen);
	} else {
	  memcpy(mfcc->tmpmfcc, mfcc->db->vec, sizeof(VECT) * para->baselen * 2);
	}
	if (para->acc) {
	  /* this new delta should be given to the accel cycle buffer */
	  ret2 = WMP_deltabuf_proceed(mfcc->ab, mfcc->tmpmfcc);
#ifdef RDEBUG
	  printf("AccelBuf: ret=%d, status=", ret2);
	  for(i=0;i<mfcc->ab->len;i++) {
	    printf("%d", mfcc->ab->is_on[i]);
	  }
	  printf(", nextstore=%d\n", mfcc->ab->store);
#endif
	  if (ret2) {
	    /* uncomputed accel was given, compute it with tmpmfcc */
	    memcpy(mfcc->tmpmfcc, mfcc->ab->vec, sizeof(VECT) * (para->veclen - para->baselen));
	    memcpy(&(mfcc->tmpmfcc[para->veclen - para->baselen]), &(mfcc->ab->vec[para->veclen - para->baselen]), sizeof(VECT) * para->baselen);
	  } else {
	    /* still no input is given: */
	    /* in case of very short input: go on to the next input */
	    continue;
	  }
	}
	
      } else {
      
	/* no data left in the delta buffer */
	if (para->acc) {
	  /* no new data, just flush the accel buffer */
	  ret2 = WMP_deltabuf_flush(mfcc->ab);
#ifdef RDEBUG
	  printf("AccelBuf: ret=%d, status=", ret2);
	  for(i=0;i<mfcc->ab->len;i++) {
	    printf("%d", mfcc->ab->is_on[i]);
	  }
	  printf(", nextstore=%d\n", mfcc->ab->store);
#endif
	  if (ret2) {
	    /* uncomputed data has flushed, compute it with tmpmfcc */
	    memcpy(mfcc->tmpmfcc, mfcc->ab->vec, sizeof(VECT) * (para->veclen - para->baselen));
	    memcpy(&(mfcc->tmpmfcc[para->veclen - para->baselen]), &(mfcc->ab->vec[para->veclen - para->baselen]), sizeof(VECT) * para->baselen);
	  } else {
	    /* actually no data exists in both delta and accel */
	    mfcc->valid = FALSE; /* disactivate this instance */
	    continue;		/* end this loop */
	  }
	} else {
	  /* only delta: input fully flushed */
	  mfcc->valid = FALSE; /* disactivate this instance */
	  continue;		/* end this loop */
	}
      }
      /* a new frame has been obtained from delta buffer to tmpmfcc */
      if(para->cmn || para->cvn) CMN_realtime(mfcc->cmn.wrk, mfcc->tmpmfcc);
      if (param_alloc(mfcc->param, mfcc->f + 1, mfcc->param->veclen) == FALSE) {
	jlog("ERROR: failed to allocate memory for incoming MFCC vectors\n");
	return FALSE;
      }
      /* store to mfcc->f */
      memcpy(mfcc->param->parvec[mfcc->f], mfcc->tmpmfcc, sizeof(VECT) * mfcc->param->veclen);
#ifdef ENABLE_PLUGIN
      /* call postprocess plugin if any */
      plugin_exec_vector_postprocess(mfcc->param->parvec[mfcc->f], mfcc->param->veclen, mfcc->f);
#endif
    }

    /* call recognition start callback */
    ok_p = FALSE;
    maxf = 0;
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      if (maxf < mfcc->f) maxf = mfcc->f;
      if (mfcc->f == 0) {
	ok_p = TRUE;
      }
    }

    if (ok_p && maxf == 0) {
      /* call callback when at least one of MFCC has initial frame */
      if (recog->jconf->decodeopt.segment) {
#ifdef BACKEND_VAD
	  /* not exec pass1 begin callback here */
#else
//	if (!recog->process_segment) {
//	  callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
//	}
//	callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
//	callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	recog->triggered = TRUE;
#endif
      } else {
//	callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
//	callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	recog->triggered = TRUE;
      }
    }

    /* proceed for the curent frame */
    ret = decode_proceed(recog);
    if (ret == -1) {		/* error */
      return -1;
    } else if (ret == 1) {	/* segmented */
      /* loop out */
      break;
    } /* else no event occured */

#ifdef BACKEND_VAD
    /* check up trigger in case of VAD segmentation */
    if (recog->jconf->decodeopt.segment) {
      if (recog->triggered == FALSE) {
	if (spsegment_trigger_sync(recog)) {
	  if (!recog->process_segment) {
//	    callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
	  }
//	  callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
//	  callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	  recog->triggered = TRUE;
	}
      }
    }
#endif

    /* call frame-wise callback */
 //   callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);

    /* move to next */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (! mfcc->valid) continue;
      mfcc->f++;
      if (mfcc->f > r->maxframelen) mfcc->valid = FALSE;
    }
  }

  /* finalize real-time 1st pass */
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    mfcc->param->header.samplenum = mfcc->f;
    mfcc->param->samplenum = mfcc->f;
  }
  /* ºÇ½ª¥Õ¥E¼¥à½èÍı¤ò¹Ô¤¤¡¤Ç§¼±¤Î·EÌ½ĞÎÏ¤È½ªÎ»½èÍı¤ò¹Ô¤¦ */
  decode_end(recog);

  return(TRUE);
}

/** 
 * <JA>
 * ¥±¥×¥¹¥È¥é¥àÊ¿¶Ñ¤Î¹¹¿·. 
 * ¼¡²ó¤ÎÇ§¼±¤ËÈ÷¤¨¤Æ¡¤ÆşÎÏ¥Ç¡¼¥¿¤«¤éCMNÍÑ¤Î¥±¥×¥¹¥È¥é¥àÊ¿¶Ñ¤ò¹¹¿·¤¹¤E 
 * 
 * @param mfcc [i/o] ·×»»ÂĞ¾İ¤Î MFCC·×»»¥¤¥ó¥¹¥¿¥ó¥¹
 * @param recog [i/o] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
 *
 * </JA>
 * <EN>
 * Update cepstral mean.
 *
 * This function updates the initial cepstral mean for CMN of the next input.
 *
 * @param mfcc [i/o] MFCC Calculation instance to update its CMN
 * @param recog [i/o] engine instance
 * </EN>
 */
void
RealTimeCMNUpdate(MFCCCalc *mfcc, Recog *recog)
{
  boolean cmn_update_p;
  Value *para;
  Jconf *jconf;
  RecogProcess *r;

  jconf = recog->jconf;
  para = mfcc->para;
  
  /* update CMN vector for next speech */
  if(para->cmn) {
    if (mfcc->cmn.update) {
      cmn_update_p = TRUE;
      for(r=recog->process_list;r;r=r->next) {
	if (!r->live) continue;
	if (r->am->mfcc != mfcc) continue;
	if (r->result.status < 0) { /* input rejected */
	  cmn_update_p = FALSE;
	  break;
	}
      }
      if (cmn_update_p) {
	/* update last CMN parameter for next spech */
	CMN_realtime_update(mfcc->cmn.wrk, mfcc->param);
      } else {
	/* do not update, because the last input is bogus */
	if (verbose_flag) {
#ifdef BACKEND_VAD
	  if (!recog->jconf->decodeopt.segment || recog->triggered) {
	    jlog("STAT: skip CMN parameter update since last input was invalid\n");
	  }
#else
	  jlog("STAT: skip CMN parameter update since last input was invalid\n");
#endif
	}
      }
    }
    /* if needed, save the updated CMN parameter to a file */
    if (mfcc->cmn.save_filename) {
      if (CMN_save_to_file(mfcc->cmn.wrk, mfcc->cmn.save_filename) == FALSE) {
	jlog("WARNING: failed to save CMN parameter to \"%s\"\n", mfcc->cmn.save_filename);
      }
    }
  }
}

/** 
 * <JA>
 * ÂE¥Ñ¥¹Ê¿¹ÔÇ§¼±½èÍı¤òÃæÃÇ¤¹¤E 
 *
 * @param recog [i/o] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
 * </JA>
 * <EN>
 * Terminate the 1st pass on-the-fly decoding.
 *
 * @param recog [i/o] engine instance
 * </EN>
 */
void
RealTimeTerminate(Recog *recog)
{
  MFCCCalc *mfcc;

  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    mfcc->param->header.samplenum = mfcc->f;
    mfcc->param->samplenum = mfcc->f;
  }

  /* ºÇ½ª¥Õ¥E¼¥à½èÍı¤ò¹Ô¤¤¡¤Ç§¼±¤Î·EÌ½ĞÎÏ¤È½ªÎ»½èÍı¤ò¹Ô¤¦ */
  decode_end(recog);
}

/** 
 * <EN>
 * Free the whole work area for 1st pass on-the-fly decoding
 * </EN>
 * <JA>
 * ÂE¥Ñ¥¹ÊÂ¹Ô½èÍı¤Î¤¿¤á¤Î¥E¼¥¯¥¨¥E¢¤ò³«ÊE¹¤E * </JA>
 * 
 * @param recog [in] engine instance
 * 
 */
void
realbeam_free(Recog *recog)
{
  RealBeam *r;

  r = &(recog->real);

  if (recog->real.window) {
    free(recog->real.window);
    recog->real.window = NULL;
  }
  if (recog->real.rest_Speech) {
    free(recog->real.rest_Speech);
    recog->real.rest_Speech = NULL;
  }
}



/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

/* MFCC realtime input */
/** 
 * <EN>
 * 
 * </EN>
 * <JA>
 * 
 * </JA>
 * 
 * @param recog 
 * @param ad_check 
 * 
 * @return 2 when input termination requested by recognition process,
 * 1 when segmentation request returned from input module, 0 when end
 * of input returned from input module, -1 on error, -2 when input
 * termination requested by ad_check().
 * 
 */
int
mfcc_go(Recog *recog, int (*ad_check)(Recog *))
{
  RealBeam *r;
  MFCCCalc *mfcc;
  int new_f;
  int ret, ret3;

  r = &(recog->real);

  r->last_is_segmented = FALSE;
  
  while(1/*in_data_vec*/) {

    ret = mfc_module_read(recog->mfcclist, &new_f);

    if (debug2_flag) {
      if (recog->mfcclist->f < new_f) {
	jlog("%d: %d (%d)\n", recog->mfcclist->f, new_f, ret);
      }
    }
 
    /* callback poll */
    if (ad_check != NULL) {
      if ((ret3 = (*(ad_check))(recog)) < 0) {
	if ((ret3 == -1 && recog->mfcclist->f == 0) || ret3 == -2) {
	  return(-2);
	}
      }
    }

    while(recog->mfcclist->f < new_f) {

      recog->mfcclist->valid = TRUE;

#ifdef ENABLE_PLUGIN
      /* call post-process plugin if exist */
      plugin_exec_vector_postprocess(recog->mfcclist->param->parvec[recog->mfcclist->f], recog->mfcclist->param->veclen, recog->mfcclist->f);
#endif

      /* ½èÍı¤E¥Õ¥E¼¥à¿Ê¤á¤E*/
      /* proceed one frame */
      
      switch(proceed_one_frame(recog)) {
      case -1:			/* error */
	return -1;
      case 0:			/* normal */
	break;
      case 1:			/* segmented by process */
	return 2;
      }

      /* 1¥Õ¥E¼¥à½èÍı¤¬¿Ê¤ó¤À¤Î¤Ç¥İ¥¤¥ó¥¿¤ò¿Ê¤á¤E*/
      /* proceed frame pointer */
      for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
	if (!mfcc->valid) continue;
	mfcc->f++;
      }
    }
    
    /* check if input end */
    switch(ret) {
    case -1: 			/* end of input */
      return 0;
    case -2:			/* error */
      return -1;
    case -3:			/* end of segment request */
      return 1;
    }
  }
  /* Í¿¤¨¤é¤E¿²»À¼¥»¥°¥á¥ó¥È¤ËÂĞ¤¹¤E§¼±½èÍı¤¬Á´¤Æ½ªÎ»
     ¸Æ¤Ó½Ğ¤·¸µ¤Ë, ÆşÎÏ¤òÂ³¤±¤Eè¤¦ÅÁ¤¨¤E*/
  /* input segment is fully processed
     tell the caller to continue input */
  return(1);
}

/* end of file */


