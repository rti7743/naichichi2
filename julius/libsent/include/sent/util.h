/**
 * @file   util.h
 *
 * <JA>
 * @brief  ÈÆÍÑ¥æ¡¼¥Æ¥£Ž¥¡¦Æ¥£´Ø¿ô·²¤Ë´Ø¤¹¤¡¦¡¦ŽÁ
 *
 * ¤³¤Î¥Õ¥¡¥¤Ž¥¡¦Ë¤Ï¡¤¥Æ¥­¥¹¥ÈÆÉ¤ß¹þ¤ß¤ä°µ½Ì¥Õ¥¡¥¤¥¡¦àºû½Ž¤
 * ¥á¥âŽ¥¡¦ä¤¡¦Õ¤±¡¤¥Ð¥¤¥È¥ª¡¼¥ÀÁàºû½¤ÈÆÍÑ¤Î¥á¥Ã¥»¡¼¥¸½ÐÎÏ´Ø¿ô¤Ê¤É¤ŽÎ
 * ÈÆÍÑ¤Î¥æ¡¼¥Æ¥£Ž¥¡¦Æ¥£´Ø¿ô¤Ë´Ø¤¹¤¡¦¡¦Á¤¬´Þ¤Þ¤¡¦Æ¤¤¤Þ¤¹¡Ž¥
 *
 * </JA>
 * <EN>
 * @brief  Definitions for common utility functions
 *
 * This file contains definitions for common utility functions:
 * text reading and parsing, compressed file input, memory allocation,
 * byte order changing, common printf function (j_printf etc.) and so on.
 *
 * </EN>
 *
 * @author Akinobu LEE
 * @date   Sat Feb 12 12:30:40 2005
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

#ifndef __SENT_UTIL_H__
#define __SENT_UTIL_H__

/*
 *  
 */
/**
 * @brief Memory block size in bytes for mybmalloc()
 *
 * mybmalloc() allocate memory per big block to reduce memory
 * management overhead.  This value sets the block size to be allocated.
 * Smaller value may leads to finer granularity, but overhead may increase.
 * Larger value may result in reduction of overhead, but too much memory
 * can be allocated for a small memory requirement.
 * 
 */
#define MYBMALLOC_BLOCK_SIZE 10000

/// Information of allocated memory block for mybmalloc()
typedef struct _bmalloc_base {
  void *base;			///< Pointer to the actually allocated memory block
  char *now;		    ///< Start Pointer of Currently assigned area
  char *end;		    ///< End Pointer of Currently assigned area
  struct _bmalloc_base *next;	///< Link to next data, NULL if no more
} BMALLOC_BASE;


#ifdef __cplusplus
extern "C" {
#endif

/* readfile.c */
char *getl(char *, int, FILE *);
char *getl_fp(char *, int, FILE *);
char *get_line_from_stdin(char *buf, int buflen, char *prompt);

/* gzfile.c */
FILE *fopen_readfile(char *);
int fclose_readfile(FILE *);
FILE *fopen_writefile(char *);
int fclose_writefile(FILE *);
size_t myfread(void *ptr, size_t size, size_t n, FILE *fp);
size_t myfwrite(void *ptr, size_t size, size_t n, FILE *fp);
int myfgetc(FILE *fp);
int myfeof(FILE *fp);
int myfrewind(FILE *fp);

/* mybmalloc.c */
void *mybmalloc2(unsigned int size, BMALLOC_BASE **list);
char *mybstrdup2(char *, BMALLOC_BASE **list);
void mybfree2(BMALLOC_BASE **list);

/* mymalloc.c */
void *mymalloc(size_t size);
void *mymalloc_big(size_t elsize, size_t nelem);
void *myrealloc(void *, size_t);
void *mycalloc(size_t, size_t);

/* endian.c */
void swap_sample_bytes(SP16 *buf, int len);
void swap_bytes(char *buf, size_t unitbyte, size_t unitnum);

/* j_printf.c */
typedef void(*JLOG_LOGGER_FUNCTION)(const char* msg);
typedef void(*JLOG_LOGREPORT_FUNCTION)(unsigned int code);
void jlog_set_output(FILE *fp);
void jlog_set_output_function(JLOG_LOGGER_FUNCTION function);
void jlogreport_set_output_function(JLOG_LOGREPORT_FUNCTION function);
FILE *jlog_get_fp();
void jlog(char *format, ...);
int jlog_flush();
void jlogreport(unsigned int code);

/* mystrtok.c */
char  *mystrtok_quotation(char *str, char *delim, int left_paren, int right_paren, int mode); /* mode:0=normal, 1=just move to next token */
char *mystrtok_quote(char *str, char *delim);
char *mystrtok(char *str, char *delim);
char *mystrtok_movetonext(char *str, char *delim);

/* confout.c */
void confout(FILE *strm);
void confout_version(FILE *strm);
void confout_audio(FILE *strm);
void confout_lm(FILE *strm);
void confout_am(FILE *strm);
void confout_lib(FILE *strm);
void confout_process(FILE *strm);

/* qsort.c */
void qsort_reentrant(void *base, int count, int size, int (*compare)(const void *, const void *, void *), void *pointer);

#ifdef __cplusplus
}
#endif

#endif /* __SENT_UTIL_H__ */
