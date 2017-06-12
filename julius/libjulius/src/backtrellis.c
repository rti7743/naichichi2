/**
 * @file   backtrellis.c
 * 
 * <JA>
 * @brief  �P��g�����X�̕ۑ��E�Q��
 *
 * ��1�p�X�̌��ʂ�P��g�����X�Ƃ��ĕۑ����C��2�p�X�ŎQ�Ƃ��邽�߂̊֐��Q
 * �ł�. Julius �ł́C��P�p�X�ŒT�����ɏI�[�������c���Ă����P��͑S�āC
 * ���̎n�I�[�t���[���C�n�[����̗ݐϖޓx����ђP�ꗚ���ƂƂ���
 * �ۑ�����C��Q�p�X�ł��̏W���̒�����ĒT�����s���܂�. 
 * ���̑�P�p�X�Ńt���[�����ƂɎc�����P����̂��Ƃ��u�g�����X�P��v�C
 * �g�����X�P��̏W���S�̂��u�P��g�����X�v�ƌĂт܂�. 
 *
 * �g�����X�P��́C��P�p�X�̔F�����Ɋe�t���[�����Ƃɕۑ�����܂�. 
 * ��P�p�X�I����C�g�����X�S�̂̐��`�E�Ĕz�u�ƃt���[�����Ƃ̃C���f�b�N�X
 * ���쐬���܂�. 
 *
 * ��Q�p�X�ł́C���̒P��g�����X���Q�Ƃ���
 * �e���ԁi���̓t���[���j�ɂ�����W�J�\�ȉ����̃��X�g�𓾂�ƂƂ��ɁC
 * ���̑�P�p�X�ł�(��������)�ݐϖޓx���C��Q�p�X�ɂ����鉼���̖��W�J������
 * ����X�R�A�Ƃ��ėp���܂�. ���̂����݂���C�P��g�����X�́u�o�b�N�g�����X�v
 * �Ƃ��Ă΂�Ă��܂�. 
 * </JA>
 * 
 * <EN>
 * @brief  Word trellis operations
 *
 * Functions to store the result of the 1st pass as "word trellis",
 * and functions to access them from the 2nd pass are defined in this
 * file.  On the 1st pass of Julius, all the promising words whose
 * word end has been survived at the 1st pass will be stored as "word
 * trellis", which consists of surviving words: word boundary,
 * accumulated score and word history.
 *
 * The trellis word will be stored per frame at the 1st pass.  After
 * the 1st pass ended, the word trellis will be re-organized and
 * indexed by frame to prepare for access at the 2nd pass.
 *
 * In the 2nd pass of reverse stack decoding, this word trellis will be
 * used to constrain the word hypothesis, and also used to estimate
 * the score of unseen area by the obtained backward scores in the 1st pass.
 * Thus the word trellis information is also called as "back trellis" in
 * Julius.
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Tue Feb 22 15:40:01 2005
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

#include <julius/julius.h>

/** 
 * <JA>
 * �P��g�����X��ێ����� �P��g�����X �\���̂�����������(�N������1�񂾂����s)
 * 
 * @param bt [in] ���������� �P��g�����X �\���̂ւ̃|�C���^ 
 * </JA>
 * <EN>
 * Initialize backtrellis that will hold the whole word trellis
 * (called once on startup).
 * 
 * @param bt [in] pointer to the backtrellis structure to initialize
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
void
bt_init(BACKTRELLIS *bt)
{
  bt->num  = NULL;
  bt->rw   = NULL;
  bt->list = NULL;
  bt->root = NULL;
}

/** 
 * <JA>
 * ����̔F���p�� �P��g�����X �\���̂��������� (�F���J�n�����ƂɎ��s). 
 * 
 * @param bt [in] �ΏۂƂ���P��g�����X�\���̂ւ̃|�C���^
 * </JA>
 * <EN>
 * Prepare backtrellis for the next input (called at beginning of each
 * speech segment).
 * 
 * @param bt [in] pointer to the word trellis structure
 * </EN>
 * @callergraph
 * @callgraph
 * 
 */
void
bt_prepare(BACKTRELLIS *bt)
{
  /* free previously allocated data */
  mybfree2(&(bt->root));

  /* reset entry point */
  bt->num = NULL;
  bt->rw = NULL;
  bt->list = NULL;
  bt->root = NULL;
}  

/** 
 * <EN>
 * Free memories of backtrellis.
 * </EN>
 * <JA>
 * �P��g�����X�̃��������J������. 
 * </JA>
 * 
 * @param bt [out] pointer to the word trellis structure.
 *
 * @callergraph
 * @callgraph
 * 
 */
void
bt_free(BACKTRELLIS *bt)
{
  if (bt->root) mybfree2(&(bt->root));
  free(bt);
}

