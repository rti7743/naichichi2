/**
 * @file   wchmm.c
 * 
 * <JA>
 * @brief  ÌÚ¹½Â¤²½¼­½ñ¤Î¹½ÃÛ
 *
 * ¤³¤³¤Ç¤Ï¡¤Í¿¤¨¤é¤E¿Ã±¸E­½E HMMÄEÁ¤ª¤è¤Ó¸À¸E©Ìó¤«¤éÌÚ¹½Â¤²½¼­½ñ¤E * ¹½ÃÛ¤¹¤EØ¿ô¤¬ÄEÁ¤µ¤EÆ¤¤¤Ş¤¹. ÌÚ¹½Â¤²½¼­½ñ¤Ïµ¯Æ°»ş¤Ë¹½ÃÛ¤µ¤E¤
 * ÂE¥Ñ¥¹¤ÎÇ§¼±¤ËÍÑ¤¤¤é¤EŞ¤¹. ÌÚ¹½Â¤²½¼­½ñ¤Ï¾õÂÖÃ±°Ì¤Ç¹½À®¤µ¤E¤
 * ³Æ¾õÂÖ¤ÏHMM½ĞÎÏ³ÎÎ¨¤ÈÁ«°ÜÀè¤ÎÂ¾¡¤¤ª¤è¤ÓÃµº÷¤Î¤¿¤á¤ÎÍÍ¡¹¤Ê¾ğÊó¤ò´Ş¤ß¤Ş¤¹. 
 *
 * ³«È¯¤Î·Ğ°Ş¾å¡¤¥½¡¼¥¹Æâ¤Ç¤ÏÌÚ¹½Â¤²½¼­½ñ¤Ï wchmm (word-conjunction HMM) ¤È
 * ¤âÉ½¸½¤µ¤EÆ¤¤¤Ş¤¹. 
 * 
 * </JA>
 * 
 * <EN>
 * @brief  Construction of tree lexicon.
 *
 * Functions to build a tree lexicon (or called word-conjunction HMM here)
 * from word dictionary, HMM and language models are defined here.  The
 * constructed tree lexicon will be used for the recognition of the 1st pass.
 * The lexicon is composed per HMM state unit, and various informations
 * about output probabilities, arcs, language model constraints, and others
 * are assembled in the lexicon.
 *
 * Note that the word "wchmm" in the source code is a synonim of
 * "tree lexicon". 
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Mon Sep 19 23:39:15 2005
 *
 * $Revision: 1.9 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

/* wchmm = word conjunction HMM = lexicon tree */

#include <julius/julius.h>


#define WCHMM_SIZE_CHECK		///< If defined, do wchmm size estimation (for debug only)

/**************************************************************/
/*********** Initialization of tree lexicon *******************/
/**************************************************************/

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¹½Â¤ÂÎ¤ò¿·µ¬¤Ë³ä¤EÕ¤±¤E 
 * 
 * @return ¿·¤¿¤Ë¥á¥â¥Eå¤Ë³ä¤EÕ¤±¤é¤E¿ÌÚ¹½Â¤²½¼­½ñ¹½Â¤ÂÎ¤Ø¤Î¥İ¥¤¥ó¥¿¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * Allocate a new tree lexicon structure.
 * 
 * @return pointer to the newly allocated tree lexicon structure.
 * </EN>
 * @callgraph
 * @callergraph
 */
WCHMM_INFO *
wchmm_new()
{
  WCHMM_INFO *w;
  w = (WCHMM_INFO *)mymalloc(sizeof(WCHMM_INFO));
  w->lmtype = LM_UNDEF;
  w->lmvar  = LM_UNDEF;
  w->ngram = NULL;
  w->dfa = NULL;
  w->winfo = NULL;
  w->malloc_root = NULL;
#ifdef PASS1_IWCD
  w->lcdset_category_root = NULL;
  w->lcdset_mroot = NULL;
#endif /* PASS1_IWCD */
  w->wrk.out_from_len = 0;
  /* reset user function entry point */
  w->uni_prob_user = NULL;
  w->bi_prob_user = NULL;
  return w;
}

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤ÎÆâÍÆ¤ò½é´E½¤¹¤E 
 * 
 * @param wchmm [out] ÌÚ¹½Â¤²½¼­½ñ¤Ø¤Î¥İ¥¤¥ó¥¿
 * </JA>
 * <EN>
 * Initialize content of a lexicon tree.
 * 
 * @param wchmm [out] pointer to the lexicon tree structure
 * </EN>
 */
static void
wchmm_init(WCHMM_INFO *wchmm)
{
  /* the resulting tree size is typically half of total state num */
  wchmm->maxwcn = wchmm->winfo->totalstatenum / 2;
  wchmm->state = (WCHMM_STATE *)mymalloc(sizeof(WCHMM_STATE)*wchmm->maxwcn);
  wchmm->self_a = (LOGPROB *)mymalloc(sizeof(LOGPROB)*wchmm->maxwcn);
  wchmm->next_a = (LOGPROB *)mymalloc(sizeof(LOGPROB)*wchmm->maxwcn);
  wchmm->ac = (A_CELL2 **)mymalloc(sizeof(A_CELL2 *)*wchmm->maxwcn);
  wchmm->stend = (WORD_ID *)mymalloc(sizeof(WORD_ID)*wchmm->maxwcn);
  wchmm->offset = (int **)mymalloc(sizeof(int *)*wchmm->winfo->num);
  wchmm->wordend = (int *)mymalloc(sizeof(int)*wchmm->winfo->num);
  wchmm->maxstartnum = STARTNODE_STEP;
  wchmm->startnode = (int *)mymalloc(sizeof(int)*STARTNODE_STEP);
  wchmm->startnum = 0;
  if (wchmm->category_tree) {
    wchmm->start2wid = (WORD_ID *)mymalloc(sizeof(WORD_ID)*STARTNODE_STEP);
  }
  if (wchmm->hmminfo->multipath) {
    wchmm->wordbegin = (int *)mymalloc(sizeof(int)*wchmm->winfo->num);
    wchmm->wrk.out_from = (int *)mymalloc(sizeof(int) * wchmm->winfo->maxwn);
    wchmm->wrk.out_from_next = (int *)mymalloc(sizeof(int) * wchmm->winfo->maxwn);
    wchmm->wrk.out_a = (LOGPROB *)mymalloc(sizeof(LOGPROB) * wchmm->winfo->maxwn);
    wchmm->wrk.out_a_next = (LOGPROB *)mymalloc(sizeof(LOGPROB) * wchmm->winfo->maxwn);
    wchmm->wrk.out_from_len = wchmm->winfo->maxwn;
  } else {
    wchmm->wordend_a = (LOGPROB *)mymalloc(sizeof(LOGPROB)*wchmm->winfo->num);
  }
#ifdef PASS1_IWCD
  wchmm->outstyle = (unsigned char *)mymalloc(sizeof(unsigned char)*wchmm->maxwcn);
#endif
#ifdef UNIGRAM_FACTORING
  wchmm->start2isolate = NULL;
  wchmm->isolatenum = 0;
#endif
  if (!wchmm->category_tree) {
#ifdef UNIGRAM_FACTORING
    wchmm->scword = NULL;
    wchmm->fscore = NULL;
#endif
    wchmm->sclist = NULL;
    wchmm->sclen = NULL;
  }

  wchmm->n = 0;
}

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤Î¾õÂÖ³ÊÇ¼ÎÎ°è¤EMAXWCNSTEP Ê¬¤À¤±¿­Ä¹¤¹¤E 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Expand state-related area in a tree lexicon by MAXWCNSTEP.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 */
static void
wchmm_extend(WCHMM_INFO *wchmm)
{
  /* practical value! */
  wchmm->maxwcn += wchmm->winfo->totalstatenum / 6;
  wchmm->state = (WCHMM_STATE *)myrealloc(wchmm->state, sizeof(WCHMM_STATE)*wchmm->maxwcn);
  wchmm->self_a = (LOGPROB *)myrealloc(wchmm->self_a, sizeof(LOGPROB)*wchmm->maxwcn);
  wchmm->next_a = (LOGPROB *)myrealloc(wchmm->next_a, sizeof(LOGPROB)*wchmm->maxwcn);
  wchmm->ac = (A_CELL2 **)myrealloc(wchmm->ac, sizeof(A_CELL2 *)*wchmm->maxwcn);
  wchmm->stend = (WORD_ID *)myrealloc(wchmm->stend, sizeof(WORD_ID)*wchmm->maxwcn);
#ifdef PASS1_IWCD
  wchmm->outstyle = (unsigned char *)myrealloc(wchmm->outstyle, sizeof(unsigned char)*wchmm->maxwcn);
#endif
}

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤ÎÃ±¸EèÆ¬¥Î¡¼¥É³ÊÇ¼ÎÎ°è¤ESTARTNODE_STEPÊ¬¤À¤±¿­Ä¹¤¹¤E  (multipath)
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Expand word-start nodes area in a tree lexicon by STARTNODE_STEP. (multipath)
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 */
static void
wchmm_extend_startnode(WCHMM_INFO *wchmm)
{
  wchmm->maxstartnum += STARTNODE_STEP;
  wchmm->startnode = (int *)myrealloc(wchmm->startnode, sizeof(int) * wchmm->maxstartnum);
  if (wchmm->category_tree) {
    wchmm->start2wid = (WORD_ID *)myrealloc(wchmm->start2wid, sizeof(WORD_ID) * wchmm->maxstartnum);
  }
}

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤ª¤è¤Ó¤½¤ÎÆâÉô¤Î³äÉÕ¥á¥â¥ê¤òÁ´¤Æ²òÊE¹¤E 
 * 
 * @param w [in] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Free all data in a tree lexicon.
 * 
 * @param w [in] tree lexicon
 * </EN>
 * @callgraph
 * @callergraph
 */
void
wchmm_free(WCHMM_INFO *w)
{
  int i;
  /* wchmm->state[i].ac malloced by mybmalloc2() */
  /* wchmm->offset[][] malloced by mybmalloc2() */
#ifdef PASS1_IWCD
  /* LRC_INFO, RC_INFO in wchmm->state[i].outsty malloced by mybmalloc2() */
#endif
  /* wchmm->sclist[][] and wchmm->sclen[] malloced by mybmalloc2() */
  /* they all will be freed by a single mybfree2() call */
  mybfree2(&(w->malloc_root));
  if (!w->category_tree) {
#ifdef UNIGRAM_FACTORING
    if (w->fscore != NULL) free(w->fscore);
#endif
  }
#ifdef UNIGRAM_FACTORING
  if (w->start2isolate != NULL) free(w->start2isolate);
#endif
#ifdef PASS1_IWCD
  free(w->outstyle);
#endif
  if (w->hmminfo->multipath) {
    free(w->wordbegin);
  } else {
    free(w->wordend_a);
  }
  if (w->category_tree) free(w->start2wid);
  free(w->startnode);
  free(w->wordend);
  free(w->offset);
  free(w->stend);
  free(w->ac);
  free(w->next_a);
  free(w->self_a);
  free(w->state);
#ifdef PASS1_IWCD
  if (w->category_tree) lcdset_remove_with_category_all(w);
#endif /* PASS1_IWCD */
  if (w->wrk.out_from_len != 0) {
    free(w->wrk.out_from);
    free(w->wrk.out_from_next);
    free(w->wrk.out_a);
    free(w->wrk.out_a_next);
    w->wrk.out_from_len = 0;
  }
  free(w);
}


/**************************************************************/
/*********** Word sort functions for tree construction ********/
/**************************************************************/

