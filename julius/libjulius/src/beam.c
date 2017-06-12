/**
 * @file   beam.c
 * 
 * <JA>
 * @brief  ¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤Î¼Â¹Ô¡ÊÂE¥Ñ¥¹¡Ë
 *
 * ÂE¥Ñ¥¹¤Î¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤ò¼Âºİ¤Ë¼Â¹Ô¤¹¤EØ¿ô·²¤Ç¤¹. 
 * Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹¤´¤È¤Ë¼Â¹Ô¤µ¤EŞ¤¹. 
 * ½é´E½¡¤1¥Õ¥E¼¥à¤ÎÇ§¼±½èÍı¡¤½ªÎ»½èÍı¡¤ÂE¥Ñ¥¹¤Î·EÌ·èÄê¡¤¥»¥°¥á¥ó¥È
 * ½ªÎ»¤Î¸¡ÃÎ¤Ê¤É¤Î½èÍı¤¬´Ş¤Ş¤EÆ¤¤¤Ş¤¹. 
 *
 * ¥¢¥E´¥Eº¥à¤Ë¤Ä¤¤¤Æ¤Ï¡¤Ã±¸EúÎò¶á»÷¤Ï 1-best ¶á»÷¤¬¥Ç¥Õ¥©¥EÈ¤Ç¤¹
 * ¤¬¡¤Ã±¸EĞ¶á»÷¤â»ÈÍÑ²ÄÇ½¤Ç¤¹. Ã±¸E-gram ¤Ç¤ÏÃ±¸EÖ¤ÎÀÜÂ³À©Ìó¤Ï 1-gram
 * factoring (2-gram factoring ¤âÁªÂò²Ä)¤òÍÑ¤¤¤Æ·×»»¤µ¤EŞ¤¹. Ê¸Ë¡¤Î
 * ¾Eç¡¤ÌÚ¹½Â¤²½¼­½ñ¤ÏÊ¸Ë¡¤Î¥«¥Æ¥´¥E±°Ì¤ÇºûÜ®¤µ¤E¤Ã±¸EÖ¤ÎÀÜÂ³(Ã±¸E * ÂĞÀ©ÌE¤ÏÃ±¸EÖÁ«°Ü¤ÇÅ¬ÍÑ¤µ¤EŞ¤¹. Ã±¸E§¼±¥â¡¼¥É¤Ç¤ÏÃ±¸EÖÀÜÂ³¤Ï
 * ¹ÍÎ¸¤µ¤EŞ¤»¤E 
 * </JA>
 * 
 * <EN>
 * @brief  Frame-synchronous beam search for the 1st pass
 *
 * These are core functions of frame-synchronous beam search using a
 * static lexicon tree, as the first pass of Julius.  These functions
 * will be called from pass1.c, to execute for each recognition
 * process instance in turn.  Functions for initialization, frame-wise
 * recognition processing, end procedure, finding best path, detecting
 * end of segment on short-pause segmentation mode, are defined here.
 *
 * About algorithm: 1-best approximation will be performed for word
 * context approximation, but normal word-pair approximation is also
 * supported. With word/class N-gram, Julius computes the language
 * score using 1-gram factoring (can be changed to 2-gram factoring if
 * you want).  With DFA grammar, Julius can compute the connection
 * constraint of words using the category-pair constraint on the
 * beginning of the words, since Julian makes a per-category tree
 * lexicon.  On isolated word recognition mode, the cross-word transitions
 * are ignored.
 *
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Tue Feb 22 17:00:45 2005
 *
 * $Revision: 1.19 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

#undef DEBUG


/* ---------------------------------------------------------- */
/*                     Âè£±¥Ñ¥¹¤Î·EÌ½èÍı                     */
/*              end procedure to get result of 1st pass       */
/* ---------------------------------------------------------- */

#ifdef WORD_GRAPH
/** 
 * <JA>
 * @brief  Ç§¼±·EÌ¤ÎÃ±¸EÈ¥EE¹¤«¤éÃ±¸E°¥é¥Õ¤òÃEĞ¤¹¤E *
 * (WORD_GRAPH »ØÄEş)
 * ¤³¤Î´Ø¿ô¤ÏÂè£±¥Ñ¥¹¤Î·EÌ¤ÎÃ±¸EÈ¥EE¹¤ò½ªÃ¼¤«¤é¥Ğ¥Ã¥¯¥È¥E¼¥¹¤·¡¤
 * ¥Ñ¥¹¾å¤Ë¤¢¤EÈ¥EE¹Ã±¸EòÃ±¸E°¥é¥Õ¤È¤·¤ÆÃEĞ¤¹¤E ¼Âºİ¤Ë¤Ï¡¤
 * Ã±¸EÈ¥EE¹¾å¤Ç¥°¥é¥Õ¾å¤Ë»Ä¤Eâ¤Î¤Î¤ß¤Ë¥Ş¡¼¥¯¤òÉÕ¤±¡¤
 * ÂE¥Ñ¥¹¤Ç¤Ï¡¤¥Ş¡¼¥¯¤Î¤Ä¤¤¤¿Ã±¸EÎ¤ß¤òÅ¸³«¤¹¤E 
 *
 * ¥°¥é¥Õ¤Ï r->result.wg1 ¤Ë³ÊÇ¼¤µ¤EE 
 * 
 * @param frame [in] Ã±¸EÈ¥EE¹¾å¤ÇÃ±¸EöÃ¼¤ò¸¡º÷¤¹¤EÕ¥E¼¥E * @param r [i/o] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * </JA>
 * <EN>
 * @brief  Extract word graph from the resulting word trellis
 *
 * If WORD_GRAPH is defined, this function trace back through the
 * word trellis from the end point, to extract the trellis words on
 * the path as a word graph.  Actually, this function only marks
 * which trellis words are included in the word graph.  On the 2nd pass,
 * only the words in the word graph will be expanded.
 *
 * The generated word graph will be stored to r->result.wg1.
 * 
 * @param frame [in] frame to lookup for word ends in the word trellis
 * @param r [i/o] recognition process instance
 * </EN>
 */
static void
generate_lattice(int frame, RecogProcess *r)
{
  BACKTRELLIS *bt;
  WORD_INFO *winfo;
  TRELLIS_ATOM *ta;
  int i, j;
  LOGPROB l;
  WordGraph *new;

  bt = r->backtrellis;
  winfo = r->lm->winfo;

  if (frame >= 0) {
    for (i=0;i<bt->num[frame];i++) {
      ta = bt->rw[frame][i];
      /* words will be saved as a part of graph only if any of its
	 following word has been survived in a beam */
      if (! ta->within_context) continue; /* not a candidate */
      if (ta->within_wordgraph) continue; /* already marked */
      /* mark  */
      ta->within_wordgraph = TRUE;

      new = (WordGraph *)mymalloc(sizeof(WordGraph));
      new->wid = ta->wid;
      new->lefttime = ta->begintime;
      new->righttime = ta->endtime;
      new->fscore_head = ta->backscore;
      new->fscore_tail = 0.0;
      new->gscore_head = 0.0;
      new->gscore_tail = 0.0;
      new->lscore_tmp = ta->lscore;
#ifdef CM_SEARCH
      new->cmscore = 0.0;
#endif
      new->forward_score = new->backward_score = 0.0;
      new->headphone = winfo->wseq[ta->wid][0];
      new->tailphone = winfo->wseq[ta->wid][winfo->wlen[ta->wid]-1];

      new->leftwordmaxnum = FANOUTSTEP;
      new->leftword = (WordGraph **)mymalloc(sizeof(WordGraph *) * new->leftwordmaxnum);
      new->left_lscore = (LOGPROB *)mymalloc(sizeof(LOGPROB) * new->leftwordmaxnum);
      new->leftwordnum = 0;
      new->rightwordmaxnum = FANOUTSTEP;
      new->rightword = (WordGraph **)mymalloc(sizeof(WordGraph *) * new->rightwordmaxnum);
      new->right_lscore = (LOGPROB *)mymalloc(sizeof(LOGPROB) * new->rightwordmaxnum);
      new->rightwordnum = 0;

      l = ta->backscore;
      if (ta->last_tre->wid != WORD_INVALID) {
	l -= ta->last_tre->backscore;
      }
      l -= ta->lscore;
      new->amavg = l / (float)(ta->endtime - ta->begintime + 1);

#ifdef GRAPHOUT_DYNAMIC
      new->purged = FALSE;
#endif
      new->saved = FALSE;
      new->graph_cm = 0.0;
      new->mark = FALSE;

      new->next = r->result.wg1;
      r->result.wg1 = new;

      /* recursive call */
      generate_lattice(ta->last_tre->endtime, r);
    }
  }
}

/** 
 * <EN>
 * Link all words in 1st pass word graph extracted by
 * generate_lattice() by their boundary frame.  All words with the
 * same boundary frame will be connected.
 * 
 * </EN>
 * <JA>
 * generate_lattice() ¤ÇÀ¸À®¤·¤¿ÂE¥Ñ¥¹¥°¥é¥ÕÃæ¤ÎÃ±¸EÉ¤¦¤·¤ò¶­³¦»ş´Ö
 * ¤Ë½¾¤Ã¤ÆÏ¢·E¹¤E Æ±¤¸¶­³¦»ş´Ö¤ò»ı¤Ä¤¹¤Ù¤Æ¤ÎÃ±¸E¬ÀÜÂ³¤µ¤EE 
 * 
 * </JA>
 * 
 * @param root [in] pointer to the root of lattice words.
 * 
 */
static void
link_lattice_by_time(WordGraph *root)
{
  WordGraph *wg;
  WordGraph *wtmp;
  int lefttime, righttime;
  
  for(wg=root;wg;wg=wg->next) {
    
    for(wtmp=root;wtmp;wtmp=wtmp->next) {
      if (wg->righttime + 1 == wtmp->lefttime) {
	wordgraph_check_and_add_leftword(wtmp, wg, wtmp->lscore_tmp);
	wordgraph_check_and_add_rightword(wg, wtmp, wtmp->lscore_tmp);
      }
      if (wtmp->righttime + 1 == wg->lefttime) {
	wordgraph_check_and_add_leftword(wg, wtmp, wg->lscore_tmp);
	wordgraph_check_and_add_rightword(wtmp, wg, wg->lscore_tmp);
      }
    }
  }

}

/** 
 * <EN>
 * re-compute 2-gram prob for all link in 1st pass word graph mode.
 * </EN>
 * <JA>
 * ÂE¥Ñ¥¹¤ÇÃ±¸E°¥é¥Õ¤òÀ¸À®¤¹¤Eâ¡¼¥É¤Ë¤ª¤¤¤Æ¡¤À¸À®¸å¤ËÃ±¸E°¥é¥Õ¾å¤Î
 * Àµ³Î¤Ê2-gram¸À¸EÎÎ©¤òºÆ·×»»¤¹¤E 
 * </JA>
 * 
 * @param root [in] pointer to root node of word graph
 * @param wchmm [in] tree lexicon used for the word graph generation
 * 
 */
static void
re_compute_lattice_lm(WordGraph *root, WCHMM_INFO *wchmm)
{
  WordGraph *wg;
  int i;
  
  for(wg=root;wg;wg=wg->next) {
    for(i=0;i<wg->leftwordnum;i++) {
      wg->left_lscore[i] = (*(wchmm->ngram->bigram_prob))(wchmm->ngram, wchmm->winfo->wton[wg->leftword[i]->wid], wchmm->winfo->wton[wg->wid]);
    }
    for(i=0;i<wg->rightwordnum;i++) {
      wg->right_lscore[i] = (*(wchmm->ngram->bigram_prob))(wchmm->ngram, wchmm->winfo->wton[wg->wid], wchmm->winfo->wton[wg->rightword[i]->wid]);
    }
  }
}

#endif

/** 
 * <JA>
 * ¤¢¤EÈ¥EE¹Ã±¸EÎ¾ğÊó¤ò¥Æ¥­¥¹¥È¤Ç½ĞÎÏ (¥Ç¥Ğ¥Ã¥°ÍÑ)
 * 
 * @param atom [in] ½ĞÎÏ¤¹¤EÈ¥EE¹Ã±¸E * @param winfo [in] Ã±¸E­½E * </JA>
 * <EN>
 * Output a trellis word information in text (for debug)
 * 
 * @param atom [in] trellis word to output
 * @param winfo [in] word dictionary
 * </EN>
 */
static void
put_atom(TRELLIS_ATOM *atom, WORD_INFO *winfo)
{
  int i;
  jlog("DEBUG: %3d,%3d %f %16s (id=%5d)", atom->begintime, atom->endtime,
       atom->backscore, winfo->wname[atom->wid], atom->wid);
  for (i=0;i<winfo->wlen[atom->wid]; i++) {
    jlog(" %s",winfo->wseq[atom->wid][i]->name);
  }
  jlog("\n");
}

/** 
 * <JA>
 * @brief Ç§¼±·EÌ¤ÎÃ±¸EÈ¥EE¹¾å¤ÎºÇÌàÃ±¸EÏÎó¤òµá¤á¤E * 
 * Í¿¤¨¤é¤E¿¥È¥EE¹Ã±¸E«¤éÆşÎÏ»ÏÃ¼¤Ë¸ş¤«¤Ã¤ÆÃ±¸EÈ¥EE¹¾å¤E * ¥È¥E¼¥¹¥Ğ¥Ã¥¯¤·, ¤½¤ÎºÇÌàÃ±¸EÏÎó¸õÊä¤ª¤è¤Ó¤½¤Î¸À¸E¹¥³¥¢¤òÊÖ¤¹. 
 * µ¯ÅÀ¤È¤Ê¤EÇ½é¤Î¥È¥EE¹Ã±¸E¬Í¿¤¨¤é¤EE¬Í×¤¬¤¢¤E 
 * 
 * @param wordseq_rt [out] ·EÌ¤ÎºÇÌàÃ±¸EÏÎó¤¬³ÊÇ¼¤µ¤EEĞ¥Ã¥Õ¥¡
 * @param rt_wordlen [out] @a wordseq_rt ¤ÎÄ¹¤µ
 * @param atom [in] ¥Ğ¥Ã¥¯¥È¥E¼¥¹¤Îµ¯ÅÀ¤È¤Ê¤EÈ¥EE¹Ã±¸E * @param winfo [in] Ã±¸E­½E * 
 * @return ÆÀ¤é¤E¿ºÇÌàÃ±¸EÏÎó¤Î¸À¸E¹¥³¥¢.
 * </JA>
 * <EN>
 * @brief Find the best word sequence in the word trellis
 *
 * This function trace back through the word trellis to the beginning
 * of input, to find the best word sequence.  The traceback starting point
 * should be specified as a trellis word.
 * 
 * @param wordseq_rt [out] buffer to store the best word sequence as result
 * @param rt_wordlen [out] length of @a wordseq_rt
 * @param atom [in] a trellis word as the starting point of the traceback
 * @param winfo [in] word dictionary
 * 
 * @return the total N-gram language score of the word sequence.
 * </EN>
 */
static LOGPROB
trace_backptr(WORD_ID wordseq_rt[MAXSEQNUM], int *rt_wordlen, TRELLIS_ATOM *atom, WORD_INFO *winfo)
{
  int wordlen = 0;		/* word length of best sentence hypothesis */
  TRELLIS_ATOM *tretmp;
  LOGPROB langscore = 0.0;
  WORD_ID wordseq[MAXSEQNUM];	/* temporal: in reverse order */
  int i;

  /* initialize */
  wordseq[0] = atom->wid;	/* start from specified atom */
  wordlen = 1;
  tretmp = atom;
  langscore += tretmp->lscore;
  if (debug2_flag) {
    put_atom(tretmp, winfo);
  }
  
  /* trace the backtrellis */
  while (tretmp->begintime > 0) {/* until beginning of input */
    tretmp = tretmp->last_tre;
/*    t = tretmp->boundtime - 1;
    tretmp = bt_binsearch_atom(backtrellis, tretmp->boundtime-1, tretmp->last_wid);*/
    if (tretmp == NULL) {	/* should not happen */
      j_internal_error("trace_backptr: last trellis missing while backtracking");
    }
    langscore += tretmp->lscore;
    wordseq[wordlen] = tretmp->wid;
    wordlen++;
    if (debug2_flag) {
      put_atom(tretmp, winfo);
    }
    if (wordlen >= MAXSEQNUM) {
      j_internal_error("trace_backptr: sentence length exceeded ( > %d)\n",MAXSEQNUM);
    }
  }
  *rt_wordlen = wordlen;
  /* reverse order -> normal order */
  for(i=0;i<wordlen;i++) wordseq_rt[i] = wordseq[wordlen-i-1];
  return(langscore);
}

/** 
 * <JA>
 * @brief  Âè£±¥Ñ¥¹¤ÎÇ§¼±½èÍı·EÌ¤«¤éÇ§¼±·EÌ¤òÈ½Äê¤·¡¤ºÇÌàÃ±¸EÏÎó¤ò¸«¤Ä¤±¤E 
 *
 * Âè£±¥Ñ¥¹¤Î·×»»·EÌ¤Ç¤¢¤E±¸EÈ¥EE¹¤«¤é¡¤Âè£±¥Ñ¥¹¤Ç¤ÎºÇÌà¸õÊä¤òµE * ¤á¡¤¥¤¥ó¥¹¥¿¥ó¥¹Æâ¤Î result.pass1 ¤ËÊİÂ¸¤¹¤E ¸õÊä¤¬ÆÀ¤é¤EÊ¤¤¾EE * ¤Ï¥¨¥é¡¼¡ÊÃµº÷¸úÀê¡§¥³¡¼¥É -1¡Ë¤È¤Ê¤E 
 *
 * ¥·¥ç¡¼¥È¥İ¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó»ş¤Ï¡¤Ç§¼±·EÌ¤¬Ìµ²»Ã±¸EÎ¤ß¤«¤é¤Ê¤EEç¡¤
 * ¥¨¥é¡¼¡Ê¥Ç¥³¡¼¥À¤Ë¤è¤EşµÑ¡§¥³¡¼¥É -4¡Ë¤È¤Ê¤E 
 *
 * ¤Ş¤¿¡¤WORD_GRAPH ÄEÁ»ş¤Ï¡¤¤³¤Î´Ø¿ôÆâ¤Ç¤µ¤é¤Ë generate_lattice() ¤E * ¸Æ¤Ó½Ğ¤·¡¤Ã±¸E°¥é¥Õ¤ÎÃEĞ¤ò¹Ô¤¦. 
 * 
 * @param framelen [in] Âè£±¥Ñ¥¹¤Ç½èÍı¤¬ÅşÃ£¤·¤¿¥Õ¥E¼¥à¿E * @param r [in] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * 
 * </JA>
 * <EN>
 * @brief  Find best path from the first pass result and set result status.
 *
 * This function find the best word sequence from the resulting word
 * trellis of the 1st pass, and store them to result.pass1 in the
 * recognition process instance.  If no candidate was found, it sets
 * error code -1 (recognition failure) and exit.
 *
 * On short-pause segmentation, it sets error code -4 (decoder rejection)
 * if the found best path consists of only silence words.
 * 
 * Also, if WORD_GRAPH is defined, this function also calls
 * generate_lattice() to extract word graph from the word trellis.
 * 
 * @param framelen [in] frame length that has been processed
 * @param r [in] recognition process instance
 * </EN>
 */
