/**
 * @file   pass1.c
 * 
 * <JA>
 * @brief  ŽÂ¡¦¥Ñ¥¹¡§¥ÕŽ¥¡¦¼¥àÆ±´¡¦Ó¡¼¥àÃµº¡¦ *
 * ÀÅÅªÌÚ¹½Â¤¼­½ñ¤òÍÑ¤¤¤Æ¡¤ÆþÎÏÆÃÄ§ÎÌ¥Ù¥¯¥ÈŽ¥¡¦ó¤ËÂÐ¤·¤Æ¡Ž¤Julius¤ÎÂè£±¥Ñ¥¹
 * ¤Ç¤¢Ž¤¡¦Õ¥¡¦¼¥àÆ±´¡¦Ó¡¼¥àÃµº÷¤ò¹Ô¤¤¤Þ¤Ž¹. 
 *
 * ÆþÎÏ¥Ç¡¼¥¿Á´ÂÎ¤¬¤¢¤é¤«¤¸¤áÆÀ¤éŽ¤¡¦Æ¤¤¤¡¦¡¦ç¤Ï¡¤°¡¦ç¤Ç·×»»¤¡¦ * ¹Ô¤¦´ØŽ¿¡¦get_back_trellis() ¤¬¥á¥¤¥ó¤«¤é¸Æ¤ÐŽ¤¡¦Þ¤Ž¹. ¥ª¥ó¥é¥¤¥óÇ§¼±
 * ¤ÎŽ¾¡¦ç¤ŽÏ realtime_1stpass.c ¤«¤é¡¤½éŽ´¡¦½¡¤¥Õ¥¡¦¼¥à¤´¤È¤Î·×»»¡Ž¤
 * ½ªÎ»½èÍý¤Î¤½Ž¤¡¦¾¤¡¦¬ÆþÎÏ¤Î¿Ê¹Ô¤Ë¤¢¤¡¦»¤Æ¸ÄÊÌ¤Ë¸Æ¤Ð¤¡¦Þ¤Ž¹. 
 *
 * ¼ÂºÝ¤Î¸Ä¡¹¤ÎÇ§¼±½èÍý¥¤¥ó¥¹¥¿¥ó¥¹¤´¤È¤Î½èÍý¤Ï beam.c ¤Ëµ­½Ò¤µŽ¤¡¦Æ¤¤¤Þ¤Ž¹. 
 *
 * </JA>
 * 
 * <EN>
 * @brief  The first pass: frame-synchronous beam search
 *
 * These functions perform a frame-synchronous beam search using a static
 * lexicon tree, as the first pass of Julius/Julian.
 *
 * When the whole input is already obtained, get_back_trellis() simply
 * does all the processing of the 1st pass.  When performing online
 * real-time recognition with concurrent speech input, each function
 * will be called separately from realtime_1stpass.c according on the
 * basis of input processing.
 *
 * The core recognition processing functions for each recognition
 * process instances are written in beam.c.
 *
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Fri Oct 12 23:14:13 2007
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

/********************************************************************/
/* Âè£±¥Ñ¥¹¤ò¼Â¹Ô¤¹Ž¤¡¦á¥¤¥ó´Ø¿¡¦                                    */
/* ÆþÎÏ¤ò¥Ñ¥¤¥×¥é¥¤¥ó½èÍý¤¹Ž¤¡¦¡¦ç¤ŽÏ realtime_1stpass.c ¤ò»²¾È¤Î¤³¤È */
/* main function to execute 1st pass                                */
/* the pipeline processing is not here: see realtime_1stpass.c      */
/********************************************************************/