/** 
 * <JA>
 * Ã±¸Eò²»ÁÇ¤Î¤Ê¤é¤Ó¤Ç¥½¡¼¥È¤¹¤Esort_reentrant´Ø¿E * 
 * @param widx1 [in] Ã±¸ED 1 ¤Ø¤Î¥İ¥¤¥ó¥¿
 * @param widx2 [in] Ã±¸ED 2 ¤Ø¤Î¥İ¥¤¥ó¥¿
 * 
 * @return Ã±¸Eidx2¤¬Ã±¸Eidx1¤Î°Eô¤«¾º½ç¤Ç¤¢¤EĞ 1, Ã±¸Eidx1¤¬Ã±¸Eidx2¤Î°Eô¤«¾º½ç¤Ç¤¢¤EĞ -1, Á´¤¯Æ±¤¸²»ÁÇÊÂ¤Ó¤Ç¤¢¤EĞ 0 ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * qsort_reentrant function to sort words by their phoneme sequence.
 * 
 * @param widx1 [in] pointer to word id #1
 * @param widx2 [in] pointer to wrod id #2
 * 
 * @return 1 if word[widx2] is part of word[widx1], -1 if word[widx1] is part of word[widx2], or 0 if the two words are equal.
 * </EN>
 */
static int
compare_wseq(WORD_ID *widx1, WORD_ID *widx2, WORD_INFO *winfo)
{
  int len1, len2, n;
  int p=0;
  
  len1 = winfo->wlen[*widx1];
  len2 = winfo->wlen[*widx2];

  n=0;
  /*  while (n < len1 && n < len2 && (p = (int)winfo->wseq[*widx1][n] - (int)winfo->wseq[*widx2][n]) == 0 ) n++;*/
  while (n < len1 && n < len2 && (p = strcmp((winfo->wseq[*widx1][n])->name, (winfo->wseq[*widx2][n])->name)) == 0 ) n++;
  if (n < len1) {
    if (n < len2) {
      /* differ */
      return(p);
    } else {
      /* 2 is part of 1 */
      return(1);
    }
  } else {
    if (n < len2) {
      /* 1 is part of 2 */
      return(-1);
    } else {
      /* same */
      return(0);
    }
  }
}

/** 
 * <JA>
 * Ã±¸ED¤Î½¸¹Ewindex[bgn..bgn+len-1] ¤òÃ±¸EÎ²»ÁÇ¤Ê¤é¤Ó¤Ç¥½¡¼¥È¤¹¤E 
 * 
 * @param winfo [in] Ã±¸E­½E * @param windex [i/o] Ã±¸ED¤Î¥¤¥ó¥Ç¥Ã¥¯¥¹Îó¡ÊÆâÉô¤Ç¥½¡¼¥È¤µ¤EEË
 * @param bgn [in] @a windex ¤Î¥½¡¼¥È³«»ÏÅÀ
 * @param len [in] @a windex ¤Î @a bgn ¤«¤é¤Î¥½¡¼¥È¤¹¤E×ÁÇ¿E * </JA>
 * <EN>
 * Sort word IDs in windex[bgn..bgn+len-1] by their phoneme sequence order.
 * 
 * @param winfo [in] word lexicon
 * @param windex [i/o] index sequence of word IDs, (will be sorted in this function)
 * @param bgn [in] start point to sort in @a windex
 * @param len [in] length of indexes to be sorted from @a bgn
 * </EN>
 */
static void
wchmm_sort_idx_by_wseq(WORD_INFO *winfo, WORD_ID *windex, WORD_ID bgn, WORD_ID len)
{
  qsort_reentrant(&(windex[bgn]), len, sizeof(WORD_ID), (int (*)(const void *, const void *, void *))compare_wseq, winfo);
}

/** 
 * <JA>
 * Ã±¸Eò¥«¥Æ¥´¥êID¤Ç¥½¡¼¥È¤¹¤Esort´Ø¿E 
 * 
 * @param widx1 [in] Í×ÁÇ1¤Ø¤Î¥İ¥¤¥ó¥¿
 * @param widx2 [in] Í×ÁÇ2¤Ø¤Î¥İ¥¤¥ó¥¿
 * 
 * @return 
 * </JA>
 * <EN>
 * qsort function to sort words by their category ID.
 * 
 * @param widx1 [in] pointer to element #1
 * @param widx2 [in] pointer to element #2
 * 
 * @return 
 * </EN>
 */
static int
compare_category(WORD_ID *widx1, WORD_ID *widx2, WORD_INFO *winfo)
{
  int c1,c2;
  c1 = winfo->wton[*widx1];
  c2 = winfo->wton[*widx2];
  return(c1 - c2);
}

/** 
 * <JA>
 * Ã±¸ED½¸¹Ewindex[0..len-1] ¤ò¥«¥Æ¥´¥êID¤Ç¥½¡¼¥È¤¹¤E 
 * 
 * @param winfo [in] Ã±¸E­½E * @param windex [i/o] Ã±¸ED¤Î¥¤¥ó¥Ç¥Ã¥¯¥¹Îó¡ÊÆâÉô¤Ç¥½¡¼¥È¤µ¤EEË
 * @param len [in] @a windex ¤ÎÍ×ÁÇ¿E * </JA>
 * <EN>
 * Sort word IDs in windex[0..len-1] by their category ID.
 * 
 * @param winfo [in] tree lexicon
 * @param windex [i/o] index sequence of word IDs, (will be sorted in this function)
 * @param len [in] number of elements in @a windex
 * </EN>
 */
static void
wchmm_sort_idx_by_category(WORD_INFO *winfo, WORD_ID *windex, WORD_ID len)
{
  qsort_reentrant(windex, len, sizeof(WORD_ID), (int (*)(const void *, const void *, void *))compare_category, winfo);
}
  

/**********************************************************************/
/************** Subroutines to link part of words  ********************/
/**********************************************************************/

/** 
 * <JA>
 * £²Ã±¸EÖ¤Ç¡¤Ã±¸EÎÀèÆ¬¤«¤éÆ±°EÇ¶¦Í­²ÄÇ½¤Ê²»ÁÇ¤Î¿ô¤òÄ´¤Ù¤E 
 * 
 * @param winfo [in] Ã±¸E­½E * @param i [in] Ã±¸E±
 * @param j [in] Ã±¸E²
 * 
 * @return ¶¦Í­²ÄÇ½¤ÊÀèÆ¬¤«¤é¤Î²»ÁÇ¿ô¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * Compare two words from word head per phoneme to see how many phones
 * can be shared among the two.
 * 
 * @param winfo [in] word dictionary
 * @param i [in] a word
 * @param j [in] another word
 * 
 * @return the number of phonemes to be shared from the head of the words.
 * </EN>
 */
static int
wchmm_check_match(WORD_INFO *winfo, int i, int j)
{
  int k,tmplen;

  for (tmplen=0,k=0;k<winfo->wlen[i];k++) {
    if (k > winfo->wlen[j]-1)
      break;
    if (! (strmatch(winfo->wseq[i][k]->name, winfo->wseq[j][k]->name)))
      break;
    tmplen++;
  }
  return(tmplen);
}

/** 
 * <EN>
 * Initialize transition information on a node.
 * </EN>
 * <JA>
 * ¥Î¡¼¥É¤ÎÁ«°Ü¾ğÊó¤ò½é´E½¤¹¤E 
 * </JA>
 * 
 * @param wchmm [i/o] tree lexicon
 * @param node [in] node id
 * 
 */
static void
acc_init(WCHMM_INFO *wchmm, int node)
{
  wchmm->self_a[node] = LOG_ZERO;
  wchmm->next_a[node] = LOG_ZERO;
  wchmm->ac[node] = NULL;
}

/** 
 * <EN>
 * Add an arc to a node.
 * This function is for transition other than self and next node.
 * </EN>
 * <JA>
 * ¥Î¡¼¥É¤ËÁ«°Ü¤òÄÉ²Ã¤¹¤E 
 * ¤³¤Î´Ø¿ô¤Ï¼«¸ÊÁ«°Ü¡¦ÎÙ¤Ø¤ÎÁ«°Ü°Ê³°¤Î¾Eç¤Ë»ÈÍÑ¤µ¤EE 
 * </JA>
 * 
 * @param wchmm [i/o] tree lexicon
 * @param node [in] node id
 * @param a [in] transition probability in log10
 * @param arc [in] transition destination node id
 * 
 */
static void
add_ac(WCHMM_INFO *wchmm, int node, LOGPROB a, int arc)
{
  A_CELL2 *ac2;

  for(ac2=wchmm->ac[node];ac2;ac2=ac2->next) {
    if (ac2->n < A_CELL2_ALLOC_STEP) break;
  }
  if (ac2 == NULL) {
    ac2 = (A_CELL2 *)mybmalloc2(sizeof(A_CELL2), &(wchmm->malloc_root));
    ac2->n = 0;
    ac2->next = wchmm->ac[node];
    wchmm->ac[node] = ac2;
  }
  ac2->arc[ac2->n] = arc;
  ac2->a[ac2->n]   = a;
  ac2->n++;
}

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤Î¤¢¤EÎ¡¼¥É¤Ë¡¤ÊÌ¤Î¥Î¡¼¥É¤Ø¤ÎÁ«°Ü¤òÄÉ²Ã¤¹¤E * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param node [in] ¥Î¡¼¥ÉÈÖ¹E * @param a [in] Á«°Ü³ÎÎ¨¡ÊÂĞ¿ô¡Ë
 * @param arc [in] Á«°ÜÀè¤Î¥Î¡¼¥ÉÈÖ¹E * </JA>
 * <EN>
 * Add a transition arc between two nodes on the tree lexicon
 * 
 * @param wchmm [i/o] tree lexicon
 * @param node [in] node number of source node
 * @param a [in] transition probability in log scale
 * @param arc [in] node number of destination node
 * </EN>
 */
static void
add_wacc(WCHMM_INFO *wchmm, int node, LOGPROB a, int arc)
{
  if (arc == node) {
    wchmm->self_a[node] = a;
  } else if (arc == node + 1) {
    wchmm->next_a[node] = a;
  } else {
    add_ac(wchmm, node, a, arc);
  }
}

/**
 * <JA>
 * ¤¢¤E±¸EÎ¤¢¤EÌÃÖ¤Î²»ÁÇ¤«¤éÃ±¸EöÃ¼¤Î³°¤Ø½Ğ¤E«°Ü¤Î¥E¹¥È¤òÆÀ¤E (multipath) 
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * @param w [in] Ã±¸ED
 * @param pos [in] ²»ÁÇ°ÌÃÖ
 * @param node [out] ²»ÁÇÆâ¤Î¡¤Ã±¸EöÃ¼³°¤Ø¤ÎÁ«°Ü¤ò»ı¤Ä¾õÂÖ¤Î¥E¹¥È
 * @param a [out] @a node ¤Î³ÆÍ×ÁÇ¤ÎÁ«°Ü³ÎÎ¨
 * @param num [out] @a node ¤ÎÍ×ÁÇ¿E È¯¸«¿ô¤À¤±Áı²Ã¤µ¤EE 
 * @param maxnum [in] @a node ¤Î³ÊÇ¼²ÄÇ½¤ÊºÇÂç¿E * @param insert_sp [in] Ã±¸EªÃ¼¤Ç¤Î sp ¶´¤ß¹ş¤ß¤ò¹ÍÎ¸¤¹¤EÊ¤éTRUE
 * </JA>
 * <EN>
 * Make outgoing transition list for given phone position of a word. (multipath)
 * 
 * @param wchmm [in] tree lexicon
 * @param w [in] word ID
 * @param pos [in] location of target phone to be inspected in the word @a w
 * @param node [out] list of wchmm states that possibly has outgoing transition
 * @param a [out] transition probabilities of the outgoing transitions in @a node
 * @param num [out] number of elements in @a out (found num will be added)
 * @param maxnum [in] maximum number of elements that can be stored in @a node
 * @param insert_sp [in] TRUE if consider short-pause insertion on word end
 * </EN>
 */