static void
find_1pass_result(int framelen, RecogProcess *r)
{
  BACKTRELLIS *backtrellis;
  WORD_INFO *winfo;
  WORD_ID wordseq[MAXSEQNUM];
  int wordlen;
  int i;
  TRELLIS_ATOM *best;
  int last_time;
  LOGPROB total_lscore;
  LOGPROB maxscore;
  TRELLIS_ATOM *tmp;
#ifdef SPSEGMENT_NAIST
  boolean ok_p;
#endif

  backtrellis = r->backtrellis;
  winfo = r->lm->winfo;

  /* look for the last trellis word */

  if (r->lmtype == LM_PROB) {

    for (last_time = framelen - 1; last_time >= 0; last_time--) {

      maxscore = LOG_ZERO;
      for (i=0;i<backtrellis->num[last_time];i++) {
	tmp = backtrellis->rw[last_time][i];
#ifdef WORD_GRAPH
	/* treat only words on a graph path */
	if (!tmp->within_context) continue;
#endif
	if (r->config->successive.enabled) {
	  /* short-pause segmentation mode */
	  /* ºÇ½ª¥Õ¥E¼¥à¤Ë»Ä¤Ã¤¿ºÇÂç¥¹¥³¥¢¤ÎÃ±¸E*/
	  /* it should be the best trellis word on the last frame */
	  if (maxscore < tmp->backscore) {
	    maxscore = tmp->backscore;
	    best = tmp;
	  }
	} else {
	  /* not segmentation mode */
	  /* ºÇ½ªÃ±¸EÏ winfo->tail_silwid ¤Ë¸ÇÄE*/
	  /* it is fixed to the tail silence model (winfo->tail_silwid) */
	  if (tmp->wid == winfo->tail_silwid && maxscore < tmp->backscore) {
	    maxscore = tmp->backscore;
	    best = tmp;
	    break;
	  }
	}
      }
      if (maxscore != LOG_ZERO) break;
    }

    if (last_time < 0) {		/* not found */
      jlog("WARNING: %02d %s: no tail silence word survived on the last frame, search failed\n", r->config->id, r->config->name);
      r->result.status = J_RESULT_STATUS_FAIL;
      //callback_exec(CALLBACK_RESULT, r);
      return;
    }
  
  }

  if (r->lmtype == LM_DFA) {

    for (last_time = framelen - 1; last_time >= 0; last_time--) {

      /* ËöÈø¤Ë»Ä¤Ã¤¿Ã±¸EÎÃæ¤ÇºÇÂç¥¹¥³¥¢¤ÎÃ±¸Ecp_end¤Ï»ÈÍÑ¤·¤Ê¤¤) */
      /* the best trellis word on the last frame (not use cp_end[]) */
      maxscore = LOG_ZERO;
      for (i=0;i<backtrellis->num[last_time];i++) {
	tmp = backtrellis->rw[last_time][i];
#ifdef WORD_GRAPH
	/* treat only words on a graph path */
	if (!tmp->within_context) continue;
#endif
	/*      if (dfa->cp_end[winfo->wton[tmp->wid]] == TRUE) {*/
	if (maxscore < tmp->backscore) {
	  maxscore = tmp->backscore;
	  best = tmp;
	}
	/*      }*/
      }
      if (maxscore != LOG_ZERO) break;
    }

    if (last_time < 0) {		/* not found */
      jlog("WARNING: %02d %s: no sentence-end word survived on last beam\n", r->config->id, r->config->name);
      r->result.status = J_RESULT_STATUS_FAIL;
      //callback_exec(CALLBACK_RESULT, r);
      return;
    }
  
  }

  /* traceback word trellis from the best word */
  total_lscore = trace_backptr(wordseq, &wordlen, best, r->lm->winfo);
#ifdef SPSEGMENT_NAIST
  if (r->config->successive.enabled) {
    /* on segmentation mode, recognition result that only consists of
       short-pause words will be treated as recognition rejection */
    ok_p = FALSE;
    for(i=0;i<wordlen;i++) {
      if (! is_sil(wordseq[i], r)) ok_p = TRUE;
    }
    if (ok_p == FALSE) {
      r->result.status = J_RESULT_STATUS_ONLY_SILENCE;
      return;
    }
  }
#endif

  /* just flush last progress output */
  /*
  if (recog->jconf->output.progout_flag) {
    recog->result.status = 1;
    recog->result.num_frame = last_time;
    recog->result.pass1.word = wordseq;
    recog->result.pass1.word_num = wordlen;
    recog->result.pass1.score = best->backscore;
    recog->result.pass1.score_lm = total_lscore;
    recog->result.pass1.score_am = best->backscore - total_lscore;
    //callback_exec(CALLBACK_RESULT_PASS1_INTERIM, recog);
    }*/

  /* output 1st pass result */    
  if (verbose_flag || ! r->config->output.progout_flag) {
    r->result.status = J_RESULT_STATUS_SUCCESS;
    r->result.num_frame = framelen;
    for(i=0;i<wordlen;i++) r->result.pass1.word[i] = wordseq[i];
    r->result.pass1.word_num = wordlen;
    r->result.pass1.score = best->backscore;
    r->result.pass1.score_lm = total_lscore;
    r->result.pass1.score_am = best->backscore - total_lscore;
    //callback_exec(CALLBACK_RESULT_PASS1, r);
  }

  /* store the result to global val (notice: in reverse order) */
  for(i=0;i<wordlen;i++) r->pass1_wseq[i] = wordseq[i];
  r->pass1_wnum = wordlen;
  r->pass1_score = best->backscore;

#ifdef WORD_GRAPH
  /* Ã±¸EÈ¥EE¹¤«¤é¡¤¥é¥Æ¥£¥¹¤òÀ¸À®¤¹¤E*/
  /* generate word graph from the word trellis */
  r->peseqlen = backtrellis->framelen;
  r->result.wg1 = NULL;
  generate_lattice(last_time, r);
  link_lattice_by_time(r->result.wg1);
  if (r->lmtype == LM_PROB) re_compute_lattice_lm(r->result.wg1, r->wchmm);
  r->result.wg1_num = wordgraph_sort_and_annotate_id(&(r->result.wg1), r);
  /* compute graph CM by forward-backward processing */
  graph_forward_backward(r->result.wg1, r);
  //callback_exec(CALLBACK_RESULT_PASS1_GRAPH, r);
  //wordgraph_clean(&(r->result.wg1));
#endif

}

/** 
 * <JA>
 * ¥È¥EE¹Ã±¸Eò¥¹¥³¥¢¤Ç¥½¡¼¥È¤¹¤Esort´Ø¿E 
 * 
 * @param x1 [in] Í×ÁÇ1¤Ø¤Î¥İ¥¤¥ó¥¿
 * @param x2 [in] Í×ÁÇ2¤Ø¤Î¥İ¥¤¥ó¥¿
 * 
 * @return qsort ¤ÎÃÍ
 * </JA>
 * <EN>
 * qsort function to sort trellis words by their score.
 * 
 * @param x1 [in] pointer to element #1
 * @param x2 [in] pointer to element #2
 * 
 * @return value required for qsort.
 * </EN>
 */
static int
compare_backscore(TRELLIS_ATOM **x1, TRELLIS_ATOM **x2)
{
  return((*x2)->backscore - (*x1)->backscore);
}

/** 
 * <JA>
 * find_1pass_result() ¤ÎÃ±¸E§¼±¥â¡¼¥ÉÈÇ. Ã±¸E§¼±¥â¡¼¥É¤Ç¤ÏÂE¥Ñ¥¹¤Ç
 * Ç§¼±¤ò½ªÎ»¤¹¤EÎ¤Ç¡¤ÆÀ¤é¤E¿¸õÊä¤ÏÄÌ¾EÎÂè£²¥Ñ¥¹¤ÈÆ±¤¸¾Eê¤Ë³ÊÇ¼¤¹¤E 
 * 
 * @param framelen [in] Âè£±¥Ñ¥¹¤Ç½èÍı¤¬ÅşÃ£¤·¤¿¥Õ¥E¼¥à¿E * @param r [i/o] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * 
 * </JA>
 * <EN>
 * Isolated word recognition version of find_1pass_result().
 * Since Julius executes only the 1st pass on Isolated word recognition
 * mode, the result candidate will be stored as the final result.
 * 
 * @param framelen [in] frame length that has been processed
 * @param r [i/o] recognition process instance
 * 
 * </EN>
 */
static void
find_1pass_result_word(int framelen, RecogProcess *r)
{
  BACKTRELLIS *bt;
  TRELLIS_ATOM *best, *tmp;
  int last_time;
  Sentence *s;
#ifdef CONFIDENCE_MEASURE
  LOGPROB sum;
#endif
  LOGPROB maxscore;
  int i;
  TRELLIS_ATOM **idx;
  int num;

  if (r->lmvar != LM_DFA_WORD) return;

  bt = r->backtrellis;
  for (last_time = framelen - 1; last_time >= 0; last_time--) {
    maxscore = LOG_ZERO;
    for (i=0;i<bt->num[last_time];i++) {
      tmp = bt->rw[last_time][i];
#ifdef WORD_GRAPH
      /* treat only words on a graph path */
      if (!tmp->within_context) continue;
#endif
      if (maxscore < tmp->backscore) {
	maxscore = tmp->backscore;
	best = tmp;
      }
    }
    if (maxscore != LOG_ZERO) break;
  }

  if (last_time < 0) {		/* not found */
    jlog("WARNING: %02d %s: no word survived on the last frame, search failed\n", r->config->id, r->config->name);
    r->result.status = J_RESULT_STATUS_FAIL;
    //callback_exec(CALLBACK_RESULT, r);
    return;
  }

#ifdef CONFIDENCE_MEASURE
  sum = 0.0;
  for (i=0;i<bt->num[last_time];i++) {
    tmp = bt->rw[last_time][i];
#ifdef WORD_GRAPH
    /* treat only words on a graph path */
    if (!tmp->within_context) continue;
#endif
    sum += pow(10, r->config->annotate.cm_alpha * (tmp->backscore - maxscore));
  }
#endif

  /* set recognition result status to normal */
  r->result.status = J_RESULT_STATUS_SUCCESS;

  if (r->config->output.output_hypo_maxnum > 1) {
    /* more than one candidate is requested */

    /* get actual number of candidates to output */
    num = r->config->output.output_hypo_maxnum;
    if (num > bt->num[last_time]) {
      num = bt->num[last_time];
    }

    /* prepare result storage */
    result_sentence_malloc(r, num);
    r->result.sentnum = num;

    /* sort by score */
    idx = (TRELLIS_ATOM **)mymalloc(sizeof(TRELLIS_ATOM *)*bt->num[last_time]);
    for (i=0;i<bt->num[last_time];i++) {
      idx[i] = bt->rw[last_time][i];
    }
    qsort(idx, bt->num[last_time], sizeof(TRELLIS_ATOM *),
	  (int (*)(const void *,const void *))compare_backscore);
    
    /* store to result storage */
    for(i=0;i<r->result.sentnum;i++) {
      s = &(r->result.sent[i]);
      tmp = idx[i];
      s->word_num = 1;
      s->word[0] = tmp->wid;
#ifdef CONFIDENCE_MEASURE
      s->confidence[0] = pow(10, r->config->annotate.cm_alpha * (tmp->backscore - maxscore)) / sum;
#endif
      s->score = tmp->backscore;
      s->score_lm = 0.0;
      s->score_am = tmp->backscore;
      if (multigram_get_all_num(r->lm) > 0) {
	s->gram_id = multigram_get_gram_from_wid(s->word[0], r->lm);
      } else {
	s->gram_id = 0;
      }
    }
    /* free work area for sort */
    free(idx);

  } else {			/* only max is needed */

    /* prepare result storage */
    result_sentence_malloc(r, 1);
    r->result.sentnum = 1;
    s = &(r->result.sent[0]);
    s->word_num = 1;
    s->word[0] = best->wid;
#ifdef CONFIDENCE_MEASURE
    s->confidence[0] = 1.0 / sum;
#endif
    s->score = best->backscore;
    s->score_lm = 0.0;
    s->score_am = best->backscore;
    if (multigram_get_all_num(r->lm) > 0) {
      s->gram_id = multigram_get_gram_from_wid(s->word[0], r->lm);
    } else {
      s->gram_id = 0;
    }
  }

  /* copy as 1st pass result */
  memcpy(&(r->result.pass1), &(r->result.sent[0]), sizeof(Sentence));
  r->result.pass1.align = NULL;

  //callback_exec(CALLBACK_RESULT, r);
  //free(r->result.sent);
}


#ifdef DETERMINE

/** 
 * <JA>
 * Âè£±¥Ñ¥¹¤ÎÅÓÃæ¥Ç¡¼¥¿¤«¤éÁá´EÎÄEÄÇ½¤«¤É¤¦¤«È½Äê¤¹¤EÊ¼Â¸³¡Ë. tremax ¤¬
 * NULL ¤Î¤È¤­¤Ï½é´E½¤¹¤E ³ÎÄEş¤Ï r->have_determine ¤ETRUE ¤Ë¤¹¤E 
 *
 * @param r [i/o] ²»À¼Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * @param t [in] ¥Õ¥E¼¥E * @param tremax [in] ¸½ºß¤Î¥Õ¥E¼¥à¾å¤ÇºÇÌà¤Î¥È¥EE¹Ã±¸E * @param thres [in] ³ÎÄEÑ¤Î¥¹¥³¥¢EÃÍ
 * @param countthres [in] ³ÎÄEÑ¤Î»ıÂ³¥Õ¥E¼¥à¿ô¤ÎEÃÍ
 *
 * @return ³ÎÄEş¤Ï tremax ¤òÊÖ¤¹. Ì¤³ÎÄEş¤Ï NULL ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * Try to Determine a word hypothesis before end of input on isolated
 * word recognition mode (EXPERIMENT).  Initialize if tremax is NULL.
 * Set r->have_determine to TRUE if determined.
 * 
 * @param r [i/o] recognition process instance
 * @param t [in] current frame
 * @param tremax [in] maximum scored trellis word on the current frame
 * @param thres [in] score threshold for determinization
 * @param countthres [in] frame duration threshold for determinization
 *
 * @return the tremax if determined, or NULL if not determined yet., 
 * </EN>
 */
static TRELLIS_ATOM *
determine_word(RecogProcess *r, int t, TRELLIS_ATOM *tremax, LOGPROB thres, int countthres)
{
  TRELLIS_ATOM *ret;
  WORD_ID w;

  //LOGPROB sum;
  //LOGPROB cm;

  int j;
  FSBeam *d;
  TOKEN2 *tk;
    
  if (tremax == NULL) {
    /* initialize */
    r->determine_count = 0;
    r->determine_maxnodescore = LOG_ZERO;
    r->determined = FALSE;
    r->determine_last_wid = WORD_INVALID;
    r->have_determine = FALSE;
    return NULL;
  }

  ret = NULL;

  /* get confidence score of the maximum word hypothesis */
/* 
 *   sum = 0.0;
 *   tre = recog->backtrellis->list;
 *   while (tre != NULL && tre->endtime == t) {
 *     sum += pow(10, recog->jconf->annotate.cm_alpha * (tre->backscore - tremax->backscore));
 *     tre = tre->next;
 *   }
 *   cm = 1.0 / sum;
 */

  /* determinization decision */
  w = tremax->wid;

  r->have_determine = FALSE;

  /* determine by score threshold from maximum node score to maximum word end node score */
  if (r->determine_last_wid == w && r->determine_maxnodescore - tremax->backscore <= thres) {
    r->determine_count++;
    if (r->determine_count > countthres) {
      if (r->determined == FALSE) {
	ret = tremax;
	r->determined = TRUE;
	r->have_determine = TRUE;
      }
    }
  } else {
    r->determine_count = 0;
  }

  //printf("determine: %d: %s: cm=%f, relscore=%f, count=%d, phase=%d\n", t, recog->model->winfo->woutput[w], cm, determine_maxnodescore - tremax->backscore, count, phase);
  r->determine_last_wid = w;

  /* update maximum node score here for next call, since
     the word path determination is always one frame later */
  d = &(r->pass1);
  r->determine_maxnodescore = LOG_ZERO;
  for (j = d->n_start; j <= d->n_end; j++) {
    tk = &(d->tlist[d->tn][d->tindex[d->tn][j]]);
    if (r->determine_maxnodescore < tk->score) r->determine_maxnodescore = tk->score;
  }

  return(ret);
}

/** 
 * <JA>
 * Ã±¸E§¼±»ş¤Ë¡¤Âè£±¥Ñ¥¹¤Î½èÍıÃæ¤ËÁá´EÎÄê¤¹¤EÊ¼Â¸³¡Ë. ³ÎÄê¤Ç¤­¤¿¾Eç¡¤
 * ÂE¥Ñ¥¹¤Î·EÌ³ÊÇ¼ÍÑ¥¨¥E¢ (r->result.pass1) ¤Ë³ÎÄEEÌ¤ò³ÊÇ¼¤¹¤E 
 * ¤Ş¤¿³ÎÄEş¤Ï r->have_determine ¤Ë TRUE ¤¬Æş¤E 
 * 
 * @param r [in] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * @param t [in] ¸½ºß¤ÎÆşÎÏ¥Õ¥E¼¥E * </JA>
 * <EN>
 * Determine word hypothesis before end of input (EXPERIMENT).  When
 * determined, the determined word will be stored to the result area
 * for the 1st pass (r->result.pass1).  r->have_determine will be
 * set to TRUE when determinized.
 * 
 * @param r [in] recognition process instance
 * @param t [in] current input frame
 * </EN>
 */
static void
check_determine_word(RecogProcess *r, int t)
{
  TRELLIS_ATOM *tre;
  TRELLIS_ATOM *tremax;
  LOGPROB maxscore;

  /* bt->list is ordered by time frame */
  maxscore = LOG_ZERO;
  tremax = NULL;
  tre = r->backtrellis->list;
  while (tre != NULL && tre->endtime == t) {
    if (maxscore < tre->backscore) {
      maxscore = tre->backscore;
      tremax = tre;
    }
    tre = tre->next;
  }

  r->result.status = J_RESULT_STATUS_SUCCESS;
  r->result.num_frame = t;

  if (maxscore != LOG_ZERO) {
    //    if ((tre = determine_word(recog, t, tremax, 0.9, 17)) != NULL) {
    if ((tre = determine_word(r, t, tremax, r->config->pass1.determine_score_thres, r->config->pass1.determine_duration_thres)) != NULL) {
      r->result.pass1.word[0] = tremax->wid;
      r->result.pass1.word_num = 1;
      r->result.pass1.score = tremax->backscore;
      r->result.pass1.score_lm = 0.0;
      r->result.pass1.score_am = tremax->backscore;
      r->result.num_frame = t;
      //callback_exec(CALLBACK_RESULT_PASS1_DETERMINED, r);
    }
  }

  
}

