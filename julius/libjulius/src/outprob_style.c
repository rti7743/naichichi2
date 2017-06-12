/**
 * @file   outprob_style.c
 * 
 * <JA>
 * @brief  ¾õÂÖ¤Î½ĞÎÏ³ÎÎ¨·×»»¡ÊÂE¥Ñ¥¹¡Ë
 *
 * ÂE¥Ñ¥¹¤Ç¡¤ÌÚ¹½Â¤²½¼­½ñ¾å¤Î¥Î¡¼¥É¤Î¡¤ÆşÎÏ¥Ù¥¯¥È¥EËÂĞ¤¹¤EHMM ¤Î
 * ½ĞÎÏÂĞ¿ô³ÎÎ¨¤ò·×»»¤¹¤E 
 *
 * ¥È¥é¥¤¥Õ¥©¥ó»ÈÍÑ»ş¤Ï¡¤Ã±¸Eâ¤Î²»ÁÇ´Ä¶­°ÍÂ¸¤Ë¤Ä¤¤¤Æ¤Ï¼­½ñÆÉ¤ß¹ş¤ß»ş¤Ë
 * ¹ÍÎ¸¤µ¤EÆÂĞ±ş¤¹¤EÈ¥é¥¤¥Õ¥©¥ó¤¬¤¹¤Ç¤Ë³ä¤Eö¤Æ¤é¤EÆ¤¤¤EÎ¤Ç¡¤¤³¤³¤Ç
 * ÆÃÊÌ¤Ê½èÍı¤Ï¹Ô¤EEÊ¤¤. Ã±¸EèÆ¬¤ª¤è¤ÓËöÈø¤Î²»ÁÇ¤Ï¡¤ÌÚ¹½Â¤²½¼­½ñ¾å¤Ç¤Ï
 * pseudo triphone ¤¬³ä¤Eö¤¿¤Ã¤Æ¤ª¤ê¡¤¤³¤Eé¤Ë¤Ä¤¤¤Æ¤Ï¡¤°Ê²¼¤Î¤è¤¦¤Ë
 * Ã±¸EÖ¤ò²»ÁÇ´Ä¶­°ÍÂ¸À­¤ò¹ÍÎ¸¤·¤¿·×»»¤¬¹Ô¤EEE 
 *  -# Ã±¸Eâ²»ÁÇ: ÄÌ¾EÌ¤E×»»¤¹¤E 
 *  -# Ã±¸EÎÀèÆ¬²»ÁÇ: Ä¾Á°Ã±¸EÎ¾ğÊó¤«¤é¡¤pseudo triphone ¤òÀµ¤·¤¤
 *     ¥È¥é¥¤¥Õ¥©¥ó¤ËÆ°Åª¤ËÀÚ¤EØ¤¨¤Æ·×»». 
 *  -# Ã±¸EÎËöÈø²»ÁÇ: ¤½¤Î pseudo triphone ¤Ë´Ş¤Ş¤EEÊÆ±¤¸º¸¥³¥ó¥Æ¥­¥¹¥È¤E *     »ı¤Ä¥È¥é¥¤¥Õ¥©¥ó¤Î¡Ë¾õÂÖ½¸¹çÃæ¤Î¤¹¤Ù¤Æ¤Î¾õÂÖ¤Ë¤Ä¤¤¤ÆÌàÅÙ¤ò·×»»¤·¡¤
 *      - "-iwcd1 max" »ØÄEş¤ÏºÇÂçÃÍ
 *      - "-iwcd1 avg" »ØÄEş¤ÏÊ¿¶ÑÃÍ(default)
 *      - "-iwcd1 best N" »ØÄEş¤Ï¾å°ÌN¸Ä¤ÎÊ¿¶ÑÃÍ
 *     ¤ò¤½¤Î¾õÂÖ¤ÎÌàÅÙ¤È¤·¤ÆºÎÍÑ¤¹¤E (¤³¤EÏ outprob_cd() Æâ¤Ç¼«Æ°Åª¤ËÁªÂE *     ¤µ¤E×»»¤µ¤EE 
 *  -# 1²»ÁÇ¤«¤é¤Ê¤E±¸EÎ¾EE ¾åµ­¤òÎ¾Êı¤È¤â¹ÍÎ¸¤¹¤E 
 *
 * ¾åµ­¤Î½èÍı¤ò¹Ô¤¦¤Ë¤Ï¡¤ÌÚ¹½Â¤²½¼­½ñ¤Î¾õÂÖ¤´¤È¤Ë¡¤¤½¤E¾¤E¬Ã±¸Eâ¤Ç¤É¤Î
 * °ÌÃÖ¤Î²»ÁÇ¤ËÂ°¤¹¤EõÂÖ¤Ç¤¢¤E«¤Î¾ğÊó¤¬É¬Í×¤Ç¤¢¤E ÌÚ¹½Â¤²½¼­½ñ¤Ç¤Ï¡¤
 * ¾õÂÖ¤´¤È¤Ë¾åµ­¤Î¤É¤Î½èÍı¤ò¹Ô¤¨¤ĞÎÉ¤¤¤«¤EAS_Style ¤Ç¤¢¤é¤«¤¸¤áÊİ»ı¤·¤Æ¤¤¤E 
 *
 * ¤Ş¤¿¡¤¾åµ­¤Î 2 ¤È 4 ¤Î¾õÂÖ¤Ç¤Ï¡¤¥³¥ó¥Æ¥­¥¹¥È¤ËÈ¼¤¦triphoneÊÑ²½¤ò¡¤
 * Ä¾Á°Ã±¸ED ¤È¤È¤â¤Ë¾õÂÖ¤´¤È¤Ë¥Õ¥E¼¥àÃ±°Ì¤Ç¥­¥ã¥Ã¥·¥å¤·¤Æ¤¤¤E ¤³¤EË¤è¤E * ·×»»ÎÌ¤ÎÁıÂç¤òËÉ¤°. 
 * </JA>
 * 
 * <EN>
 * @brief  Compute output probability of a state (1st pass)
 *
 * These functions compute the output probability of an input vector
 * from a state on the lexicon tree.
 *
 * When using triphone acoustic model, the cross-word triphone handling is
 * done here.  The head and tail phoneme of every words has corresponding
 * pseudo phone set on the tree lexicon, so the actual likelihood computation
 * will be done as the following:
 *   -# word-internal: compute as normal.
 *   -# Word head phone: the correct triphone phone, according to the last
 *      word information on the passing token, will be dynamically assigned
 *      to compute the cross-word dependency.
 *   -# Word tail phone: all the states in the pseudo phone set (they are
 *      states of triphones that has the same left context as the word end)
 *      will be computed, and use
 *       - maximum value if "-iwcd1 max" specified, or
 *       - average value if "-iwcd1 avg" specified, or
 *       - average of best N states if "-iwcd1 best N" specified (default: 3)
 *      the actual pseudo phoneset computation will be done in outprob_cd().
 *   -# word with only one state: both of above should be considered.
 *
 *  To denote which operation to do for a state, AS_Style ID is assigned
 *  to each state.
 *
 *  The triphone transformation, that will be performed on the state
 *  of 2 and 4 above, will be cached on the tree lxicon by each state
 *  per frame, to suppress computation overhead.
 *   
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Mon Aug 22 17:14:26 2005
 *
 * $Revision: 1.5 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

#ifdef PASS1_IWCD

/** 
 * <JA>
 * ¸E¬¥È¥é¥¤¥Õ¥©¥óÊÑ²½ÍÑ¥­¥ã¥Ã¥·¥å¤Î½é´E½
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Initialize cache for triphone changing on every word head.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 * @callgraph
 * @callergraph
 */