static void
get_outtrans_list(WCHMM_INFO *wchmm, WORD_ID w, int pos, int *node, LOGPROB *a, int *num, int maxnum, boolean insert_sp)
{
  HMM_Logical *ltmp;
  int states;
  int k;
  LOGPROB prob;
  int oldnum;

  if (pos < 0) {
    
    /* set the word-beginning node, and return */
    node[*num] = wchmm->wordbegin[w];
    a[*num] = 0.0;
    (*num)++;
    
  } else {

    ltmp = wchmm->winfo->wseq[w][pos];
    states = hmm_logical_state_num(ltmp);

    /* check initial->final state */
    if ((hmm_logical_trans(ltmp))->a[0][states-1] != LOG_ZERO) {
      /* recursive call for previous phone */
      oldnum = *num;
      get_outtrans_list(wchmm, w, pos-1, node, a, num, maxnum, FALSE); /* previous phone should not be an sp-inserted phone */
      /* add probability of the skip transition to all the previous ones */
      for(k=oldnum;k<*num;k++) {
	a[k] += (hmm_logical_trans(ltmp))->a[0][states-1];
      }
    }
    /* add to list the arcs from output state to final state */
    for (k = 1; k < states - 1; k++) {
      prob = (hmm_logical_trans(ltmp))->a[k][states-1];
      if (prob != LOG_ZERO) {
	if (*num >= maxnum) {
	  j_internal_error("get_outtrans_list: maximum outtrans list num exceeded %d\n", maxnum);
	}
	node[*num] = wchmm->offset[w][pos] + k - 1;
	a[*num] = prob;
	(*num)++;
      }
    }
    /* for -iwsp, add outgoing arc from the tail sp model
       only if need_sp == TRUE.
       need_sp should be TRUE only when the connecting [pos] phone is also an end phone of the to-be-added word (i.e. homophone word)
     */
    /*  */
    if (insert_sp) {
      /* consider sp */
      for (k = 1; k < hmm_logical_state_num(wchmm->hmminfo->sp) - 1; k++) {
	prob = hmm_logical_trans(wchmm->hmminfo->sp)->a[k][hmm_logical_state_num(wchmm->hmminfo->sp)-1];
	if (prob != LOG_ZERO) {
	  if (*num >= maxnum) {
	    j_internal_error("get_outtrans_list: maximum outtrans list num exceeded %d\n", maxnum);
	  }
	  node[*num] = wchmm->offset[w][pos] + (states - 2) + k - 1;
	  a[*num] = prob;
	  (*num)++;
	}
      }
    }
  }
  /*printf("   %d(%s)-%d:\"%s\", num=%d\n", w, wchmm->winfo->woutput[w], pos,
    (pos < 0) ? "BGN" : wchmm->winfo->wseq[w][pos]->name, *num);*/
  return;
}  

/** 
 * <JA>
 * ¤¢¤E»ÁÇ¤ÎËöÈø¤Î¾õÂÖ¤«¤é¡¤¤¢¤E»ÁÇ¤ÎÀèÆ¬¾õÂÖ¤Ø¤ÎÁ«°Ü¤òÄÉ²Ã¤¹¤E 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param from_node [in] ¤¢¤E»ÁÇ¤ÎËöÈø¤Î¾õÂÖ
 * @param to_node [in] ¤¢¤E»ÁÇ¤ÎÀèÆ¬¾õÂÖ
 * @param tinfo [in] @a from_node ¤ÎÂ°¤¹¤E»ÁÇHMM¤ÎÁ«°Ü³ÎÎ¨¹ÔÎE * </JA>
 * <EN>
 * Add a transition from end node of a phone to start node of another phone.
 * 
 * @param wchmm [i/o] tree lexicon
 * @param from_node [in] end node of a phone
 * @param to_node [in] start node of a phone
 * @param tinfo [in] transition prob. matrix of the @a from_node phone.
 * </EN>
 */
static void
wchmm_link_hmm(WCHMM_INFO *wchmm, int from_node, int to_node, HTK_HMM_Trans *tinfo)
{     
  A_CELL2 *actmp;
  LOGPROB a;
  int i, j;
  boolean tflag;

  /* get transition probability to outer state in tinfo */
  for(i = tinfo->statenum - 2; i >= 0; i--) {
    if ((a = tinfo->a[i][tinfo->statenum-1]) != LOG_ZERO) { /* found */
      /* check if the arc already exist */
      tflag = FALSE;
      if (to_node == from_node && wchmm->self_a[from_node] == a) {
	tflag = TRUE;
      } else if (to_node == from_node + 1 && wchmm->next_a[from_node] == a) {
	tflag = TRUE;
      } else {
	for (actmp = wchmm->ac[from_node]; actmp; actmp = actmp->next) {
	  for(j=0;j<actmp->n;j++) {
	    if (actmp->arc[j] == to_node && actmp->a[j] == a) {
	      tflag = TRUE;
	      break;
	    }
	  }
	  if (tflag == TRUE) break;
	}
      }
      if (tflag) break;
      /* add the arc to wchmm */
      add_wacc(wchmm, from_node, a, to_node);
      return;			/* exit function here */
    }
  }      
  j_internal_error("wchmm_link_hmm: No arc to endstate?\n");
}

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñÃæ¤Î£²Ã±¸Eæ¤Î¤¢¤E»ÁÇ´Ö¤òÀÜÂ³¤¹¤E 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param from_word [in] Á«°Ü¸µ¤ÎÃ±¸EÎID
 * @param from_seq [in] Á«°Ü¸µ¤ÎÃ±¸Eæ¤ÎÀÜÂ³¤¹¤E»ÁÇ¤Î°ÌÃÖ
 * @param to_word [in] Á«°ÜÀè¤ÎÃ±¸EÎID
 * @param to_seq [in] Á«°ÜÀè¤ÎÃ±¸Eæ¤ÎÀÜÂ³¤¹¤E»ÁÇ¤Î°ÌÃÖ
 * </JA>
 * <EN>
 * Connect two phonemes in tree lexicon.
 * 
 * @param wchmm [i/o] tree lexicon
 * @param from_word [in] source word ID
 * @param from_seq [in] index of source phoneme in @a from_word from which the other will be connected
 * @param to_word [in] destination word ID
 * @param to_seq [in] index of destination phoneme in @a to_word to which the other will connect
 * </EN>
 */
static void
wchmm_link_subword(WCHMM_INFO *wchmm, int from_word, int from_seq, int to_word, int to_seq)
{     
  HMM_Logical *last;
  int lastp;

  last = wchmm->winfo->wseq[from_word][from_seq];
  lastp = wchmm->offset[from_word][from_seq] + hmm_logical_state_num(last)-2 -1;
  wchmm_link_hmm(wchmm, lastp, wchmm->offset[to_word][to_seq],
		 hmm_logical_trans(last));
}

/**************************************************************/
/******** homophone processing: duplicating leaf nodes ********/
/**************************************************************/

/**
 * @note
 * <JA>
 * Æ±²»¸EèÍı:
 * ÌÚ¹½Â¤²½¼­½ñ¤Ë¤ª¤¤¤Æ¤¹¤Ù¤Æ¤ÎÃ±¸EÏÆÈÎ©¤·¤¿ºÇ½ª¾õÂÖ¤ò»ı¤ÄÉ¬Í×¤¬¤¢¤E¿¤á¡¤
 * Æ±²»¸EÏÃúÌÕ¿¼¤¯°·¤¦É¬Í×¤¬¤¢¤E ¤³¤Î¤¿¤á¡¤ºÇ½é¤ÎÌÚ¹½Â¤²½¼­½ñ¤ò¹½ÃÛ¤·¤¿¸E 
 * ÊÌ¤ÎÃ±¸EÈ´°Á´¤Ë¶¦Í­¤µ¤E¿Ã±¸EÆ±²»¸E, ¤¢¤E¤¤ÏÊÌ¤ÎÃ±¸EÎ°Eô¤È¤·¤Æ
 * Ëä¤á¹ş¤Ş¤EÆ¤·¤Ş¤Ã¤Æ¤¤¤E±¸EòÈ¯¸«¤¹¤EÈ¤È¤â¤Ë, ¤½¤ÎºÇ½ª¥Î¡¼¥É¤E * ¥³¥Ô¡¼¤·¤Æ¿·¤¿¤ÊÃ±¸EªÃ¼¥Î¡¼¥É¤òºûÀE¬Í×¤¬¤¢¤E 
 * </JA>
 * <EN>
 * Homophones:
 * As all words need to have an uniq state as a final state in a lexicon tree,
 * homophones should be handled carefully.  After primal tree has been made,
 * we look through the tree to find the fully shared or embedded words
 * (homophone or part of other word), and duplicate the last leaf node 
 * to have uniq end state.
 * </EN>
 */

/** 
 * <JA>
 * Ã±¸EªÃ¼¾õÂÖ¤ÎÆÈÎ©²½¡§Í¿¤¨¤é¤E¿Ã±¸EÎ½ªÃ¼¥Î¡¼¥É¤ò¥³¥Ô¡¼¤·¤Æ¡¤
 * ¿·¤¿¤Ë¤¢¤E±¸EÎºÇ½ª¾õÂÖ¤È¤·¤ÆÄEÁ¤¹¤E 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param node [in] Æ±²»¸EÎ½ªÃ¼¥Î¡¼¥ÉÈÖ¹E * @param word [in] ¿·¤¿¤ËÅĞÏ¿¤¹¤E±¸E * </JA>
 * <EN>
 * Isolation of word-end nodes for homophones: duplicate the word-end state,
 * link as the same as original, and make it the new word-end node of the
 * given new word.
 * 
 * @param wchmm [i/o] tree lexicon
 * @param node [in] the word end node of the already existing homophone
 * @param word [in] word ID to be added to the tree
 * </EN>
 */