/** 
 * <EN>
 * @brief  Process one input frame for all recognition process instance.
 *
 * This function proceeds the recognition for one frame.  All
 * recognition process instance will be processed synchronously.
 * The input frame for each instance is stored in mfcc->f, where mfcc
 * is the MFCC calculation instance assigned to each process instance.
 *
 * If an instance's mfcc->invalid is set to TRUE, its processing will
 * be skipped.
 *
 * When using GMM, GMM computation will also be executed here.
 * If GMM_VAD is defined, GMM-based voice detection will be performed
 * inside this function, by using a scheme of short-pause segmentation.
 *
 * This function also handles segmentation of recognition process.  A
 * segmentation will occur when end of speech is detected by level-based
 * sound detection or GMM-based / decoder-based VAD, or by request from
 * application.  When segmented, it stores current frame and return with
 * that status.
 *
 * The frame-wise callbacks will be executed inside this function,
 * when at least one valid recognition process instances exists.
 * 
 * </EN>
 * <JA>
 * @brief  Á´¤Æ¤ÎÇ§¼±½èÍý¥¤¥ó¥¹¥¿¥ó¥¹½èÍýŽ¤¡¦¥ÕŽ¥¡¦¼¥àÊ¬¿Ê¤á¤¡¦
 *
 * Á´¤Æ¤ÎÇ§¼±½èÍý¥¤¥ó¥¹¥¿¥ó¥¹¤Ë¤Ä¤¤¤Æ¡¤³äŽ¤¡¦Õ¤±¤é¤¡¦Æ¤¤¤¡¦FCC·×»»¥¤¥ó¥¹¥¿¥ó¥¹
 * ¤Î mfcc->f ¤ò¥«Ž¥¡¦ó¥È¥Õ¥¡¦¼¥à¤È¤·¤Æ½èÍý¤¡¦¥ÕŽ¥¡¦¼¥à¿Ê¤á¤¡¦ 
 *
 * ¤Ê¤ª¡¤mfcc->invalid ¤¬ TRUE ¤È¤Ê¤Ã¤Æ¤¤Ž¤¡¦èÍý¥¤¥ó¥¹¥¿¥ó¥¹¤Î½èÍý¤Ï¥¹¥­¥Ã¥Ž×
 * ¤µŽ¤¡¦¡¦ 
 *
 * GMM¤Î·×»»¤â¤³¤³¤Ç¸Æ¤Ó½Ð¤µŽ¤¡¦¡¦ GMM_VAD ŽÄ¡¦Á»þ¤Ï¡Ž¤GMM ¤Ë¤èŽ¤¡¦ * È¯ÏÃ¶è´Ö³«»Ï¡¦½ªÎ»¤Î¸¡½Ð¤¬¤³¤³¤Ç¹ÔŽ¤¡¦¡¦¡¦ ¤Þ¤¿¡¤GMM¤Î·×»»Ž·¡¦Ì¡Ž¤
 * ¤¢Ž¤¡¦¤¤ÏÇ§¼±½èÍýÆâ¤Î¥·¥ç¡¼¥È¥Ý¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥óÈ½Äê¤ä¥Ç¥Ð¥¤¥¹¡¦³°É¡¦ * ¤«¤é¤ÎÍ×µá¤Ë¤èŽ¤¡¦»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó¤¬Í×µá¤µ¤¡¦¿¤«¤É¤¦¤«¤ÎÈ½Äê¤â¹Ô¤Ž¦. 
 *
 * ¥ÕŽ¥¡¦¼¥àÃ±°Ì¤Ç¸Æ¤Ó½Ð¤µ¤¡¦¡¦³¡¼¥¡¦Ð¥Ã¥¯¤¬ÅÐÏ¿¤µ¤¡¦Æ¤¤¤¡¦¡¦ç¤Ï¡¤¤½¤¡¦é¤ŽÎ
 * ¸Æ½Ð¤·¤â¹Ô¤¦. 
 * </JA>
 * 
 * @param recog [in] engine instance
 * 
 * @return 0 on success, -1 on error, or 1 when an input segmentation
 * occured/requested inside this function.
 *
 * @callgraph
 * @callergraph
 * 
 */