void
outprob_style_cache_init(WCHMM_INFO *wchmm)
{
  int n;
  for(n=0;n<wchmm->n;n++) {
    if (wchmm->state[n].out.state == NULL) continue;
    if (wchmm->outstyle[n] == AS_RSET) {
      (wchmm->state[n].out.rset)->cache.state = NULL;
    } else if (wchmm->outstyle[n] == AS_LRSET) {
      (wchmm->state[n].out.lrset)->cache.state = NULL;
    }
  }
}

/**********************************************************************/

/** 
 * <JA>
 * @brief  Ã±¸EöÈø¤Î¥È¥é¥¤¥Õ¥©¥ó¥»¥Ã¥È (pseudo phone set) ¤ò¸¡º÷¤¹¤E 
 *
 * Ê¸Ë¡Ç§¼±¤Ç¤Ï¡¤³Æ¥«¥Æ¥´¥ê¤´¤È¤ËÆÈÎ©¤·¤¿ pseudo phone set ¤òÍÑ¤¤¤E 
 * ¤³¤³¤Ç¤ÏÃ±¸EöÍÑ¥«¥Æ¥´¥EÕ¤­ pseudo phone set ¤ò¸¡º÷¤¹¤E 
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½E * @param hmm [in] Ã±¸EÎËöÈø¤Î HMM
 * @param category [in] Ã±¸EÎÂ°¤¹¤E«¥Æ¥´¥E * 
 * @return ³ºÅEset ¤¬¸«¤Ä¤«¤EĞ¤½¤³¤Ø¤Î¥İ¥¤¥ó¥¿¡¤¤¢¤E¤¤Ï¸«¤Ä¤«¤é¤Ê¤±¤EĞ
 * NULL ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * Lookup a word-end triphone set (aka pseudo phone set) with
 * category id for grammar recognition.
 * 
 * @param wchmm [in] word lexicon tree
 * @param hmm [in] logical HMM of word end phone
 * @param category [in] belonging category id of the word
 * 
 * @return pointer to the corresponding phone set if found, or NULL if
 * not found.
 * </EN>
 * @callgraph
 * @callergraph
 */