static void
wchmm_duplicate_state(WCHMM_INFO *wchmm, int node, int word) /* source node, new word */
{
  int j, n;
  int n_src, n_prev;
  A_CELL2	*ac;
  HMM_Logical *lastphone;

  /* 1 state will newly created: expand tree if needed */
  if (wchmm->n + 1 >= wchmm->maxwcn) {
    wchmm_extend(wchmm);
  }
  /* n: the target new node to which 'node' is copied */
  n = wchmm->n;

  n_src = node;

  /* copy output probability info */
#ifdef PASS1_IWCD
  {
    RC_INFO *rcnew;
    LRC_INFO *lrcnew;
    wchmm->outstyle[n] = wchmm->outstyle[n_src];
    if (wchmm->outstyle[n] == AS_RSET) {
      /* duplicate RC_INFO because it has its own cache */
      rcnew = (RC_INFO *)mybmalloc2(sizeof(RC_INFO), &(wchmm->malloc_root));
      memcpy(rcnew, wchmm->state[n_src].out.rset, sizeof(RC_INFO));
      wchmm->state[n].out.rset = rcnew;
    } else if (wchmm->outstyle[n] == AS_LRSET) {
      /* duplicate LRC_INFO because it has its own cache */
      lrcnew = (LRC_INFO *)mybmalloc2(sizeof(LRC_INFO), &(wchmm->malloc_root));
      memcpy(lrcnew, wchmm->state[n_src].out.lrset, sizeof(LRC_INFO));
      wchmm->state[n].out.lrset = lrcnew;
    } else {
      /* share same info, simply copy the pointer */
      memcpy(&(wchmm->state[n].out), &(wchmm->state[n_src].out), sizeof(ACOUSTIC_SPEC));
    }
  }
#else  /* ~PASS1_IWCD */
  memcpy(&(wchmm->state[n].out), &(wchmm->state[n_src].out), sizeof(HTK_HMM_State *));
#endif

  lastphone = wchmm->winfo->wseq[word][wchmm->winfo->wlen[word]-1];
  acc_init(wchmm, n);

  /* add self transition arc */
  wchmm->self_a[n] = wchmm->self_a[n_src];
  
  /* copy transition arcs whose destination is the source node to new node */
  if (hmm_logical_state_num(lastphone) == 3) { /* = 1 state */
    /* phone with only 1 state should be treated carefully */
    if (wchmm->winfo->wlen[word] == 1) { /* word consists of only this phone */
      /* no arcs need to be copied: this is also a start node of a word */
      wchmm->offset[word][0] = n;
      /* index the new word-beginning node as startnode (old ststart) */
      if (wchmm->lmtype != LM_PROB || word != wchmm->winfo->head_silwid) {
	wchmm->startnode[wchmm->startnum] = n;
	if (wchmm->category_tree) wchmm->start2wid[wchmm->startnum] = word;
	/* expand data area if necessary */
	if (++wchmm->startnum >= wchmm->maxstartnum) wchmm_extend_startnode(wchmm);
      }
    } else {
      /* copy arcs from the last state of the previous phone */
      n_prev = wchmm->offset[word][wchmm->winfo->wlen[word]-2]
	+ hmm_logical_state_num(wchmm->winfo->wseq[word][wchmm->winfo->wlen[word]-2]) - 3;
      if(n_src == n_prev + 1) {
	add_wacc(wchmm, n_prev, wchmm->next_a[n_prev], n);
      } else {
	for(ac=wchmm->ac[n_prev];ac;ac=ac->next) {
	  for(j=0;j<ac->n;j++) {
	    if (ac->arc[j] == n_src) {
	      add_wacc(wchmm, n_prev, ac->a[j], n);
	    }
	  }
	}
      }
      /* also update the last offset (== wordend in this case) */
      wchmm->offset[word][wchmm->winfo->wlen[word]-1] = n;
    }
  } else {			/* phone with more than 2 states */
    /* copy arcs from/to the source node to new node */
    for (n_prev = wchmm->offset[word][wchmm->winfo->wlen[word]-1]; n_prev < n_src; n_prev++) {
      if (n_src == n_prev + 1) {
	add_wacc(wchmm, n_prev, wchmm->next_a[n_prev], n);
      } else {
	for(ac=wchmm->ac[n_prev];ac;ac=ac->next) {
	  for(j=0;j<ac->n;j++) {
	    if (ac->arc[j] == n_src) {
	      add_wacc(wchmm, n_prev, ac->a[j], n);
	    }
	  }
	}
      }
      if (n_prev == n_src + 1) {
	add_wacc(wchmm, n, wchmm->next_a[n_src], n_prev);
      } else {
	for(ac=wchmm->ac[n_src];ac;ac=ac->next) {
	  for(j=0;j<ac->n;j++) {
	    if (ac->arc[j] == n_prev) {
	      add_wacc(wchmm, n, ac->a[j], n_prev);
	    }
	  }
	}
      }
    }
  }

  /* map word <-> node */
  wchmm->stend[n]   = word;	/* 'n' is an end node of word 'word' */
  wchmm->wordend[word] = n;	/* the word end node of 'word' is 'n' */

  /* new state has been created: increment the size */
  wchmm->n++;
  
}

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñÁ´ÂÎ¤òÁöºº¤·¤Æ¡¤¤¹¤Ù¤Æ¤ÎÆ±²»¸EË¤Ä¤¤¤ÆÃ±¸EªÃ¼¾õÂÖ¤ÎÆÈÎ©²½
 * ¤ò¹Ô¤¦. 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Scan the whole lexicon tree to find already registered homophones, and
 * make word-end nodes of the found homophones isolated from others.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 */
static int
wchmm_duplicate_leafnode(WCHMM_INFO *wchmm)
{
  int w, nlast, n, narc, narc_model;
  boolean *dupw;		/* node marker */
  A_CELL2 *actmp;
  int dupcount;

  dupcount = 0;

  nlast = wchmm->n;
  dupw = (boolean *)mymalloc(sizeof(boolean) * nlast);
  for(n=0;n<nlast;n++) dupw[n] = FALSE;	/* initialize all marker */

  for (w=0;w<wchmm->winfo->num;w++) {
    n = wchmm->wordend[w];
    if (dupw[n]) {		/* if already marked (2nd time or later */
      wchmm_duplicate_state(wchmm, n, w); dupcount++; /* duplicate */
    } else {			/* if not marked yet (1st time) */
      /* try to find an arc outside the word */
      {
	/* count number of model-internal arc from the last state */
	HMM_Logical *lastphone;
	HTK_HMM_Trans *tinfo;
	int laststate, i;
	lastphone = wchmm->winfo->wseq[w][wchmm->winfo->wlen[w]-1];
	laststate = hmm_logical_state_num(lastphone) - 2;
	tinfo = hmm_logical_trans(lastphone);
	narc_model=0;
	for(i=1;i<hmm_logical_state_num(lastphone)-1;i++) {
	  if (tinfo->a[laststate][i] != LOG_ZERO) narc_model++;
	}
	/* count number of actual arc from the last state in the tree */
	narc = 0;
	if (wchmm->self_a[n] != LOG_ZERO) narc++;
	if (wchmm->next_a[n] != LOG_ZERO) narc++;
	for(actmp=wchmm->ac[n];actmp;actmp=actmp->next) narc += actmp->n;
      }
      /* if both number does not match, it means it is not a single word tail */
      if (narc_model != narc) {
	/* word 'w' is embedded as part of other words at this node 'n' */
	/* duplicate this node now */
	wchmm_duplicate_state(wchmm, n, w); dupcount++;
	/* as new node has been assigned as word end node of word 'w',
	   reset this source node as it is not the word end node */
	wchmm->stend[n] = WORD_INVALID;
      } else {
	/* no arc to other node found, it means it is a single word tail */
	/* as this is first time, only make sure that this node is word end of [w] */
	wchmm->stend[n] = w;
      }
      /* mark node 'n' */
      dupw[n] = TRUE;
    }
  }
  free(dupw);

  return(dupcount);
}

/**************************************************************/
/*************** add a word to wchmm lexicon tree *************/
/**************************************************************/

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤Ë¿·¤¿¤ËÃ±¸EòÄÉ²Ã¤¹¤E ÄÉ²Ã¾Eê¤Î¾ğÊó¤È¤·¤Æ¡¤¸½ºß¤ÎÌÚ¹½Â¤²½
 * ¼­½ñÆâ¤ÇºÇ¤â¤½¤ÎÃ±¸EÈÀèÆ¬¤«¤éÎÉ¤¯¥Ş¥Ã¥Á¤¹¤E±¸E¤¤ª¤è¤Ó¤½¤Î¥Ş¥Ã¥Á¤¹¤E¹¤µ
 * ¤ò»ØÄê¤¹¤E 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param word [in] ÄÉ²Ã¤¹¤E­½ñÃ±¸EÎID
 * @param matchlen [in] @a word ¤È @a matchword ¤ÎÀèÆ¬¤«¤é¥Ş¥Ã¥Á¤¹¤E»ÁÇÄ¹
 * @param matchword [in] ´ûÂ¸¤ÎÌÚ¹½Â¤²½¼­½ñÃæ¤Ç @a word ¤ÈºÇ¤â¥Ş¥Ã¥Á¤¹¤E±¸E * @param enable_iwsp [in] Ã±¸EÖ¥·¥ç¡¼¥È¥İ¡¼¥ºµ¡Ç½»ÈÍÑ»şTRUE¤ò»ØÄE * </JA>
 * <EN>
 * Add a new word to the lexicon tree.  The longest matched word in the current
 * lexicon tree and the length of the matched phoneme from the word head should
 * be specified to tell where to insert the new word to the tree.
 * 
 * @param wchmm [i/o] tree lexicon
 * @param word [in] word id to be added to the lexicon
 * @param matchlen [in] phoneme match length between @a word and @a matchword.
 * @param matchword [in] the longest matched word with @a word in the current lexicon tree
 * @param enable_iwsp [in] should be TRUE when using inter-word short pause option
 * </EN>
 */