int
decode_proceed(Recog *recog)
{
  MFCCCalc *mfcc;
  boolean break_flag;
  boolean break_decode;
  RecogProcess *p;
  boolean ok_p;
#ifdef GMM_VAD
  GMMCalc *gmm;
  boolean break_gmm;
#endif
  
  break_decode = FALSE;

  for(p = recog->process_list; p; p = p->next) {
    p->have_interim = FALSE;
  }
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    mfcc->segmented = FALSE;
  }

  for(p = recog->process_list; p; p = p->next) {
    if (!p->live) continue;
    mfcc = p->am->mfcc;
    if (!mfcc->valid) {
      /* ¤³¤Î¥ÕŽ¥¡¦¼¥à¤Î½èÍý¤ò¥¹¥­¥Ã¥Ž× */
      /* skip processing the frame */
      continue;
    }

    /* mfcc-f ¤Î¥ÕŽ¥¡¦¼¥à¤Ë¤Ä¤¤¤ÆÇ§¼±½èÍŽý(¥ÕŽ¥¡¦¼¥àÆ±´¡¦Ó¡¼¥àÃµº¡¦¤ò¿Ê¤áŽ¤¡¦*/
    /* proceed beam search for mfcc->f */
    if (mfcc->f == 0) {
      /* ºÇ½é¤Î¥ÕŽ¥¡¦¼¥¡¦ Ãµº÷½èÍý¤ò½éŽ´¡¦Ž½ */
      /* initial frame: initialize search process */
      if (get_back_trellis_init(mfcc->param, p) == FALSE) {
	jlog("ERROR: %02d %s: failed to initialize the 1st pass\n", p->config->id, p->config->name);
	return -1;
      }
    }
    if (mfcc->f > 0 || p->am->hmminfo->multipath) {
      /* 1¥ÕŽ¥¡¦¼¥àÃµº÷¤ò¿Ê¤á¤¡¦*/
      /* proceed search for 1 frame */
      if (get_back_trellis_proceed(mfcc->f, mfcc->param, p, FALSE) == FALSE) {
	mfcc->segmented = TRUE;
	break_decode = TRUE;
      }
      if (p->config->successive.enabled) {
	if (detect_end_of_segment(p, mfcc->f - 1)) {
	  /* ¥»¥°¥á¥ó¥È½ªÎ»¸¡ÃÎ: Âè£±¥Ñ¥¹¤³¤³¤ÇÃæÃÇ */
	  mfcc->segmented = TRUE;
	  break_decode = TRUE;
	}
      }
    }
  }

  /* ¥»¥°¥á¥ó¥È¤¹¤Ù¤­¤«¤É¤¦¤«ºÇ½ªÅª¤ÊÈ½Äê¤ò¹Ô¤¦¡¥
     ¥Ç¥³¡¼¥À¥Ù¡¼¥¹VAD¤¢Ž¤¡¦¤¤ŽÏ spsegment ¤ÎŽ¾¡¦ç¡¤Ê£¿ô¥¤¥ó¥¹¥¿¥ó¥¹´Ö¤ŽÇ OR
     ¤ò¼èŽ¤¡¦¥¤Þ¤¿¡Ž¤GMM¤Ê¤ÉÊ£¿ô´ð½à¤¬¤¢Ž¤¡¦¡¦ç¤Ï´ð½à´Ö¤ŽÇ AND ¤ò¼èŽ¤¡¦Ž¥*/
  /* determine whether to segment at here
     If multiple segmenter exists, take their AND */
  break_flag = FALSE;
  if (break_decode
      ) {
    break_flag = TRUE;
  }

  if (break_flag) {
    /* Ãµº÷½èÍý¤Î½ªÎ»¤¬È¯À¸¤·¤¿¤Î¤Ç¤³¤³¤ÇÇ§¼±¤ò½ª¤¨Ž¤¡¦ 
       ºÇ½é¤Î¥ÕŽ¥¡¦¼¥à¤«¤¡¦[f-1] ÈÖÌÜ¤Þ¤Ç¤¬Ç§¼±¤µŽ¤¡¦¿¤³¤È¤Ë¤Ê¤¡¦    */
    /* the recognition process tells us to stop recognition, so
       recognition should be terminated here.
       the recognized data are [0..f-1] */

    /* ºÇ½ª¥ÕŽ¥¡¦¼¥à¤¡¦last_time ¤Ë¥»¥Ã¥È */
    /* set the last frame to last_time */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->last_time = mfcc->f - 1;
    }

    if (! recog->jconf->decodeopt.segment) {
      /* ¥·¥ç¡¼¥È¥Ý¡¼¥º°Ê³°¤ÇÀÚŽ¤¡¦¿¾¡¦ç¡¤»Ä¤ê¤Î¥µ¥ó¥×¥¡¦ÏÇ§¼±¤»¤º¤Ë¼Î¤Æ¤¡¦*/
      /* drop rest inputs if segmented by error */
      for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
	mfcc->param->header.samplenum = mfcc->f;
	mfcc->param->samplenum = mfcc->f;
      }
    }

    return 1;
  }

  return 0;
}