CD_Set *
lcdset_lookup_with_category(WCHMM_INFO *wchmm, HMM_Logical *hmm, WORD_ID category)
{
  CD_Set *cd;

  leftcenter_name(hmm->name, wchmm->lccbuf);
  sprintf(wchmm->lccbuf2, "%s::%04d", wchmm->lccbuf, category);
  if (wchmm->lcdset_category_root != NULL) {
    cd = aptree_search_data(wchmm->lccbuf2, wchmm->lcdset_category_root);
    if (cd == NULL) return NULL;
    if (strmatch(wchmm->lccbuf2, cd->name)) {
      return cd;
    }
  }
  return NULL;
}

/** 
 * <JA>
 * @brief  Ã±¸EöÍÑ¥«¥Æ¥´¥EÕ¤­ pseudo phone set ¤òÀ¸À®¤¹¤E 
 *
 * Julian ¤Ç¤Ï¡¤¤¢¤E±¸EË¸åÂ³²ÄÇ½¤ÊÃ±¸E¸¹ç¤ÏÊ¸Ë¡¤Ë¤è¤Ã¤ÆÀ©¸Â¤µ¤EE ¤è¤Ã¤Æ¡¤
 * Ã±¸EöÈø¤«¤é¼¡¤Ë¸åÂ³¤·¤¦¤E±¸EèÆ¬²»ÁÇ¤Î¼Eà¤âÊ¸Ë¡¤Ë¤è¤Ã¤Æ¸ÂÄE * ¤µ¤EE ¤½¤³¤Ç¡¤Í¿¤¨¤é¤E¿¼­½ñ¾å¤Ç¡¤Ã±¸EÎ¥«¥Æ¥´¥ê¤´¤È¤Ë¡¤¸åÂ³¤·¤¦¤EèÆ¬²»ÁÇ
 * ¤ò¥«¥Æ¥´¥EĞ¾ğÊó¤«¤éºûÜ®¤·¡¤¤½¤Eé¤ò¥«¥Æ¥´¥EÕ¤­ pseudo phone set ¤È¤·¤Æ
 * ÄEÁ¤·¤ÆÃ±¸EªÃ¼¤ËÍÑ¤¤¤E³¤È¤Ç¡¤Julian ¤Ë¤ª¤±¤E±¸EÖ¥È¥é¥¤¥Õ¥©¥ó¤Î
 * ¶á»÷¸úÖ¹¤ò¾®¤µ¤¯¤¹¤E³¤È¤¬¤Ç¤­¤E 
 * 
 * ¤³¤Î phone set ¤ÎÌ¾Á°¤ÏÄÌ¾EÎ "a-k" ¤Ê¤É¤È°Û¤Ê¤E"a-k::38" ¤È¤Ê¤E * (¿ô»ú¤Ï¥«¥Æ¥´¥êID). ¤³¤³¤Ç¤Ï¡¤¼­½ñ¤ò¸¡º÷¤·¤Æ²ÄÇ½¤Ê¤¹¤Ù¤Æ¤Î¥«¥Æ¥´¥EÕ¤­
 * pseudo phone set ¤ò¡¤À¸À®¤¹¤E ¤³¤EÏÄÌ¾EÎ pseudo phone set ¤È¤ÏÊÌ¤Ë
 * Êİ»ı¤µ¤E¤Ã±¸EöÃ¼¤Î¤ß¤Ç»ÈÍÑ¤µ¤EE 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * @param hmm [in] ¤³¤E«¤éÅĞÏ¿¤¹¤E±¸EÎ½ªÃ¼¤ÎÏÀÍıHMM
 * @param category [in] ¤³¤E«¤éÅĞÏ¿¤¹¤E±¸EÎÊ¸Ë¡¥«¥Æ¥´¥êID
 * 
 * </JA>
 * <EN>
 * @brief  Make a category-indexed context-dependent (pseudo) state set
 * for word ends.
 *
 * In Julian, the word-end pseudo triphone set can be shrinked by using the
 * category-pair constraint, since the number of possible right-context
 * phones on the word end will be smaller than all phone.  This shrinking not
 * only saves computation time but also improves recognition since the
 * approximated value will be closer to the actual value.
 * 
 * For example, if a word belongs to category ID 38 and has a phone "a-k"
 * at word end, CD_Set "a-k::38" is generated and assigned to the
 * phone instead of normal CD_Set "a-k".  The "a-k::38" set consists
 * of triphones whose right context are the beginning phones within
 * possibly fllowing categories.  These will be separated from the normal
 * pseudo phone set.
 * 
 * @param wchmm [i/o] tree lexicon
 * @param hmm [in] logical HMM at the end of a word, of which the
 * category-indexed pseudo state set will be generated.
 * @param category [in] category ID of the word.
 * 
 * </EN>
 */