static boolean
wchmm_add_word(WCHMM_INFO *wchmm, int word, int matchlen, int matchword, boolean enable_iwsp)
{
  boolean ok_p;
  int   j,k,n;
  int   add_head, add_tail, add_to;
  int   word_len, matchword_len;
  HMM_Logical *ltmp;
  int ato;
  LOGPROB prob;
  int ntmp;
  int ltmp_state_num;
#ifdef PASS1_IWCD
  CD_Set *lcd = NULL;
#endif
  int *out_from;
  int *out_from_next;
  LOGPROB *out_a;
  LOGPROB *out_a_next;

  
  /* for multipath handling */
  int out_num_prev, out_num_next;
  int kkk;

  ok_p = TRUE;
  if (wchmm->hmminfo->multipath) {
    out_from = wchmm->wrk.out_from;
    out_from_next = wchmm->wrk.out_from_next;
    out_a = wchmm->wrk.out_a;
    out_a_next = wchmm->wrk.out_a_next;
  }
  
/* 
 *   if (matchlen > 0) {
 *     printf("--\n");
 *     put_voca(stdout, wchmm->winfo, word);
 *     put_voca(stdout, wchmm->winfo, matchword);
 *     printf("matchlen=%d\n", matchlen);
 *   }
 */
  
  /* variable abbreviations */
  n = wchmm->n;
  word_len      = wchmm->winfo->wlen[word];
  matchword_len = wchmm->winfo->wlen[matchword];

  /* malloc phone offset area */
  wchmm->offset[word] = (int *)mybmalloc2(sizeof(int)*word_len, &(wchmm->malloc_root));

  /* allocate unshared (new) part */
  add_head = matchlen;
  add_tail = word_len - 1;
  add_to   = matchlen - 1;

  if (wchmm->hmminfo->multipath) {
    /* make word-beginning node if needed */
    if (matchlen == 0) {
      /* create word-beginning node */
      wchmm->wordbegin[word] = n;
      wchmm->stend[n] = WORD_INVALID;
      acc_init(wchmm, n);
      wchmm->state[n].out.state = NULL;
      /* index the new word-beginning node as startnode (old ststart) */
      wchmm->startnode[wchmm->startnum] = n;
      if (wchmm->category_tree) wchmm->start2wid[wchmm->startnum] = word;
      /* expand data area if necessary */
      if (++wchmm->startnum >= wchmm->maxstartnum) wchmm_extend_startnode(wchmm);
      if (++n >= wchmm->maxwcn) wchmm_extend(wchmm);
    } else {
      wchmm->wordbegin[word] = wchmm->wordbegin[matchword];
    }

    /* now n is at beginning of output state */

    /* store the initial outgoing arcs to out_from[] and out_a[] */
    out_num_prev = 0;
    if (matchlen == 0) {
      /* set the word-beginning node */
      out_from[0] = wchmm->wordbegin[word];
      out_a[0] = 0.0;
      out_num_prev = 1;
    } else {
      /*printf("%d(%s)\n", word, wchmm->winfo->woutput[word]);*/
      /* on -iwsp, trailing sp is needed only when no phone will be created */
      get_outtrans_list(wchmm, matchword, add_to, out_from, out_a, &out_num_prev, wchmm->winfo->maxwn, (enable_iwsp && add_tail - add_head + 1 <= 0) ? TRUE : FALSE);
      /*printf("NUM=%d\n", out_num_prev);*/
    }
  } else { /*  end of multipath block */
    if (matchlen == 0) {
      if (wchmm->lmtype != LM_PROB || word != wchmm->winfo->head_silwid) {
	/* index the new word-beginning node as startnode (old ststart) */
	wchmm->startnode[wchmm->startnum] = n;
	if (wchmm->category_tree) wchmm->start2wid[wchmm->startnum] = word;
	/* expand data area if necessary */
	if (++wchmm->startnum >= wchmm->maxstartnum) wchmm_extend_startnode(wchmm);
      }
    }
  }
  
  if (add_tail - add_head + 1 > 0) { /* there are new phones to be created */
      ntmp = n;
      for (j=add_head; j <= add_tail; j++) { /* for each new phones */
	ltmp = wchmm->winfo->wseq[word][j];
	ltmp_state_num = hmm_logical_state_num(ltmp);
#ifdef PASS1_IWCD
	if (wchmm->ccd_flag) {
	  /* in the triphone lexicon tree, the last phone of a word has
	     left-context cdset */
	  if (wchmm->winfo->wlen[word] > 1 && j == wchmm->winfo->wlen[word] - 1) {
	    if (wchmm->category_tree) {
#ifdef USE_OLD_IWCD
	      lcd = lcdset_lookup_by_hmmname(wchmm->hmminfo, ltmp->name);
#else
	      lcd = lcdset_lookup_with_category(wchmm, ltmp, wchmm->winfo->wton[word]);
	      if (lcd == NULL) {
		/* no category-aware cdset found.  This is case when no word
		   can follow this word grammatically.
		   so fallback to normal state */
		jlog("WARNING: wchmm: no lcdset found for [%s::%04d], fallback to [%s]\n", ltmp->name, wchmm->winfo->wton[word], ltmp->name);
		lcd = lcdset_lookup_by_hmmname(wchmm->hmminfo, ltmp->name);
	      }
#endif
	    } else {
	      lcd = lcdset_lookup_by_hmmname(wchmm->hmminfo, ltmp->name);
	    }
	    if (lcd == NULL) {
	      jlog("ERROR: wchmm: at word #%d: no lcdset found for [%s]\n", word, ltmp->name);
	      ok_p = FALSE;
	    }
	  }
	}
#endif /* PASS1_IWCD */
	for (k = 1; k < ltmp_state_num - 1; k++) { /* for each state in the phone */
	  /* set state output prob info */
#ifdef PASS1_IWCD
	  if (wchmm->ccd_flag) {
	    /* output info of triphones needs special handling */
	    if (wchmm->winfo->wlen[word] == 1) { /* word with only 1 phone */
	      wchmm->outstyle[ntmp] = AS_LRSET;
	      wchmm->state[ntmp].out.lrset = (LRC_INFO *)mybmalloc2(sizeof(LRC_INFO), &(wchmm->malloc_root));
	      (wchmm->state[ntmp].out.lrset)->hmm       = ltmp;
	      (wchmm->state[ntmp].out.lrset)->state_loc = k;
	      if (wchmm->category_tree) {
		(wchmm->state[ntmp].out.lrset)->category  = wchmm->winfo->wton[word];
	      }
	    } else if (j == 0) {	/* head phone of a word */
	      wchmm->outstyle[ntmp] = AS_RSET;
	      wchmm->state[ntmp].out.rset = (RC_INFO *)mybmalloc2(sizeof(RC_INFO), &(wchmm->malloc_root));
	      (wchmm->state[ntmp].out.rset)->hmm       = ltmp;
	      (wchmm->state[ntmp].out.rset)->state_loc = k;
	    } else if (j == wchmm->winfo->wlen[word] - 1) { /* last phone of a word */
	      wchmm->outstyle[ntmp] = AS_LSET;
	      wchmm->state[ntmp].out.lset = &(lcd->stateset[k]);
	    } else {
	      wchmm->outstyle[ntmp] = AS_STATE;
	      if (ltmp->is_pseudo) {
		jlog("WARNING: wchmm: word-internal phone should not be pseudo\n");
		put_voca(stdout, wchmm->winfo, word);
		ok_p = FALSE;
	      }
	      wchmm->state[ntmp].out.state = ltmp->body.defined->s[k];
	    }
	  } else {
	    /* monophone */
	    if (ltmp->is_pseudo) {
	      j_internal_error("wchmm_add_word: CDSET phoneme exist in monophone?\n");
	      put_voca(stdout, wchmm->winfo, word);
	      ok_p = FALSE;
	    }
	    wchmm->outstyle[ntmp] = AS_STATE;
	    wchmm->state[ntmp].out.state = ltmp->body.defined->s[k];
	  }
#else  /* ~PASS1_IWCD */
	  if (ltmp->is_pseudo) {
	    j_internal_error("wchmm_add_word: CDSET phoneme exist in monophone?\n");
	    put_voca(stdout, wchmm->winfo, word);
	    ok_p = FALSE;
	  }
	  wchmm->state[ntmp].out = ltmp->body.defined->s[k];
#endif /* PASS1_IWCD */
	  
 	  /* initialize other info */
	  acc_init(wchmm, ntmp);
	  wchmm->stend[ntmp] = WORD_INVALID;
	  if (! wchmm->hmminfo->multipath) {
	    /* make transition arc from HMM transition info */
	    for (ato = 1; ato < ltmp_state_num; ato++) {
	      prob = (hmm_logical_trans(ltmp))->a[k][ato];
	      if (prob != LOG_ZERO) {
		  if (j == add_tail && k == ltmp_state_num - 2 && ato == ltmp_state_num - 1) {
		    /* arc outside new part will be handled later */
		  } else {
		    add_wacc(wchmm, ntmp, prob, ntmp + ato - k);
		  }
		}
	    }
	  }
	  
	  ntmp++;
	  /* expand wchmm if neccesary */
	  if (ntmp >= wchmm->maxwcn) wchmm_extend(wchmm);
        } /* end of state loop */
      }	/* end of phone loop */

      if (wchmm->hmminfo->multipath) {
	  
	/* On multipath version, the skip transition should be handled! */
      
	/* make transition arc from HMM transition info */
	ntmp = n;
	for (j = add_head; j <= add_tail; j++) {
	  ltmp = wchmm->winfo->wseq[word][j];
	  ltmp_state_num = hmm_logical_state_num(ltmp);
	  out_num_next = 0;
	  /* arc from initial state ... need arc expansion from precious phone */
	  for (ato = 1; ato < ltmp_state_num; ato++) {
	    prob = (hmm_logical_trans(ltmp))->a[0][ato];
	    if (prob != LOG_ZERO) {
	      /* expand arc from previous HMM */
	      if (ato == ltmp_state_num - 1) {
		/* to final state ... just register states for next expansion */
		for(kkk=0; kkk<out_num_prev; kkk++) {
		  out_from_next[out_num_next] = out_from[kkk];
		  out_a_next[out_num_next] = out_a[kkk] + prob;
		  out_num_next++;
		}
	      } else {
		for(kkk=0; kkk<out_num_prev; kkk++) {
		  add_wacc(wchmm, out_from[kkk], out_a[kkk] + prob, ntmp + ato - 1);
		}
	      }
	    }
	  } /* end of state loop */
	  /* from outprob state */
	  for(k = 1; k < ltmp_state_num - 1; k++) {
	    for (ato = 1; ato < ltmp_state_num; ato++) {
	      prob = (hmm_logical_trans(ltmp))->a[k][ato];
	      if (prob != LOG_ZERO) {
		if (ato == ltmp_state_num - 1) {
		  /* to final state ... register states for next expansion */
		  out_from_next[out_num_next] = ntmp;
		  out_a_next[out_num_next] = prob;
		  out_num_next++;
		} else {
		  add_wacc(wchmm, ntmp, prob, ntmp + ato - k);
		}
	      }
	    }
	    ntmp++;
	  } /* end of state loop */
	  /* swap out list for next phone */
	  for(kkk=0;kkk<out_num_next;kkk++) {
	    out_from[kkk] = out_from_next[kkk];
	    out_a[kkk] = out_a_next[kkk];
	  }
	  out_num_prev = out_num_next;
	}	/* end of phone loop */
      }	/* end of multipath block */
      
  } /* new phone node creation loop for this word */


  /*************************************/
  /* Short Pause appending (multipath) */
  /*************************************/
  
  /* if -iwsp, add noise model to the end of word at ntmp */
  if (wchmm->hmminfo->multipath && enable_iwsp && add_tail - add_head + 1 > 0) { /* there are new phones to be created */
    int ntmp_bak;
    
    /* set short pause state info */
    ntmp_bak = ntmp;
    if (wchmm->hmminfo->sp->is_pseudo) {
      for(k = 1;k < hmm_logical_state_num(wchmm->hmminfo->sp) - 1; k++) {
	wchmm->outstyle[ntmp] = AS_LSET;
	wchmm->state[ntmp].out.lset = &(wchmm->hmminfo->sp->body.pseudo->stateset[k]);
	acc_init(wchmm, ntmp);
	wchmm->stend[ntmp] = WORD_INVALID;
	ntmp++;
	if (ntmp >= wchmm->maxwcn) wchmm_extend(wchmm);
      }
    } else {
      for(k = 1;k < hmm_logical_state_num(wchmm->hmminfo->sp) - 1; k++) {
	wchmm->outstyle[ntmp] = AS_STATE;
	wchmm->state[ntmp].out.state = wchmm->hmminfo->sp->body.defined->s[k];
	acc_init(wchmm, ntmp);
	wchmm->stend[ntmp] = WORD_INVALID;
	ntmp++;
	if (ntmp >= wchmm->maxwcn) wchmm_extend(wchmm);
      }
    }
    ntmp = ntmp_bak;
    /* connect incoming arcs from previous phone */
    out_num_next = 0;
    for (ato = 1; ato < hmm_logical_state_num(wchmm->hmminfo->sp); ato++) {
      prob = hmm_logical_trans(wchmm->hmminfo->sp)->a[0][ato];
      if (prob != LOG_ZERO) {
	/* to control short pause insertion, transition probability toward
	 the word-end short pause will be given a penalty */
	prob += wchmm->hmminfo->iwsp_penalty;
	if (ato == hmm_logical_state_num(wchmm->hmminfo->sp) - 1) {
	  /* model has a model skip transition, just inherit them to next */
	  for(kkk=0; kkk<out_num_prev; kkk++) {
	    out_from_next[out_num_next] = out_from[kkk];
	    out_a_next[out_num_next] = out_a[kkk] + prob;
	    out_num_next++;
	  }
	} else {
	  /* connect incoming arcs from previous phone to this phone */
	  for(kkk=0; kkk<out_num_prev; kkk++) {
	    add_wacc(wchmm, out_from[kkk], out_a[kkk] + prob, ntmp + ato - 1);
	  }
	}
      }
    }
    /* if short pause model doesn't have a model skip transition, also add it */
    if (hmm_logical_trans(wchmm->hmminfo->sp)->a[0][hmm_logical_state_num(wchmm->hmminfo->sp)-1] == LOG_ZERO) {
      /* to make insertion sp model to have no effect on the original path,
	 the skip transition probability should be 0.0 (=100%) */
      prob = 0.0;
      for(kkk=0; kkk<out_num_prev; kkk++) {
	out_from_next[out_num_next] = out_from[kkk];
	out_a_next[out_num_next] = out_a[kkk] + prob;
	out_num_next++;
      }
    }
    /* connect arcs within model, and store new outgoing arcs for wordend node */
    for (k = 1; k < hmm_logical_state_num(wchmm->hmminfo->sp) - 1; k++) {
      for (ato = 1; ato < hmm_logical_state_num(wchmm->hmminfo->sp); ato++) {
	prob = hmm_logical_trans(wchmm->hmminfo->sp)->a[k][ato];
	if (prob != LOG_ZERO) {
	  if (ato == hmm_logical_state_num(wchmm->hmminfo->sp) - 1) {
	    out_from_next[out_num_next] = ntmp;
	    out_a_next[out_num_next] = prob;
	    out_num_next++;
	  } else {
	    add_wacc(wchmm, ntmp, prob, ntmp + ato - k);
	  }
	}
      }
      ntmp++;
    }
    /* swap work area for next */
    for(kkk=0;kkk<out_num_next;kkk++) {
      out_from[kkk] = out_from_next[kkk];
      out_a[kkk] = out_a_next[kkk];
    }
    out_num_prev = out_num_next;

  } /* end of inter-word short pause appending block */

  /* make mapping: word <-> node on wchmm */
  for (j=0;j<word_len;j++) {
    if (j < add_head) {	/* shared part */
      wchmm->offset[word][j] = wchmm->offset[matchword][j];
    } else if (add_tail < j) { /* shared tail part (should not happen..) */
      wchmm->offset[word][j] = wchmm->offset[matchword][j+(matchword_len-word_len)];
    } else {			/* newly created part */
      wchmm->offset[word][j] = n;
      n += hmm_logical_state_num(wchmm->winfo->wseq[word][j]) - 2;
    }
  }


  if (wchmm->hmminfo->multipath) {
    /* create word-end node */

    /* paranoia check if the short-pause addition has been done well */
    if (enable_iwsp && add_tail - add_head + 1 > 0) {
      n += hmm_logical_state_num(wchmm->hmminfo->sp) - 2;
      if (n != ntmp) j_internal_error("wchmm_add_word: cannot match\n");
    }
    
    /* create word-end node */
    wchmm->wordend[word] = n;	/* tail node of 'word' is 'n' */
    wchmm->stend[n] = word;	/* node 'k' is a tail node of 'word' */
    acc_init(wchmm, n);
    wchmm->state[n].out.state = NULL;
    
    /* connect the final outgoing arcs in out_from[] to the word end node */
    for(k = 0; k < out_num_prev; k++) {
      add_wacc(wchmm, out_from[k], out_a[k], n);
    }
    n++;
    if (n >= wchmm->maxwcn) wchmm_extend(wchmm);
    
    if (matchlen == 0) {
      /* check if the new word has whole word-skipping transition */
      /* (use out_from and out_num_prev temporary) */
      out_num_prev = 0;
      get_outtrans_list(wchmm, word, word_len-1, out_from, out_a, &out_num_prev, wchmm->winfo->maxwn, enable_iwsp);
      for(k=0;k<out_num_prev;k++) {
	if (out_from[k] == wchmm->wordbegin[word]) {
	  jlog("ERROR: *** ERROR: WORD SKIPPING TRANSITION NOT ALLOWED ***\n");
	  jlog("ERROR:   Word id=%d (%s[%s]) has \"word skipping transition\".\n", word, wchmm->winfo->wname[word], wchmm->winfo->woutput[word]);
	  jlog("ERROR:   All HMMs in the word:\n    ");
	  for(kkk=0;kkk<wchmm->winfo->wlen[word];kkk++) {
	    jlog("%s ", wchmm->winfo->wseq[word][kkk]->name);
	  }
	  jlog("\n");
	  jlog("ERROR:  has transitions from initial state to final state.\n");
	  jlog("ERROR:  This type of word skipping is not supported.\n");
	  ok_p = FALSE;
	}
      }
    }

    wchmm->n = n;

  } else {

    wchmm->n = n;
    k = wchmm->offset[word][word_len-1] + hmm_logical_state_num(wchmm->winfo->wseq[word][word_len-1])-2 -1;
    wchmm->wordend[word] = k;	/* tail node of 'word' is 'k' */
    wchmm->stend[k] = word;	/* node 'k' is a tail node of 'word' */
    
    if (matchlen != 0 && add_tail - add_head + 1 > 0) {
      /* new part has been created in the above procedure: */
      /* now make link from shared part to the new part */
      wchmm_link_subword(wchmm, matchword,add_to,word,add_head);	
    }

  }

  return(ok_p);
  
}