#ifdef POWER_REJECT
boolean
power_reject(Recog *recog)
{
  MFCCCalc *mfcc;

  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    /* skip if not realtime and raw file processing */
    if (mfcc->avg_power == 0.0) continue;
    if (debug2_flag) jlog("STAT: power_reject: MFCC%02d: avg_power = %f\n", mfcc->id, mfcc->avg_power / mfcc->param->samplenum);
    if (mfcc->avg_power / mfcc->param->samplenum < recog->jconf->reject.powerthres) return TRUE;
  }
  return FALSE;
}
#endif

/** 
 * <EN>
 * @brief  End procedure of the first pass (when segmented)
 *
 * This function do things for ending the first pass and prepare for
 * the next recognition, when the input was segmented at the middle of
 * recognition by some reason.
 *
 * First, the best path at each recognition process instance will be parsed
 * and stored.  In case of recognition error or input rejection, the error
 * status will be set.
 *
 * Then, the last pause segment of the processed input will be cut and saved
 * to be processed at first in the recognition of the next or remaining input.
 * 
 * </EN>
 * <JA>
 * @brief  ŽÂ¡¦¥Ñ¥¹¤Î½ªÎ»½èÍý¡Ê¥»¥°¥á¥ó¥È»þ¡Ë
 * 
 * ÆþÎÏ¤¬²¿¤é¤«¤Î»öÍ³¤Ë¤è¤Ã¤ÆÅÓÃæ¤Ç¥»¥°¥á¥ó¥È¤µŽ¤¡¦¿»þ¤Ë¡¤Â¡¦¥Ñ¥¹¤ÎÇ§¼±½èÍýŽ¤¡¦ * ½ªÎ»¤·¤Æ¼¡²óºÆ³«¤¹Ž¤¡¦¿¤á¤Î½èÍý¤ò¹Ô¤Ž¦. 
 *
 * ¤Þ¤º¡¤³ÆÇ§¼±½èÍý¥¤¥ó¥¹¥¿¥ó¥¹¤ËÂÐ¤·¤Æ¡¤ºÇÌàÃ±Ž¸¡¦ÏÎó¤ò¸«ÉÕ¤±¡¤Â¡¦¥Ñ¥¹¤Î
 * Ç§¼±Ž·¡¦Ì¤È¤·¤Æ³ÊÇ¼¤¹¤¡¦ ¤Þ¤¿¡¤Ç§¼±¼ºÇÔ¡¦ÆþÎÏ´þµÑ¤Î»þ¤Ï¥¨¥é¡¼¥¹¥Æ¡¼¥¿¥¹¤ò¤½
 * Ž¤¡¦¾¤¡¦»¥Ã¥È¤¹¤¡¦
 * 
 * ¤½¤·¤Æ¡¤¼¡²ó¤ÎÇ§¼±¤Ç¡¤¼¡¤Î¥»¥°¥á¥ó¥È¤ÎÇ§¼±¤ò¡¤¸¡½Ð¤µŽ¤¡¦¿ËöÈø»¨²Ž»
 * ¶è´Ö¤«¤éºÆ³«¤¹Ž¤¡¦¿¤á¤Ë¡¤¤½¤ÎËöÈø»¨²»¶è´Ö¤òÀÚ¤¡¦Ð¤·¤Æ¤ª¤¯½èÍý¤ò¸Æ¤ŽÖ. 
 * 
 * </JA>
 * 
 * @param recog [in] engine instance
 * 
 * @callgraph
 * @callergraph
 */
void
decode_end_segmented(Recog *recog)
{
  boolean ok_p;
  int mseclen;
  RecogProcess *p;
  int last_status;

  /* rejectshort »ØŽÄ¡¦Žþ, ÆþÎÏ¤¬Ã»¤±Ž¤¡¦Ð¤³¤³¤ÇÂ¡¦¥Ñ¥¹Ž·¡¦Ì¤ò½ÐÎÏ¤·¤Ê¤Ž¤ */
  /* suppress 1st pass output if -rejectshort and input shorter than specified */
  ok_p = TRUE;
  if (recog->jconf->reject.rejectshortlen > 0) {
    mseclen = (float)recog->mfcclist->last_time * (float)recog->jconf->input.period * (float)recog->jconf->input.frameshift / 10000.0;
    if (mseclen < recog->jconf->reject.rejectshortlen) {
      last_status = J_RESULT_STATUS_REJECT_SHORT;
      ok_p = FALSE;
    }
  }

#ifdef POWER_REJECT
  if (ok_p) {
    if (power_reject(recog)) {
      last_status = J_RESULT_STATUS_REJECT_POWER;
      ok_p = FALSE;
    }
  }
#endif

  if (ok_p) {
    for(p=recog->process_list;p;p=p->next) {
      if (!p->live) continue;
      finalize_1st_pass(p, p->am->mfcc->last_time);
    }
  } else {
    for(p=recog->process_list;p;p=p->next) {
      if (!p->live) continue;
      p->result.status = last_status;
    }
  }
  if (recog->jconf->decodeopt.segment) {
    finalize_segment(recog);
  }

  if (recog->gmm != NULL) {
    /* GMM ·×»»¤Î½ªÎ» */
    gmm_end(recog);
  }
}

