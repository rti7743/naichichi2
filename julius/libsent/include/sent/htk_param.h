/**
 * @file   htk_param.h
 *
 * <EN>
 * @brief Structures for storing input speech parameters
 *
 * This file defines a structure for holding a vector sequence of
 * input speech parameters.  The speech parameter sequence will be
 * stored in HTK_Param.  The HTK_Param also holds information about
 * the extraction condition, i.e., frame shift length, window size and
 * so on.
 *
 * The speech input vector can be read from HTK parameter file, or
 * Julius extracts the parameters directly from input speech.  Julius
 * supports extraction of only MFCC format of fixed dimension.
 * More precisely, only parameter type of MFCC_{0|E}_D[_Z][_N] with {25|26}
 * dimensions is supported.
 *
 * When recognition, the parameter types of both acoustic model and
 * input parameter should be the same.  Please note that only the
 * parameter type is checked, and other parameters such as source sampling
 * rate, frame shift length and window sizes will not be checked.
 * </EN>
 * <JA>
 * @brief HTK¤ÎÆÃÄ§¥Ñ¥é¥á¡¼¥¿¤òÊİ»ı¤¹¤E½Â¤ÂÎ¤Ë´ØÏ¢¤¹¤EEÁ
 *
 * ¤³¤Î¥Õ¥¡¥¤¥EË¤Ï¡¤²»À¼ÆÃÄ§ÎÌ¤Î¥Ù¥¯¥È¥EÏÎó¤òÊİ»ı¤¹¤E½Â¤ÂÎ¤¬
 * ÄEÁ¤µ¤EÆ¤¤¤Ş¤¹¡¥ÆşÎÏ²»À¼¤«¤é·×»»¤µ¤E¿MFCCÅù¤Î²»À¼ÆÃÄ§ÎÌ¤Ï¡¤
 * ¤³¤³¤ÇÄEÁ¤µ¤EE½Â¤ÂÎ HTK_Param ¤ËÊİÂ¸¤µ¤EŞ¤¹¡¥HTK_Param¤Ë¤Ï
 * ¤Ş¤¿¡¤ÆÃÄ§ÎÌÃEĞ»ş¤Î¥Õ¥E¼¥à¥·¥Õ¥ÈÉı¤ä¥¦¥£¥ó¥É¥¦Ä¹¤Ê¤É¤Î¾ğÊó¤¬
 * Êİ»ı¤µ¤EŞ¤¹¡¥
 *
 * ²»À¼ÆÃÄ§ÎÌ¤Ï³°Éô¤Ç HTK ¤Ê¤É¤Ë¤è¤Ã¤ÆÃEĞ¤µ¤E¿HTK·Á¼°¤ÎÆÃÄ§ÎÌ¥Õ¥¡¥¤¥EE * ÆÉ¤ß¹ş¤à¤³¤È¤¬¤Ç¤­¤Ş¤¹¡¥¤Ş¤¿¡¤MFCC ·Á¼°¤Ç¤¢¤EĞ Julius Æâ¤Ç
 * Ä¾ÀÜ²»À¼ÇÈ·Á¤«¤éÃEĞ¤¹¤E³¤È¤¬¤Ç¤­¤Ş¤¹¡¥¼Âºİ¤ËJulius¤¬ÆâÉô¤ÇÃEĞ¤¹¤E * ¤¹¤E³¤È¤¬¤Ç¤­¤EÃÄ§ÎÌ¤Ï {25|26} ¼¡¸µ¤Î MFCC_{0|E}_D[_Z][_N] ¤Î¤ß¤Ç¤¹¡¥
 *
 * »ÈÍÑ¤¹¤E»¶Á¥â¥Ç¥E%HMM)¤¬³Ø½¬»ş¤ËÍÑ¤¤¤¿ÆÃÄ§ÎÌ¤ÈÇ§¼±ÂĞ¾İ¤È¤¹¤EşÎÏ¤Î
 * ÆÃÄ§ÎÌ¤Î·Á¼°¤Ï°E×¤µ¤»¤E¬Í×¤¬¤¢¤ê¤Ş¤¹¡¥Ç§¼±¼Â¹Ô»ş¤Ë¤Ï¡¤²»¶Á¥â¥Ç¥EÈÆşÎÏ
 * ¥Õ¥¡¥¤¥EÎÆÃÄ§ÎÌ·Á¼°¤¬¥Á¥§¥Ã¥¯¤µ¤E¤Å¬¹ç¤·¤Ê¤¤¾Eç¤Ï¥¨¥é¡¼¤È¤Ê¤ê¤Ş¤¹¡¥
 * ¤¿¤À¤·¡¤ÆşÎÏ²»À¼¤Î¥µ¥ó¥×¥Eó¥°¼şÇÈ¿ô¤ä¥Õ¥E¼¥à¥·¥Õ¥ÈÉı¡¤¥¦¥£¥ó¥É¥¦Ä¹¤Î
 * ¾ğÊó¤ÏHTK·Á¼°¤Î²»¶Á¥â¥Ç¥EË¤ÏÊİ»ı¤µ¤EÆ¤¤¤Ê¤¤¤¿¤á¡¤¥Á¥§¥Ã¥¯¤Ç¤­¤Ş¤»¤ó¡¥
 * ÃúÌÕ¤·¤Æ²¼¤µ¤¤¡¥
 * </JA>
 *
 * @sa htk_defs.h
 *
 * @author Akinobu LEE
 * @date   Fri Feb 11 02:52:52 2005
 *
 * $Revision: 1.4 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#ifndef __SENT_HTK_PARAM_H__
#define __SENT_HTK_PARAM_H__

#include <sent/stddefs.h>
#include <sent/htk_defs.h>

/// Parameter types and extraction conditions
typedef struct {
  unsigned int samplenum;	///< Number of samples (or frames)
  unsigned int wshift;		///< Window shift (unit: 100ns) 
  unsigned short sampsize;	///< Bytes per sample 
  short samptype;		///< Parameter type, see also htk_defs.h
} HTK_Param_Header;

/// Input speech parameter
typedef struct {
  HTK_Param_Header header;	///< Parameter header information
  unsigned int samplenum;	///< Number of sample (same in header.samplenum) 
  short veclen;			///< Vector length of a sample
  VECT **parvec;		///< Actual parameter vectors [0..samplenum-1][0..veclen-1]
  short veclen_alloc;		///< Allocated vector length of a sample
  unsigned int samplenum_alloc;	///< Alllocated number of samples
  BMALLOC_BASE *mroot;		///< Pointer for block memory allocation
} HTK_Param;

/**
 * Increment step of HTK Parameter holder in frames
 * 
 */
#define HTK_PARAM_INCREMENT_STEP_FRAME 200

#ifdef __cplusplus
extern "C" {
#endif

boolean rdparam(char *, HTK_Param *);
HTK_Param *new_param();
void free_param(HTK_Param *);
short param_qualstr2code(char *);
short param_str2code(char *);
char *param_qualcode2str(char *, short, boolean);
char *param_code2str(char *, short, boolean);
int guess_basenum(HTK_Param *p, short qualtype);
boolean param_strip_zero(HTK_Param *param);

void param_init_content(HTK_Param *p);
boolean param_alloc(HTK_Param *p, unsigned int samplenum, short veclen);
void param_free_content(HTK_Param *p);


/* hmminfo/put_htkdata_info.c */
void put_param_head(FILE *fp, HTK_Param_Header *h);
void put_vec(FILE *fp, VECT **p, int num, short veclen);
void put_param(FILE *fp, HTK_Param *pinfo);
void put_param_info(FILE *fp, HTK_Param *pinfo);

#ifdef __cplusplus
}
#endif

#endif /* __SENT_HTK_PARAM_H__ */