/*************************************************************/
/**** parse whole structure (after wchmm has been built) *****/
/*************************************************************/

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤òÁöºº¤·¡¤Ã±¸EÎ½ªÃ¼¾õÂÖ¤«¤é³°¤Ø¤Î¼¡Á«°Ü³ÎÎ¨¤Î¥E¹¥È¤òºûÜ®¤¹¤E 
 * (non multipath)
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Scan the lexicon tree to make list of emission probability from the word end
 * state. (non multipath)
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 */
static void
wchmm_calc_wordend_arc(WCHMM_INFO *wchmm)
{
  WORD_ID w;
  HTK_HMM_Trans *tr;
  LOGPROB a;

  for (w=0;w<wchmm->winfo->num;w++) {
    tr = hmm_logical_trans(wchmm->winfo->wseq[w][wchmm->winfo->wlen[w]-1]);
    a = tr->a[tr->statenum-2][tr->statenum-1];
    wchmm->wordend_a[w] = a;
  }
}

#ifdef SEPARATE_BY_UNIGRAM

/********************************************************************/
/****** for separation (linearization) of high-frequent words *******/
/********************************************************************/

/** 
 * <JA>
 * unigram³ÎÎ¨¤Ç¥½¡¼¥È¤¹¤E¿¤á¤Î qsort ¥³¡¼¥EĞ¥Ã¥¯´Ø¿E 
 * 
 * @param a [in] Í×ÁÇ1
 * @param b [in] Í×ÁÇ2
 * 
 * @return ±é»»¤Î·EÌ¤ÎÉä¹ç¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * qsort callback function to sort unigram values.
 * 
 * @param a [in] element #1
 * @param b [in] element #2
 * 
 * @return the result of comparison.
 * </EN>
 */
static int
compare_prob(LOGPROB *a, LOGPROB *b)
{
  if (*a < *b)  return (1);
  if (*a > *b)  return (-1);
  return(0);
}

/** 
 * <JA>
 * 1-gram¥¹¥³¥¢¤Î¾å°Ì N ÈÖÌÜ¤ÎÃÍ¤òµá¤á¤E 
 * 
 * @param winfo [in] Ã±¸E­½E * @param n [in] µá¤á¤Eç°Ì
 * 
 * @return ¾å°Ì N ÈÖÌÜ¤Î uni-gram ³ÎÎ¨¤ÎÃÍ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * Get the Nth-best unigram probability from all words.
 * 
 * @param winfo [in] word dictionary
 * @param n [in] required rank
 * 
 * @return the Nth-best unigram probability.
 * </EN>
 */
static LOGPROB
get_nbest_uniprob(WCHMM_INFO *wchmm, int n)
{
  LOGPROB *u_p;
  WORD_ID w;
  LOGPROB x;
  WORD_INFO *winfo;
  NGRAM_INFO *ngram;

  winfo = wchmm->winfo;
  ngram = wchmm->ngram;

  if (n < 1) n = 1;
  if (n > winfo->num) n = winfo->num;

  /* store all unigram probability to u_p[] */
  u_p = (LOGPROB *)mymalloc(sizeof(LOGPROB) * winfo->num);
  for(w=0;w<winfo->num;w++) {
    if (ngram) {
      x = uni_prob(ngram, winfo->wton[w])
#ifdef CLASS_NGRAM
	+ winfo->cprob[w]
#endif
	;
    } else {
      x = LOG_ZERO;
    }
    if (wchmm->lmvar == LM_NGRAM_USER) {
      x = (*(wchmm->uni_prob_user))(wchmm->winfo, w, x);
    }
    u_p[w] = x;
  }

  /* sort them downward */
  qsort(u_p, winfo->num, sizeof(LOGPROB),
	(int (*)(const void *,const void *))compare_prob);

  /* return the Nth value */
  x = u_p[n-1];
  free(u_p);
  return(x);
}

#endif

/**********************************************************/
/****** MAKE WCHMM (LEXICON TREE) --- main function *******/
/**********************************************************/

#define COUNT_STEP 500         ///< Word count step for debug progress output

/** 
 * <JA>
 * Í¿¤¨¤é¤E¿Ã±¸E­½ñ¤È¸À¸Eâ¥Ç¥E«¤éÌÚ¹½Â¤²½¼­½ñ¤ò¹½ÃÛ¤¹¤E ¤³¤Î´Ø¿ô¤Ï
 * ½èÍı¤¬ÃÙ¤¯¡¤Julian¤Ç"-oldtree"¥ª¥×¥·¥ç¥ó»ØÄEş¤Î¤ß»ÈÍÑ¤µ¤EŞ¤¹. ¥ª¥×¥·¥ç¥E * Èó»ØÄEş¤ª¤è¤ÓJulius¤Ç¤ÏÂå¤Eê¤Ë build_wchmm2() ¤¬ÍÑ¤¤¤é¤EŞ¤¹. 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param lmconf [in] ¸À¸Eâ¥Ç¥ELM)ÀßÄEÑ¥é¥á¡¼¥¿
 * </JA>
 * <EN>
 * Build a tree lexicon from given word dictionary and language model.
 * This function is slow and only used when "-oldtree" option is specified
 * in Julian.  Julian without that option and Julius uses build_wchmm2()
 * instead of this.
 * 
 * @param wchmm [i/o] lexicon tree
 * @param lmconf [in] language model (LM) configuration parameters
 * </EN>
 * @callgraph
 * @callergraph
 */