static void
lcdset_register_with_category(WCHMM_INFO *wchmm, HMM_Logical *hmm, WORD_ID category)
{
  WORD_ID c2, i, w;
  HMM_Logical *ltmp;

  int cnt_c, cnt_w, cnt_p;

  if (lcdset_lookup_with_category(wchmm, hmm, category) == NULL) {
    leftcenter_name(hmm->name, wchmm->lccbuf);
    sprintf(wchmm->lccbuf2, "%s::%04d", wchmm->lccbuf, category);
    if (debug2_flag) {
      jlog("DEBUG: category-aware lcdset {%s}...", wchmm->lccbuf2);
    }
    cnt_c = cnt_w = cnt_p = 0;
    /* search for category that can connect after this category */
    for(c2=0;c2<wchmm->dfa->term_num;c2++) {
      if (! dfa_cp(wchmm->dfa, category, c2)) continue;
      /* for each word in the category, register triphone whose right context
	 is the beginning phones  */
      for(i=0;i<wchmm->dfa->term.wnum[c2];i++) {
	w = wchmm->dfa->term.tw[c2][i];
	ltmp = get_right_context_HMM(hmm, wchmm->winfo->wseq[w][0]->name, wchmm->hmminfo);
	if (ltmp == NULL) {
	  ltmp = hmm;
	  if (ltmp->is_pseudo) {
	    error_missing_right_triphone(hmm, wchmm->winfo->wseq[w][0]->name);
	  }
	}
	if (! ltmp->is_pseudo) {
	  if (regist_cdset(&(wchmm->lcdset_category_root), ltmp->body.defined, wchmm->lccbuf2, &(wchmm->lcdset_mroot))) {
	    cnt_p++;
	  }
	}
      }
      cnt_c++;
      cnt_w += wchmm->dfa->term.wnum[c2];
    }
    if (debug2_flag) {
      jlog("%d categories (%d words) can follow, %d HMMs registered\n", cnt_c, cnt_w, cnt_p);
    }
  }
}