/** 
 * <EN>
 * Allocate a new trellis word atom.
 * </EN>
 * <JA>
 * �g�����X�P���V���Ɋ���t����. 
 * </JA>
 * 
 * @param bt [out] pointer to the word trellis structure.
 *
 * @return pointer to the newly allocated trellis word.
 *
 * @callergraph
 * @callgraph
 * 
 */
TRELLIS_ATOM *
bt_new(BACKTRELLIS *bt)
{
  TRELLIS_ATOM *new;

  new = (TRELLIS_ATOM *)mybmalloc2(sizeof(TRELLIS_ATOM), &(bt->root));
  return new;
}



/** 
 * <JA>
 * ��1�p�X�ŏo�������g�����X�P��i�P��I�[�̃g�����X���j���i�[����. 
 *
 * �����ł͊i�[�����s���C��1�p�X�I����� bt_relocate_rw() ��
 * �t���[�����ɍĔz�u����. 
 * 
 * @param bt [i/o] �g�����X�P����i�[����o�b�N�g�����X�\����
 * @param tatom [in] �o�������g�����X�P��ւ̃|�C���^
 * </JA>
 * <EN>
 * Store a trellis word generated on the 1st pass for the 2nd pass.
 *
 * This function just store the new atom into backtrellis.
 * They will be re-located per frame after 1st pass for quick access
 * in the 2nd pass.
 * 
 * @param bt [i/o] backtrellis structure to store the trellis word
 * @param tatom [in] the trellis word to be stored
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
void
bt_store(BACKTRELLIS *bt, TRELLIS_ATOM *tatom)
{
#ifdef WORD_GRAPH
  tatom->within_context = FALSE;
  tatom->within_wordgraph = FALSE;
#endif
  tatom->next = bt->list;
  bt->list = tatom;
}

/** 
 * <JA>
 * ��1�p�X�I����, �i�[���ꂽ�P��g�����X�����t���[�����ɍĔz�u����. 
 * 
 * @param bt [i/o] �P��g�����X�\����
 * </JA>
 * <EN>
 * Re-locate the stored atom lists per frame (will be called after the
 * 1st pass).
 * 
 * @param bt [i/o] word trellis structure
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
void
bt_relocate_rw(BACKTRELLIS *bt)
{
  TRELLIS_ATOM *tre;
  int t;
  int totalnum, n;
  TRELLIS_ATOM **tmp;

  if (bt->framelen == 0) {
    bt->num = NULL;
    return;
  }

  bt->num = (int *)mybmalloc2(sizeof(int) * bt->framelen, &(bt->root));

  /* count number of trellis atom (= survived word end) for each frame */
  for (t=0;t<bt->framelen;t++) bt->num[t] = 0;
  totalnum = 0;
  for (tre=bt->list;tre;tre=tre->next) {
    /* the last frame (when triggered from sp to non-sp) should be discarded */
    if (tre->endtime >= bt->framelen) continue;
    bt->num[tre->endtime]++;
    totalnum++;
  }
  /* if no atom found, return here with all bt->num[t] set to 0 */
  if (totalnum <= 0) {
    bt->num = NULL;
    return;
  }
  
  /* allocate area */
  bt->rw  = (TRELLIS_ATOM ***)mybmalloc2(sizeof(TRELLIS_ATOM **) * bt->framelen, &(bt->root));
  tmp = (TRELLIS_ATOM **)mybmalloc2(sizeof(TRELLIS_ATOM *) * totalnum, &(bt->root));
  n = 0;
  for (t=0;t<bt->framelen;t++) {
    if (bt->num[t] > 0) {
      bt->rw[t] = (TRELLIS_ATOM **)&(tmp[n]);
      n += bt->num[t];
    }
  }
  /* then store the atoms */
  for (t=0;t<bt->framelen;t++) bt->num[t] = 0;
  for (tre=bt->list;tre;tre=tre->next) {
    /* the last frame (when triggered from sp to non-sp) should be discarded */
    if (tre->endtime >= bt->framelen) continue;
    t = tre->endtime;
    
    bt->rw[t][bt->num[t]] = tre;
    bt->num[t]++;
  }
}


/* �ȉ��̊֐��� bt_relocate_rw ���s��ɂ̂ݎg�p�\�ƂȂ�. */
/* functions below this line should be called after bt_relocate_rw() */