boolean
build_wchmm(WCHMM_INFO *wchmm, JCONF_LM *lmconf)
{
  int i,j;
  int matchword=0, sharelen=0, maxsharelen=0;
  int num_duplicated;
#ifdef SEPARATE_BY_UNIGRAM
  LOGPROB separate_thres;
  LOGPROB p;
#endif
  boolean ok_p;

  /* lingustic infos must be set before build_wchmm() is called */
  /* check if necessary lingustic info is already assigned (for debug) */
  if (wchmm->winfo == NULL
      || (wchmm->lmvar == LM_NGRAM && wchmm->ngram == NULL)
      || (wchmm->lmvar == LM_DFA_GRAMMAR && wchmm->dfa == NULL)
      ) {
    jlog("ERROR: wchmm: linguistic info not available!!\n");
    return FALSE;
  }

  ok_p = TRUE;
  
#ifdef SEPARATE_BY_UNIGRAM
  /* ¾å°Ì[separate_wnum]ÈÖÌÜ¤Î1-gram¥¹¥³¥¢¤òµá¤á¤E*/
  /* 1-gram¥¹¥³¥¢¤¬¤³¤ÎÃÍ°Ê¾å¤Î¤â¤Î¤ÏÌÚ¤«¤éÊ¬¤±¤E*/
  separate_thres = get_nbest_uniprob(wchmm, lmconf->separate_wnum);
#endif

#ifdef PASS1_IWCD
#ifndef USE_OLD_IWCD
  if (wchmm->category_tree) {
    if (wchmm->ccd_flag) {
      /* Á´¤Æ¤Î¥«¥Æ¥´¥êIDÉÕ¤­ lcd_set ¤òºûÜ® */
      lcdset_register_with_category_all(wchmm);
    }
  }
#endif
#endif /* PASS1_IWCD */
  

  /* wchmm¤ò½é´E½ */
  wchmm_init(wchmm);

  /* ¥«¥¦¥ó¥¿¥E»¥Ã¥È */
  wchmm->separated_word_count=0;

  jlog("STAT: wchmm: Building HMM lexicon tree (left-to-right)\n");
  for (i=0;i<wchmm->winfo->num;i++) {

    if (wchmm->lmtype == LM_PROB) {
      if (i == wchmm->winfo->head_silwid || i == wchmm->winfo->tail_silwid) {
	/* ÀèÆ¬/ËöÈø¤ÎÌµ²»¥â¥Ç¥EÏÌÚ¹½Â¤²½¤»¤º¡¤
	 * ÀèÆ¬¤ÎÌµ²»Ã±¸EÎÀèÆ¬¤Ø¤ÎÁ«°Ü¡¤ËöÈøÃ±¸EÎËöÈø¤«¤é¤ÎÁ«°Ü¤ÏºûÀé¤Ê¤¤*/
	/* sharelen=0¤Ç¤½¤Î¤Ş¤Ş */
	if (wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp) == FALSE) {
	  jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
	  ok_p = FALSE;
	}
	continue;
      }
#ifndef NO_SEPARATE_SHORT_WORD
      if (wchmm->winfo->wlen[i] <= SHORT_WORD_LEN) {
	/* Ä¹¤µ¤ÎÃ»¤¤Ã±¸EòÌÚ¹½Â¤²½¤·¤Ê¤¤(¤³¤³¤Ç¤Ï1²»ÀE */
	/* sharelen=0¤Ç¤½¤Î¤Ş¤Ş */
	if (wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp) == FALSE) {
	  jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
	  ok_p = FALSE;
	}
	wchmm->separated_word_count++;
	continue;
      }
#endif
#ifdef SEPARATE_BY_UNIGRAM
      if (wchmm->ngram) {
	p = uni_prob(wchmm->ngram, wchmm->winfo->wton[i])
#ifdef CLASS_NGRAM
	  + wchmm->winfo->cprob[i]
#endif
	  ;
      } else {
	p = LOG_ZERO;
      }
      if (wchmm->lmvar == LM_NGRAM_USER) {
	p = (*(wchmm->uni_prob_user))(wchmm->winfo, i, p);
      }
      if (p >= separate_thres && wchmm->separated_word_count < lmconf->separate_wnum) {
	/* ÉÑÅÙ¤Î¹â¤¤Ã±¸EòÌÚ¹½Â¤²½¤·¤Ê¤¤ */
	/* separate_thres ¤Ï¾å°Ìseparate_wnumÈÖÌÜ¤Î¥¹¥³¥¢ */
	if (wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp) == FALSE) {
	  jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
	  ok_p = FALSE;
	}
	wchmm->separated_word_count++;
	continue;
      }
#endif
    }

    /* ºÇ¤âÄ¹¤¯²»ÁÇ¤ò¶¦Í­½ĞÍè¤E±¸EòÃµ¤¹ */
    maxsharelen=0;
    for (j=0;j<i;j++) {
      if (wchmm->category_tree  && wchmm->lmtype == LM_DFA) {
	if (wchmm->winfo->wton[i] != wchmm->winfo->wton[j]) continue;
      }
      sharelen = wchmm_check_match(wchmm->winfo, i, j);
      if (sharelen == wchmm->winfo->wlen[i] && sharelen == wchmm->winfo->wlen[j]) {
       /* word ¤ËÆ±²»¸E¬Â¸ºß¤¹¤E*/
       /* É¬¤ººÇÂç¤ÎÄ¹¤µ¤Ç¤¢¤ê¡¤½ÅÊ£¥«¥¦¥ó¥È¤òÈò¤±¤E¿¤á¤³¤³¤ÇÈ´¤±¤E*/
       maxsharelen = sharelen;
       matchword = j;
       break;
      }
      if (sharelen > maxsharelen) {
       matchword = j;
       maxsharelen = sharelen;
      }
    }
    if (wchmm_add_word(wchmm, i, maxsharelen, matchword, lmconf->enable_iwsp) == FALSE) {
      jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
      ok_p = FALSE;
    }
  }

#if 0
  /* ÌÚ¹½Â¤¤òºûÀé¤Ê¤¤ */
  for (i=0;i<wchmm->winfo->num;i++) {
    if (wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp) == FALSE) {
      jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
      ok_p = FALSE;
    }
  }
#endif  
  jlog("STAT:  %5d words ended     (%6d nodes)\n",i,wchmm->n);

  if (! wchmm->hmminfo->multipath) {
    /* Æ±°E»ÁÇ·ÏÎó¤ò»ı¤ÄÃ±¸E±»Î¤Î leaf node ¤E½Å²½¤·¤Æ¶èÊÌ¤¹¤E*/
    num_duplicated = wchmm_duplicate_leafnode(wchmm);
    jlog("STAT:  %d leaf nodes are made unshared\n", num_duplicated);
    
    /* Ã±¸EÎ½ªÃ¼¤«¤é³°¤Ø¤ÎÁ«°Ü³ÎÎ¨¤òµá¤á¤Æ¤ª¤¯ */
    wchmm_calc_wordend_arc(wchmm);
  }

  /* wchmm¤ÎÀ°¹çÀ­¤ò¥Á¥§¥Ã¥¯¤¹¤E*/
  check_wchmm(wchmm);

  /* factoringÍÑ¤Ë³Æ¾õÂÖ¤Ë¸åÂ³Ã±¸EÎ¥E¹¥È¤òÉÕ²Ã¤¹¤E*/
  if (!wchmm->category_tree) {

#ifdef UNIGRAM_FACTORING
    if (wchmm->lmtype == LM_PROB) {
      /* Æ±»ş¤ËÁ°¤â¤Ã¤ÆfactoringÃÍ¤ò·×»» */
      make_successor_list_unigram_factoring(wchmm);
      jlog("STAT:  1-gram factoring values has been pre-computed\n");
    } else {
      make_successor_list(wchmm);
    }
#else 
    make_successor_list(wchmm);
#endif /* UNIGRAM_FACTORING */
    
    if (wchmm->hmminfo->multipath) {
      /* ¹½ÃÛ¤µ¤E¿ factoring ¾ğÊó¤ò¥¹¥­¥Ã¥×Á«°Ü¤ª¤è¤ÓÊ¸Æ¬Ê¸Ë¡¥Î¡¼¥É¤Ë¥³¥Ô¡¼ */
      adjust_sc_index(wchmm);
    }
    
#ifdef UNIGRAM_FACTORING
    if (wchmm->lmtype == LM_PROB) {
      /* Ã±¸EÖLM¥­¥ã¥Ã¥·¥å¤¬É¬Í×¤Ê¥Î¡¼¥É¤Î¥E¹¥È¤òºûÀE*/
      make_iwcache_index(wchmm);
    }
#endif /* UNIGRAM_FACTORING */

  }

  jlog("STAT: done\n");

  return ok_p;
}

/** 
 * <JA>
 * Í¿¤¨¤é¤E¿Ã±¸E­½ñ¤È¸À¸Eâ¥Ç¥E«¤éÌÚ¹½Â¤²½¼­½ñ¤ò¹½ÃÛ¤¹¤E 
 * ¤³¤Î´Ø¿ô¤Ï bulid_wchmm() ¤ÈÆ±¤¸½èÍı¤ò¹Ô¤¤¤Ş¤¹¤¬¡¤
 * ºÇ½é¤ËÃ±¸Eò²»ÁÇÎó¤Ç¥½¡¼¥È¤·¤Æ²»ÁÇÎó¤Î»÷¤¿½ç¤ËÃ±¸EòÊÂ¤Ù¤E¿¤á¡¤
 * ¤è¤EâÂ®¤ËÌÚ¹½Â¤²½¤ò¹Ô¤¦¤³¤È¤¬¤Ç¤­¤E ¤È¤¯¤Ë¥ª¥×¥·¥ç¥ó»ØÄê¤ò¤·¤Ê¤¤
 * ¸Â¤ê¡¤Julius/Julian¤Ç¤Ï¤³¤Á¤é¤¬ÍÑ¤¤¤é¤EE 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param lmconf [in] ¸À¸Eâ¥Ç¥ELM)ÀßÄEÑ¥é¥á¡¼¥¿
 * </JA>
 * <EN>
 * Build a tree lexicon from given word dictionary and language model.
 * This function does the same job as build_wchmm(), but it is much
 * faster because finding of the longest matched word to an adding word
 * is done by first sorting all the words in the dictoinary by their phoneme
 * sequence order.  This function will be used instead of build_wchmm()
 * by default.
 * 
 * @param wchmm [i/o] lexicon tree
 * @param lmconf [in] language model (LM) configuration parameters
 * </EN>
 * @callgraph
 * @callergraph
 */  
boolean
build_wchmm2(WCHMM_INFO *wchmm, JCONF_LM *lmconf)
{
  int i,j, last_i;
  int num_duplicated;
  WORD_ID *windex;
#ifdef SEPARATE_BY_UNIGRAM
  LOGPROB separate_thres;
  LOGPROB p;
#endif
  boolean ok_p;
  boolean ret;

  /* lingustic infos must be set before build_wchmm() is called */
  /* check if necessary lingustic info is already assigned (for debug) */
  if (wchmm->winfo == NULL
      || (wchmm->lmvar == LM_NGRAM && wchmm->ngram == NULL)
      || (wchmm->lmvar == LM_DFA_GRAMMAR && wchmm->dfa == NULL)
      ) {
    jlog("ERROR: wchmm: linguistic info not available!!\n");
    return FALSE;
  }

  ok_p = TRUE;
  
  wchmm->separated_word_count = 0;
  
  jlog("STAT: Building HMM lexicon tree\n");
  
  if (wchmm->lmtype == LM_PROB) {
#ifdef SEPARATE_BY_UNIGRAM
    /* compute score threshold beforehand to separate words from tree */
    /* here we will separate best [separate_wnum] words from tree */
    separate_thres = get_nbest_uniprob(wchmm, lmconf->separate_wnum);
#endif
  }

#ifdef PASS1_IWCD
#ifndef USE_OLD_IWCD
  if (wchmm->category_tree) {
    if (wchmm->ccd_flag) {
      /* when Julian mode (category-tree) and triphone is used,
	 make all category-indexed context-dependent phone set (cdset) here */
      /* these will be assigned on the last phone of each word on tree */
      lcdset_register_with_category_all(wchmm);
    }
  }
#endif
#endif /* PASS1_IWCD */

 /* initialize wchmm */
  wchmm_init(wchmm);

  /* make sorted word index ordered by phone sequence */
  windex = (WORD_ID *)mymalloc(sizeof(WORD_ID) * wchmm->winfo->num);
  for(i=0;i<wchmm->winfo->num;i++) windex[i] = i;

  if (wchmm->category_tree && wchmm->lmtype == LM_DFA) {

    /* sort by category -> sort by word ID in each category */
    wchmm_sort_idx_by_category(wchmm->winfo, windex, wchmm->winfo->num);
    {
      int last_cate;
      last_i = 0;
      last_cate = wchmm->winfo->wton[windex[0]];
      for(i = 1;i<wchmm->winfo->num;i++) {
	if (wchmm->winfo->wton[windex[i]] != last_cate) {
	  wchmm_sort_idx_by_wseq(wchmm->winfo, windex, last_i, i - last_i);
	  last_cate = wchmm->winfo->wton[windex[i]];
	  last_i = i;
	}
      }
      wchmm_sort_idx_by_wseq(wchmm->winfo, windex, last_i, wchmm->winfo->num - last_i);
    }

  } else {

    /* sort by word ID for whole vocabulary */
    wchmm_sort_idx_by_wseq(wchmm->winfo, windex, 0, wchmm->winfo->num);

  }

/* 
 *   {
 *     int i,w;
 *     for(i=0;i<wchmm->winfo->num;i++) {
 *	 w = windex[i];
 *	 printf("%d: cate=%4d wid=%4d %s\n",i, wchmm->winfo->wton[w], w, wchmm->winfo->woutput[w]);
 *     }
 *   }
 */

  /* incrementaly add words to lexicon tree */
  /* now for each word, the previous word (last_i) is always the most matched one */
  last_i = WORD_INVALID;
  for (j=0;j<wchmm->winfo->num;j++) {
    i = windex[j];

    if (wchmm->lmtype == LM_PROB) {

      /* start/end silence word should not be shared */
      if (i == wchmm->winfo->head_silwid || i == wchmm->winfo->tail_silwid) {
	/* add whole word as new (sharelen=0) */
	if (wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp) == FALSE) {
	  jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
	  ok_p = FALSE;
	}
	continue;
      }
#ifndef NO_SEPARATE_SHORT_WORD
      /* separate short words from tree */
      if (wchmm->winfo->wlen[i] <= SHORT_WORD_LEN) {
	if (wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp) == FALSE) {
	  jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
	  ok_p = FALSE;
	}
	wchmm->separated_word_count++;
	continue;
      }
#endif
#ifdef SEPARATE_BY_UNIGRAM
      if (wchmm->ngram) {
	p = uni_prob(wchmm->ngram, wchmm->winfo->wton[i])
#ifdef CLASS_NGRAM
	  + wchmm->winfo->cprob[i]
#endif
	  ;
      } else {
	p = LOG_ZERO;
      }
      if (wchmm->lmvar == LM_NGRAM_USER) {
	p = (*(wchmm->uni_prob_user))(wchmm->winfo, i, p);
      }
      /* separate high-frequent words from tree (threshold = separate_thres) */
      if (p >= separate_thres && wchmm->separated_word_count < lmconf->separate_wnum) {
	if (wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp) == FALSE) {
	  jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
	  ok_p = FALSE;
	}
	wchmm->separated_word_count++;
	continue;
      }