#endif /* DETERMINE */

/** 
 * <JA>
 * Âè£±¥Ñ¥¹¤Î½èÍıÃæ¤Ë¡¤¤¢¤EÕ¥E¼¥à¤Ş¤Ç¤Î¥Ù¥¹¥È¥Ñ¥¹¤òÉ½¼¨¤¹¤E 
 * 
 * @param r [i/o] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * @param t [in] ¸½ºß¤ÎÆşÎÏ¥Õ¥E¼¥E * </JA>
 * <EN>
 * Output the current best word sequence ending
 * at a specified time frame in the course of the 1st pass.
 * 
 * @param r [i/o] recognition process instance
 * @param t [in] current input frame
 * </EN>
 */
static void
bt_current_max(RecogProcess *r, int t)
{
  int wordlen;
  TRELLIS_ATOM *tre;
  TRELLIS_ATOM *tremax;
  LOGPROB maxscore;
  LOGPROB lscore;

  /* bt->list is ordered by time frame */
  maxscore = LOG_ZERO;
  tremax = NULL;
  tre = r->backtrellis->list;
  while (tre != NULL && tre->endtime == t) {
    if (maxscore < tre->backscore) {
      maxscore = tre->backscore;
      tremax = tre;
    }
    tre = tre->next;
  }

  r->result.status = J_RESULT_STATUS_SUCCESS;
  r->result.num_frame = t;

  if (maxscore == LOG_ZERO) {
    r->result.pass1.word_num = 0;
  } else {
    if (r->lmvar == LM_DFA_WORD) {
      r->result.pass1.word[0] = tremax->wid;
      r->result.pass1.word_num = 1;
      r->result.pass1.score = tremax->backscore;
      r->result.pass1.score_lm = 0.0;
      r->result.pass1.score_am = tremax->backscore;
    } else {
      lscore = trace_backptr(r->result.pass1.word, &wordlen, tremax, r->lm->winfo);
      r->result.pass1.word_num = wordlen;
      r->result.pass1.score = tremax->backscore;
      r->result.pass1.score_lm = lscore;
      r->result.pass1.score_am = tremax->backscore;
    }
  }
  //callback_exec(CALLBACK_RESULT_PASS1_INTERIM, r);
}

/** 
 * <JA>
 * Âè£±¥Ñ¥¹¤Î½èÍıÃæ¤Ë¡¤¤¢¤EÕ¥E¼¥à¾å¤ÎºÇÌàÃ±¸EòÉ½¼¨¤¹¤E¥Ç¥Ğ¥Ã¥°ÍÑ)
 * 
 * @param r [i/o] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * @param t [in] ¸½ºß¤ÎÆşÎÏ¥Õ¥E¼¥E * </JA>
 * <EN>
 * Output the current best word on a specified time frame in the course
 * of the 1st pass.
 * 
 * @param r [i/o] recognition process instance
 * @param t [in] current input frame
 * </EN>
 */
static void
bt_current_max_word(RecogProcess *r, int t)
{

  TRELLIS_ATOM *tre;
  TRELLIS_ATOM *tremax;
  LOGPROB maxscore;
  WORD_ID w;

  /* bt->list ¤Ï»ş´Ö½ç¤Ë³ÊÇ¼¤µ¤EÆ¤¤¤E*/
  /* bt->list is order by time */
  maxscore = LOG_ZERO;
  tremax = NULL;
  tre = r->backtrellis->list;
  while (tre != NULL && tre->endtime == t) {
    if (maxscore < tre->backscore) {
      maxscore = tre->backscore;
      tremax = tre;
    }
    tre = tre->next;
  }

  if (maxscore != LOG_ZERO) {
    jlog("DEBUG: %3d: ",t);
    w = tremax->wid;
    jlog("\"%s [%s]\"(id=%d)",
	 r->lm->winfo->wname[w], r->lm->winfo->woutput[w], w);
    jlog(" [%d-%d] %f", tremax->begintime, t, tremax->backscore);
    w = tremax->last_tre->wid;
    if (w != WORD_INVALID) {
      jlog(" <- \"%s [%s]\"(id=%d)\n",
	       r->lm->winfo->wname[w], r->lm->winfo->woutput[w], w);
    } else {
      jlog(" <- bgn\n");
    }
  }
}


/* -------------------------------------------------------------------- */
/*                 ¥Ó¡¼¥àÃµº÷Ãæ¤Î¥È¡¼¥¯¥ó¤ò°·¤¦¥µ¥Ö´Ø¿E                */
/*                functions to handle hypothesis tokens                 */
/* -------------------------------------------------------------------- */

/** 
 * <JA>
 * Âè£±¥Ñ¥¹¤Î¥Ó¡¼¥àÃµº÷ÍÑ¤Î½é´EE¼¥¯¥¨¥E¢¤ò³ÎÊİ¤¹¤E 
 * Â­¤ê¤Ê¤¤¾Eç¤ÏÃµº÷Ãæ¤ËÆ°Åª¤Ë¿­Ä¹¤µ¤EE 
 *
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param n [in] ÌÚ¹½Â¤²½¼­½ñ¤Î¥Î¡¼¥É¿E * @param ntoken_init [in] ºÇ½é¤Ë³ÎÊİ¤¹¤EÈ¡¼¥¯¥ó¤Î¿E * </JA>
 * <EN>
 * Allocate initial work area for beam search on the 1st pass.
 * If filled while search, they will be expanded on demand.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param n [in] number of nodes in lexicon tree
 * @param ntoken_init [in] number of token space to be allocated at first
 * </EN>
 */
static void
malloc_nodes(FSBeam *d, int n, int ntoken_init)
{
  d->totalnodenum = n;
  d->token     = (TOKENID *)mymalloc(sizeof(TOKENID) * d->totalnodenum);
  //d->maxtnum = ntoken_init;
  if (d->maxtnum < ntoken_init) d->maxtnum = ntoken_init;
  d->tlist[0]  = (TOKEN2 *)mymalloc(sizeof(TOKEN2) * d->maxtnum);
  d->tlist[1]  = (TOKEN2 *)mymalloc(sizeof(TOKEN2) * d->maxtnum);
  d->tindex[0] = (TOKENID *)mymalloc(sizeof(TOKENID) * d->maxtnum);
  d->tindex[1] = (TOKENID *)mymalloc(sizeof(TOKENID) * d->maxtnum);
  //d->expand_step = ntoken_step;
  d->nodes_malloced = TRUE;
  d->expanded = FALSE;
}

/** 
 * <JA>
 * Âè£±¥Ñ¥¹¤Î¥Ó¡¼¥àÃµº÷ÍÑ¤Î¥E¼¥¯¥¨¥E¢¤ò¿­¤Ğ¤·¤ÆºÆ³ÎÊİ¤¹¤E 
 *
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * </JA>
 * <EN>
 * Re-allocate work area for beam search on the 1st pass.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * </EN>
 */
static void
expand_tlist(FSBeam *d)
{
  d->maxtnum += d->expand_step;
  d->tlist[0]  = (TOKEN2 *)myrealloc(d->tlist[0],sizeof(TOKEN2) * d->maxtnum);
  d->tlist[1]  = (TOKEN2 *)myrealloc(d->tlist[1],sizeof(TOKEN2) * d->maxtnum);
  d->tindex[0] = (TOKENID *)myrealloc(d->tindex[0],sizeof(TOKENID) * d->maxtnum);
  d->tindex[1] = (TOKENID *)myrealloc(d->tindex[1],sizeof(TOKENID) * d->maxtnum);
  if (debug2_flag) jlog("STAT: token space expanded to %d\n", d->maxtnum);
  d->expanded = TRUE;
}

/** 
 * <EN>
 * Clear nodes for the next input.  Julius will call this function for
 * each input to re-set the work area for the beam search.  If the size
 * of tree lexicon has been changed since the last input, Julius will
 * free and re-allocate the work area.
 * </EN>
 * <JA>
 * ¥Î¡¼¥É¾ğÊó¤ò½é´E½¤¹¤E Julius ¤Ï¡¤ÌÚ¹½Â¤²½¼­½ñ¤Î¥µ¥¤¥º¤¬Ä¾Á°¤ÎÆşÎÏ
 * »ş¤ÈÊÑ²½¤¬¤Ê¤¤¤È¤­¤Ï¡¤¤³¤Î´Ø¿ô¤Ë¤è¤Ã¤Æ¥Î¡¼¥É¾ğÊó¤ò½é´E½¤¹¤EÀ¤±¤Ç
 * ¤è¤¤. ¥µ¥¤¥º¤¬ÊÑ¹¹¤µ¤EÆ¤¤¤EÈ¤­¤Ï¥Î¡¼¥É¤ò³«ÊE¦ºÆ³ÎÊİ¤¹¤E 
 * </JA>
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param ntoken_step [in] required token step
 * 
 */
static void
prepare_nodes(FSBeam *d, int ntoken_step)
{
  d->tnum[0] = d->tnum[1] = 0;
  if (d->expand_step < ntoken_step) d->expand_step = ntoken_step;
}

/** 
 * <JA>
 * Âè£±¥Ñ¥¹¤Î¥Ó¡¼¥àÃµº÷ÍÑ¤Î¥E¼¥¯¥¨¥E¢¤òÁ´¤Æ²òÊE¹¤E 
 *
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * 
 * </JA>
 * <EN>
 * Free all the work area for beam search on the 1st pass.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * 
 * </EN>
 */
static void
free_nodes(FSBeam *d)
{
  if (d->nodes_malloced) {
    free(d->token);
    free(d->tlist[0]);
    free(d->tlist[1]);
    free(d->tindex[0]);
    free(d->tindex[1]);
    d->nodes_malloced = FALSE;
  }
}

/** 
 * <JA>
 * ¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤ò¥E»¥Ã¥È¤¹¤E 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param tt [in] ¥E¼¥¯¥¨¥E¢ID (0 ¤Ş¤¿¤Ï 1)
 * </JA>
 * <EN>
 * Reset the token space.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param tt [in] work area id (0 or 1)
 * </EN>
 */
static void
clear_tlist(FSBeam *d, int tt)
{
  d->tnum[tt] = 0;
}

/** 
 * <JA>
 * ¥¢¥¯¥Æ¥£¥Ö¥È¡¼¥¯¥ó¥E¹¥È¤ò¥¯¥E¢¤¹¤E 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param tt [in] Ä¾Á°¤Î¥E¼¥¯¥¨¥E¢ID (0 ¤Ş¤¿¤Ï 1)
 * </JA>
 * <EN>
 * Clear the active token list.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param tt [in] work area id of previous frame (0 or 1)
 * </EN>
 */
static void
clear_tokens(FSBeam *d, int tt)
{
  int j;
  /* initialize active token list: only clear ones used in the last call */
  for (j=0; j<d->tnum[tt]; j++) {
    d->token[d->tlist[tt][j].node] = TOKENID_UNDEFINED;
  }
}

/** 
 * <JA>
 * ¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤«¤é¿·¤¿¤Ê¥È¡¼¥¯¥ó¤ò¼è¤ê¤À¤¹. 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * 
 * @return ¿·¤¿¤Ë¼è¤EĞ¤µ¤E¿¥È¡¼¥¯¥ó¤ÎID
 * </JA>
 * <EN>
 * Assign a new token from token space.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 
 * @return the id of the newly assigned token.
 * </EN>
 */
static TOKENID
create_token(FSBeam *d)
{
  TOKENID newid;
  int tn;
  tn = d->tn;
  newid = d->tnum[tn];
  d->tnum[tn]++;
  while (d->tnum[tn]>=d->maxtnum) expand_tlist(d);
  d->tindex[tn][newid] = newid;
#ifdef WPAIR
  /* initialize link */
  d->tlist[tn][newid].next = TOKENID_UNDEFINED;
#endif
  return(newid);
}

/** 
 * <JA>
 * @brief  ÌÚ¹½Â¤²½¼­½ñ¤Î¥Î¡¼¥É¤Ë¥È¡¼¥¯¥ó¤ò³ä¤EÕ¤±¤E 
 *
 * ÌÚ¹½Â¤²½¼­½ñ¤Î¥Î¡¼¥É¤Î¥¢¥¯¥Æ¥£¥Ö¥È¡¼¥¯¥ó¥E¹¥È¤Ë¥È¡¼¥¯¥ó¤òÊİÂ¸¤¹¤E 
 * ¤Ş¤¿¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤Ë¤ª¤¤¤Æ¥È¡¼¥¯¥ó¤«¤é¥Î¡¼¥ÉÈÖ¹æ¤Ø¤Î¥Eó¥¯¤òÊİÂ¸¤¹¤E 
 * 
 * ´û¤Ë¥È¡¼¥¯¥ó¤¬¤¢¤EEç¤Ï¡¤¿·¤¿¤Ê¥È¡¼¥¯¥ó¤Ë¤è¤Ã¤Æ¾å½ñ¤­¤µ¤EE ¤Ê¤ª
 * WPAIR »ØÄEş¤Ï¤½¤Î¥E¹¥È¤Ë¿·¤¿¤Ê¥È¡¼¥¯¥ó¤òÄÉ²Ã¤¹¤E 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param node [in] ÌÚ¹½Â¤²½¼­½ñ¤Î¥Î¡¼¥ÉÈÖ¹E * @param tkid [in] ¥È¡¼¥¯¥óÈÖ¹E * </JA>
 * <EN>
 * @brief  Assign token to a node on tree lexicon
 *
 * Save the token id to the specified node in the active token list.
 * Also saves the link to the node from the token in token space.
 *
 * If a token already exist on the node, it will be overridden by the new one.
 * If WPAIR is defined, the new token will be simply added to the list of
 * active tokens on the node.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param node [in] node id on the tree lexicon
 * @param tkid [in] token id to be assigned
 * </EN>
 */
static void
node_assign_token(FSBeam *d, int node, TOKENID tkid)
{
#ifdef WPAIR
  /* add to link list */
  d->tlist[d->tn][tkid].next = d->token[node];
#endif
  d->token[node] = tkid;
  d->tlist[d->tn][tkid].node = node;
}

/** 
 * <JA>
 * @brief  ÌÚ¹½Â¤²½¼­½ñ¾å¤Î¤¢¤EÎ¡¼¥É¤¬¡¤¸½ºß¤Ê¤ó¤é¤«¤Î¥È¡¼¥¯¥ó¤E * Êİ»ı¤·¤Æ¤¤¤E«¤ò¥Á¥§¥Ã¥¯¤¹¤E 
 *
 * WPAIR ¤¬ÄEÁ¤µ¤EÆ¤¤¤EEç¡¤¥Î¡¼¥É¤ÏÄ¾Á°Ã±¸E´¤È¤Ë°Û¤Ê¤EÈ¡¼¥¯¥ó¤òÊ£¿E * Êİ»ı¤¹¤E ¤³¤Î¾EE »ØÄê¤µ¤E¿Ã±¸ED¤òÄ¾Á°Ã±¸EÈ¤¹¤EÈ¡¼¥¯¥ó¤¬
 * ¤½¤Î¥Î¡¼¥É¤ËÊİ»ı¤µ¤EÆ¤¤¤E«¤É¤¦¤«¤¬¥Á¥§¥Ã¥¯¤µ¤EE ¤¹¤Ê¤EÁ¡¤´û¤Ë¥È¡¼¥¯¥E * ¤¬Â¸ºß¤·¤Æ¤â¡¤¤½¤Î¥È¡¼¥¯¥ó¤ÎÉ½¤¹¥Ñ¥¹¤ÎÄ¾Á°Ã±¸E¬»ØÄê¤·¤¿Ã±¸EÈ°Û¤Ê¤Ã¤Æ
 * ¤¤¤EĞÌ¤Êİ»ı (TOKENID_UNDEFINED) ¤òÊÖ¤¹. 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param tt [in] Ä¾Á°¤Î¥E¼¥¯¥¨¥E¢ID (0 ¤Ş¤¿¤Ï 1)
 * @param node [in] ¥Î¡¼¥ÉÈÖ¹E * @param wid [in] Ä¾Á°Ã±¸EÎID (WPAIRÄEÁ»ş¤Î¤ßÍ­¸E Â¾¤Ç¤ÏÌµ»Eµ¤EE
 *
 * @return ¤½¤Î¥Î¡¼¥É¤¬´û¤ËÊİ»ı¤¹¤EÈ¡¼¥¯¥óÈÖ¹æ¡¤Ìµ¤±¤EĞ TOKENID_UNDEFINED. 
 * </JA>
 * <EN>
 * @brief  Check if a node holds any token
 *
 * This function checks if a node on the tree lexicon already holds a token.
 *
 * If WPAIR is defined, a node has multiple tokens according to the previous
 * word context.  In this case, only token with the same previous word will be
 * checked.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param tt [in] work area id (0 or 1)
 * @param node [in] node id of lexicon tree
 * @param wid [in] word id of previous word (ignored if WPAIR is not defined)
 * 
 * @return the token id on the node, or TOKENID_UNDEFINED if no token has
 * been assigned in this frame.
 * </EN>
 */
static TOKENID
node_exist_token(FSBeam *d, int tt, int node, WORD_ID wid)
{
#ifdef WPAIR
  /* In word-pair mode, multiple tokens are assigned to a node as a list.
     so we have to search for tokens with same last word ID */
#ifdef WPAIR_KEEP_NLIMIT
  /* 1¥Î¡¼¥É¤´¤È¤ËÊİ»ı¤¹¤Eoken¿ô¤Î¾å¸Â¤òÀßÄE*/
  /* token¤¬Ìµ¤¤¤¬¾å¸Â¤ËÃ£¤·¤Æ¤¤¤EÈ¤­¤Ï°EÖ¥¹¥³¥¢¤ÎÄã¤¤token¤ò¾å½ñ¤­¤¹¤E*/
  /* N-best: limit number of assigned tokens to a node */
  int i = 0;
  TOKENID lowest_token = TOKENID_UNDEFINED;
#endif
  TOKENID tmp;
  for(tmp=d->token[node]; tmp != TOKENID_UNDEFINED; tmp=d->tlist[tt][tmp].next) {
    if (d->tlist[tt][tmp].last_tre->wid == wid) {
      return(tmp);
    }
#ifdef WPAIR_KEEP_NLIMIT
    if (lowest_token == TOKENID_UNDEFINED ||
	d->tlist[tt][lowest_token].score > d->tlist[tt][tmp].score)
      lowest_token = tmp;
    if (++i >= d->wpair_keep_nlimit) break;
#endif
  }
#ifdef WPAIR_KEEP_NLIMIT
  if (i >= d->wpair_keep_nlimit) { /* overflow, overwrite lowest score */
    return(lowest_token);
  } else {
    return(TOKENID_UNDEFINED);
  }
#else 
  return(TOKENID_UNDEFINED);
#endif
  
#else  /* not WPAIR */
  /* 1¤Ä¤À¤±Êİ»ı,¤³¤Eò¾EË¾å½ñ¤­ */
  /* Only one token is kept in 1-best mode (default), so
     simply return the ID */
  return(d->token[node]);
#endif
}