/** 
 * <JA>
 * Á´¤Æ¤ÎÃ±¸EöÍÑ¥«¥Æ¥´¥EÕ¤­ pseudo phone set ¤òÀ¸À®¤¹¤E 
 * ¼­½ñ¾å¤Î¤¹¤Ù¤Æ¤ÎÃ±¸EË¤Ä¤¤¤Æ¡¤¤½¤ÎËöÈø¤ËÅĞ¾E·¤¦¤E«¥Æ¥´¥EÕ¤­ pseudo phone
 * set ¤òÀ¸À®¤¹¤EÊÊ¸Ë¡Ç§¼±ÍÑ¡Ë. 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½ñ¾ğÊE * </JA>
 * <EN>
 * Generate all possible category-indexed pseudo phone sets for
 * grammar recognition.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 * @callgraph
 * @callergraph
 */
void
lcdset_register_with_category_all(WCHMM_INFO *wchmm)
{
  WORD_INFO *winfo;
  WORD_ID c1, w, w_prev;
  int i;
  HMM_Logical *ltmp;

  winfo = wchmm->winfo;

  /* (1) Ã±¸EªÃ¼¤Î²»ÁÇ¤Ë¤Ä¤¤¤Æ */
  /*     word end phone */
  for(w=0;w<winfo->num;w++) {
    ltmp = winfo->wseq[w][winfo->wlen[w]-1];
    lcdset_register_with_category(wchmm, ltmp, winfo->wton[w]);
  }
  /* (2)£±²»ÁÇÃ±¸EÎ¾EE Àè¹Ô¤·¤¦¤E±¸EÎ½ªÃ¼²»ÁÇ¤ò¹ÍÎ¸ */
  /*    for one-phoneme word, possible left context should be also considered */
  for(w=0;w<winfo->num;w++) {
    if (winfo->wlen[w] > 1) continue;
    for(c1=0;c1<wchmm->dfa->term_num;c1++) {
      if (! dfa_cp(wchmm->dfa, c1, winfo->wton[w])) continue;
      for(i=0;i<wchmm->dfa->term.wnum[c1];i++) {
	w_prev = wchmm->dfa->term.tw[c1][i];
	ltmp = get_left_context_HMM(winfo->wseq[w][0], winfo->wseq[w_prev][winfo->wlen[w_prev]-1]->name, wchmm->hmminfo);
	if (ltmp == NULL) continue; /* 1²»ÁÇ¼«¿È¤Îlcd_set¤Ï(1)¤ÇºûÜ®ºÑ */
	if (ltmp->is_pseudo) continue; /* pseudo phone ¤Ê¤élcd_set¤Ï¤¤¤é¤Ê¤¤ */
	lcdset_register_with_category(wchmm, ltmp, winfo->wton[w]);
      }
    }
  }
}

/** 
 * <JA>
 * ¥«¥Æ¥´¥EÕ¤­ pseudo phone set ¤ò¤¹¤Ù¤Æ¾ÃµûÀ¹¤E ¤³¤Î´Ø¿ô¤Ï Julian ¤ÇÊ¸Ë¡¤¬
 * ÊÑ¹¹¤µ¤E¿ºİ¤Ë¡¤¥«¥Æ¥´¥EÕ¤­ pseudo phone set ¤òºÆ¹½ÃÛ¤¹¤EÎ¤ËÍÑ¤¤¤é¤EE 
 * 
 * @param wchmm [i/o] ÌÚ¹½Â¤²½¼­½E * </JA>
 * <EN>
 * Remove all the registered category-indexed pseudo state sets.
 * This function will be called when a grammar is changed to re-build the
 * state sets.
 * 
 * @param wchmm [i/o] lexicon tree information
 * </EN>
 * @callgraph
 * @callergraph
 */