/** 
 * <JA>
 * �����f�R�[�f�B���O��, ��P�p�X�I����ɁC
 * ���̓Z�O�����g�̗��[�Ɏc�����ŖޒP�ꉼ�������o��, ������
 * ��2�p�X�ɂ����鏉��/�ŏI�����Ƃ��ăZ�b�g����. 
 * 
 * @param r [in] �F�������C���X�^���X
 * </JA>
 * <EN>
 * When using progressive decoding with short pause segmentation,
 * This function extracts the best word hypothesis on head and tail of
 * the current input segment just after the 1st pass ends,
 * and store them as start/end word in the following 2nd pass.
 * 
 * @param r [in] recognition process instance
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
void
set_terminal_words(RecogProcess *r)
{
  LOGPROB maxscore;
  int i,t;
  BACKTRELLIS *bt;

  bt = r->backtrellis;

  if (bt->num == NULL) return;

  maxscore = LOG_ZERO;
  /* find last frame where a word exists */
  for(t=bt->framelen-1;t>=0;t--) {
    if (bt->num[t] > 0) break;
  }
  /* get maximum word hypothesis at that frame */
  for(i=0;i<bt->num[t];i++) {
    if (maxscore < (bt->rw[t][i])->backscore) {
      maxscore = (bt->rw[t][i])->backscore;
      r->sp_break_2_begin_word = (bt->rw[t][i])->wid;
    }
  }
  maxscore = LOG_ZERO;
  /* find first frame where a word exists */
  for(t=0;t<bt->framelen;t++) {
    if (bt->num[t] > 0) break;
  }
  /* get maximum word hypothesis at that frame */
  for(i=0;i<bt->num[t];i++) {
    if (maxscore < (bt->rw[t][i])->backscore) {
      maxscore = (bt->rw[t][i])->backscore;
      r->sp_break_2_end_word = (bt->rw[t][i])->wid;
    }
  }
#ifdef SP_BREAK_DEBUG
  jlog("DEBUG: 2nd pass begin word: %s\n",
	 (r->sp_break_2_begin_word == WORD_INVALID) ? "WORD_INVALID" : r->lm->winfo->wname[r->sp_break_2_begin_word]);
  jlog("DEBUG: 2nd pass end word: %s\n",
	 (r->sp_break_2_end_word == WORD_INVALID) ? "WORD_INVALID" : r->lm->winfo->wname[r->sp_break_2_end_word]);
#endif
}


/* the outprob on the trellis connection point should be discounted */
/** 
 * <JA>
 * ��1�p�X�I����, ��2�p�X�ł̃g�����X�Đڑ��v�Z�̂��߂ɁC
 * �S���Ԃɓn���Ċe�g�����X�P��̏I�[�̍ŏI��Ԃ̏o�͖ޓx���Čv�Z��,
 * �����ݐς��獷�������Ă���. ��Q�p�X�ł́C�����ڑ����ɂ�
 * �ڑ��������l�����Đڑ��_�̏�Ԃ̖ޓx���Čv�Z�����. 
 * 
 * @param wchmm [in] �؍\��������
 * @param bt [in] �P��g�����X�\����
 * @param param [in] ���̓p�����[�^���
 * </JA>
 * <EN>
 * Discount the output probabilities of the last state from the accumulated
 * score on word edge for all trellis words survived on the 1st pass,
 * for the acoustic re-computation on the 2nd pass.
 * The acousitic likelihood of the word edge state will be re-computed
 * when the next word hypotheses are expanded on the next 2nd pass.
 * 
 * @param wchmm [in] tree lexicon
 * @param bt [in] word trellis structure
 * @param param [in] input parameter
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
void
bt_discount_pescore(WCHMM_INFO *wchmm, BACKTRELLIS *bt, HTK_Param *param)
{
  int t,i;
  TRELLIS_ATOM *tre;

  if (bt->num == NULL) return;
  
  for (t=0; t<bt->framelen; t++) {
    for (i=0; i<bt->num[t]; i++) {
      tre = bt->rw[t][i];
      /* On normal version, both language score and the output prob. score
	 at the connection point should removed on the trellis for the
	 later connection.  On multi-path mode, removing only the language
	 score is enough. */
      tre->backscore -= outprob_style(wchmm, wchmm->wordend[tre->wid], tre->last_tre->wid, t, param);
    }
  }
}

/** 
 * <EN>
 * Subtract 2-gram scores at each trellis word for the 2nd pass.
 * </EN>
 * <JA>
 * ��2�p�X�̂��߂�2-gram�X�R�A���g�����X��̒P�ꂩ�獷������. 
 * </JA>
 * 
 * @param bt [in] word trellis
 *
 * @callergraph
 * @callgraph
 * 
 */
void
bt_discount_lm(BACKTRELLIS *bt)
{
  int t,i;
  TRELLIS_ATOM *tre;

  if (bt->num == NULL) return;
  
  /* the LM score of the last word should be subtracted, because
     their LM will be assigned by 3-gram on the 2nd pass. */
  for (t=0; t<bt->framelen; t++) {
    for (i=0; i<bt->num[t]; i++) {
      tre = bt->rw[t][i];
      tre->backscore -= tre->lscore;
    }
  }
}