#ifdef DEBUG
/* tlist ¤È token ¤ÎÂĞ±ş¤ò¥Á¥§¥Ã¥¯¤¹¤Edebug) */
/* for debug: check tlist <-> token correspondence
   where  tlist[tt][tokenID].node = nodeID and
          token[nodeID] = tokenID
 */
static void
node_check_token(FSBeam *d, int tt)
{
  int i;
  for(i=0;i<d->tnum[tt];i++) {
    if (node_exist_token(d, tt, d->tlist[tt][i].node, d->tlist[tt][i].last_tre->wid) != i) {
      jlog("ERROR: token %d not found on node %d\n", i, d->tlist[tt][i].node);
    }
  }
}
#endif



/* -------------------------------------------------------------------- */
/*       ¥È¡¼¥¯¥ó¤ò¥½¡¼¥È¤· ¾å°Ì N ¥È¡¼¥¯¥ó¤òÈ½ÊÌ¤¹¤E(heap sort)       */
/*        Sort generated tokens and get N-best (use heap sort)          */
/* -------------------------------------------------------------------- */
/* ¥Ó¡¼¥à¤ÎEÃÍ¤È¤·¤Æ¾å°Ì N ÈÖÌÜ¤Î¥¹¥³¥¢¤¬Íß¤·¤¤¤À¤±¤Ç¤¢¤ê¡¤¼Âºİ¤Ë¥½¡¼¥È
   ¤µ¤EE¬Í×¤Ï¤Ê¤¤ */
/* we only want to know the N-th score for determining beam threshold, so
   order is not considered here */

#define SD(A) tindex_local[A-1]	///< Index locater for sort_token_*()
#define SCOPY(D,S) D = S	///< Content copier for sort_token_*()
#define SVAL(A) (tlist_local[tindex_local[A-1]].score) ///< Score locater for sort_token_*()
#define STVAL (tlist_local[s].score) ///< Indexed score locater for sort_token_*()

/** 
 * <JA>
 * @brief  ¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤ò¥¹¥³¥¢¤ÎÂç¤­¤¤½ç¤Ë¥½¡¼¥È¤¹¤E 
 *
 * heap sort ¤òÍÑ¤¤¤Æ¸½ºß¤Î¥È¡¼¥¯¥ó½¸¹ç¤ò¥¹¥³¥¢¤ÎÂç¤­¤¤½ç¤Ë¥½¡¼¥È¤¹¤E 
 * ¾å°Ì @a neednum ¸Ä¤Î¥È¡¼¥¯¥ó¤¬¥½¡¼¥È¤µ¤EEĞ¤½¤³¤Ç½èÍı¤ò½ªÎ»¤¹¤E 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param neednum [in] ¾å°Ì @a neednum ¸Ä¤¬ÆÀ¤é¤EEŞ¤Ç¥½¡¼¥È¤¹¤E * @param totalnum [in] ¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹Æâ¤ÎÍ­¸ú¤Ê¥È¡¼¥¯¥ó¿E * </JA>
 * <EN>
 * @brief  Sort the token space upward by score.
 *
 * This function sort the whole token space in upward direction, according
 * to their accumulated score.
 * This function terminates sort as soon as the top
 * @a neednum tokens has been found.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param neednum [in] sort until top @a neednum tokens has been found
 * @param totalnum [in] total number of assigned tokens in the token space
 * </EN>
 */
static void
sort_token_upward(FSBeam *d, int neednum, int totalnum)
{
  int n,root,child,parent;
  TOKENID s;
  TOKEN2 *tlist_local;
  TOKENID *tindex_local;

  tlist_local = d->tlist[d->tn];
  tindex_local = d->tindex[d->tn];

  for (root = totalnum/2; root >= 1; root--) {
    SCOPY(s, SD(root));
    parent = root;
    while ((child = parent * 2) <= totalnum) {
      if (child < totalnum && SVAL(child) < SVAL(child+1)) {
	child++;
      }
      if (STVAL >= SVAL(child)) {
	break;
      }
      SCOPY(SD(parent), SD(child));
      parent = child;
    }
    SCOPY(SD(parent), s);
  }
  n = totalnum;
  while ( n > totalnum - neednum) {
    SCOPY(s, SD(n));
    SCOPY(SD(n), SD(1));
    n--;
    parent = 1;
    while ((child = parent * 2) <= n) {
      if (child < n && SVAL(child) < SVAL(child+1)) {
	child++;
      }
      if (STVAL >= SVAL(child)) {
	break;
      }
      SCOPY(SD(parent), SD(child));
      parent = child;
    }
    SCOPY(SD(parent), s);
  }
}

/** 
 * <JA>
 * @brief  ¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤ò¥¹¥³¥¢¤Î¾®¤µ¤¤½ç¤Ë¥½¡¼¥È¤¹¤E 
 *
 * ¥Ó¡¼¥à¤Î¤·¤­¤¤ÃÍ·èÄê¤Î¤¿¤á¤Ë¡¤heap sort ¤òÍÑ¤¤¤Æ
 * ¸½ºß¤Î¥È¡¼¥¯¥ó½¸¹ç¤ò¥¹¥³¥¢¤Î¾®¤µ¤¤½ç¤Ë¥½¡¼¥È¤¹¤E 
 * ²¼°Ì @a neednum ¸Ä¤Î¥È¡¼¥¯¥ó¤¬¥½¡¼¥È¤µ¤EEĞ¤½¤³¤Ç½èÍı¤ò½ªÎ»¤¹¤E 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param neednum [in] ²¼°Ì @a neednum ¸Ä¤¬ÆÀ¤é¤EEŞ¤Ç¥½¡¼¥È¤¹¤E * @param totalnum [in] ¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹Æâ¤ÎÍ­¸ú¤Ê¥È¡¼¥¯¥ó¿E * </JA>
 * <EN>
 * @brief  Sort the token space downward by score.
 *
 * This function sort the whole token space in downward direction, according
 * to their accumulated score for hypothesis pruning.
 *
 * This function terminates sort as soon as the bottom
 * @a neednum tokens has been found.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param neednum [in] sort until bottom @a neednum tokens has been found
 * @param totalnum [in] total number of assigned tokens in the token space
 * </EN>
 */
static void
sort_token_downward(FSBeam *d, int neednum, int totalnum)
{
  int n,root,child,parent;
  TOKENID s;
  TOKEN2 *tlist_local;
  TOKENID *tindex_local;

  tlist_local = d->tlist[d->tn];
  tindex_local = d->tindex[d->tn];

  for (root = totalnum/2; root >= 1; root--) {
    SCOPY(s, SD(root));
    parent = root;
    while ((child = parent * 2) <= totalnum) {
      if (child < totalnum && SVAL(child) > SVAL(child+1)) {
	child++;
      }
      if (STVAL <= SVAL(child)) {
	break;
      }
      SCOPY(SD(parent), SD(child));
      parent = child;
    }
    SCOPY(SD(parent), s);
  }
  n = totalnum;
  while ( n > totalnum - neednum) {
    SCOPY(s, SD(n));
    SCOPY(SD(n), SD(1));
    n--;
    parent = 1;
    while ((child = parent * 2) <= n) {
      if (child < n && SVAL(child) > SVAL(child+1)) {
	child++;
      }
      if (STVAL <= SVAL(child)) {
	break;
      }
      SCOPY(SD(parent), SD(child));
      parent = child;
    }
    SCOPY(SD(parent), s);
  }
}

/** 
 * <JA>
 * @brief ¥È¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤ò¥½¡¼¥È¤·¤Æ¥Ó¡¼¥àÆâ¤Ë»Ä¤EÈ¡¼¥¯¥ó¤ò·èÄê¤¹¤E * 
 * heap sort ¤òÍÑ¤¤¤Æ¸½ºß¤Î¥È¡¼¥¯¥ó½¸¹ç¤ò¥½¡¼¥È¤·¡¤¾å°Ì¥¹¥³¥¢¤Î¥È¡¼¥¯¥E * ½¸¹ç¤òµá¤á¤E ¾å°Ì @a neednum ¸Ä¤Î¥È¡¼¥¯¥ó½¸¹ç¤¬ÆÀ¤é¤EEĞÎÉ¤¤¤Î¤Ç¡¤
 * Á´ÂÎ¤¬´°Á´¤Ë¥½¡¼¥È¤µ¤EÆ¤¤¤E¬Í×¤Ï¤Ê¤¤. ¤è¤Ã¤Æ
 * ¾å°Ì @a neednum ¸Ä¤Î¥È¡¼¥¯¥ó¤Î¤ß¤ò¥½¡¼¥È¤¹¤E ¼Âºİ¤Ë¤Ï¡¤Á´ÂÎ¤Î¥È¡¼¥¯¥E * ¿ô¤ÈÉ¬Í×¤Ê¥È¡¼¥¯¥ó¿ô¤«¤Esort_token_upward()
 * ¤È sort_token_downward() ¤ÎÁá¤¤Êı¤¬ÍÑ¤¤¤é¤EE 
 * 
 * @param d [i/o] ÂE¥Ñ¥¹Ãµº÷½èÍıÍÑ¥E¼¥¯¥¨¥E¢
 * @param neednum [in] µá¤á¤Eå°Ì¥È¡¼¥¯¥ó¤Î¿E * @param start [out] ¾å°Ì @a neednum ¤Î¥È¡¼¥¯¥ó¤¬Â¸ºß¤¹¤EÈ¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤ÎºÇ½é¤Î¥¤¥ó¥Ç¥Ã¥¯¥¹ÈÖ¹E * @param end [out] ¾å°Ì @a neednum ¤Î¥È¡¼¥¯¥ó¤¬Â¸ºß¤¹¤EÈ¡¼¥¯¥ó¥¹¥Ú¡¼¥¹¤ÎºÇ¸å¤Î¥¤¥ó¥Ç¥Ã¥¯¥¹ÈÖ¹E * </JA>
 * <EN>
 * @brief Sort the token space to find which tokens to be survived in the beam
 *
 * This function sorts the currrent tokens in the token space to find
 * the tokens to be survived in the current frame.  Only getting the top
 * @a neednum tokens is required, so the sort will be terminate just after
 * the top @a neednum tokens are determined.  Actually, either
 * sort_token_upward() or sort_token_downward() will be used depending of
 * the number of needed tokens and total tokens.
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param neednum [in] number of top tokens to be found
 * @param start [out] start index of the top @a neednum nodes
 * @param end [out] end index of the top @a neednum nodes
 * </EN>
 */
static void
sort_token_no_order(FSBeam *d, int neednum, int *start, int *end)
{
  int totalnum;
  int restnum;

  totalnum = d->tnum[d->tn];

  restnum = totalnum - neednum;

  if (neednum >= totalnum) {
    /* no need to sort */
    *start = 0;
    *end = totalnum - 1;
  } else if (neednum < restnum)  {
    /* needed num is smaller than rest, so sort for the needed tokens */
    sort_token_upward(d, neednum,totalnum);
    *start = totalnum - neednum;
    *end = totalnum - 1;
  } else {
    /* needed num is bigger than rest, so sort for the rest token */
    sort_token_downward(d, restnum,totalnum);
    *start = 0;
    *end = neednum - 1;
  }
}

/* -------------------------------------------------------------------- */
/*             Âè£±¥Ñ¥¹(¥Õ¥E¼¥àÆ±´EÓ¡¼¥à¥µ¡¼¥Á) ¥á¥¤¥E               */
/*           main routines of 1st pass (frame-synchronous beam search)  */
/* -------------------------------------------------------------------- */

/** 
 * <JA>
 * @brief  ½é´E¾Àâ¤ÎÀ¸À®
 *
 * ½é´E¾Àâ¤Ï¡¤N-gram¤Ç¤Ï winfo->head_silwid ¤Ë¸ÇÄê¤µ¤EÆ¤¤¤E DFA ¤Ç¤Ï
 * Ê¸Ë¡¾åÊ¸Æ¬¤Ë¤­¤¦¤E±¸E¹¤Ù¤Æ¤¬½é´E¾Àâ¤È¤Ê¤E Ã±¸E§¼±¥â¡¼¥É¤Ç¤Ï
 * Á´¤Æ¤ÎÃ±¸E¬½é´E¾Àâ¤È¤Ê¤E 
 *
 * ²»¶Á¥â¥Ç¥E¬Èómultipath¤Î¾Eç¡¤¤³¤³¤ÇºÇ½é¤Î¥Õ¥E¼¥à¤Î½ĞÎÏ³ÎÎ¨
 * ·×»»¤Ş¤Ç¹Ô¤EEE 
 * 
 * @param param [in] ÆşÎÏ¥Ù¥¯¥È¥Eó¾ğÊEºÇ½é¤Î¥Õ¥E¼¥à¤Î¤ßÉ¬Í×)
 * @param r [in] ²»À¼Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * </JA>
 * <EN>
 * @brief  Generate initial hypotheses
 *
 * The initial hypothesis is: 1) winfo->head_silwid for N-gram, 2) all words
 * that can be at beginning of sentence for DFA, or 3) all words in dictionary
 * for isolated word recognition mode.
 *
 * If acoustic model is NOT a multi-path model, the output probabilities for
 * the first frame (t=0) will also be computed in this function.
 * 
 * @param param [in] input vectors (only the first frame will be used)
 * @param r [in] recognition process instance
 * </EN>
 */
static boolean
init_nodescore(HTK_Param *param, RecogProcess *r)
{
  WCHMM_INFO *wchmm;
  FSBeam *d;
  TOKENID newid;
  TOKEN2 *new;
  WORD_ID beginword;
  int node;
  int i;

  wchmm = r->wchmm;
  d = &(r->pass1);

  /* ½é´E¾ÀâÍÑÃ±¸EúÎE*/
  /* setup initial word context */
  if (r->config->successive.enabled) { /* sp segment mode */
    /* initial word context = last non-sp word of previous 2nd pass at last segment*/
    if (r->lmtype == LM_PROB) {
      if (r->sp_break_last_nword == wchmm->winfo->tail_silwid) {
	/* if end with silE, initialize as normal start of sentence */
	d->bos.wid = WORD_INVALID;
      } else {
	d->bos.wid = r->sp_break_last_nword;
      }
    } else {
      d->bos.wid = WORD_INVALID;
    }
  } else {			/* not sp segment mode */
    d->bos.wid = WORD_INVALID;	/* no context */
  }

  d->bos.begintime = d->bos.endtime = -1;

  /* ¥Î¡¼¥É¡¦¥È¡¼¥¯¥ó¤ò½é´E½ */
  /* clear tree lexicon nodes and tokens */
  for(node = 0; node < d->totalnodenum; node++) {
    d->token[node] = TOKENID_UNDEFINED;
  }
  d->tnum[0] = d->tnum[1]  = 0;
  
#ifdef PASS1_IWCD
  /* ½ĞÎÏ³ÎÎ¨·×»»¥­¥ã¥Ã¥·¥å¤ò½é´E½ */
  /* initialize outprob cache */
  outprob_style_cache_init(wchmm);
#endif

  /* ½é´E¾Àâ¤ÎºûÜ®: ½é´E±¸EÎ·èÄê¤È½é´EÈ¡¼¥¯¥ó¤ÎÀ¸À® */
  /* initial word hypothesis */

  if (r->lmtype == LM_PROB) {

    if (r->config->successive.enabled) { /* sp segment mode */
      if (r->sp_break_last_word != WORD_INVALID) { /* last segment exist */
	/* ³«»ÏÃ±¸EáÁ°¤Î¥»¥°¥á¥ó¥È·×»»»ş¤ÎºÇ¸å¤ÎºÇÌàÃ±¸E*/
	/* Ê¸½ªÎ»Ã±¸EsilE,¶çÅÀ(IPA¥â¥Ç¥E)¤Ê¤é¡¤silB ¤Ç³«»Ï */
	/* initial word = best last word hypothesis on the last segment */
	/* if silE or sp, begin with silB */
	/*if (is_sil(recog.sp_break_last_word, wchmm->winfo, wchmm->hmminfo)) {*/
	if (r->sp_break_last_word == wchmm->winfo->tail_silwid) {
	  beginword = wchmm->winfo->head_silwid;
	  d->bos.wid = WORD_INVALID;	/* reset initial word context */
	} else {
	  beginword = r->sp_break_last_word;
	}
      } else {
	/* initial segment: initial word set to silB */
	beginword = wchmm->winfo->head_silwid;
      }
    } else {			/* not sp segment mode */
      /* initial word fixed to silB */
      beginword = wchmm->winfo->head_silwid;
    }

#ifdef SP_BREAK_DEBUG
    jlog("DEBUG: startword=[%s], last_nword=[%s]\n",
	   (beginword == WORD_INVALID) ? "WORD_INVALID" : wchmm->winfo->wname[beginword],
	   (d->bos.wid == WORD_INVALID) ? "WORD_INVALID" : wchmm->winfo->wname[d->bos.wid]);
#endif
    /* create the first token at the first node of initial word */
    newid = create_token(d);
    new = &(d->tlist[d->tn][newid]);

    /* initial node = head node of the beginword */
    if (wchmm->hmminfo->multipath) {
      node = wchmm->wordbegin[beginword];
    } else {
      node = wchmm->offset[beginword][0];
    }

    /* set initial LM score */
    if (wchmm->state[node].scid != 0) {
      /* if initial node is on a factoring branch, use the factored score */
      new->last_lscore = max_successor_prob(wchmm, d->bos.wid, node);
    } else {
      /* else, set to 0.0 */
      new->last_lscore = 0.0;
    }
#ifdef FIX_PENALTY
    new->last_lscore = new->last_lscore * d->lm_weight;
#else
    new->last_lscore = new->last_lscore * d->lm_weight + d->lm_penalty;
#endif
    /* set initial word history */
    new->last_tre = &(d->bos);
    new->last_cword = d->bos.wid;
    if (wchmm->hmminfo->multipath) {
      /* set initial score using the initial LM score */
      new->score = new->last_lscore;
    } else {
      /* set initial score using the initial LM score and AM score of the first state */
      new->score = outprob_style(wchmm, node, d->bos.wid, 0, param) + new->last_lscore;
    }
    /* assign the initial node to token list */
    node_assign_token(d, node, newid);

  }

  if (r->lmtype == LM_DFA && r->lmvar == LM_DFA_GRAMMAR) {
  
    /* ½é´E¾ÀE Ê¸Ë¡¾åÊ¸Æ¬¤ËÀÜÂ³¤·¤¦¤E±¸E¸¹E*/
    /* initial words: all words that can be begin of sentence grammatically */
    /* ¥¢¥¯¥Æ¥£¥Ö¤ÊÊ¸Ë¡¤ËÂ°¤¹¤E±¸EÎ¤ßµö¤¹ */
    /* only words in active grammars are allowed to be an initial words */
    MULTIGRAM *m;
    int t,tb,te;
    WORD_ID iw;
    boolean flag;
    DFA_INFO *gdfa;

    gdfa = r->lm->dfa;

    flag = FALSE;
    /* for all active grammar */
    for(m = r->lm->grammars; m; m = m->next) {
      if (m->active) {
	tb = m->cate_begin;
	te = tb + m->dfa->term_num;
	for(t=tb;t<te;t++) {
	  /* for all word categories that belong to the grammar */
	  if (dfa_cp_begin(gdfa, t) == TRUE) {
	    /* if the category can appear at beginning of sentence, */
	    flag = TRUE;
	    for (iw=0;iw<gdfa->term.wnum[t];iw++) {
	      /* create the initial token at the first node of all words that belongs to the category */
	      i = gdfa->term.tw[t][iw];
	      if (wchmm->hmminfo->multipath) {
		node = wchmm->wordbegin[i];
	      } else {
		node = wchmm->offset[i][0];
	      }
	      /* in tree lexicon, words in the same category may share the same root node, so skip it if the node has already existed */
	      if (node_exist_token(d, d->tn, node, d->bos.wid) != TOKENID_UNDEFINED) continue;
	      newid = create_token(d);
	      new = &(d->tlist[d->tn][newid]);
	      new->last_tre = &(d->bos);
#ifdef FIX_PENALTY
	      new->last_lscore = 0.0;
#else
	      new->last_lscore = d->penalty1;
#ifdef CLASS_NGRAM
	      /* add per-word penalty */
	      new->last_lscore += wchmm->winfo->cprob[i];
#endif
#endif
	      if (wchmm->hmminfo->multipath) {
		new->score = new->last_lscore;
	      } else {
		new->score = outprob_style(wchmm, node, d->bos.wid, 0, param) + new->last_lscore;
	      }
	      node_assign_token(d, node, newid);
	    }
	  }
	}
      }
    }
    if (!flag) {
      jlog("ERROR: init_nodescore: no initial state found in active DFA grammar\n");
      return FALSE;
    }
  }

  if (r->lmtype == LM_DFA && r->lmvar == LM_DFA_WORD) {
    /* ¥¢¥¯¥Æ¥£¥Ö¤ÊÊ¸Ë¡¤ËÂ°¤¹¤E±¸EÎ¤ßµö¤¹ */
    /* only words in active grammars are allowed to be an initial words */
    MULTIGRAM *m;

    for(m = r->lm->grammars; m; m = m->next) {
      if (m->active) {
	for(i = m->word_begin; i < m->word_begin + m->winfo->num; i++) {
	  if (wchmm->hmminfo->multipath) {
	    node = wchmm->wordbegin[i];
	  } else {
	    node = wchmm->offset[i][0];
	  }
	  if (node_exist_token(d, d->tn, node, d->bos.wid) != TOKENID_UNDEFINED) continue;
	  newid = create_token(d);
	  new = &(d->tlist[d->tn][newid]);
	  new->last_tre = &(d->bos);
	  new->last_lscore = 0.0;
	  if (wchmm->hmminfo->multipath) {
	    new->score = 0.0;
	  } else {
	    new->score = outprob_style(wchmm, node, d->bos.wid, 0, param);
	  }
	  node_assign_token(d, node, newid);
	}
      }
    }
  }

  return TRUE;

}

