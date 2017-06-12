/**
 * @file   jlog.c
 * 
 * <JA>
 * @brief  ¥á¥Ã¥»¡¼¥¸¥Æ¥­¥¹¥È½ÐÎÏ¤È¥¨¥é¡¼½ªÎ»
 *
 * ¥á¥Ã¥»¡¼¥¸½ÐÎÏÍÑ¤ÎÈÆÍÑ´Ø¿ô¤ÎŽÄ¡¦Á¤Ç¤¹¡Ž¥
 * Ç§¼±Ž·¡¦Ì¤ÎÃ¼Ëö½ÐÎÏ¤ä¥Í¥Ã¥È¥¡¦¼¥¯¤Ø¤Î½ÐÎÏ¤È¤¤¤Ã¤¿¡Ž¤
 * Julius ¤Î¼çÍ×¤Ê½ÐÎÏ¤Ï¤³¤³¤Î´Ø¿ô¤ò»È¤Ã¤Æ¹ÔŽ¤¡¦¡¦Þ¤¹¡Ž¥
 * ¤Þ¤¿¡¤½ÐÎÏÊ¸»ú¥³¡¼¥É¤ÎÊÑ´¹¤â¤³¤³¤Ç¹Ô¤¤¤Þ¤¹¡¥
 * </JA>
 * 
 * <EN>
 * @brief  Message text output and error exit functions
 *
 * These are generic functions for text message output.
 * Most of text messages such as recognition results, status, informations
 * in Julius/Julian will be output to TTY or via network using these functions.
 * The character set conversion will also be performed here.
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Feb 17 16:02:41 2005
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

#include <sent/stddefs.h>
#include <sent/tcpip.h>
#include <stdarg.h>


void defaultLogger(const char* msg)
{
	puts(msg);
}
void defaultReporter(unsigned int code)
{
}

#if (_MSC_VER) && (_MSC_VER <= 1400)
#define VSNPRINTF  _vsnprintf
#else
#define VSNPRINTF  vsnprintf
#endif

static JLOG_LOGGER_FUNCTION g_jloggerFunction = defaultLogger;
static JLOG_LOGREPORT_FUNCTION g_jlogreportFunction = defaultReporter;

#define MAX_PRINTF_LEN 4096	///< Maximum string length at one printf call

/** 
 * Set file pointer to output the log.  Set to NULL to totally disable
 * the log outputs.
 * 
 * @param fp [in] file pointer or NULL to disable
 * 
 */
void
jlog_set_output(FILE *fp)
{
}


void
jlog_set_output_function(JLOG_LOGGER_FUNCTION function)
{
  g_jloggerFunction = function;
}

void
jlogreport_set_output_function(JLOG_LOGREPORT_FUNCTION function)
{
  g_jlogreportFunction = function;
}
void jlogreport(unsigned int code)
{
  g_jlogreportFunction(code);
}


/** 
 * Return the current file pointer to output log.
 * 
 * @return the file pointer.
 * 
 */
FILE *
jlog_get_fp()
{
  return stdout;
}

/** 
 * @brief  Output a normal message to log
 *
 * 
 * @param fmt [in] format string, like printf.
 * @param ... [in] variable length argument like printf.
 * 
 */
void
jlog(char *fmt, ...)
{
	va_list ap;
	char buffer[1024];

	va_start(ap,fmt);
	VSNPRINTF(buffer,1023, fmt, ap);
	buffer[1023] = 0;
	va_end(ap);

	g_jloggerFunction(buffer);
	return;
}

/** 
 * @brief  Flush text message
 *
 * The output device can be changed by set_print_func().
 * 
 * @return the same as fflush, i.e. 0 on success, other if failed.
 */
int
jlog_flush()
{
	return 0;
}