/** 
 * <JA>
 * bt_sort_rw()�p��qsort�R�[���o�b�N. 
 * 
 * @param a [in] �v�f�P
 * @param b [in] �v�f�Q
 * 
 * @return �����\�[�g�ɕK�v�Ȓl
 * </JA>
 * <EN>
 * qsort callback for bt_sort_rw().
 * 
 * @param a [in] first element
 * @param b [in] second element
 * 
 * @return a value needed to do upward sort.
 * </EN>
 *
 * 
 */
static int
compare_wid(TRELLIS_ATOM **a, TRELLIS_ATOM **b)
{
  if ((*a)->wid > (*b)->wid) return 1;
  if ((*a)->wid < (*b)->wid) return -1;
  return 0;
}

/** 
 * <JA>
 * bt_relocate_rw() �I����, �����A�N�Z�X�̂��߂�
 * �o�b�N�g�����X�\���̓��̃g�����X�P����t���[�����Ƃ�
 * �P��ID�Ń\�[�g���Ă���. 
 * 
 * @param bt [i/o] �P��g�����X�\����
 * 
 * </JA>
 * <EN>
 * Sort the trellis words in the backtrellis by the word IDs per each frame,
 * for rapid access on the 2nd pass.  This should be called just after
 * bt_relocate_rw() was called.
 * 
 * @param bt [i/o] word trellis structure
 * 
 * </EN>
 * @callergraph
 * @callgraph
 * 
 */
void
bt_sort_rw(BACKTRELLIS *bt)
{
  int t;

  if (bt->num == NULL) return;

  for (t=0;t<bt->framelen;t++) {
    qsort(bt->rw[t], bt->num[t], sizeof(TRELLIS_ATOM *),
	  (int (*)(const void *,const void *))compare_wid);
  }
}

/* �ȉ��̊֐��͎��O��bt_sort_rw() ���Ă΂�Ă��邱��(��2�p�X�p) */
/* functions below should be called after bt_sort_rw() */

/** 
 * <JA>
 * �P��g�����X���̎w�莞���t���[����ɁC�w��P��̏I�[�����邩�ǂ�����
 * ��������. 
 * 
 * @param bt [in] �P��g�����X�\����
 * @param t [in] ��������I�[�����i�t���[���j
 * @param wkey [in] ��������P��̒P��h�c
 * 
 * @return ���������ꍇ���̃g�����X�P��ւ̃|�C���^�C������Ȃ���� NULL. 
 * </JA>
 * <EN>
 * Search a word on the specified frame in a word trellis data.
 * 
 * @param bt [in] word trellis structure
 * @param t [in] word end frame on which to search
 * @param wkey [in] word ID to search
 * 
 * @return pointer to the found trellis word, or NULL if not found.
 * </EN>
 *
 * @callergraph
 * @callgraph
 * 
 */
TRELLIS_ATOM *
bt_binsearch_atom(BACKTRELLIS *bt, int t, WORD_ID wkey)
{
  /* do binary search */
  /* assume rw are ordered by wid */
  int left, right, mid;
  TRELLIS_ATOM *tmp;
#ifdef WPAIR
  int i;
  LOGPROB maxscore;
  TRELLIS_ATOM *maxtre;
#endif
  
  if (bt->num[t] == 0) return(NULL);
  
  left = 0;
  right = bt->num[t] - 1;
  while (left < right) {
    mid = (left + right) / 2;
    if ((bt->rw[t][mid])->wid < wkey) {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  tmp = bt->rw[t][left];
  if (tmp->wid == wkey) {
#ifdef WPAIR
    /* same word with different context will be found:
       most likely one will be returned */
    maxscore = LOG_ZERO;
    maxtre = NULL;
    i = left;
    while (i >= 0) {
      tmp = bt->rw[t][i];
      if (tmp->wid != wkey) break;
#ifdef WORD_GRAPH
      /* only words on a graph path should be counted */
      if (!tmp->within_wordgraph) {
	i--; continue;
      }
#endif
      if (maxscore < tmp->backscore) {
	maxscore = tmp->backscore;
	maxtre = tmp;
      }
      i--;
    }
    i = left;
    while (i < bt->num[t]) {
      tmp = bt->rw[t][i];
      if (tmp->wid != wkey) break;
#ifdef WORD_GRAPH
      /* only words on a graph path should be counted */
      if (!tmp->within_wordgraph) {
	i++; continue;
      }
#endif
      if (maxscore < tmp->backscore) {
	maxscore = tmp->backscore;
	maxtre = tmp;
      }
      i++;
    }
    tmp = maxtre;
#else
#ifdef WORD_GRAPH
    /* treat only words on a graph path */
    if (! tmp->within_wordgraph) {
      return NULL;
    }
#endif
#endif /* WPAIR */

    return(tmp);
  } else {
    return(NULL);
  }
}

/* end of file */