/** 
 * <EN>
 * @brief  End procedure of the first pass
 *
 * This function finish the first pass, when the input was fully
 * processed to the end.
 *
 * The best path at each recognition process instance will be parsed
 * and stored.  In case of recognition error or input rejection, the
 * error status will be set.
 *
 * </EN>
 * <JA>
 * @brief  ŽÂ¡¦¥Ñ¥¹¤Î½ªÎ»½èÍý
 * 
 * ÆþÎÏ¤¬ºÇ¸å¤Þ¤Ç½èÍý¤µŽ¤¡¦Æ½ªÎ»¤·¤¿¤È¤­¤Ë¡¤Â¡¦¥Ñ¥¹¤ÎÇ§¼±½èÍýŽ¤¡¦ * ½ªÎ»¤µ¤»Ž¤¡¦ 
 *
 * ³ÆÇ§¼±½èÍý¥¤¥ó¥¹¥¿¥ó¥¹¤ËÂÐ¤·¤Æ¡¤¤½¤Î»þÅÀ¤Ç¤ÎŽÂ¡¦¥Ñ¥¹¤ÎºÇÌàÃ±Ž¸¡¦ * ·ÏÎó¤ò³ÊÇ¼¤¹Ž¤¡¦ ¤Þ¤¿¡¤Ç§¼±¼ºÇÔ¡¦ÆþÎÏ´þµÑ¤Î»þ¤Ï¥¨¥é¡¼¥¹¥Æ¡¼¥¿¥¹¤ò¤½
 * Ž¤¡¦¾¤¡¦»¥Ã¥È¤¹¤¡¦
 * 
 * </JA>
 * 
 * @param recog [in] engine instance
 * 
 * @callgraph
 * @callergraph
 */
void
decode_end(Recog *recog)
{
  MFCCCalc *mfcc;
  int mseclen;
  boolean ok_p;
  RecogProcess *p;
  int last_status;

  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    mfcc->segmented = FALSE;
  }

  if (recog->gmm != NULL) {
    /* GMM ·×»»¤Î½ªÎ» */
    gmm_end(recog);
  }

#ifdef GMM_VAD
  /* ¤â¤·¥ÈŽ¥¡¦¬¤¬¤«¤«¤é¤Ê¤¤¤Þ¤ÞÆþÎÏ½ªÎ»¤ËÃ£¤·¤¿¤Î¤Ê¤é¡¤¤½¤Î¤Þ¤Þ¥¨¥é¡¼½ªÎŽ» */
  if (recog->jconf->decodeopt.segment) {
    if (recog->gmm) {
      if (recog->gc->after_trigger == FALSE) {
	for(p=recog->process_list;p;p=p->next) {
	  p->result.status = J_RESULT_STATUS_ONLY_SILENCE;	/* reject by decoding */
	}
	/* ¥·¥ç¡¼¥È¥Ý¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó¤ÎŽ¾¡¦¡¦
	   ÆþÎÏ¥Ñ¥é¥á¡¼¥¿Ê¬³ä¤Ê¤É¤ÎºÇ½ª½èÍý¤â¹Ô¤Ê¤¦ */
	/* When short-pause segmentation enabled */
	finalize_segment(recog);
	return;
      }
    }
  }