/******************************************************/
/* ¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤Î¼Â¹Ô --- ºÇ½é¤Î¥Õ¥E¼¥àÍÑ  */
/* frame synchronous beam search --- first frame only */
/******************************************************/

/** 
 * <JA>
 * @brief  ¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤Î½é´E½
 *
 * ¤³¤³¤Ç¤Ï¥Ó¡¼¥à¥µ¡¼¥Á¤ËÍÑ¤¤¤EE¼¥¯¥¨¥E¢¤Î³ÎÊİ¤È½é´E½¤ò¹Ô¤¦. 
 * ½é´E½Àâ¤ÎÀ¸À®¤Ï init_nodescore() ¤Ç¹Ô¤EEE 
 * 
 * @param param [in] ÆşÎÏ¥Ù¥¯¥È¥Eó¾ğÊE(ºÇ½é¤Î£±¥Õ¥E¼¥àÌÜ¤Î¤ßÍÑ¤¤¤é¤EE
 * @param r [i/o] ²»À¼Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * </JA>
 * <EN>
 * @brief  Initialization of the frame synchronous beam search
 *
 * This function will initialize work area for the 1st pass.
 * Generation of initial hypotheses will be performed in init_nodescore().
 * 
 * @param param [in] input vectors (only the first frame will be used)
 * @param r [i/o] recognition process instance
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
boolean
get_back_trellis_init(HTK_Param *param,	RecogProcess *r)
{
  WCHMM_INFO *wchmm;
  BACKTRELLIS *backtrellis;
  FSBeam *d;

  wchmm = r->wchmm;
  backtrellis = r->backtrellis;
  d = &(r->pass1);

  /* Viterbi±é»»ÍÑ¥E¼¥¯¥¨¥E¢¤Î¥¹¥¤¥Ã¥Á¥ã¡¼ tl,tn ¤Î½é´EÍÀßÄE*/
  /* tn: ¤³¤Î¥Õ¥E¼¥àÍÑID   tl: £±¥Õ¥E¼¥àÁ°¤ÎID */
  /* initialize switch tl, tn for Viterbi computation */
  /* tn: this frame  tl: last frame */
  d->tn = 0;
  d->tl = 1;

  /* ·EÌ¤ÎÃ±¸EÈ¥EE¹¤ò³ÊÇ¼¤¹¤EĞ¥Ã¥¯¥È¥EE¹¹½Â¤ÂÎ¤ò½é´E½ */
  /* initialize backtrellis structure to store resulting word trellis */
  bt_prepare(backtrellis);

  /* ·×»»ÍÑ¥E¼¥¯¥¨¥E¢¤ò½é´E½ */
  /* initialize some data on work area */

  if (r->lmtype == LM_PROB) {
    d->lm_weight  = r->config->lmp.lm_weight;
    d->lm_penalty = r->config->lmp.lm_penalty;
  }
  if (r->lmtype == LM_DFA) {
    d->penalty1 = r->config->lmp.penalty1;
  }
#if defined(WPAIR) && defined(WPAIR_KEEP_NLIMIT)
  d->wpair_keep_nlimit = r->config->pass1.wpair_keep_nlimit;
#endif

  /* ¥E¼¥¯¥¨¥E¢¤ò³ÎÊİ */
  /* malloc work area */
  /* »ÈÍÑ¤¹¤EÈ¡¼¥¯¥óÎÌ = viterbi»ş¤ËÁ«°ÜÀè¤È¤Ê¤EõÂÖ¸õÊä¤Î¿E   * Í½Â¬: ¥Ó¡¼¥à¿Ex 2 (¼«¸ÊÁ«°Ü+¼¡¾õÂÖ) + ÌÚ¹½Â¤²½¼­½ñ¤Î¥E¼¥È¥Î¡¼¥É¿E   */
  /* assumed initial number of needed tokens: beam width x 2 (self + next trans.)
   * + root node on the tree lexicon (for inter-word trans.)
   */
  if (d->totalnodenum != wchmm->n) {
    free_nodes(d);
  }
  if (d->nodes_malloced == FALSE) {
    malloc_nodes(d, wchmm->n, r->trellis_beam_width * 2 + wchmm->startnum);
  }
  prepare_nodes(d, r->trellis_beam_width);
  
  /* ½é´E¹¥³¥¢¤Enodescore[tn] ¤Ë¥»¥Ã¥È */
  /* set initial score to nodescore[tn] */
  if (init_nodescore(param, r) == FALSE) {
    jlog("ERROR: get_back_trellis_init: failed to set initial node scores\n");
    return FALSE;
  }

  sort_token_no_order(d, r->trellis_beam_width, &(d->n_start), &(d->n_end));

  /* Á²¼¡½ĞÎÏ¤ò¹Ô¤Ê¤¦¾Eç¤Î¥¤¥ó¥¿¡¼¥Ğ¥Eò·×»» */
  /* set interval frame for progout */
  r->config->output.progout_interval_frame = (int)((float)r->config->output.progout_interval / ((float)param->header.wshift / 10000.0));

  if (r->config->successive.enabled) {
    /* ¥·¥ç¡¼¥È¥İ¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥óÍÑ¥Ñ¥é¥á¡¼¥¿¤Î½é´E½ */
    /* initialize parameter for short pause segmentation */
    d->in_sparea = TRUE;		/* assume beginning is silence */
    r->am->mfcc->sparea_start = d->tmp_sparea_start = 0; /* set start frame to 0 */
#ifdef SP_BREAK_RESUME_WORD_BEGIN
    d->tmp_sp_break_last_word = WORD_INVALID;
#endif
    r->sp_break_last_word = WORD_INVALID;
    /* ºÇ½é¤Î¥»¥°¥á¥ó¥È: ¼¡¤ÎÈó¥İ¡¼¥º¥Õ¥E¼¥à¤ÇÂE¥Ñ¥¹¤Ø°Ü¹Ô¤·¤Ê¤¤ */
    /* the first end of pause segment should be always silB, so
       skip the first segment */
    d->first_sparea = TRUE;
    r->sp_break_2_begin_word = WORD_INVALID;
  }

#ifdef DETERMINE
  if (r->lmvar == LM_DFA_WORD) {
    /* initialize */
    determine_word(r, 0, NULL, 0, 0);
  }
#endif

#ifdef SCORE_PRUNING
  d->score_pruning_threshold = LOG_ZERO;
  d->score_pruning_count = 0;
#endif

  return TRUE;
}

/*****************************************************/
/* frame synchronous beam search --- proceed 1 frame */
/* ¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤Î¼Â¹Ô --- 1¥Õ¥E¼¥à¿Ê¤á¤E */
/*****************************************************/

/** 
 * <EN>
 * Propagate a token to next node.
 * 
 * </EN>
 * <JA>
 * ¥È¡¼¥¯¥ó¤ò¼¡¥Î¡¼¥É¤ËÅÁÈÂ¤¹¤E 
 * 
 * </JA>
 * 
 * @param d [i/o] work area for 1st pass recognition processing
 * @param next_node [in] next node id
 * @param next_score [in] score when transmitted to the next node
 * @param last_tre [in] previous word context for the next node
 * @param last_cword [in] previous context-valid word for the next node
 * @param last_lscore [in] LM score to be propagated
 * 
 */
static void
propagate_token(FSBeam *d, int next_node, LOGPROB next_score, TRELLIS_ATOM *last_tre, WORD_ID last_cword, LOGPROB last_lscore)
{
  TOKEN2 *tknext;
  TOKENID tknextid;

  if ((tknextid = node_exist_token(d, d->tn, next_node, last_tre->wid)) != TOKENID_UNDEFINED) {
    /* Á«°ÜÀè¥Î¡¼¥É¤Ë¤Ï´û¤ËÂ¾¥Î¡¼¥É¤«¤éÅÁÈÂºÑ¤ß: ¥¹¥³¥¢¤¬¹â¤¤¤Û¤¦¤ò»Ä¤¹ */
    /* the destination node already has a token: compare score */
    tknext = &(d->tlist[d->tn][tknextid]);
    if (tknext->score < next_score) {
      /* ¤½¤ÎÁ«°ÜÀè¥Î¡¼¥É¤¬»ı¤Ä¥È¡¼¥¯¥ó¤ÎÆâÍÆ¤ò¾å½ñ¤­¤¹¤E¿·µ¬¥È¡¼¥¯¥ó¤ÏºûÀé¤Ê¤¤) */
      /* overwrite the content of existing destination token: not create a new token */
      tknext->last_tre = last_tre; /* propagate last word info */
      tknext->last_cword = last_cword; /* propagate last context word info */
      tknext->last_lscore = last_lscore; /* set new LM score */
      tknext->score = next_score; /* set new score */
    }
  } else {
    /* Á«°ÜÀè¥Î¡¼¥É¤ÏÌ¤ÅÁÈÂ: ¿·µ¬¥È¡¼¥¯¥ó¤òºûÀÃ¤Æ³ä¤EÕ¤±¤E*/
    /* token unassigned: create new token and assign */
    if (next_score > LOG_ZERO) { /* valid token */
      tknextid = create_token(d); /* get new token */
    }
    tknext = &(d->tlist[d->tn][tknextid]);
    tknext->last_tre = last_tre; /* propagate last word info */
    tknext->last_cword = last_cword; /* propagate last context word info */
    tknext->last_lscore = last_lscore;
    tknext->score = next_score; /* set new score */
    node_assign_token(d, next_node, tknextid); /* assign this new token to the next node */
  }
}

/** 
 * <JA>
 * Ã±¸Eâ¤Î¤¢¤EÎ¡¼¥É´Ö¤ÎÁ«°Ü¤ò¹Ô¤¦. 
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * @param d [i/o] ÂE¥Ñ¥¹¥E¼¥¯¥¨¥E¢
 * @param tk_ret [i/o] ÅÁÈÂ¸µ¤Î¥È¡¼¥¯¥ó¡ÊÆâÉô¤Ç¥İ¥¤¥ó¥¿¹¹¿·»ş¤Ï¾å½ñ¤­¡Ë
 * @param j [in] @a tk_ret ¤Î¸µ¤Î¥È¡¼¥¯¥ó¥E¹¥È¤ÎID
 * @param next_node [in] Á«°ÜÀè¤Î¥Î¡¼¥ÉÈÖ¹E * @param next_a [in] Á«°Ü³ÎÎ¨
 * </JA>
 * <EN>
 * Word-internal transition for a set of nodes.
 * 
 * @param wchmm [in] tree lexicon
 * @param d [i/o] work area for the 1st pass
 * @param tk_ret [in] source token (if pointer updated, overwrite this)
 * @param j [in] the token ID of @a tk_ret
 * @param next_node [in] id of next node
 * @param next_a [in] transition probability
 * 
 * </EN>
 */
static void
beam_intra_word_core(WCHMM_INFO *wchmm, FSBeam *d, TOKEN2 **tk_ret, int j, int next_node, LOGPROB next_a)
{
  int node; ///< Temporal work to hold the current node number on the lexicon tree
  LOGPROB tmpsum;
  LOGPROB ngram_score_cache;
  TOKEN2 *tk;

  tk = *tk_ret;

  node = tk->node;

  /* now, 'node' is the source node, 'next_node' is the destication node,
     and ac-> holds transition probability */
  /* tk->score is the accumulated score at the 'node' on previous frame */
  
  /******************************************************************/
  /* 2.1.1 Á«°ÜÀè¤Ø¤Î¥¹¥³¥¢·×»»(Á«°Ü³ÎÎ¨¡Ü¸À¸E¹¥³¥¢)               */
  /*       compute score of destination node (transition prob + LM) */
  /******************************************************************/
  tmpsum = tk->score + next_a;
  ngram_score_cache = LOG_ZERO;
  /* the next score at 'next_node' will be computed on 'tmpsum', and
     the new LM probability (if updated) will be stored on 'ngram_score_cache' at below */
  
  if (!wchmm->category_tree) {
    /* ¸À¸E¹¥³¥¢ factoring:
       arc¤¬¼«¸ÊÁ«°Ü¤Ç¤Ê¤¤Ã±¸Eâ¤ÎÁ«°Ü¤Ç¡¤¤«¤ÄÁ«°ÜÀè¤Ësuccessor¥E¹¥È
       ¤¬¤¢¤EĞ¡¤lexicon tree ¤ÎÊ¬´ôÉôÊ¬¤ÎÁ«°Ü¤Ç¤¢¤E*/
    /* LM factoring:
       If this is not a self transition and destination node has successor
       list, this is branching transition
    */
    if (next_node != node) {
      if (wchmm->state[next_node].scid != 0
#ifdef UNIGRAM_FACTORING
	  /* 1-gram factoring »ÈÍÑ»ş¤Ï, Ê£¿ô¤Ç¶¦Í­¤µ¤EEŞ¤Ç¤Ï
	     wchmm->state[node].scid ¤ÏÉé¤ÎÃÍ¤È¤Ê¤ê¡¤¤½¤ÎÀäÂĞÃÍ¤E	     Åº»ú¤È¤·¤Æ wchmm->fscore[] ¤ËÃ±¸E¸¹ç¤Î1-gram¤ÎºÇÂçÃÍ¤¬³ÊÇ¼
	     ¤µ¤EÆ¤¤¤E ËöÃ¼¤Î»Ş(Ê£¿ôÃ±¸EÇ¶¦Í­¤µ¤EÊ¤¤)¤Ç¤Ï¡¤
	     wchmm->state[node].scid ¤ÏÀµ¤ÎÃÍ¤È¤Ê¤ê¡¤
	     £±Ã±¸EEsc ¤È¤·¤Æ»ı¤Ä¤Î¤Ç¤½¤³¤ÇÀµ³Î¤Ê2-gram¤ò·×»»¤¹¤E*/
	  /* When uni-gram factoring,
	     wchmm->state[node].scid is below 0 for shared branches.
	     In this case the maximum uni-gram probability for sub-tree
	     is stored in wchmm->fscore[- wchmm->state[node].scid].
	     Leaf branches (with only one successor word): the scid is
	     larger than 0, and has
	     the word ID in wchmm->sclist[wchmm->state[node].scid].
	     So precise 2-gram is computed in this point */
#endif
	  ){
	
	if (wchmm->lmtype == LM_PROB) {
	  /* ¤³¤³¤Ç¸À¸Eâ¥Ç¥EÎÎ¨¤ò¹¹¿·¤¹¤E*/
	  /* LM value should be update at this transition */
	  /* N-gram³ÎÎ¨¤«¤éfactoring ÃÍ¤ò·×»» */
	  /* compute new factoring value from N-gram probabilities */
#ifdef FIX_PENALTY
	  /* if at the beginning of sentence, not add lm_penalty */
	  if (tk->last_cword == WORD_INVALID) {
	    ngram_score_cache = max_successor_prob(wchmm, tk->last_cword, next_node) * d->lm_weight;
	  } else {
	    ngram_score_cache = max_successor_prob(wchmm, tk->last_cword, next_node) * d->lm_weight + d->lm_penalty;
	  }
#else
	  ngram_score_cache = max_successor_prob(wchmm, tk->last_cword, next_node) * d->lm_weight + d->lm_penalty;
#endif
	  /* ¥¹¥³¥¢¤Î¹¹¿·: tk->last_lscore ¤ËÃ±¸Eâ¤Ç¤ÎºÇ¸å¤ÎfactoringÃÍ¤¬
	     Æş¤Ã¤Æ¤¤¤EÎ¤Ç, ¤½¤Eò¥¹¥³¥¢¤«¤é°ú¤¤¤Æ¥E»¥Ã¥È¤·, ¿·¤¿¤Ê¥¹¥³¥¢¤E	     ¥»¥Ã¥È¤¹¤E*/
	  /* update score: since 'tk->last_lscore' holds the last LM factoring
	     value in this word, we first remove the score from the current
	     score, and then add the new LM value computed above. */
	  tmpsum -= tk->last_lscore;
	  tmpsum += ngram_score_cache;
	}
	
	if (wchmm->lmtype == LM_DFA && wchmm->lmvar == LM_DFA_GRAMMAR) {
	  /* Ê¸Ë¡¤òÍÑ¤¤¤EEE ¥«¥Æ¥´¥E±°Ì¤ÎÌÚ¹½Â¤²½¤¬¤Ê¤µ¤EÆ¤¤¤EĞ,
	     ÀÜÂ³À©Ìó¤ÏÃ±¸EÖÁ«°Ü¤Î¤ß¤Ç°·¤EEEÎ¤Ç¡¤factoring ¤ÏÉ¬Í×¤Ê¤¤. 
	     ¥«¥Æ¥´¥E±°ÌÌÚ¹½Â¤²½¤¬¹Ô¤EEÊ¤¤¾EE Ê¸Ë¡´Ö¤ÎÀÜÂ³À©Ìó¤Ï¤³¤³
	     ¤Ç factoring ¤Ç¹Ô¤EEE³¤È¤Ë¤Ê¤E */
	  /* With DFA, we use category-pair constraint extracted from the DFA
	     at this 1st pass.  So if we compose a tree lexicon per word's
	     category, the each category tree has the same connection
	     constraint and thus we can apply the constraint on the cross-word
	     transition.  This per-category tree lexicon is enabled by default,
	     and in the case the constraint will be applied at the word end.
	     If you disable per-category tree lexicon by undefining
	     'CATEGORY_TREE', the word-pair contrained will be applied in a
	     factoring style at here.
	  */
	  
	  /* ·èÄEªfactoring: Ä¾Á°Ã±¸EËÂĞ¤·¤Æ,sub-treeÆâ¤Ë¥«¥Æ¥´¥EĞÀ©Ìó¾E	     ÀÜÂ³¤·¤¦¤E±¸E¬£±¤Ä¤â¤Ê¤±¤EĞ, ¤³¤ÎÁ«°Ü¤ÏÉÔ²Ä */
	  /* deterministic factoring in grammar mode:
	     transition disabled if there are totally no sub-tree word that can
	     grammatically (in category-pair constraint) connect
	     to the previous word.
	  */
	  if (!can_succeed(wchmm, tk->last_tre->wid, next_node)) {
	    tmpsum = LOG_ZERO;
	  }
	}
	
      }
    }
  }
  /* factoring not needed when DFA mode and uses category-tree */
  
  /****************************************/
  /* 2.1.2 Á«°ÜÀè¥Î¡¼¥É¤Ø¥È¡¼¥¯¥óÅÁÈÂ     */
  /*       pass token to destination node */
  /****************************************/
  
  if (ngram_score_cache == LOG_ZERO) ngram_score_cache = tk->last_lscore;
  propagate_token(d, next_node, tmpsum, tk->last_tre, tk->last_cword, ngram_score_cache);
  
  if (d->expanded) {
    /* if work area has been expanded at 'create_token()' above,
       the inside 'realloc()' will destroy the pointers.
       so, reset local pointers from token index */
    tk = &(d->tlist[d->tl][d->tindex[d->tl][j]]);
    d->expanded = FALSE;
  }
  
  *tk_ret = tk;

}