void
lcdset_remove_with_category_all(WCHMM_INFO *wchmm)
{
  free_cdset(&(wchmm->lcdset_category_root), &(wchmm->lcdset_mroot));
}

#endif /* PASS1_IWCD */

/** 
 * <JA>
 * ÌÚ¹½Â¤²½¼­½ñ¾å¤Î¾õÂÖ¤Î½ĞÎÏ³ÎÎ¨¤ò·×»»¤¹¤E 
 * 
 * @param wchmm [in] ÌÚ¹½Â¤²½¼­½ñ¾ğÊE * @param node [in] ¥Î¡¼¥ÉÈÖ¹E * @param last_wid [in] Ä¾Á°Ã±¸EÊÃ±¸EèÆ¬¤Î¥È¥é¥¤¥Õ¥©¥ó·×»»¤ËÍÑ¤¤¤EË
 * @param t [in] »ş´Ö¥Õ¥E¼¥E * @param param [in] ÆÃÄ§ÎÌ¥Ñ¥é¥á¡¼¥¿¹½Â¤ÂÎ (@a t ÈÖÌÜ¤Î¥Ù¥¯¥È¥EË¤Ä¤¤¤Æ·×»»¤¹¤E
 * 
 * @return ½ĞÎÏ³ÎÎ¨¤ÎÂĞ¿ôÃÍ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * Calculate output probability on a tree lexion node.  This function
 * calculates log output probability of an input vector on time frame @a t
 * in input paramter @a param at a node on tree lexicon.
 * 
 * @param wchmm [in] tree lexicon structure
 * @param node [in] node ID to compute the output probability
 * @param last_wid [in] word ID of last word hypothesis (used when the node is
 * within the word beginning phone and triphone is used.
 * @param t [in] time frame of input vector in @a param to compute.
 * @param param [in] input parameter structure
 * 
 * @return the computed log probability.
 * </EN>
 * @callgraph
 * @callergraph
 */