#endif

  /* Âè£±¥Ñ¥¹¤ÎºÇ¸å¤Î¥ÕŽ¥¡¦¼¥à¤ÎÇ§¼±½èÍý¤ò¹Ô¤Ž¦ */
  /* finalize 1st pass */
  for(p=recog->process_list;p;p=p->next) {
    if (!p->live) continue;
#ifdef SPSEGMENT_NAIST
    if (recog->jconf->decodeopt.segment) {
      if (p->pass1.after_trigger == FALSE) continue;
    }
#endif
    mfcc = p->am->mfcc;
    if (mfcc->f > 0) {
      get_back_trellis_end(mfcc->param, p);
    }
  }

  /* ½ªÎ»½èÍý */
  for(p=recog->process_list;p;p=p->next) {
    if (!p->live) continue;

    ok_p = TRUE;

    /* check rejection by no input */
    if (ok_p) {
      mfcc = p->am->mfcc;
      /* ÆþÎÏÄ¹¤¬¥ÇŽ¥¡¦¿¤Î·×»»¤Ë½½Ê¬¤Ç¤Ê¤¤¾¡¦ç¡¤ÆþÎÏÌµ¤·¤È¤¹¤¡¦Ž¥ */
      /* if input is short for compute all the delta coeff., terminate here */
      if (mfcc->f == 0) {
//	jlog("STAT: no input frame\n");
	last_status = J_RESULT_STATUS_FAIL;
	ok_p = FALSE;
      }
    }

    /* check rejection by input length */
    if (ok_p) {
      if (recog->jconf->reject.rejectshortlen > 0) {
	mseclen = (float)mfcc->param->samplenum * (float)recog->jconf->input.period * (float)recog->jconf->input.frameshift / 10000.0;
	if (mseclen < recog->jconf->reject.rejectshortlen) {
	  last_status = J_RESULT_STATUS_REJECT_SHORT;
	  ok_p = FALSE;
	}
      }
    }

#ifdef POWER_REJECT
    /* check rejection by average power */
    if (ok_p) {
      if (power_reject(recog)) {
	last_status = J_RESULT_STATUS_REJECT_POWER;
	ok_p = FALSE;
      }
    }
#endif

#ifdef SPSEGMENT_NAIST
    /* check rejection non-triggered input segment */
    if (ok_p) {
      if (recog->jconf->decodeopt.segment) {
	if (p->pass1.after_trigger == FALSE) {
	  last_status = J_RESULT_STATUS_ONLY_SILENCE;	/* reject by decoding */
	  ok_p = FALSE;
	}
      }
    }
#endif

    if (ok_p) {
      /* valid input segment, finalize it */
      finalize_1st_pass(p, mfcc->param->samplenum);
    } else {
      /* invalid input segment */
      p->result.status = last_status;
    }
  }
  if (recog->jconf->decodeopt.segment) {
    /* ¥·¥ç¡¼¥È¥Ý¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó¤ÎŽ¾¡¦¡¦
       ÆþÎÏ¥Ñ¥é¥á¡¼¥¿Ê¬³ä¤Ê¤É¤ÎºÇ½ª½èÍý¤â¹Ô¤Ê¤¦ */
    /* When short-pause segmentation enabled */
    finalize_segment(recog);
  }
}