/** 
 * <JA>
 * Ã±¸EâÁ«°Ü¤ò¹Ô¤¦. 
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * @param d [i/o] ÂE¥Ñ¥¹¥E¼¥¯¥¨¥E¢
 * @param tk_ret [i/o] ÅÁÈÂ¸µ¤Î¥È¡¼¥¯¥ó¡ÊÆâÉô¤Ç¥İ¥¤¥ó¥¿¹¹¿·»ş¤Ï¾å½ñ¤­¡Ë
 * @param j [in] @a tk_ret ¤Î¸µ¤Î¥È¡¼¥¯¥ó¥E¹¥È¤ÎID
 * </JA>
 * <EN>
 * Word-internal transition.
 * 
 * @param wchmm [in] tree lexicon
 * @param d [i/o] work area for the 1st pass
 * @param tk_ret [in] source token (if pointer updated, overwrite this)
 * @param j [in] the token ID of @a tk_ret
 * 
 * </EN>
 */
static void
beam_intra_word(WCHMM_INFO *wchmm, FSBeam *d, TOKEN2 **tk_ret, int j)
{
  A_CELL2 *ac; ///< Temporal work to hold the next states of a node
  TOKEN2 *tk;
  int node;
  int k;

  tk = *tk_ret;

  node = tk->node;

  if (wchmm->self_a[node] != LOG_ZERO) {
    beam_intra_word_core(wchmm, d, tk_ret, j, node, wchmm->self_a[node]);
  }

  if (wchmm->next_a[node] != LOG_ZERO) {
    beam_intra_word_core(wchmm, d, tk_ret, j, node+1, wchmm->next_a[node]);
  }

  for(ac=wchmm->ac[node];ac;ac=ac->next) {
    for(k=0;k<ac->n;k++) {
      beam_intra_word_core(wchmm, d, tk_ret, j, ac->arc[k], ac->a[k]);
    }
  }
}

/**************************/
/* 2.2. ¥È¥EE¹Ã±¸EİÂ¸  */
/*      save trellis word */
/**************************/
/** 
 * <JA>
 * ¥È¡¼¥¯¥ó¤«¤é¥È¥EE¹Ã±¸EòÊİÂ¸¤¹¤E 
 * 
 * @param bt [i/o] ¥Ğ¥Ã¥¯¥È¥EE¹¹½Â¤ÂÎ
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * @param tk [in] Ã±¸EöÃ¼¤ËÅşÃ£¤·¤Æ¤¤¤EÈ¡¼¥¯¥E * @param t [in] ¸½ºß¤Î»ş´Ö¥Õ¥E¼¥E * @param final_for_multipath [in] ÆşÎÏºÇ¸å¤Î£±²ó½èÍı»ş TRUE
 * 
 * @return ¿·¤¿¤Ë³ÊÇ¼¤µ¤E¿¥È¥EE¹Ã±¸EØ¤Î¥İ¥¤¥ó¥¿
 * </JA>
 * <EN>
 * Store a new trellis word on the token.
 *
 * @param bt [i/o] backtrellis data to save it
 * @param wchmm [in] tree lexicon
 * @param tk [in] source token at word edge
 * @param t [in] current time frame
 * @param final_for_multipath [in] TRUE if this is final frame
 *
 * @return pointer to the newly stored trellis word.
 * </EN>
 */
static TRELLIS_ATOM *
save_trellis(BACKTRELLIS *bt, WCHMM_INFO *wchmm, TOKEN2 *tk, int t, boolean final_for_multipath)
{
  TRELLIS_ATOM *tre;
  int sword;
 
  sword = wchmm->stend[tk->node];

  /* ¤³¤ÎÁ«°Ü¸µ¤ÎÃ±¸EªÃ¼¥Î¡¼¥É¤Ï¡ÖÄ¾Á°¥Õ¥E¼¥à¤Ç¡×À¸¤­»Ä¤Ã¤¿¥Î¡¼¥É. 
     (¡Ö¤³¤Î¥Õ¥E¼¥à¡×¤Ç¤Ê¤¤¤³¤È¤ËÃúÌÕ¡ª¡ª)
     ¤è¤Ã¤Æ¤³¤³¤Ç, »ş´Ö(t-1) ¤òÃ±¸EªÃ¼¤È¤¹¤EÈ¥EE¹¾å¤ÎÃ±¸E¾ÀE     (TRELLIS_ATOM)¤È¤·¤Æ¡¤Ã±¸EÈ¥EE¹¹½Â¤ÂÎ¤ËÊİÂ¸¤¹¤E */
  /* This source node (= word end node) has been survived in the
     "last" frame (notice: not "this" frame!!).  So this word end
     is saved here to the word trellis structure (BACKTRELLIS) as a
     trellis word (TRELLIS_ATOM) with end frame (t-1). */
  tre = bt_new(bt);
  tre->wid = sword;		/* word ID */
  tre->backscore = tk->score; /* log score (AM + LM) */
  tre->begintime = tk->last_tre->endtime + 1; /* word beginning frame */
  tre->endtime   = t-1;	/* word end frame */
  tre->last_tre  = tk->last_tre; /* link to previous trellis word */
  tre->lscore    = tk->last_lscore;	/* log LM score  */
  bt_store(bt, tre); /* save to backtrellis */
#ifdef WORD_GRAPH
  if (tre->last_tre != NULL) {
    /* mark to indicate that the following words was survived in beam */
    tre->last_tre->within_context = TRUE;
  }
  if (final_for_multipath) {
    /* last node */
    if (tre->wid == wchmm->winfo->tail_silwid) {
      tre->within_context = TRUE;
    }
  }
#endif /* WORD_GRAPH */

  return tre;
}
      

/** 
 * <JA>
 * Ã±¸Eö¥È¡¼¥¯¥ó¤«¤é¤ÎÃ±¸EÖÁ«°Ü. 
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * @param d [i/o] ÂE¥Ñ¥¹¥E¼¥¯¥¨¥E¢
 * @param tk_ret [in] ÅÁÈÂ¸µ¤ÎÃ±¸Eö¥È¡¼¥¯¥E * @param tre [in] @a tk_ret ¤«¤éÀ¸À®¤µ¤E¿¥È¥EE¹Ã±¸E * @param j [in] @a tk_ret ¤Î¸µ¤Î¥È¡¼¥¯¥ó¥E¹¥È¤ÎID
 * </JA>
 * <EN>
 * Cross-word transition processing from word-end token.
 * 
 * @param wchmm [in] tree lexicon
 * @param d [i/o] work area for the 1st pass
 * @param tk_ret [in] source token where the propagation is from
 * @param tre [in] the trellis word generated from @a tk_ret
 * @param j [in] the token ID of @a tk_ret
 * </EN>
 */
static void
beam_inter_word(WCHMM_INFO *wchmm, FSBeam *d, TOKEN2 **tk_ret, TRELLIS_ATOM *tre, int j)
{
  A_CELL2 *ac;
  TOKEN2 *tk;
  int sword;
  int node, next_node;
  LOGPROB *iwparray; ///< Temporal pointer to hold inter-word cache array
  int stid;
#ifdef UNIGRAM_FACTORING
  int isoid; ///< Temporal work to hold isolated node
#endif
  LOGPROB tmpprob, tmpsum, ngram_score_cache;
  int k;
  WORD_ID last_word;

  tk = *tk_ret;
 
  node = tk->node;
  sword = wchmm->stend[node];
  last_word = wchmm->winfo->is_transparent[sword] ? tk->last_cword : sword;

  if (wchmm->lmtype == LM_PROB) {

    /* Á«°Ü¸µÃ±¸E¬ËöÈøÃ±¸EÎ½ªÃ¼¤Ê¤é¡¤¤É¤³¤Ø¤âÁ«°Ü¤µ¤»¤Ê¤¤ */
    /* do not allow transition if the source word is end-of-sentence word */
    if (sword == wchmm->winfo->tail_silwid) return;

#ifdef UNIGRAM_FACTORING
#ifndef WPAIR
    /* ¤¢¤È¤Ç¶¦Í­Ã±¸EèÆ¬¥Î¡¼¥É¤ËÂĞ¤·¤ÆÃ±¸EÖÁ«°Ü¤ò¤Ş¤È¤á¤Æ·×»»¤¹¤E¿¤á¡¤*/
    /* ¤³¤Î¥E¼¥×Æâ¤Ç¤ÏºÇÂçÌàÅÙ¤ò»ı¤ÄÃ±¸EªÃ¼¥Î¡¼¥É¤òµ­Ï¿¤·¤Æ¤ª¤¯ */
    /* here we will record the best wordend node of maximum likelihood
       at this frame, to compute later the cross-word transitions toward
       shared factoring word-head node */
    tmpprob = tk->score;
    if (!wchmm->hmminfo->multipath) tmpprob += wchmm->wordend_a[sword];
    if (d->wordend_best_score < tmpprob) {
      d->wordend_best_score = tmpprob;
      d->wordend_best_node = node;
      d->wordend_best_tre = tre;
      d->wordend_best_last_cword = tk->last_cword;
    }
#endif
#endif
    
    /* N-gram¤Ë¤ª¤¤¤Æ¤Ï¾EËÁ´Ã±¸EÎÀÜÂ³¤ò¹ÍÎ¸¤¹¤E¬Í×¤¬¤¢¤E¿¤á¡¤
       ¤³¤³¤ÇÃ±¸EÖ¤Î¸À¸EÎÎ¨ÃÍ¤ò¤¹¤Ù¤Æ·×»»¤·¤Æ¤ª¤¯. 
       ¥­¥ã¥Ã¥·¥å¤Ï max_successor_prob_iw() Æâ¤Ç¹ÍÎ¸. */
    /* As all words are possible to connect in N-gram, we first compute
       all the inter-word LM probability here.
       Cache is onsidered in max_successor_prob_iw(). */
    if (wchmm->winfo->is_transparent[sword]) {
      iwparray = max_successor_prob_iw(wchmm, tk->last_cword);
    } else {
      iwparray = max_successor_prob_iw(wchmm, sword);
    }
  }

  /* ¤¹¤Ù¤Æ¤ÎÃ±¸EÏÃ¼¥Î¡¼¥É¤ËÂĞ¤·¤Æ°Ê²¼¤ò¼Â¹Ô */
  /* for all beginning-of-word nodes, */
  /* wchmm->startnode[0..stid-1] ... Ã±¸EÏÃ¼¥Î¡¼¥É¥E¹¥È */
  /* wchmm->startnode[0..stid-1] ... list of word start node (shared) */
  for (stid = wchmm->startnum - 1; stid >= 0; stid--) {
    next_node = wchmm->startnode[stid];
    if (wchmm->hmminfo->multipath) {
      if (wchmm->lmtype == LM_PROB) {
	/* connection to the head silence word is not allowed */
	if (wchmm->wordbegin[wchmm->winfo->head_silwid] == next_node) continue;
      }
    }
    
    /*****************************************/
    /* 2.3.1. Ã±¸EÖ¸À¸E©Ìó¤òÅ¬ÍÑ           */
    /*        apply cross-word LM constraint */
    /*****************************************/
	
    if (wchmm->lmtype == LM_PROB) {
      /* N-gram³ÎÎ¨¤ò·×»» */
      /* compute N-gram probability */
#ifdef UNIGRAM_FACTORING
      /* wchmm,start2isolate[0..stid-1] ... ¥Î¡¼¥É¤ò¶¦Í­¤·¤Ê¤¤Ã±¸EÏ
	 ¤½¤ÎÄÌ¤·ID, ¶¦Í­¤¹¤E¥­¥ã¥Ã¥·¥å¤ÎÉ¬Í×¤Î¤Ê¤¤)Ã±¸EÏ -1 */
      /* wchmm->start2isolate[0..stid-1] ... isolate ID for
	 beginning-of-word state.  value: -1 for states that has
	 1-gram factoring value (share nodes with some other words),
	 and ID for unshared words
      */
      isoid = wchmm->start2isolate[stid];
#ifdef WPAIR
      /* Efficient cross-word LM handling should be disabled for
	 word-pair approximation */
      if (isoid == -1) {
	tmpprob = wchmm->fscore[- wchmm->state[next_node].scid];
      } else {
	tmpprob = iwparray[isoid];
      }
#else  /* ~WPAIR */
      /* 1-gram factoring ¤Ë¤ª¤±¤E±¸EÖ¸À¸EÎÎ¨¥­¥ã¥Ã¥·¥å¤Î¸úÎ¨²½:
	 1-gram factoring ¤ÏÃ±¸EúÎò¤Ë°ÍÂ¸¤·¤Ê¤¤¤Î¤Ç¡¤
	 ¤³¤³¤Ç»²¾È¤¹¤Efactoring ÃÍ¤ÎÂ¿¤¯¤Ï
	 wchmm->fscore[] ¤Ë´û¤Ë³ÊÇ¼¤µ¤E Ãµº÷Ãæ¤âÉÔÊÑ¤Ç¤¢¤E 
	 ¤è¤Ã¤Æ·×»»¤¬É¬Í×¤ÊÃ±¸E¤É¤ÎÃ±¸EÈ¤â¥Î¡¼¥É¤ò¶¦Í­¤·¤Ê¤¤Ã±¸E
	 ¤Ë¤Ä¤¤¤Æ¤Î¤ß iwparray[] ¤Ç·×»»¡¦¥­¥ã¥Ã¥·¥å¤¹¤E  */
      /* Efficient cross-word LM cache:
	 As 1-gram factoring values are independent of word context,
	 they remain unchanged while search.  So, in cross-word LM
	 computation, beginning-of-word states which share nodes with
	 others and has factoring value in wchmm does not need cache.
	 So only the unshared beginning-of-word states are computed and
	 cached here in iwparray[].
      */
      /* ·×»»¤¬É¬Í×¤Ç¤Ê¤¤Ã±¸EèÆ¬¥Î¡¼¥É¤Ï¥Ñ¥¹¤ò¤Ş¤È¤á¤Æ¸å¤Ë·×»»¤¹¤EÎ¤Ç
	 ¤³¤³¤Ç¤Ï¥¹¥­¥Ã¥× */
      /* the shared nodes will be computed afterward, so just skip them
	 here */
      if (isoid == -1) continue;
      tmpprob = iwparray[isoid];
#endif /* ~WPAIR */
#else  /* ~UNIGRAM_FACTORING */
      tmpprob = iwparray[stid];
#endif
    }

    /* Á«°ÜÀè¤ÎÃ±¸E¬ÀèÆ¬Ã±¸EÊ¤éÁ«°Ü¤µ¤»¤Ê¤¤. 
       ¤³¤EÏ wchmm.c ¤Ç³ºÅöÃ±¸EË stid ¤ò³ä¤E¶¤é¤Ê¤¤¤³¤È¤ÇÂĞ±ş
       ¤·¤Æ¤¤¤EÎ¤Ç¡¤¤³¤³¤Ç¤Ï²¿¤â¤·¤Ê¤¯¤Æ¤è¤¤ */
    /* do not allow transition if the destination word is
       beginning-of-sentence word.  This limitation is realized by
       not assigning 'stid' for the word in wchmm.c, so we have nothing
       to do here.
    */
    
    if (wchmm->category_tree) {
      /* Ê¸Ë¡¤Î¾EE À©Ìó¤Ï·èÄEª: ¥«¥Æ¥´¥EĞÀ©Ìó¾åµö¤µ¤EÊ¤¤¾Eç¤ÏÁ«°Ü¤µ¤»¤Ê¤¤ */
      /* With DFA and per-category tree lexicon,
	 LM constraint is deterministic:
	 do not allow transition if the category connection is not allowed
	 (with category tree, constraint can be determined on top node) */
      if (dfa_cp(wchmm->dfa, wchmm->winfo->wton[sword], wchmm->winfo->wton[wchmm->start2wid[stid]]) == FALSE) continue;
    }

    /*******************************************************************/
    /* 2.3.2. Á«°ÜÀè¤ÎÃ±¸EèÆ¬¤Ø¤Î¥¹¥³¥¢·×»»(Á«°Ü³ÎÎ¨¡Ü¸À¸E¹¥³¥¢)     */
    /*        compute score of destination node (transition prob + LM) */
    /*******************************************************************/
    tmpsum = tk->score;
    if (!wchmm->hmminfo->multipath) tmpsum += wchmm->wordend_a[sword];

    /* 'tmpsum' now holds outgoing score from the wordend node */
    if (wchmm->lmtype == LM_PROB) {
      /* ¸À¸E¹¥³¥¢¤òÄÉ²Ã */
      /* add LM score */
      ngram_score_cache = tmpprob * d->lm_weight + d->lm_penalty;
      tmpsum += ngram_score_cache;
      if (wchmm->winfo->is_transparent[sword] && wchmm->winfo->is_transparent[tk->last_cword]) {
	    
	tmpsum += d->lm_penalty_trans;
      }
    }
    if (wchmm->lmtype == LM_DFA) {
      /* grammar: Ã±¸EŞÆş¥Ú¥Ê¥EÆ¥£¤òÄÉ²Ã */
      /* grammar: add insertion penalty */
      ngram_score_cache = d->penalty1;
#ifdef CLASS_NGRAM
      /* add per-word penalty of last word as delayed penalty */
      ngram_score_cache += wchmm->winfo->cprob[last_word];
#endif
      tmpsum += ngram_score_cache;

      /* grammar: deterministic factoring (in case category-tree not enabled) */
      if (!wchmm->category_tree) {
	if (!can_succeed(wchmm, sword, next_node)) {
	  tmpsum = LOG_ZERO;
	}
      }
    }
    
    /*********************************************************************/
    /* 2.3.3. Á«°ÜÀè¥Î¡¼¥É¤Ø¥È¡¼¥¯¥óÅÁÈÂ(Ã±¸EúÎò¾ğÊó¤Ï¹¹¿·)             */
    /*        pass token to destination node (updating word-context info */
    /*********************************************************************/

    if (wchmm->hmminfo->multipath) {
      /* since top node has no ouput, we should go one more step further */
      if (wchmm->self_a[next_node] != LOG_ZERO) {
	propagate_token(d, next_node, tmpsum + wchmm->self_a[next_node], tre, last_word, ngram_score_cache);
	if (d->expanded) {
	  /* if work area has been expanded at 'create_token()' above,
	     the inside 'realloc()' will destroy the pointers.
	     so, reset local pointers from token index */
	  tk = &(d->tlist[d->tn][d->tindex[d->tn][j]]);
	  d->expanded = FALSE;
	}
      }
      if (wchmm->next_a[next_node] != LOG_ZERO) {
	propagate_token(d, next_node+1, tmpsum + wchmm->next_a[next_node], tre, last_word, ngram_score_cache);
	if (d->expanded) {
	  /* if work area has been expanded at 'create_token()' above,
	     the inside 'realloc()' will destroy the pointers.
	     so, reset local pointers from token index */
	  tk = &(d->tlist[d->tn][d->tindex[d->tn][j]]);
	  d->expanded = FALSE;
	}
      }
      for(ac=wchmm->ac[next_node];ac;ac=ac->next) {
	for(k=0;k<ac->n;k++) {
	  propagate_token(d, ac->arc[k], tmpsum + ac->a[k], tre, last_word, ngram_score_cache);
	  if (d->expanded) {
	    /* if work area has been expanded at 'create_token()' above,
	       the inside 'realloc()' will destroy the pointers.
	       so, reset local pointers from token index */
	    tk = &(d->tlist[d->tn][d->tindex[d->tn][j]]);
	    d->expanded = FALSE;
	  }
	}
      }
    } else {
      propagate_token(d, next_node, tmpsum, tre, last_word, ngram_score_cache);
      if (d->expanded) {
	/* if work area has been expanded at 'create_token()' above,
	   the inside 'realloc()' will destroy the pointers.
	   so, reset local pointers from token index */
	tk = &(d->tlist[d->tl][d->tindex[d->tl][j]]);
	d->expanded = FALSE;
      }
    }
	
  }	/* end of next word heads */

  *tk_ret = tk;


} /* end of cross-word processing */