LOGPROB
outprob_style(WCHMM_INFO *wchmm, int node, int last_wid, int t, HTK_Param *param)
{
  char rbuf[MAX_HMMNAME_LEN]; ///< Local workarea for HMM name conversion

#ifndef PASS1_IWCD
  
  /* if cross-word triphone handling is disabled, we simply compute the
     output prob of the state */
  return(outprob_state(wchmm->hmmwrk, t, wchmm->state[node].out, param));
  
#else  /* PASS1_IWCD */

  /* state type and context cache is considered */
  HMM_Logical *ohmm, *rhmm;
  RC_INFO *rset;
  LRC_INFO *lrset;
  CD_Set *lcd;
  WORD_INFO *winfo = wchmm->winfo;
  HTK_HMM_INFO *hmminfo = wchmm->hmminfo;

  /* the actual computation is different according to their context dependency
     handling */
  switch(wchmm->outstyle[node]) {
  case AS_STATE:
    /* normal state (word-internal or context-independent )*/
    /* compute as usual */
    return(outprob_state(wchmm->hmmwrk, t, wchmm->state[node].out.state, param));
  case AS_LSET:
    /* node in word end phone */
    /* compute approximated value using the state set in pseudo phone */
    return(outprob_cd(wchmm->hmmwrk, t, wchmm->state[node].out.lset, param));
  case AS_RSET:
    /* note in the beginning phone of word */
    /* depends on the last word hypothesis to compute the actual triphone */
    rset = wchmm->state[node].out.rset;
    /* consult cache */
    if (rset->cache.state == NULL || rset->lastwid_cache != last_wid) {
      /* cache miss...calculate */
      /* rset contains either defined biphone or pseudo biphone */
      if (last_wid != WORD_INVALID) {
	/* lookup triphone with left-context (= last phoneme) */
	if ((ohmm = get_left_context_HMM(rset->hmm, (winfo->wseq[last_wid][winfo->wlen[last_wid]-1])->name, hmminfo)) != NULL) {
	  rhmm = ohmm;
	} else {
	  /* if triphone not found, try to use the bi-phone itself */
	  rhmm = rset->hmm;
	  /* If the bi-phone is explicitly specified in hmmdefs/HMMList,
	     use it.  if both triphone and biphone not found in user-given
	     hmmdefs/HMMList, use "pseudo" phone, as same as the end of word */
	  if (debug2_flag) {
	    if (rhmm->is_pseudo) {
	    error_missing_left_triphone(rset->hmm, (winfo->wseq[last_wid][winfo->wlen[last_wid]-1])->name);
	    }
	  }
	}
      } else {
	/* if last word is WORD_INVALID try to use the bi-phone itself */
	rhmm = rset->hmm;
	/* If the bi-phone is explicitly specified in hmmdefs/HMMList,
	   use it.  if not, use "pseudo" phone, as same as the end of word */
	if (debug2_flag) {
	  if (rhmm->is_pseudo) {
	    error_missing_left_triphone(rset->hmm, (winfo->wseq[last_wid][winfo->wlen[last_wid]-1])->name);
	  }
	}
      }
      /* rhmm may be a pseudo phone */
      /* store to cache */
      if (rhmm->is_pseudo) {
	rset->last_is_lset  = TRUE;
	rset->cache.lset    = &(rhmm->body.pseudo->stateset[rset->state_loc]);
      } else {
	rset->last_is_lset  = FALSE;
	rset->cache.state   = rhmm->body.defined->s[rset->state_loc];
      }
      rset->lastwid_cache = last_wid;
    }
    /* calculate outprob and return */
    if (rset->last_is_lset) {
      return(outprob_cd(wchmm->hmmwrk, t, rset->cache.lset, param));
    } else {
      return(outprob_state(wchmm->hmmwrk, t, rset->cache.state, param));
    }
  case AS_LRSET:
    /* node in word with only one phoneme --- both beginning and end */
    lrset = wchmm->state[node].out.lrset;
    if (lrset->cache.state == NULL || lrset->lastwid_cache != last_wid) {
      /* cache miss...calculate */
      rhmm = lrset->hmm;
      /* lookup cdset for given left context (= last phoneme) */
      strcpy(rbuf, rhmm->name);
      if (last_wid != WORD_INVALID) {
	add_left_context(rbuf, (winfo->wseq[last_wid][winfo->wlen[last_wid]-1])->name);
      }
      if (wchmm->category_tree) {
#ifdef USE_OLD_IWCD
	lcd = lcdset_lookup_by_hmmname(hmminfo, rbuf);
#else
	/* use category-indexed cdset */
	if (last_wid != WORD_INVALID &&
	    (ohmm = get_left_context_HMM(rhmm, (winfo->wseq[last_wid][winfo->wlen[last_wid]-1])->name, hmminfo)) != NULL) {
	  lcd = lcdset_lookup_with_category(wchmm, ohmm, lrset->category);
	} else {
	  lcd = lcdset_lookup_with_category(wchmm, rhmm, lrset->category);
	}
#endif
      } else {
	lcd = lcdset_lookup_by_hmmname(hmminfo, rbuf);
      }
      if (lcd != NULL) {	/* found, set to cache */
	lrset->last_is_lset  = TRUE;
        lrset->cache.lset    = &(lcd->stateset[lrset->state_loc]);
        lrset->lastwid_cache = last_wid;
      } else {
	/* no relating lcdset found, falling to normal state */
	if (rhmm->is_pseudo) {
	  lrset->last_is_lset  = TRUE;
	  lrset->cache.lset    = &(rhmm->body.pseudo->stateset[lrset->state_loc]);
	  lrset->lastwid_cache = last_wid;
	} else {
	  lrset->last_is_lset  = FALSE;
	  lrset->cache.state   = rhmm->body.defined->s[lrset->state_loc];
	  lrset->lastwid_cache = last_wid;
	}
      }
      /*printf("[%s->%s]\n", lrset->hmm->name, rhmm->name);*/
    }
    /* calculate outprob and return */
    if (lrset->last_is_lset) {
      return(outprob_cd(wchmm->hmmwrk, t, lrset->cache.lset, param));
    } else {
      return(outprob_state(wchmm->hmmwrk, t, lrset->cache.state, param));
    }
  default:
    /* should not happen */
    /* j_internal_error("outprob_style: no outprob style??\n"); */
    return(LOG_ZERO);
  }

#endif  /* PASS1_IWCD */

}

