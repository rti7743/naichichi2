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
 * @brief HTKの特徴パラメータを保持す��E渋ぢ里亡慙△垢�E�E�
 *
 * このファイ��E砲蓮げ酸柴団�量のベクト��E藁鵑鯤飮�す��E渋ぢ里�
 * ��E舛気�E討い泙后テ�力音声から計算さ��E�MFCC等の音声特徴量は，
 * ここで��E舛気�E�E渋ぢ� HTK_Param に保存さ��E泙后�HTK_Paramには
 * また，特徴量��E仍�のフ��E璽爛轡侫班�やウィンドウ長などの情報が
 * 保持さ��E泙后�
 *
 * 音声特徴量は外部で HTK などによって��E个気�E�HTK形式の特徴量ファイ��E�E * 読み込むことができます．また，MFCC 形式であ��E� Julius 内で
 * 直接音声波形から��E个垢�E海箸�できます．実際にJuliusが内部で��E个垢�E * す��E海箸�でき��E団�量は {25|26} 次元の MFCC_{0|E}_D[_Z][_N] のみです．
 *
 * 使用す��E散船皀妊�E%HMM)が学習時に用いた特徴量と認識対象とす��E�力の
 * 特徴量の形式は��E廚気擦�E�要があります．認識実行時には，音響モデ��E汎�力
 * ファイ��E瞭団�量形式がチェックさ��Eづ�合しない��E腓魯┘蕁爾箸覆蠅泙后�
 * ただし，入力音声のサンプ��E鵐絢�波数やフ��E璽爛轡侫班�，ウィンドウ長の
 * 情報はHTK形式の音響モデ��E砲亙飮�さ��E討い覆い燭瓠ぅ船Д奪�できません．
 * 丁盲して下さい．
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