#ifdef UNIGRAM_FACTORING

/** 
 * <JA>
 * @brief  1-gram factoring ÍÑÃ±¸EÖÁ«°Ü¤ÎÄÉ²Ã½èÍı
 * 
 * 1-gram factoring »ÈÍÑ»ş¤Ï¡¢Ê£¿ô¤ÎÃ±¸EÖ¤Ç¶¦Í­¤µ¤EÆ¤¤¤E * Ã±¸EèÆ¬¤Î¥Î¡¼¥É (= factoring ¤µ¤EÆ¤¤¤E±¸EèÆ¬¥Î¡¼¥É) ¤Ë¤Ä¤¤¤Æ¤Ï¡¢
 * ¤¹¤Ù¤Æ¡¢ºÇ¤âÌàÅÙ¤Î¹â¤¤Ã±¸EªÃ¼¤«¤é¤ÎÁ«°Ü¤¬ÁªÂò¤µ¤EE£¤³¤ÎÀ­¼Á¤E * ÍÑ¤¤¤Æ¡¢¤³¤Î´Ø¿ô¤Ç¤Ï¤¢¤é¤«¤¸¤áµá¤á¤é¤E¿ºÇ¤âÌàÅÙ¤Î¹â¤¤Ã±¸EªÃ¼
 * ¤«¤é¡¢¥Õ¥¡¥¯¥¿¥Eó¥°¤µ¤E¿Ã±¸EèÆ¬¥Î¡¼¥É¤Ø¤ÎÁ«°Ü·×»»¤ò°EÙ¤Ë¹Ô¤¦¡£
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * @param d [in] ÂE¥Ñ¥¹ÍÑ¥E¼¥¯¥¨¥E¢
 * </JA>
 * <EN>
 * @brief  Additional cross-word transition processing for 1-gram factoring.
 *
 * When using 1-gram factoring, The word end of maximum likelihood will be
 * chosen at cross-word viterbi for factored word-head node, since the
 * LM factoring value is independent of the context.  This function performs
 * viterbi processing to the factored word-head nodes from the maximum
 * likelihood word end previously stored.
 * 
 * @param wchmm [in] tree lexicon
 * @param d [in] work area for the 1st pass
 * </EN>
 */
static void
beam_inter_word_factoring(WCHMM_INFO *wchmm, FSBeam *d)
{
  int sword;
  int node, next_node;
  int stid;
  LOGPROB tmpprob, tmpsum, ngram_score_cache;
  A_CELL2 *ac;
  int j;
  WORD_ID last_word;

  node = d->wordend_best_node;
  sword = wchmm->stend[node];
  last_word = wchmm->winfo->is_transparent[sword] ? d->wordend_best_last_cword : sword;

  for (stid = wchmm->startnum - 1; stid >= 0; stid--) {
    next_node = wchmm->startnode[stid];
    /* compute transition from 'node' at end of word 'sword' to 'next_node' */
    /* skip isolated words already calculated in the above main loop */
    if (wchmm->start2isolate[stid] != -1) continue;
    /* rest should have 1-gram factoring score at wchmm->fscore */
    if (wchmm->state[next_node].scid >= 0) {
      j_internal_error("get_back_trellis_proceed: scid mismatch at 1-gram factoring of shared states\n");
    }
    tmpprob = wchmm->fscore[- wchmm->state[next_node].scid];
    ngram_score_cache = tmpprob * d->lm_weight + d->lm_penalty;
    tmpsum = d->wordend_best_score;
    tmpsum += ngram_score_cache;
    if (wchmm->winfo->is_transparent[sword] && wchmm->winfo->is_transparent[d->wordend_best_last_cword]) {
      tmpsum += d->lm_penalty_trans;
    }
#ifdef SCORE_PRUNING
    if (tmpsum < d->score_pruning_threshold) {
      d->score_pruning_count++;
      continue;
    }
#endif
    if (wchmm->hmminfo->multipath) {
      /* since top node has no ouput, we should go one more step further */
      if (wchmm->self_a[next_node] != LOG_ZERO) {
	propagate_token(d, next_node, tmpsum + wchmm->self_a[next_node], d->wordend_best_tre, last_word, ngram_score_cache);
	if (d->expanded) {
	  d->expanded = FALSE;
	}
      }
      if (wchmm->next_a[next_node] != LOG_ZERO) {
	propagate_token(d, next_node+1, tmpsum + wchmm->next_a[next_node], d->wordend_best_tre, last_word, ngram_score_cache);
	if (d->expanded) {
	  d->expanded = FALSE;
	}
      }
      for(ac=wchmm->ac[next_node];ac;ac=ac->next) {
	for(j=0;j<ac->n;j++) {
	  propagate_token(d, ac->arc[j], tmpsum + ac->a[j], d->wordend_best_tre, last_word, ngram_score_cache);
	  if (d->expanded) {
	    d->expanded = FALSE;
	  }
	}
      }
      
    } else {
      propagate_token(d, next_node, tmpsum, d->wordend_best_tre, last_word, ngram_score_cache);
      if (d->expanded) {
	d->expanded = FALSE;
      }
    }

  }
}

#endif /* UNIGRAM_FACTORING */


/** 
 * <JA>
 * @brief  ¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤ò¿Ê¹Ô¤¹¤E 
 *
 * Í¿¤¨¤é¤E¿£±¥Õ¥E¼¥àÊ¬¡¤Ãµº÷½èÍı¤ò¿Ê¤á¤E ¤Ş¤¿¡¤¥Õ¥E¼¥àÆâ¤Ë»Ä¤Ã¤¿
 * Ã±¸EòÃ±¸EÈ¥EE¹¹½Â¤ÂÎ¤ËÊİÂ¸¤¹¤E ¥·¥ç¡¼¥È¥İ¡¼¥º¥»¥°¥á¥ó¥Æ¡¼¥·¥ç¥ó»ş
 * ¤Ï¥»¥°¥á¥ó¥È½ªÎ»¤ÎÈ½ÃÇ¤â¤³¤ÎÃæ¤«¤é¸Æ¤Ó½Ğ¤µ¤EE 
 * 
 * @param t [in] ¸½ºß¤Î¥Õ¥E¼¥E(¤³¤Î¥Õ¥E¼¥à¤Ë¤Ä¤¤¤Æ·×»»¤¬¿Ê¤á¤é¤EE
 * @param param [in] ÆşÎÏ¥Ù¥¯¥È¥Eó¹½Â¤ÂÎ (@a t ÈÖÌÜ¤Î¥Õ¥E¼¥à¤Î¤ßÍÑ¤¤¤é¤EE
 * @param r [in] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * @param final_for_multipath [i/o] ÆşÎÏºÇ¸å¤Î¥Õ¥E¼¥à¤ò½èÍı¤¹¤EÈ¤­¤Ë TRUE
 * 
 * @return TRUE (ÄÌ¾EÉ¤ª¤EªÎ») ¤¢¤E¤¤Ï FALSE (¤³¤³¤ÇÃµº÷¤òÃæÃÇ¤¹¤E * ¾EE Ãà¼¡¥Ç¥³¡¼¥Ç¥£¥ó¥°»ş¤Ë¥·¥ç¡¼¥È¥İ¡¼¥º¶è´Ö¤ò¸¡½Ğ¤·¤¿¤«¡¤¥Ó¡¼¥àÆâ¤Î
 * ¥¢¥¯¥Æ¥£¥Ö¥Î¡¼¥É¿ô¤¬0¤Ë¤Ê¤Ã¤¿¤È¤­)
 * </JA>
 * <EN>
 * @brief  Frame synchronous beam search: proceed for 2nd frame and later.
 *
 * This is the main function of beam search on the 1st pass.  Given a
 * input vector of a frame, it proceeds the computation for the one frame,
 * and store the words survived in the beam width to the word trellis
 * structure.  get_back_trellis_init() should be used for the first frame.
 * For detailed procedure, please see the comments in this
 * function.
 * 
 * @param t [in] current frame to be computed in @a param
 * @param param [in] input vector structure (only the vector at @a t will be used)
 * @param r [in] recognition process instance
 * @param final_for_multipath [i/o] TRUE if this is last frame of an input
 * 
 * @return TRUE if processing ended normally, or FALSE if the search was
 * terminated (in case of short pause segmentation in successive decoding
 * mode, or active nodes becomes zero).
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
boolean
get_back_trellis_proceed(int t, HTK_Param *param, RecogProcess *r, boolean final_for_multipath)
{
  /* local static work area for get_back_trellis_proceed() */
  /* these are local work area and need not to be kept for another call */
  TRELLIS_ATOM *tre; ///< Local workarea to hold the generated trellis word
  int node; ///< Temporal work to hold the current node number on the lexicon tree
  int lmtype, lmvar;

  WCHMM_INFO *wchmm;
  FSBeam *d;
  int j;
  TOKEN2  *tk;
  LOGPROB minscore;

  /* local copied variables */
  int tn, tl;

  /* store pointer to local for rapid access */
  wchmm = r->wchmm;
  d = &(r->pass1);
  

  lmtype = r->lmtype;
  lmvar  = r->lmvar;

  /*********************/
  /* 1. ½é´E½         */
  /*    initialization */
  /*********************/

  /* tl ¤È tn ¤òÆş¤EØ¤¨¤ÆºûÒÈÎÎ°è¤òÀÚ¤EØ¤¨ */
  /* tl (= Ä¾Á°¤Î tn) ¤ÏÄ¾Á°¥Õ¥E¼¥à¤Î·EÌ¤ò»ı¤Ä */
  /* swap tl and tn to switch work buffer */
  /* tl (= last tn) holds result of the previous frame */
  d->tl = d->tn;
  if (d->tn == 0) d->tn = 1; else d->tn = 0;
  
  /* store locally for rapid access */
  tl = d->tl;
  tn = d->tn;

#ifdef UNIGRAM_FACTORING
#ifndef WPAIR
  /* 1-gram factoring ¤Ç¤ÏÃ±¸EèÆ¬¤Ç¤Î¸À¸EÎÎ¨¤¬°Eê¤ÇÄ¾Á°Ã±¸EË°ÍÂ¸¤·¤Ê¤¤
     ¤¿¤á¡¤Ã±¸EÖ Viterbi ¤Ë¤ª¤¤¤ÆÁª¤Ğ¤EE¾Á°Ã±¸EÏ,¼¡Ã±¸EË¤è¤é¤º¶¦ÄÌ¤Ç¤¢¤E 
     ¤è¤Ã¤ÆÃ±¸EªÃ¼¤«¤éfactoringÃÍ¤Î¤¢¤E±¸EèÆ¬¤Ø¤ÎÁ«°Ü¤Ï£±¤Ä¤Ë¤Ş¤È¤á¤é¤EE 
     ¤¿¤À¤·¡¤ÌÚ¤«¤éÆÈÎ©¤·¤¿Ã±¸EË¤Ä¤¤¤Æ¤Ï, Ã±¸EèÆ¬¤ÇÍúÎò¤Ë°ÍÂ¸¤·¤¿2-gram¤¬
     Í¿¤¨¤é¤EE¿¤E ºÇÌà¤ÎÃ±¸EÖ Viterbi ¥Ñ¥¹¤Ï¼¡Ã±¸E´¤È¤Ë°Û¤Ê¤E 
     ¤è¤Ã¤Æ¤½¤Eé¤Ë¤Ä¤¤¤Æ¤Ï¤Ş¤È¤á¤º¤ËÊÌ¤Ë·×»»¤¹¤E*/
  /* In 1-gram factoring, the language score on the word-head node is constant
     and independent of the previous word.  So, the same word hypothesis will
     be selected as the best previous word at the inter-word Viterbi
     processing.  So, in inter-word processing, we can (1) select only
     the best word-end hypothesis, and then (2) process viterbi from the node
     to each word-head node.  On the other hand, the isolated words,
     i.e. words not sharing any node with other word, has unique word-head
     node and the true 2-gram language score is determined at the top node.
     In such case the best word hypothesis prior to each node will differ
     according to the language scores.  So we have to deal such words separately. */
  /* initialize max value to delect best word-end hypothesis */
  if (lmtype == LM_PROB) {
    d->wordend_best_score = LOG_ZERO;
  }
#endif
#endif

#ifdef DEBUG
  /* debug */
  /* node_check_token(d, tl); */