/** 
 * <JA>
 * @brief  ¥È¥é¥¤¥Õ¥©¥ó¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¡§±¦¥³¥ó¥Æ¥­¥¹¥ÈÍÑ
 * 
 * »ØÄê¤·¤¿±¦¥³¥ó¥Æ¥­¥¹¥È¤ò»ı¤Ä¥È¥é¥¤¥Õ¥©¥ó¤¬
 * ¸«¤Ä¤«¤é¤Ê¤«¤Ã¤¿¾Eç¤Ë¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¤ò½ĞÎÏ¤¹¤EØ¿E 
 * 
 * @param base [in] ¥Ù¡¼¥¹¤Î¥È¥é¥¤¥Õ¥©¥E * @param rc_name [in] ±¦¥³¥ó¥Æ¥­¥¹¥È¤Î²»ÁÇÌ¾
 * </JA>
 * <EN>
 * @brief  Triphone error message for right context.
 * 
 * Output error message when a triphone with the specified right context is
 * not defined.
 * 
 * @param base [in] base triphone
 * @param rc_name [in] name of right context phone 
 * </EN>
 * @callgraph
 * @callergraph
 */
void
error_missing_right_triphone(HMM_Logical *base, char *rc_name)
{
  char rbuf[MAX_HMMNAME_LEN]; ///< Local workarea for HMM name conversion
  /* only output message */
  strcpy(rbuf, base->name);
  add_right_context(rbuf, rc_name);
  /* jlog("WARNING: IW-triphone for word end \"%s\" not found, fallback to pseudo {%s}\n", rbuf, base->name); */
}

/** 
 * <JA>
 * @brief  ¥È¥é¥¤¥Õ¥©¥ó¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¡§º¸¥³¥ó¥Æ¥­¥¹¥ÈÍÑ
 * 
 * »ØÄê¤·¤¿º¸¥³¥ó¥Æ¥­¥¹¥È¤ò»ı¤Ä¥È¥é¥¤¥Õ¥©¥ó¤¬
 * ¸«¤Ä¤«¤é¤Ê¤«¤Ã¤¿¾Eç¤Ë¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¤ò½ĞÎÏ¤¹¤EØ¿E 
 * 
 * @param base [in] ¥Ù¡¼¥¹¤Î¥È¥é¥¤¥Õ¥©¥E * @param lc_name [in] º¸¥³¥ó¥Æ¥­¥¹¥È¤Î²»ÁÇÌ¾
 * </JA>
 * <EN>
 * @brief  Triphone error message for left context.
 * 
 * Output error message when a triphone with the specified right context is
 * not defined.
 * 
 * @param base [in] base triphone
 * @param lc_name [in] name of left context phone 
 * </EN>
 * @callgraph
 * @callergraph
 */
void
error_missing_left_triphone(HMM_Logical *base, char *lc_name)
{
  char rbuf[MAX_HMMNAME_LEN]; ///< Local workarea for HMM name conversion
  /* only output message */
  strcpy(rbuf, base->name);
  add_left_context(rbuf, lc_name);
  /* jlog("WARNING: IW-triphone for word head \"%s\" not found, fallback to pseudo {%s}\n", rbuf, base->name); */
}

/* end of file */