#endif
    }

    if (last_i == WORD_INVALID) { /* first word */
      ret = wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp);
    } else {
      /* the previous word (last_i) is always the most matched one */
      if (wchmm->category_tree && wchmm->lmtype == LM_DFA) {
	if (wchmm->winfo->wton[i] != wchmm->winfo->wton[last_i]) {
	  ret = wchmm_add_word(wchmm, i, 0, 0, lmconf->enable_iwsp);
	} else {
	  ret = wchmm_add_word(wchmm, i, wchmm_check_match(wchmm->winfo, i, last_i), last_i, lmconf->enable_iwsp);
	}
      } else {
	ret = wchmm_add_word(wchmm, i, wchmm_check_match(wchmm->winfo, i, last_i), last_i, lmconf->enable_iwsp);
      }
    }
    if (ret == FALSE) {
      jlog("ERROR: wchmm: failed to add word #%d to lexicon tree\n");
      ok_p = FALSE;
    }
    last_i = i;
    
  } /* end of add word loop */
  
  /*j_printerr("\r %5d words ended     (%6d nodes)\n",j,wchmm->n);*/

  /* free work area */
  free(windex);

  if (wchmm->hmminfo->multipath) {
    jlog("STAT: lexicon size: %d nodes\n", wchmm->n);
  } else {
    /* duplicate leaf nodes of homophone/embedded words */
    jlog("STAT: lexicon size: %d", wchmm->n);
    num_duplicated = wchmm_duplicate_leafnode(wchmm);
    jlog("+%d=%d\n", num_duplicated, wchmm->n);
  }

  if (! wchmm->hmminfo->multipath) {
    /* calculate transition probability of word end node to outside */
    wchmm_calc_wordend_arc(wchmm);
  }

  /* check wchmm coherence (internal debug) */
  check_wchmm(wchmm);

  /* make successor list for all branch nodes for N-gram factoring */
  if (!wchmm->category_tree) {

#ifdef UNIGRAM_FACTORING
    if (wchmm->lmtype == LM_PROB) {
      /* for 1-gram factoring, we can compute the values before search */
       make_successor_list_unigram_factoring(wchmm);
       jlog("STAT:  1-gram factoring values has been pre-computed\n");
    } else {
      make_successor_list(wchmm);
    }
#else
    make_successor_list(wchmm);
#endif /* UNIGRAM_FACTORING */
    if (wchmm->hmminfo->multipath) {
      /* Copy the factoring data according to the skip transitions and startword nodes */
      adjust_sc_index(wchmm);
    }
#ifdef UNIGRAM_FACTORING
    if (wchmm->lmtype == LM_PROB) {
      /* make list of start nodes that needs inter-word LM cache */
      make_iwcache_index(wchmm);
    }
#endif /* UNIGRAM_FACTORING */

  }

  //jlog("STAT: done\n");

#ifdef WCHMM_SIZE_CHECK
  if (debug2_flag) {
    /* detailed check of lexicon tree size (inaccurate!) */
    jlog("STAT: --- memory size of word lexicon ---\n");
    jlog("STAT: wchmm: %d words, %d nodes\n", wchmm->winfo->num, wchmm->n);
    jlog("STAT: %9d bytes: wchmm->state[node] (exclude ac, sc)\n", sizeof(WCHMM_STATE) * wchmm->n);
    {
      int count1 = 0;
      int count2 = 0;
      int count3 = 0;
      for(i=0;i<wchmm->n;i++) {
	if (wchmm->self_a[i] != LOG_ZERO) count1++;
	if (wchmm->next_a[i] != LOG_ZERO) count2++;
	if (wchmm->ac[i] != NULL) count3++;
      }
      jlog("STAT: %9d bytes: wchmm->self_a[node] (%4.1f%% filled)\n", sizeof(LOGPROB) * wchmm->n, 100.0 * count1 / (float)wchmm->n);
      jlog("STAT: %9d bytes: wchmm->next_a[node] (%4.1f%% filled)\n", sizeof(LOGPROB) * wchmm->n, 100.0 * count2 / (float)wchmm->n);
      jlog("STAT: %9d bytes: wchmm->ac[node] (%4.1f%% used)\n", sizeof(A_CELL2 *) * wchmm->n, 100.0 * count3 / (float)wchmm->n);
    }
    jlog("STAT: %9d bytes: wchmm->stend[node]\n", sizeof(WORD_ID) * wchmm->n);
    {
      int w,count;
      count = 0;
      for(w=0;w<wchmm->winfo->num;w++) {
	count += wchmm->winfo->wlen[w] * sizeof(int) + sizeof(int *);
      }
      jlog("STAT: %9d bytes: wchmm->offset[w][]\n", count);
    }
    if (wchmm->hmminfo->multipath) {
      jlog("STAT: %9d bytes: wchmm->wordbegin[w]\n", wchmm->winfo->num * sizeof(int));
    }
    jlog("STAT: %9d bytes: wchmm->wordend[w]\n", wchmm->winfo->num * sizeof(int));
    jlog("STAT: %9d bytes: wchmm->startnode[]\n", wchmm->startnum * sizeof(int));
    if (wchmm->category_tree) {
      jlog("STAT: %9d bytes: wchmm->start2wid[]\n", wchmm->startnum * sizeof(WORD_ID));
    }
#ifdef UNIGRAM_FACTORING
    if (wchmm->lmtype == LM_PROB) {
      jlog("STAT: %9d bytes: wchmm->start2isolate[]\n", wchmm->isolatenum * sizeof(int));
    }
#endif
    if (!wchmm->hmminfo->multipath) {
      jlog("STAT: %9d bytes: wchmm->wordend_a[]\n", wchmm->winfo->num * sizeof(LOGPROB));
    }
#ifdef PASS1_IWCD
    jlog("STAT: %9d bytes: wchmm->outstyle[]\n", wchmm->n * sizeof(unsigned char));
    {
      int c;
      c = 0;
      for(i=0;i<wchmm->n;i++) {
	switch(wchmm->outstyle[i]) {
	case AS_RSET:
	  c += sizeof(RC_INFO);
	  break;
	case AS_LRSET:
	  c += sizeof(LRC_INFO);
	  break;
      }
      }
      if (c > 0) jlog("STAT: %9d bytes: wchmm->out (RC_INFO / LRC_INFO)\n", c);
    }
#endif
    if (!wchmm->category_tree) {
      int c = 0;
#ifdef UNIGRAM_FACTORING
      jlog("STAT: %9d bytes: wchmm->scword[]\n", sizeof(WORD_ID) * wchmm->scnum);
#else
      for(i=1;i<wchmm->scnum;i++) {
	c += wchmm->sclen[i];
      }
      jlog("STAT: %9d bytes: wchmm->sclist[]\n", c * sizeof(WORD_ID) + wchmm->scnum * sizeof(WORD_ID *));
      jlog("STAT: %9d bytes: wchmm->sclen[]\n", wchmm->scnum * sizeof(WORD_ID));
#endif
#ifdef UNIGRAM_FACTORING
      if (wchmm->lmtype == LM_PROB) {
	jlog("STAT: %9d bytes: wchmm->fscore[]\n", wchmm->fsnum * sizeof(LOGPROB));
      }
#endif  
    }
    
    {
      int count, n;
      A_CELL2 *ac;
      count = 0;
      for(n=0;n<wchmm->n;n++) {
	for(ac=wchmm->ac[n];ac;ac=ac->next) {
	  count += sizeof(A_CELL2);
	}
      }
      jlog("STAT: %9d bytes: A_CELL2\n", count);
    }
  }

#endif /* WCHMM_SIZE_CHECK */


  return ok_p;

}


/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¤Î¥µ¥¤¥º¤Ê¤É¤Î¾ğÊó¤òÉ¸½à½ĞÎÏ¤Ë½ĞÎÏ¤¹¤E 
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Output some specifications of the tree lexicon (size etc.) to stdout.
 * 
 * @param wchmm [in] tree lexicon already built
 * </EN>
 * @callgraph
 * @callergraph
 */
void
print_wchmm_info(WCHMM_INFO *wchmm)
{
  int n,i, rootnum;

  if (wchmm->hmminfo->multipath) {
    rootnum = wchmm->startnum;
  } else {
    if (wchmm->lmtype == LM_PROB) {
      rootnum = wchmm->startnum + 1;	/* including winfo->head_silwid */
    } else if (wchmm->lmtype == LM_DFA) {
      rootnum = wchmm->startnum;
    }
  }
  
  jlog(" Lexicon tree:\n");
  jlog("\t total node num = %6d\n", wchmm->n);
  if (wchmm->lmtype == LM_PROB) {
    jlog("\t  root node num = %6d\n", rootnum);
#ifdef NO_SEPARATE_SHORT_WORD
#ifdef SEPARATE_BY_UNIGRAM
    jlog("\t(%d hi-freq. words are separated from tree lexicon)\n", wchmm->separated_word_count);
#else
    jlog(" (no words are separated from tree)\n");
#endif /* SEPARATE_BY_UNIGRAM */
#else
    jlog(" (%d short words (<= %d phonemes) are separated from tree)\n", wchmm->separated_word_count, SHORT_WORD_LEN);
#endif /* NO_SEPARATE_SHORT_WORD */
  }
  if (wchmm->lmtype == LM_DFA) {
    jlog("\t  root node num = %6d\n", rootnum);
  }
  for(n=0,i=0;i<wchmm->n;i++) {
    if (wchmm->stend[i] != WORD_INVALID) n++;
  }
  jlog("\t  leaf node num = %6d\n", n);
  if (!wchmm->category_tree) {
    jlog("\t fact. node num = %6d\n", wchmm->scnum - 1);
  }
}

/* end of file */