/** 
 * <JA>
 * @brief  ¥ÕŽ¥¡¦¼¥àÆ±´¡¦Ó¡¼¥àÃµº÷¥á¥¤¥ó´Ø¿ô¡Ê¥Ð¥Ã¥Á½èÍýÍÑ¡ŽË
 *
 * Í¿¤¨¤éŽ¤¡¦¿ÆþÎÏ¥Ù¥¯¥È¥¡¦ó¤ËÂÐ¤·¤ÆÂè£±¥Ñ¥Ž¹(¥ÕŽ¥¡¦¼¥àÆ±´¡¦Ó¡¼¥àÃµº¡¦Ž¤¡¦ * ¹Ô¤¤¡¤¤½¤ÎŽ·¡¦Ì¤ò½ÐÎÏ¤¹¤¡¦ ¤Þ¤¿Á´¥ÕŽ¥¡¦¼¥à¤ËÅÏ¤¡¦±¸¡¦ªÃ¼¤ò¡¤Âè£²¥Ñ¥Ž¹
 * ¤Î¤¿¤á¤ËÃ±Ž¸¡¦È¥¡¦¡¦¹¹½Â¤ÂÎ¤Ë³ÊÇ¼¤¹¤¡¦ 
 * 
 * ¤³¤Î´Ø¿ô¤ÏÆþÎÏ¥Ù¥¯¥ÈŽ¥¡¦ó¤¬¤¢¤é¤«¤¸¤áÆÀ¤é¤¡¦Æ¤¤¤¡¦¡¦ç¤ËÍÑ¤¤¤é¤¡¦¡¦ 
 * Âè£±¥Ñ¥¹¤¬ÆþÎÏ¤ÈÊÂÎó¤·¤Æ¼Â¹Ô¤µŽ¤¡¦¡¦ª¥ó¥é¥¤¥óÇ§¼±¤Î¾¡¦ç¡Ž¤
 * ¤³¤Î´Ø¿ô¤ÏÍÑ¤¤¤éŽ¤¡¦º¡¤Âå¤¡¦ê¤Ë¤³¤Î¥Õ¥¡¥¤¥¡¦ÇÄ¡¦Á¤µ¤¡¦Æ¤¤¤¡¦Æ¥µ¥Ö´Ø¿ô¤Ž¬
 * Ä¾ÀÜ realtime-1stpass.c Æâ¤«¤é¸Æ¤ÐŽ¤¡¦¡¦ 
 * 
 * @param recog [in] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
 * </JA>
 * <EN>
 * @brief  Frame synchronous beam search: the main (for batch mode)
 *
 * This function perform the 1st recognition pass of frame-synchronous beam
 * search and output the result.  It also stores all the word ends in every
 * input frame to word trellis structure.
 *
 * This function will be called if the whole input vector is already given
 * to the end.  When online recognition, where the 1st pass will be
 * processed in parallel with input, this function will not be used.
 * In that case, functions defined in this file will be directly called
 * from functions in realtime-1stpass.c.
 * 
 * @param recog [in] engine instance
 * </EN>
 * @callgraph
 * @callergraph
 */
boolean
get_back_trellis(Recog *recog)
{
  boolean ok_p;
  MFCCCalc *mfcc;
  int rewind_frame;
  PROCESS_AM *am;
  boolean reprocess;

  /* initialize mfcc instances */
  for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
    /* mark all as valid, since all frames are fully prepared beforehand */
    if (mfcc->param->samplenum == 0) mfcc->valid = FALSE;
    else mfcc->valid = TRUE;
    /* set frame pointers to 0 */
    mfcc->f = 0;
  }

  recog->triggered = TRUE;

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
      /* ¤¹¤Ù¤Æ¤Î MFCC ¤¬½ªŽ¤¡¦ê¤ËÃ£¤·¤¿¤Î¤Ç¥¡¦¼¥×½ªÎŽ» */
      /* all MFCC has been processed, end of loop  */
      break;
    }

    switch (decode_proceed(recog)) {
    case -1: /* error */
      return FALSE;
      break;
    case 0:			/* success */
      break;
    case 1:			/* segmented */
      /* Ãµº÷ÃæÃÇ: ½èÍý¤µŽ¤¡¦¿ÆþÎÏ¤ŽÏ 0 ¤«Ž¤¡¦t-2 ¤Þ¤Ç */
      /* search terminated: processed input = [0..t-2] */
      /* ¤³¤Î»þÅÀ¤ÇŽÂ¡¦¥Ñ¥¹¤ò½ªÎ»¤¹Ž¤¡¦*/
      /* end the 1st pass at this point */
      decode_end_segmented(recog);
      /* terminate 1st pass here */
      return TRUE;
    }


//    if (spsegment_need_restart(recog, &rewind_frame, &reprocess) == TRUE) {
//      /* do rewind for all mfcc here */
//      spsegment_restart_mfccs(recog, rewind_frame, reprocess);
//      /* reset outprob cache for all AM */
//      for(am=recog->amlist;am;am=am->next) {
//	outprob_prepare(&(am->hmmwrk), am->mfcc->param->samplenum);
//      }
//    }
    /* call frame-wise callback */
//    callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);

    /* 1¥ÕŽ¥¡¦¼¥à½èÍý¤¬¿Ê¤ó¤À¤Î¤Ç¥Ý¥¤¥ó¥¿¤ò¿Ê¤á¤¡¦*/
    /* proceed frame pointer */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      mfcc->f++;
    }

  }

  /* ºÇ½ª¥ÕŽ¥¡¦¼¥à½èÍý¤ò¹Ô¤¤¡¤Ç§¼±¤Î·¡¦Ì½ÐÎÏ¤È½ªÎ»½èÍý¤ò¹Ô¤Ž¦ */
  decode_end(recog);

  return TRUE;
}

/* end of file */