#endif

  /* ¥È¡¼¥¯¥ó¥Ğ¥Ã¥Õ¥¡¤ò½é´E½: Ä¾Á°¥Õ¥E¼¥à¤Ç»È¤EE¿ÉôÊ¬¤À¤±¥¯¥E¢¤¹¤EĞ¤è¤¤ */
  /* initialize token buffer: for speedup, only ones used in the last call will be cleared */
  clear_tokens(d, tl);

  /**************************/
  /* 2. Viterbi·×»»         */
  /*    Viterbi computation */
  /**************************/
  /* Ä¾Á°¥Õ¥E¼¥à¤«¤é¤³¤Î¥Õ¥E¼¥à¤Ø¤Î Viterbi ·×»»¤ò¹Ô¤Ê¤¦ */
  /* tindex[tl][n_start..n_end] ¤ËÄ¾Á°¥Õ¥E¼¥à¾å°Ì¥Î¡¼¥É¤ÎID¤¬³ÊÇ¼¤µ¤EÆ¤¤¤E*/
  /* do one viterbi computation from last frame to this frame */
  /* tindex[tl][n_start..n_end] holds IDs of survived nodes in last frame */

  if (wchmm->hmminfo->multipath) {
    /*********************************/
    /* MULTIPATH MODE */
    /*********************************/

    for (j = d->n_start; j <= d->n_end; j++) {
      /* tk: ÂĞ¾İ¥È¡¼¥¯¥E node: ¤½¤Î¥È¡¼¥¯¥ó¤ò»ı¤ÄÌÚ¹½Â¤²½¼­½ñ¥Î¡¼¥ÉID */
      /* tk: token data  node: lexicon tree node ID that holds the 'tk' */
      tk = &(d->tlist[tl][d->tindex[tl][j]]);
      if (tk->score <= LOG_ZERO) continue; /* invalid node */
#ifdef SCORE_PRUNING
      if (tk->score < d->score_pruning_threshold) {
	d->score_pruning_count++;
	continue;
      }
#endif
      node = tk->node;
      /*********************************/
      /* 2.1. Ã±¸EâÁ«°Ü               */
      /*      word-internal transition */
      /*********************************/
      beam_intra_word(wchmm, d, &tk, j);
    }
    /*******************************************************/
    /* 2.2. ¥¹¥³¥¢¤Ç¥È¡¼¥¯¥ó¤ò¥½¡¼¥È¤·¥Ó¡¼¥àÉıÊ¬¤Î¾å°Ì¤ò·èÄE*/
    /*    sort tokens by score up to beam width            */
    /*******************************************************/
    sort_token_no_order(d, r->trellis_beam_width, &(d->n_start), &(d->n_end));
  
    /*************************/
    /* 2.3. Ã±¸EÖViterbi·×»»  */
    /*    cross-word viterbi */
    /*************************/
    for(j = d->n_start; j <= d->n_end; j++) {
      tk = &(d->tlist[tn][d->tindex[tn][j]]);
      node = tk->node;
#ifdef SCORE_PRUNING
      if (tk->score < d->score_pruning_threshold) {
	d->score_pruning_count++;
	continue;
      }
#endif
      /* Á«°Ü¸µ¥Î¡¼¥É¤¬Ã±¸EªÃ¼¤Ê¤é¤Ğ */
      /* if source node is end state of a word, */
      if (wchmm->stend[node] != WORD_INVALID) {

	/**************************/
	/* 2.4. ¥È¥EE¹Ã±¸EİÂ¸  */
	/*      save trellis word */
	/**************************/
#ifdef SPSEGMENT_NAIST
 	if (r->config->successive.enabled && !d->after_trigger) {
 	  tre = tk->last_tre;	/* dummy */
 	} else {
 	  tre = save_trellis(r->backtrellis, wchmm, tk, t, final_for_multipath);
	}
#else
	tre = save_trellis(r->backtrellis, wchmm, tk, t, final_for_multipath);
#endif
	/* ºÇ½ª¥Õ¥E¼¥à¤Ç¤¢¤EĞ¤³¤³¤Ş¤Ç¡§Á«°Ü¤Ï¤µ¤»¤Ê¤¤ */
	/* If this is a final frame, does not do cross-word transition */
	if (final_for_multipath) continue;
	/* Ã±¸E§¼±¥â¡¼¥É¤Ç¤ÏÃ±¸EÖÁ«°Ü¤ÏÉ¬Í×¤Ê¤¤ */
	if (lmvar == LM_DFA_WORD) continue;

	/******************************/
	/* 2.5. Ã±¸EÖÁ«°Ü            */
	/*      cross-word transition */
	/******************************/

#ifdef UNIGRAM_FACTORING
	/* ¤³¤³¤Ç½èÍı¤µ¤EEÎ¤Ï isolated words ¤Î¤ß¡¤
	   shared nodes ¤Ï¤Ş¤È¤á¤Æ¤³¤Î¥E¼¥×¤Î³°¤Ç·×»»¤¹¤E*/
	/* Only the isolated words will be processed here.
	   The shared nodes with constant factoring values will be computed
	   after this loop */
#endif
	beam_inter_word(wchmm, d, &tk, tre, j);

      } /* end of cross-word processing */
    
    } /* end of main viterbi loop */



  } else {

    /*********************************/
    /* NORMAL MODE */
    /*********************************/

    for (j = d->n_start; j <= d->n_end; j++) {
      /* tk: ÂĞ¾İ¥È¡¼¥¯¥E node: ¤½¤Î¥È¡¼¥¯¥ó¤ò»ı¤ÄÌÚ¹½Â¤²½¼­½ñ¥Î¡¼¥ÉID */
      /* tk: token data  node: lexicon tree node ID that holds the 'tk' */
      tk = &(d->tlist[tl][d->tindex[tl][j]]);
      if (tk->score <= LOG_ZERO) continue; /* invalid node */
#ifdef SCORE_PRUNING
      if (tk->score < d->score_pruning_threshold) {
	d->score_pruning_count++;
	continue;
      }
#endif
      node = tk->node;
      
      /*********************************/
      /* 2.1. Ã±¸EâÁ«°Ü               */
      /*      word-internal transition */
      /*********************************/
      beam_intra_word(wchmm, d, &tk, j);

      /* Á«°Ü¸µ¥Î¡¼¥É¤¬Ã±¸EªÃ¼¤Ê¤é¤Ğ */
      /* if source node is end state of a word, */
      if (wchmm->stend[node] != WORD_INVALID) {
	
	/**************************/
	/* 2.2. ¥È¥EE¹Ã±¸EİÂ¸  */
	/*      save trellis word */
	/**************************/
#ifdef SPSEGMENT_NAIST
 	if (r->config->successive.enabled && !d->after_trigger) {
 	  tre = tk->last_tre;	/* dummy */
 	} else {
 	  tre = save_trellis(r->backtrellis, wchmm, tk, t, final_for_multipath);
	}
#else
	tre = save_trellis(r->backtrellis, wchmm, tk, t, final_for_multipath);
#endif
	/* Ã±¸E§¼±¥â¡¼¥É¤Ç¤ÏÃ±¸EÖÁ«°Ü¤ÏÉ¬Í×¤Ê¤¤ */
	if (lmvar == LM_DFA_WORD) continue;

	/******************************/
	/* 2.3. Ã±¸EÖÁ«°Ü            */
	/*      cross-word transition */
	/******************************/
	
#ifdef UNIGRAM_FACTORING
	/* ¤³¤³¤Ç½èÍı¤µ¤EEÎ¤Ï isolated words ¤Î¤ß¡¤
	   shared nodes ¤Ï¤Ş¤È¤á¤Æ¤³¤Î¥E¼¥×¤Î³°¤Ç·×»»¤¹¤E*/
	/* Only the isolated words will be processed here.
	   The shared nodes with constant factoring values will be computed
	   after this loop */
#endif

	beam_inter_word(wchmm, d, &tk, tre, j);

      } /* end of cross-word processing */
      
    } /* end of main viterbi loop */


  }

#ifdef UNIGRAM_FACTORING
#ifndef WPAIR

  if (lmtype == LM_PROB) {

    /***********************************************************/
    /* 2.x Ã±¸EªÃ¼¤«¤éfactoringÉÕ¤­Ã±¸EèÆ¬¤Ø¤ÎÁ«°Ü ***********/
    /*    transition from wordend to shared (factorized) nodes */
    /***********************************************************/
    /* d->wordend_best_* holds the best word ends at this frame. */
    if (d->wordend_best_score > LOG_ZERO) {
      beam_inter_word_factoring(wchmm, d);
    }
  }
#endif
#endif /* UNIGRAM_FACTORING */

  /***************************************/
  /* 3. ¾õÂÖ¤Î½ĞÎÏ³ÎÎ¨·×»»               */
  /*    compute state output probability */
  /***************************************/

  /* ¼¡ÃÊ¤ÎÍ­¸ú¥Î¡¼¥É¤Ë¤Ä¤¤¤Æ½ĞÎÏ³ÎÎ¨¤ò·×»»¤·¤Æ¥¹¥³¥¢¤Ë²Ã¤¨¤E*/
  /* compute outprob for new valid (token assigned) nodes and add to score */
  /* º£°·¤Ã¤Æ¤¤¤EÎ¤¬ÆşÎÏ¤ÎºÇ½ª¥Õ¥E¼¥à¤Î¾Eç½ĞÎÏ³ÎÎ¨¤Ï·×»»¤·¤Ê¤¤ */
  /* don't calculate the last frame (transition only) */

#ifdef SCORE_PRUNING
  d->score_pruning_max = LOG_ZERO;
  minscore = 0.0;
#endif
  if (wchmm->hmminfo->multipath) {
    if (! final_for_multipath) {
      for (j = 0; j < d->tnum[tn]; j++) {
	tk = &(d->tlist[tn][d->tindex[tn][j]]);
	/* skip non-output state */
	if (wchmm->state[tk->node].out.state == NULL) continue;
	tk->score += outprob_style(wchmm, tk->node, tk->last_tre->wid, t, param);
#ifdef SCORE_PRUNING
	if (d->score_pruning_max < tk->score) d->score_pruning_max = tk->score;
	if (minscore > tk->score) minscore = tk->score;
#endif
      }
    }
  } else {
    for (j = 0; j < d->tnum[tn]; j++) {
      tk = &(d->tlist[tn][d->tindex[tn][j]]);
      tk->score += outprob_style(wchmm, tk->node, tk->last_tre->wid, t, param);
#ifdef SCORE_PRUNING
      if (d->score_pruning_max < tk->score) d->score_pruning_max = tk->score;
      if (minscore > tk->score) minscore = tk->score;
#endif
    }
  }
#ifdef SCORE_PRUNING
  if (r->config->pass1.score_pruning_width >= 0.0) {
    d->score_pruning_threshold = d->score_pruning_max - r->config->pass1.score_pruning_width;
    //printf("width=%f, tnum=%d\n", d->score_pruning_max - minscore, d->tnum[tn]);
  } else {
    // disable score pruning
    d->score_pruning_threshold = LOG_ZERO;
  }
#endif
  /*******************************************************/
  /* 4. ¥¹¥³¥¢¤Ç¥È¡¼¥¯¥ó¤ò¥½¡¼¥È¤·¥Ó¡¼¥àÉıÊ¬¤Î¾å°Ì¤ò·èÄE*/
  /*    sort tokens by score up to beam width            */
  /*******************************************************/

  /* tlist[tl]¤ò¼¡ÃÊ¤Î¤¿¤á¤Ë¥E»¥Ã¥È */
  clear_tlist(d, tl);

  /* ¥Ò¡¼¥×¥½¡¼¥È¤òÍÑ¤¤¤Æ¤³¤ÎÃÊ¤Î¥Î¡¼¥É½¸¹ç¤«¤é¾å°Ì(bwidth)¸Ä¤òÆÀ¤Æ¤ª¤¯ */
  /* (¾å°ÌÆâ¤Î½çÎó¤ÏÉ¬Í×¤Ê¤¤) */
  sort_token_no_order(d, r->trellis_beam_width, &(d->n_start), &(d->n_end));
  /***************/
  /* 5. ½ªÎ»½èÍı */
  /*    finalize */
  /***************/

#ifdef SPSEGMENT_NAIST
  if (!r->config->successive.enabled || d->after_trigger) {
#endif

    /* call frame-wise callback */
    r->have_interim = FALSE;
    if (t > 0) {
      if (r->config->output.progout_flag) {
	/* Á²¼¡½ĞÎÏ: ¸½¥Õ¥E¼¥à¤Î¥Ù¥¹¥È¥Ñ¥¹¤ò°EEş´Ö¤ª¤­¤Ë¾å½ñ¤­½ĞÎÏ */
	/* progressive result output: output current best path in certain time interval */
	if (((t-1) % r->config->output.progout_interval_frame) == 0) {
	  r->have_interim = TRUE;
	  bt_current_max(r, t-1);
	}
      }
    }
    
    /* jlog("DEBUG: %d: %d\n",t,tnum[tn]); */
    /* for debug: output current max word */
    if (debug2_flag) {
      bt_current_max_word(r, t-1);
    }

#ifdef DETERMINE
    if (lmvar == LM_DFA_WORD) {
      check_determine_word(r, t-1);
    }
#endif

#ifdef SPSEGMENT_NAIST
  }
#endif
    
  /* ¥Ó¡¼¥àÆâ¥Î¡¼¥É¿ô¤¬ 0 ¤Ë¤Ê¤Ã¤Æ¤·¤Ş¤Ã¤¿¤é¡¤¶¯À©½ªÎ» */
  if (d->tnum[tn] == 0) {
    jlog("ERROR: get_back_trellis_proceed: %02d %s: frame %d: no nodes left in beam, now terminates search\n", r->config->id, r->config->name, t);
    return(FALSE);
  }

  return(TRUE);
    
}

/*************************************************/
/* frame synchronous beam search --- last frame  */
/* ¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤Î¼Â¹Ô --- ºÇ½ª¥Õ¥E¼¥E*/
/*************************************************/

/** 
 * <JA>
 * @brief  ¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¡§ºÇ½ª¥Õ¥E¼¥E *
 * Âè£±¥Ñ¥¹¤Î¥Õ¥E¼¥àÆ±´EÓ¡¼¥àÃµº÷¤ò½ªÎ»¤¹¤E¿¤á¤Ë¡¤
 * (param->samplenum -1) ¤ÎºÇ½ª¥Õ¥E¼¥à¤ËÂĞ¤¹¤EªÎ»½èÍı¤ò¹Ô¤¦. 
 * 
 * 
 * @param param [in] ÆşÎÏ¥Ù¥¯¥È¥EE(param->samplenum ¤ÎÃÍ¤Î¤ßÍÑ¤¤¤é¤EE
 * @param r [in] ²»À¼Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * </JA>
 * <EN>
 * @brief  Frame synchronous beam search: last frame
 *
 * This function should be called at the end of the 1st pass.
 * The last procedure will be done for the (param->samplenum - 1) frame.
 * 
 * @param param [in] input vectors (only param->samplenum is referred)
 * @param r [in] recognition process instance
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
void
get_back_trellis_end(HTK_Param *param, RecogProcess *r)
{
  WCHMM_INFO *wchmm;
  FSBeam *d;
  int j;
  TOKEN2 *tk;

  wchmm = r->wchmm;
  d = &(r->pass1);

  /* ºÇ¸å¤Ë¥Ó¡¼¥àÆâ¤Ë»Ä¤Ã¤¿Ã±¸EªÃ¼¥È¡¼¥¯¥ó¤ò½èÍı¤¹¤E*/
  /* process the last wordend tokens */


  if (r->am->hmminfo->multipath) {
    /* MULTI-PATH VERSION */

    /* Ã±¸Eö¥Î¡¼¥É¤Ø¤ÎÁ«°Ü¤Î¤ß·×»» */
    /* only arcs to word-end node is calculated */
    get_back_trellis_proceed(param->samplenum, param, r, TRUE);

  } else {
    /* NORMAL VERSION */

    /* ºÇ¸å¤ÎÁ«°Ü¤Î¤¢¤È¤ÎÃ±¸EªÃ¼½èÍı¤ò¹Ô¤¦ */
    /* process the word-ends at the last frame */
    d->tl = d->tn;
    if (d->tn == 0) d->tn = 1; else d->tn = 0;
    for (j = d->n_start; j <= d->n_end; j++) {
      tk = &(d->tlist[d->tl][d->tindex[d->tl][j]]);
      if (wchmm->stend[tk->node] != WORD_INVALID) {
	save_trellis(r->backtrellis, wchmm, tk, param->samplenum, TRUE);
      }
    }

  }
#ifdef SCORE_PRUNING
  if (debug2_flag) jlog("STAT: %d tokens pruned by score beam\n", d->score_pruning_count);
#endif
    
}

/*************************/
/* Ãµº÷½ªÎ» --- ½ªÎ»½èÍı */
/* end of search         */
/*************************/
/** 
 * <JA>
 * @brief  Âè£±¥Ñ¥¹¤Î½ªÎ»½èÍı¤ò¹Ô¤¦. 
 *
 * ¤³¤Î´Ø¿ô¤Ï get_back_trellis_end() ¤ÎÄ¾¸å¤Ë¸Æ¤Ğ¤E¤Âè£±¥Ñ¥¹¤Î½ªÎ»½èÍı¤E * ¹Ô¤¦. À¸À®¤·¤¿Ã±¸EÈ¥EE¹¹½Â¤ÂÎ¤ÎºÇ½ªÅª¤Ê¸å½èÍı¤ò¹Ô¤¤Âè£²¥Ñ¥¹¤Ç
 * ¥¢¥¯¥»¥¹²ÄÇ½¤Ê·Á¤ËÆâÉô¤òÊÑ´¹¤¹¤E ¤Ş¤¿¡¤
 * ²¾Àâ¤Î¥Ğ¥Ã¥¯¥È¥E¼¥¹¤ò¹Ô¤¤Âè£±¥Ñ¥¹¤Î¥Ù¥¹¥È²¾Àâ¤ò½ĞÎÏ¤¹¤E 
 * 
 * @param r [in] Ç§¼±½èÍı¥¤¥ó¥¹¥¿¥ó¥¹
 * @param len [in] Âè£±¥Ñ¥¹¤Ç½èÍı¤µ¤E¿ºÇ½ªÅª¤Ê¥Õ¥E¼¥àÄ¹
 * 
 * @return Âè£±¥Ñ¥¹¤ÎºÇÌà²¾Àâ¤ÎÎßÀÑÌàÅÙ¡¤¤¢¤E¤¤Ï²¾Àâ¤¬¸«¤Ä¤«¤é¤Ê¤¤¾EE * ¤Ï LOG_ZERO. 
 * </JA>
 * <EN>
 * @brief  Finalize the 1st pass.
 *
 * This function will be called just after get_back_trellis_end() to
 * finalize the 1st pass.  It processes the resulting word trellis structure
 * to be accessible from the 2nd pass, and output the best sentence hypothesis
 * by backtracing the word trellis.
 * 
 * @param r [in] recoginirion process instance
 * @param len [in] total number of processed frames
 * 
 * @return the maximum score of the best hypothesis, or LOG_ZERO if search
 * failed.
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
void
finalize_1st_pass(RecogProcess *r, int len)
{
  BACKTRELLIS *backtrellis;

  backtrellis = r->backtrellis;
 
  backtrellis->framelen = len;

  /* Ã±¸EÈ¥EE¹(backtrellis) ¤òÀ°Íı: ¥È¥EE¹Ã±¸EÎºÆÇÛÃÖ¤È¥½¡¼¥È */
  /* re-arrange backtrellis: index them by frame, and sort by word ID */

  bt_relocate_rw(backtrellis);
  bt_sort_rw(backtrellis);
  if (backtrellis->num == NULL) {
    if (backtrellis->framelen > 0) {
      jlog("WARNING: %02d %s: input processed, but no survived word found\n", r->config->id, r->config->name);
    }
    /* reognition failed */
    r->result.status = J_RESULT_STATUS_FAIL;
    return;
  }

  /* ÂE¥Ñ¥¹¤Î¥Ù¥¹¥È¥Ñ¥¹¤ò·EÌ¤Ë³ÊÇ¼¤¹¤E*/
  /* store 1st pass result (best hypothesis) to result */
  if (r->lmvar == LM_DFA_WORD) {
    find_1pass_result_word(len, r);
  } else {
    find_1pass_result(len, r);
  }
}

/** 
 * <EN>
 * Free work area for the first pass
 * </EN>
 * <JA>
 * ÂE¥Ñ¥¹¤Î¤¿¤á¤Î¥E¼¥¯¥¨¥E¢ÎÎ°è¤ò³«ÊE¹¤E * </JA>
 * 
 * @param d [in] work are for 1st pass input handling
 * 
 * @callergraph
 * @callgraph
 * 
 */
void
fsbeam_free(FSBeam *d)
{
  free_nodes(d);
  if (d->pausemodelnames != NULL) {
    free(d->pausemodelnames);
    free(d->pausemodel);
  }
}

/* end of file */
