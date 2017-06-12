/**
* @file   recogmain.c
* 
* <JA>
* @brief  Ç§¼±¥á¥¤¥ó´ØŽ¿¡¦ * </JA>
* 
* <EN>
* @brief  Main function of recognition process.
* </EN>
* 
* @author Akinobu Lee
* @date   Wed Aug  8 14:53:53 2007
*
* $Revision: 1.19 $
* 
*/

/*
* Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
* Copyright (c) 1997-2000 Information-technology Promotion Agency, Japan
* Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
* Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
* All rights reserved
*/
/**
* @mainpage
*
* <EN>
* This is a source code browser of Julius.
*
* - Sample code to use JuliusLib: julius-simple.c
* - JuliusLib API reference: @ref jfunc
* - List of callbacks: libjulius/include/julius/callback.h
*
* You can access documentation for files, functions and structures
* from the tabs at the top of this page.
* 
* </EN>
* <JA>
* ¤³Ž¤¡¦ŽÏ Julius ¤Î¥½¡¼¥¹¥³¡¼¥É¤Î¥Ö¥é¥¦¥¶¤Ç¤¹¡¥
*
* - JuliusLib¤ò»ÈÍÑ¤¹Ž¤¡¦µ¥ó¥×¥¡¦³¡¼¥ŽÉ: julius-simple/julius-simple.c
* - JuliusLib API Ž¥¡¦Õ¥¡¥¡¦ó¥¹¡Ž§ @ref jfunc
* - ¥³¡¼Ž¥¡¦Ð¥Ã¥Ž¯ Ž°¡¦¡¦ libjulius/include/julius/callback.h
*
* ¥Ú¡¼¥¸¾åÉô¤Î¥¿¥Ö¤«¤é¥Õ¥¡¥¤Ž¥¡¦¦´Ø¿ô¡¦¹½Â¤ÂÎÅù¤ÎÀâÌÀ¤ò¸«¤¡¦³¤È¤¬½ÐÍè¤Þ¤¹¡Ž¥
* 
* </JA>
* 
*/
/**
* @defgroup jfunc JuliusLib API
*
* <EN>
* Here is a reference of all Julius library API functions.
* </EN>
* <JA>
* Julius ¥é¥¤¥Ö¥éŽ¥¡¦API ´Ø¿ô¤ÎŽ¥¡¦Õ¥¡¥¡¦ó¥¹¤Ç¤Ž¹. 
* </JA>
* 
*/
/**
* @defgroup engine Basic API
* @ingroup jfunc
*
* <EN>
* Basic functions to start-up and initialize engines.
* </EN>
* <JA>
* Ç§¼±¥¨¥ó¥¸¥ó¤ÎÀßŽÄ¡¦¡¦ * </JA>
* 
*/
/**
* @defgroup callback Callback API
* @ingroup jfunc
*
* <EN>
* Functions to add callback to get results and status.
* </EN>
* <JA>
* Ç§¼±Ž·¡¦Ì¤ä¥¨¥ó¥¸¥ó¾õÂÖ¤òÃÎ¤¡¦¿¤á¤Î¥³¡¼¥¡¦Ð¥Ã¥Ž¯
* </JA>
* 
*/
/**
* @defgroup pauseresume Pause and Resume API
* @ingroup jfunc
*
* <EN>
* Functions to pause / resume engine inputs.
* </EN>
* <JA>
* ¥¨¥ó¥¸¥ó¤ÎŽ°¡¦þÄä»ß¡¦ºÆ³Ž«
* </JA>
* 
*/
/**
* @defgroup userfunc User function API
* @ingroup jfunc
*
* <EN>
* Functions to register user function to be applied inside Julius.
* </EN>
* <JA>
* ¥æ¡¼¥¶´Ø¿ô¤ÎÅÐÏ¿
* </JA>
* 
*/
/**
* @defgroup jfunc_process Process API
* @ingroup jfunc
*
* <EN>
* Functions to create / remove / (de)activate recognition process and models
* on live.
* </EN>
* <JA>
* ¥â¥ÇŽ¥¡¦ª¤è¤ÓÇ§¼±¥×¥úÁ»¥¹¤ÎÆ°ÅªÄÉ²Ã¡¦º¡¦¡¦¦Í­¸ú²½¡¦Ìµ¸ú²Ž½
* </JA>
* 
*/
/**
* @defgroup grammar Grammar / Dictionary API
* @ingroup jfunc
*
* <EN>
* Functions to manage grammars or word dictionaries at run time.
* </EN>
* <JA>
* Ê¸Ë¡¡¦Ã±Ž¸¡¦­½ñ¤ÎÁàº¡¦ * </JA>
* 
*/
/**
* @defgroup jconf Jconf configuration API
* @ingroup jfunc
*
* <EN>
* Functions to load / create configuration parameters.
* </EN>
* <JA>
* Jconf ¹½Â¤ÂÎ¤Ë¤èŽ¤¡¦Ñ¥é¥á¡¼¥¿¾ðÊó¤Î´ÉÍ¡¦
* </JA>
* 
*/
/**
* @defgroup instance LM/AM/SR instance API
* @ingroup jfunc
*
* <EN>
* Functions to handle modules and processes directly.
* </EN>
* <JA>
* ¥â¥ÇŽ¥¡¦â¥¸¥å¡¼¥¡¦ä¥×¥úÁ»¥¹¤òÄ¾ÀÜ°·¤¦´Ø¿ô¡Ž¥
* </JA>
* 
*/

#define GLOBAL_VARIABLE_DEFINE	///< Actually make global vars in global.h
#include <julius/julius.h>
#include <signal.h>
#if defined(_WIN32) && !defined(__CYGWIN32__)
#include <mbctype.h>
#include <mbstring.h>
#endif

/* ---------- utility functions -----------------------------------------*/
#ifdef REPORT_MEMORY_USAGE
/** 
* <JA>
* ÄÌŽ¾¡¦ªÎ»»þ¤Ë»ÈÍÑ¥á¥â¥¡¦Ì¤òÄ´¤Ù¤Æ½ÐÎÏ¤¹¤¡¦(Linux, sol2)
* 
* </JA>
* <EN>
* Get process size and output on normal exit. (Linux, sol2)
* 
* </EN>
*/
static void
	print_mem()
{
	char buf[200];
	sprintf(buf,"ps -o vsz,rss -p %d",getpid());
	system(buf);
	fflush(stdout);
	fflush(stderr);
}
#endif


/** 
* <EN>
* allocate storage of recognition alignment results.
*
* @return the new pointer
* </EN>
* <JA>
* ¥¢¥é¥¤¥ó¥á¥ó¥ÈŽ·¡¦Ì¤Î³ÊÇ¼¾¡¦ê¤ò³ÎÊŽÝ
*
* @return ³ÎÊÝ¤µŽ¤¡¦¿ÎÎ°è¤Ø¤Î¥Ý¥¤¥ó¥Ž¿
* </JA>
*
* @callgraph
* @callergraph
* 
*/
SentenceAlign *
	result_align_new()
{
	SentenceAlign *new;
	new = (SentenceAlign *)mymalloc(sizeof(SentenceAlign));
	new->w = NULL;
	new->ph = NULL;
	new->loc = NULL;
	new->begin_frame = NULL;
	new->end_frame = NULL;
	new->avgscore = NULL;
	new->is_iwsp = NULL;
	new->next = NULL;
	return new;
}

/** 
* <EN>
* free storage of recognition alignment results.
*
* @param a [i/o] alignment data to be released
* </EN>
* <JA>
* ¥¢¥é¥¤¥ó¥á¥ó¥ÈŽ·¡¦Ì¤Î³ÊÇ¼¾¡¦ê¤ò³ÎÊŽÝ
*
* @param a [i/o] ²òŽÊ¡¦µ¤¡¦¡¦¢¥é¥¤¥ó¥á¥ó¥È¥Ç¡¼¥Ž¿
* </JA>
*
* @callgraph
* @callergraph
* 
*/
void
	result_align_free(SentenceAlign *a)
{
	if (a->w) free(a->w);
	if (a->ph) free(a->ph);
	if (a->loc) free(a->loc);
	if (a->begin_frame) free(a->begin_frame);
	if (a->end_frame) free(a->end_frame);
	if (a->avgscore) free(a->avgscore);
	if (a->is_iwsp) free(a->is_iwsp);
	free(a);
}

/** 
* <EN>
* Allocate storage of recognition results.
* </EN>
* <JA>
* Ç§¼±Ž·¡¦Ì¤Î³ÊÇ¼¾¡¦ê¤ò³ÎÊÝ¤¹¤¡¦ 
* </JA>
* 
* @param r [out] recognition process instance
* @param num [in] number of sentences to be output
*
* @callgraph
* @callergraph
* 
*/
void
	result_sentence_malloc(RecogProcess *r, int num)
{
	int i;
	r->result.sent = (Sentence *)mymalloc(sizeof(Sentence) * num);
	for(i=0;i<num;i++) r->result.sent[i].align = NULL;
	r->result.sentnum = 0;
}

/** 
* <EN>
* Free storage of recognition results.
* </EN>
* <JA>
* Ç§¼±Ž·¡¦Ì¤Î³ÊÇ¼¾¡¦ê¤ò²òÊ¡¦¹¤¡¦ 
* </JA>
* 
* @param r [i/o] recognition process instance
* 
* @callgraph
* @callergraph
*/
void
	result_sentence_free(RecogProcess *r)
{  
	int i;
	SentenceAlign *a, *atmp;
	if (r->result.sent) {
		for(i=0;i<r->result.sentnum;i++) {
			a = r->result.sent[i].align;
			while(a) {
				atmp = a->next;
				result_align_free(a);
				a = atmp;
			}
		}
		free(r->result.sent);
		r->result.sent = NULL;
	}
}

/** 
* <EN>
* Clear all result storages for next input.
* </EN>
* <JA>
* Ç§¼±Ž·¡¦Ì¤Î³ÊÇ¼¾¡¦ê¤òÁ´¤Æ¥¯¥¡¦¢¤¹¤¡¦ 
* </JA>
* 
* @param r [in] recognition process instance.
* 
* @callgraph
* @callergraph
*/
void
	clear_result(RecogProcess *r)
{
#ifdef WORD_GRAPH
	/* clear 1st pass word graph output */
	wordgraph_clean(&(r->result.wg1));
#endif

	if (r->lmvar == LM_DFA_WORD) {
		if (r->result.status == J_RESULT_STATUS_SUCCESS) {
			/* clear word recog result of first pass as in final result */
			free(r->result.sent);
		}
	} else {
		if (r->graphout) {
			if (r->config->graph.confnet) {
				/* free confusion network clusters */
				cn_free_all(&(r->result.confnet));
			} else if (r->config->graph.lattice) {
			}
			/* clear all wordgraph */
			wordgraph_clean(&(r->result.wg));
		}
		result_sentence_free(r);
	}
}

/* --------------------- speech buffering ------------------ */

/** 
* <JA>
* @brief  ¸¡½Ð¤µŽ¤¡¦¿²»¤ò¥Ð¥Ã¥Õ¥¡¤ËÊÝÂ¸¤¹¤¡¦adin_go() ¥³¡¼Ž¥¡¦Ð¥Ã¥Ž¯
*
* ¤³¤Î´Ø¿ô¤Ï¡¤¸¡½Ð¤µŽ¤¡¦¿²»À¼ÆþÎÏ¤ò½ç¼Ž¡ recog->speech ¤Ëµ­Ï¿¤·¤Æ
* ¤¤¤¯. ¥Ð¥Ã¥Õ¥¡½èÍý¥â¡¼¥É¡Ê¡áÈóŽ¥¡¦¢¥¡¦¿¥¤¥à¥â¡¼¥É¡Ë¤ÇÇ§¼±¤ò¹Ô¤Ê¤Ž¦
* ¤È¤­¤ËÍÑ¤¤¤éŽ¤¡¦¡¦ 
* 
* @param now [in] ¸¡½Ð¤µŽ¤¡¦¿²»À¼ÇÈ·Á¥Ç¡¼¥¿¤ÎÃÇÊŽÒ
* @param len [in] @a now ¤ÎÄ¹¤µ(¥µ¥ó¥×Ž¥¡¦¡¦
* @param recog [i/o] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
* 
* @return ¥¨¥é¡¼»þ -1 (adin_go ¤ÏÂ¨»þÃæÃÇ¤¹Ž¤¡¦¡¤ÄÌŽ¾¡¦¡¦ 0 (adin_go ¤Ï
* Â³¹Ô¤¹Ž¤¡¦¡¤¶è´Ö½ªÎ»Í×µá»þ 1 (adin_go ¤Ï¸½ºß¤Î²»À¼¶è´Ö¤òÊÄ¤¸Ž¤¡¦. 
* 
* </JA>
* <EN>
* @brief  adin_go() callback to score triggered inputs to buffer.
*
* This function records the incomping speech segments detected in adin_go()
* to recog->speech.  This function will be used when recognition runs
* in buffered mode (= non-realtime mode).
* 
* @param now [in] input speech samples.
* @param len [in] length of @a now in samples
* @param recog [i/o] engine instance
* 
* @return -1 on error (tell adin_go() to terminate), 0 on success (tell
* adin_go() to continue recording), or 1 when this function requires
* input segmentation.
* </EN>
*/
int
	adin_cut_callback_store_buffer(SP16 *now, int len, Recog *recog)
{
	if (recog->speechlen == 0) {		/* first part of a segment */
		if (!recog->process_active) {
			return(1);
		}
	}

	if (recog->speechlen + len > recog->speechalloclen) {
		while (recog->speechlen + len > recog->speechalloclen) {
			recog->speechalloclen += MAX_SPEECH_ALLOC_STEP;
		}
		if (recog->speech == NULL) {
			recog->speech = (SP16 *)mymalloc(sizeof(SP16) * recog->speechalloclen);
		} else {
			if (debug2_flag) {
				jlog("STAT: expanding recog->speech to %d samples\n", recog->speechalloclen);
			}
			recog->speech = (SP16 *)myrealloc(recog->speech, sizeof(SP16) * recog->speechalloclen);
		}
	}

	/* store now[0..len] to recog->speech[recog->speechlen] */
	memcpy(&(recog->speech[recog->speechlen]), now, len * sizeof(SP16));
	recog->speechlen += len;
	return(0);			/* tell adin_go to continue reading */
}


/* --------------------- adin check callback --------------- */
/** 
* <JA>
* @brief  ²»À¼ÆþÎÏÃæ¤ËŽÄ¡¦¡¦ª¤Ë¼Â¹Ô¤µ¤¡¦¡¦³¡¼¥¡¦Ð¥Ã¥Ž¯. 
*
* ¤³¤Î´Ø¿ô¤Ï¡¤adin_go() ¤Ë¤Æ²»À¼ÆþÎÏÂÔ¤Á¡¤¤¢Ž¤¡¦¤¤Ï²»À¼Ç§¼±Ãæ¤ŽË
* ŽÄ¡¦¡¦ª¤Ë·«¤¡¦Ö¤·¸Æ¤Ó½Ð¤µ¤¡¦¡¦Ø¿ô¤Ç¤¢¤¡¦ ¥æ¡¼¥¶ŽÄ¡¦Á¤Î¥³¡¼¥¡¦Ð¥Ã¥Ž¯
* (CALLBACK_POLL) ¤Î¸Æ¤Ó½Ð¤·¡¤¤ª¤è¤ÓÃæÃÇÈ½Äê¤ò¹Ô¤¦. 
*
* @param recog [in] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
* 
* @return ÄÌŽ¾¡¦¡¦ 0, Â¨»þÃæÃÇ¤òÍ×µá»þ -2, Ç§¼±Ãæ»ß¤ÎÍ×µá»þ¤Ï -1 ¤òÊÖ¤¹. 
* </JA>
* <EN>
* @brief  callback function periodically called while input.
*
* This function will be called periodically from adin_go() while
* waiting input or processing recognition.  It will call user-defined
* callback registered in CALLBACK_POLL,  check for the process
* status and issue recognition termination request.
*
* @param recog [in] engine instance
* 
* @return 0 normally, -2 for immediate termination, and -1 if requesting
* recognition stop.
* 
* </EN>
*/
static int
	callback_check_in_adin(Recog *recog)
{
	/* module: check command and terminate recording when requested */
//	callback_exec(CALLBACK_POLL, recog);
	/* With audio input via adinnet, TERMINATE command will issue terminate
	command to the adinnet client.  The client then stops recording
	immediately and return end-of-segment ack.  Then it will cause this
	process to stop recognition as normal.  So we need not to
	perform immediate termination at this callback, but just ignore the
	results in the main.c.  */
#if 1
	if (recog->process_want_terminate) { /* TERMINATE ... force termination */
		return(-2);
	}
	if (recog->process_want_reload) {
		return(-1);
	}
#else
	if (recog->process_want_terminate /* TERMINATE ... force termination */
		&& recog->jconf->input.speech_input != SP_ADINNET) {
			return(-2);
	}
	if (recog->process_want_reload) {
		return(-1);
	}
#endif
	return(0);
}

/*********************/
/* open input stream */
/*********************/
/** 
* <EN>
* Open input stream.
* </EN>
* <JA>
* ²»À¼ÆþÎÏ¥¹¥È¥ê¡¼¥à¤ò³«¤¯
* </JA>
* 
* @param recog [i/o] engine instance
* @param file_or_dev_name [in] file or device name of the device
* 
* @return 0 on success, -1 on error, -2 on device initialization error.
* 
* @callgraph
* @callergraph
* @ingroup engine
*/
int
	j_open_stream(Recog *recog, char *file_or_dev_name)
{
	Jconf *jconf;
	char *p;

	jconf = recog->jconf;

	if (jconf->input.type == INPUT_WAVEFORM) {
		/* begin A/D input */
		if (adin_begin(recog->adin, file_or_dev_name) == FALSE) {
			return -2;
		}
		/* create A/D-in thread here */
#ifdef HAVE_PTHREAD
		if (recog->adin->enable_thread && ! recog->adin->input_side_segment) {
			if (adin_thread_create(recog) == FALSE) {
				return -2;
			}
		}
#endif
		/* when using adin func, input name should be obtained when called */
	} else {
		switch(jconf->input.speech_input) {
		case SP_MFCMODULE:
			param_init_content(recog->mfcclist->param);
			if (mfc_module_begin(recog->mfcclist) == FALSE) return -2;
			/* when using mfc module func, input name should be obtained when called */
			break;
		case SP_MFCFILE:
			/* read parameter file */
			param_init_content(recog->mfcclist->param);
			if (rdparam(file_or_dev_name, recog->mfcclist->param) == FALSE) {
				jlog("ERROR: error in reading parameter file: %s\n", file_or_dev_name);
				return -1;
			}
			/* check and strip invalid frames */
			if (jconf->preprocess.strip_zero_sample) {
				param_strip_zero(recog->mfcclist->param);
			}
			/* output frame length */
			//callback_exec(CALLBACK_STATUS_PARAM, recog);
			/* store the input filename here */
			strncpy(recog->adin->current_input_name, file_or_dev_name, MAXPATHLEN);
			break;
		default:
			jlog("ERROR: j_open_stream: none of SP_MFC_*??\n");
			return -1;
		}
	}

	if (jconf->input.speech_input != SP_MFCFILE) {
		/* store current input name using input source specific function */
		p = j_get_current_filename(recog);
		if (p) {
			strncpy(recog->adin->current_input_name, p, MAXPATHLEN);
		} else {
			recog->adin->current_input_name[0] = '\0';
		}
	}

	return 0;

}

/** 
* <EN>
* Close input stream.  The main recognition loop will be stopped after
* stream has been closed.
* </EN>
* <JA>
* ²»À¼ÆþÎÏ¥¹¥È¥ê¡¼¥à¤òÊÄ¤¸Ž¤¡¦¥Ç§¼±¤Î¥á¥¤¥ó¥¡¦¼¥×¤ÏÊÄ¤¸¤é¤¡¦¿¸å½ªÎ»¤¹¤¡¦Ž¥
* </JA>
* 
* @param recog [i/o] engine instance
* 
* @return 0 on success, -1 on general error, -2 on device error.
* 
* @callgraph
* @callergraph
* @ingroup engine
*/
int
	j_close_stream(Recog *recog)
{
	Jconf *jconf;

	jconf = recog->jconf;

	if (jconf->input.type == INPUT_WAVEFORM) {
#ifdef HAVE_PTHREAD
		/* close A/D-in thread here */
		if (! recog->adin->input_side_segment) {
			if (recog->adin->enable_thread) {
				if (adin_thread_cancel(recog) == FALSE) {
					return -2;
				}
			} else {
				recog->adin->end_of_stream = TRUE;
			}
		}
#else
		if (! recog->adin->input_side_segment) {
			recog->adin->end_of_stream = TRUE;
		}
#endif
	} else {
		switch(jconf->input.speech_input) {
		case SP_MFCMODULE:
			if (mfc_module_end(recog->mfcclist) == FALSE) return -2;
			break;
		case SP_MFCFILE:
			/* nothing to do */
			break;
		default:
			jlog("ERROR: j_close_stream: none of SP_MFC_*??\n");
			return -1;
		}
	}

	return 0;

}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/** 
* <EN>
* Recognition error handling.
* </EN>
* <JA>
* ¥¨¥é¡¼¤Ë¤èŽ¤¡¦§¼±½ªÎ»»þ¤Î½èÍ¡¦. 
* </JA>
* 
* @param recog [in] engine instance
* @param status [in] error status to be set
* 
*/
static void
	result_error(Recog *recog, int status)
{
	MFCCCalc *mfcc;
	RecogProcess *r;
	boolean ok_p;

	for(r=recog->process_list;r;r=r->next) r->result.status = status;

	ok_p = FALSE;
	for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
		if (mfcc->f > 0) {
			ok_p = TRUE;
			break;
		}
	}
	if (ok_p) {			/* had some input */
		/* output as rejected */
		callback_exec(CALLBACK_RESULT, recog);
#ifdef ENABLE_PLUGIN
		plugin_exec_process_result(recog);
#endif
	}
}

/** 
* <EN>
* @brief  Execute recognition.
*
* This function repeats recognition sequences until the input stream
* reached its end.  It detects speech segment (if needed), recognize
* the detected segment, output result, and go back to the first.
*
* This function will be stopped and exited if reached end of stream
* (mostly in case of file input), some error has been occured, or
* termination requested from application by calling
* j_request_pause() and j_request_terminate().
* 
* </EN>
* <JA>
* @brief  ²»À¼Ç§¼±¤Î¼Â¹Ô. 
*
* ¤³¤Î´Ø¿ô¤ÏÆþÎÏ¥¹¥È¥ê¡¼¥à¤¬½ªŽ¤¡¦¡¦Þ¤Ç²»À¼Ç§¼±¤ò·«¤¡¦Ö¤Ž¹. 
* É¬Í×¤Ç¤¢Ž¤¡¦ÐÆþÎÏÂÔ¤Á¤ò¹Ô¤Ã¤Æ¶è´Ö¤ò¸¡½Ð¤·¡¤²»À¼Ç§¼±¤ò¹Ô¤¤¡¤·¡¦Ì¤¡¦ * ½ÐÎÏ¤·¤Æ¤Õ¤¿¤¿¤ÓÆþÎÏÂÔ¤Á¤ËÌáŽ¤¡¦ 
*
* ÆþÎÏ¥¹¥È¥ê¡¼¥à¤ò½ªŽ¤¡¦ê¤Þ¤ÇÇ§¼±¤¹¤¡¦«¡¤¥¨¥é¡¼¤¬À¸¤¸¤¿¤È¤­¤Ë½ªÎ»¤¹¤¡¦ 
*
* ¤¢Ž¤¡¦¤¤Ï¡¤Ç§¼±½èÍýÃæ¤Ë¡Ž¤j_request_pause() Ž¤¡¦j_request_terminate() ¤¬
* ¥¢¥×¥ê¤«¤é¸Æ¤ÐŽ¤¡¦¿¾¡¦ç¡¤Ç§¼±½èÍý¤ÎÀÚ¤¡¦Ü¤Ç½ªÎ»¤¹¤¡¦ 
* 
* </JA>
* 
* @param recog [i/o] engine instance
* 
* @return 1 when stopped by application request, 0 when reached end of stream,
* or -1 when an error occured.  Note that the input stream can still continues
* when 1 is returned.
* 
*/
static int
	j_recognize_stream_core(Recog *recog)
{
	Jconf *jconf;
	int ret;
	float seclen, mseclen;
	RecogProcess *r;
	MFCCCalc *mfcc;
	PROCESS_AM *am;
	PROCESS_LM *lm;
	boolean ok_p;
	boolean process_segment_last;
	boolean on_the_fly;
	boolean pass2_p;

	jconf = recog->jconf;

	/* determine whether on-the-fly decoding should be done */
	on_the_fly = FALSE;
	switch(jconf->input.type) {
	case INPUT_VECTOR:
		switch(jconf->input.speech_input) {
		case SP_MFCFILE: 
			on_the_fly = FALSE;
			break;
		case SP_MFCMODULE:
			on_the_fly = TRUE;
			break;
		}
		break;
	case INPUT_WAVEFORM:
		if (jconf->decodeopt.realtime_flag) {
			on_the_fly = TRUE;
		} else {
			on_the_fly = FALSE;
		}
		break;
	}

	if (jconf->input.type == INPUT_WAVEFORM || jconf->input.speech_input == SP_MFCMODULE) {
		for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
			param_init_content(mfcc->param);
		}
	}

	/* if no process instance exist, start with terminated */
	if (recog->process_list == NULL) {
		jlog("STAT: no recog process, engine inactive\n");
		j_request_pause(recog);
	}

	/* update initial recognition process status */
	for(r=recog->process_list;r;r=r->next) {
		if (r->active > 0) {
			r->live = TRUE;
		} else if (r->active < 0) {
			r->live = FALSE;
		}
		r->active = 0;
	}

	/******************************************************************/
	/* do recognition for each incoming segment from the input stream */
	/******************************************************************/
	while (1) {

start_recog:

		/*************************************/
		/* Update recognition process status */
		/*************************************/
		for(r=recog->process_list;r;r=r->next) {
			if (r->active > 0) {
				r->live = TRUE;
				jlog("STAT: SR%02d %s now active\n", r->config->id, r->config->name);
			} else if (r->active < 0) {
				r->live = FALSE;
				jlog("STAT: SR%02d %s now inactive\n", r->config->id, r->config->name);
			}
			r->active = 0;
		}
		if (debug2_flag) {
			for(r=recog->process_list;r;r=r->next) {
				jlog("DEBUG: %s: SR%02d %s\n", r->live ? "live" : "dead", r->config->id, r->config->name);
			}
		}
		/* check if any process is live */
		if (recog->process_active) {
			ok_p = FALSE;
			for(r=recog->process_list;r;r=r->next) {
				if (r->live) ok_p = TRUE;
			}
			if (!ok_p) {		/* no process is alive */
				/* make whole process as inactive */
				jlog("STAT: all recog process inactive, pause engine now\n");
				j_request_pause(recog);
			}
		}

		/* Check whether process status was changed while in the last run */
		if (recog->process_online != recog->process_active) {
			recog->process_online = recog->process_active;
//			if (recog->process_online) callback_exec(CALLBACK_EVENT_PROCESS_ONLINE, recog);
//			else callback_exec(CALLBACK_EVENT_PROCESS_OFFLINE, recog);
		}
		/* execute poll callback */
//		if (recog->process_active) {
//			callback_exec(CALLBACK_POLL, recog);
//		}
		/* reset reload flag here */
		j_reset_reload(recog);

		if (!recog->process_active) {
			/* now sleeping, return */
			/* in the next call, we will resume from here */
			return 1;
		}
		/* update process status */
		if (recog->process_online != recog->process_active) {
			recog->process_online = recog->process_active;
		}

		/*********************************************************/
		/* check for grammar to change, and rebuild if necessary */
		/*********************************************************/
		for(lm=recog->lmlist;lm;lm=lm->next) {
			if (lm->lmtype == LM_DFA) {
				multigram_update(lm); /* some modification occured if return TRUE*/
			}
		}
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			if (r->lmtype == LM_DFA && r->lm->global_modified) {
				multigram_build(r);
			}
		}
		for(lm=recog->lmlist;lm;lm=lm->next) {
			if (lm->lmtype == LM_DFA) lm->global_modified = FALSE;
		}

		ok_p = FALSE;
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			if (r->lmtype == LM_DFA) {
				if (r->lm->winfo == NULL ||
					(r->lmvar == LM_DFA_GRAMMAR && r->lm->dfa == NULL)) {
						/* make this instance inactive */
						r->active = -1;
						ok_p = TRUE;
				}
			}
		}
		if (ok_p) {			/* at least one instance has no grammar */
			goto start_recog;
		}


		/******************/
		/* start 1st pass */
		/******************/
		if (on_the_fly) {

			/********************************************/
			/* REALTIME ON-THE-FLY DECODING OF 1ST-PASS */
			/********************************************/
			/* store, analysis and search in a pipeline  */
			/* main function is RealTimePipeLine() at realtime-1stpass.c, and
			it will be periodically called for each incoming input segment
			from the AD-in function adin_go().  RealTimePipeLine() will be
			called as a callback function from adin_go() */
			/* after this part, directly jump to the beginning of the 2nd pass */

			if (recog->process_segment) {
				/*****************************************************************/
				/* short-pause segmentation: process last remaining frames first */
				/*****************************************************************/
				/* last was segmented by short pause */
				/* the margin segment in the last input will be re-processed first,
				and then the speech input will be processed */
				/* process the last remaining parameters */
				ret = RealTimeResume(recog);
				if (ret < 0) {		/* error end in the margin */
					jlog("ERROR: failed to process last remaining samples on RealTimeResume\n"); /* exit now! */
					return -1;
				}
				if (ret != 1) {	/* if segmented again in the margin, not process the rest */
					/* last parameters has been processed, so continue with the
					current input as normal */
					/* process the incoming input */
					if (jconf->input.type == INPUT_WAVEFORM) {
						/* get speech and process it on real-time */
						ret = adin_go(RealTimePipeLine, callback_check_in_adin, recog);
					} else {
						/* get feature vector and process it */
						ret = mfcc_go(recog, callback_check_in_adin);
					}
					if (ret < 0) {		/* error end in adin_go */
						if (ret == -2 || recog->process_want_terminate) {
							/* terminated by callback */
							RealTimeTerminate(recog);
							/* reset param */
							for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
								param_init_content(mfcc->param);
							}
							/* execute callback at end of pass1 */
							if (recog->triggered) {
								/* output result terminate */
								result_error(recog, J_RESULT_STATUS_TERMINATE);
							}
							goto end_recog; /* cancel this recognition */
						}
						jlog("ERROR: an error occured at on-the-fly 1st pass decoding\n");          /* exit now! */
						return(-1);
					}
				}

			} else {

				/***********************************************************/
				/* last was not segmented, process the new incoming input  */
				/***********************************************************/
				/* end of this input will be determined by either end of stream
				(in case of file input), or silence detection by adin_go(), or
				'TERMINATE' command from module (if module mode) */
				/* prepare work area for on-the-fly processing */
				if (RealTimePipeLinePrepare(recog) == FALSE) {
					jlog("ERROR: failed to prepare for on-the-fly 1st pass decoding\n");
					return (-1);
				}
				/* process the incoming input */
				if (jconf->input.type == INPUT_WAVEFORM) {
					/* get speech and process it on real-time */
					ret = adin_go(RealTimePipeLine, callback_check_in_adin, recog);
				} else {
					/* get feature vector and process it */
					ret = mfcc_go(recog, callback_check_in_adin);
				}

				if (ret < 0) {		/* error end in adin_go */
					if (ret == -2 || recog->process_want_terminate) {	
						/* terminated by callback */
						RealTimeTerminate(recog);
						/* reset param */
						for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
							param_init_content(mfcc->param);
						}
						/* execute callback at end of pass1 */
						if (recog->triggered) {
							/* output result terminate */
							result_error(recog, J_RESULT_STATUS_TERMINATE);
						}
						goto end_recog;
					}
					jlog("ERROR: an error occured at on-the-fly 1st pass decoding\n");          /* exit now! */
					return(-1);
				}
			}
			/******************************************************************/
			/* speech stream has been processed on-the-fly, and 1st pass ends */
			/******************************************************************/
			if (ret == 1 || ret == 2) {		/* segmented */
#ifdef HAVE_PTHREAD
				/* check for audio overflow */
				if (recog->adin->enable_thread && recog->adin->adinthread_buffer_overflowed) {
					jlog("Warning: input buffer overflow: some input may be dropped, so disgard the input\n");
					result_error(recog, J_RESULT_STATUS_BUFFER_OVERFLOW);
					/* skip 2nd pass */
					goto end_recog;
				}
#endif
				/* check for long input */
				for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
					if (mfcc->f >= recog->real.maxframelen) {
						jlog("Warning: MFCC too long input (> %d frames), segment it now\n", recog->real.maxframelen);
					}
				}
			}
			/* last procedure of 1st-pass */
			if (RealTimeParam(recog) == FALSE) {
				jlog("ERROR: fatal error occured, program terminates now\n");
				return -1;
			}

#ifdef BACKEND_VAD
			/* if not triggered, skip this segment */
			if (recog->jconf->decodeopt.segment && ! recog->triggered) {
				goto end_recog;
			}
#endif

			/* output segment status */
			if (recog->adin->adin_cut_on && (jconf->input.speech_input == SP_RAWFILE || jconf->input.speech_input == SP_STDIN || jconf->input.speech_input == SP_MEMORY)) {
				seclen = (float)recog->adin->last_trigger_sample / (float)jconf->input.sfreq;
				jlog("STAT: triggered: [%d..%d] %.2fs from %02d:%02d:%02.2f\n", recog->adin->last_trigger_sample, recog->adin->last_trigger_sample + recog->adin->last_trigger_len, (float)(recog->adin->last_trigger_len) / (float)jconf->input.sfreq, (int)(seclen / 3600), (int)(seclen / 60), seclen - (int)(seclen / 60) * 60);
			}

			/* if terminate signal has been received, discard this input */
			if (recog->process_want_terminate) {
				result_error(recog, J_RESULT_STATUS_TERMINATE);
				goto end_recog;
			}

			/* END OF ON-THE-FLY INPUT AND DECODING OF 1ST PASS */

		} else {

			/******************/
			/* buffered input */
			/******************/

			if (jconf->input.type == INPUT_VECTOR) {
				/***********************/
				/* feature vector input */
				/************************/
				if (jconf->input.speech_input == SP_MFCFILE) {
					/************************/
					/* parameter file input */
					/************************/
					/* parameter type check --- compare the type to that of HMM,
					and adjust them if necessary */
					if (jconf->input.paramtype_check_flag) {
						for(am=recog->amlist;am;am=am->next) {
							/* return param itself or new malloced param */
							if (param_check_and_adjust(am->hmminfo, am->mfcc->param, verbose_flag) == -1) {	/* failed */

								for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
									param_init_content(mfcc->param);
								}
								/* tell failure */
								result_error(recog, J_RESULT_STATUS_FAIL);
								goto end_recog;
							}
						}
					}
					/* whole input is already read, so set input status to end of stream */
					/* and jump to the start point of 1st pass */
					ret = 0;
				}
			} else {
				/*************************/
				/* buffered speech input */
				/*************************/
				if (!recog->process_segment) { /* no segment left */

					/****************************************/
					/* store raw speech samples to speech[] */
					/****************************************/
					recog->speechlen = 0;
					for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
						param_init_content(mfcc->param);
					}
					/* tell module to start recording */
					/* the "adin_cut_callback_store_buffer" simply stores
					the input speech to a buffer "speech[]" */
					/* end of this input will be determined by either end of stream
					(in case of file input), or silence detection by adin_go(), or
					'TERMINATE' command from module (if module mode) */
					ret = adin_go(adin_cut_callback_store_buffer, callback_check_in_adin, recog);
					if (ret < 0) {		/* error end in adin_go */
						if (ret == -2 || recog->process_want_terminate) {
							/* terminated by module */
							/* output fail */
							result_error(recog, J_RESULT_STATUS_TERMINATE);
							goto end_recog;
						}
						jlog("ERROR: an error occured while recording input\n");
						return -1;
					}

					/* output recorded length */
					seclen = (float)recog->speechlen / (float)jconf->input.sfreq;
					jlog("STAT: %d samples (%.2f sec.)\n", recog->speechlen, seclen);

					/* -rejectshort »ØŽÄ¡¦¡¦, ÆþÎÏ¤¬»ØŽÄ¡¦þ´Ö°Ê²¼¤Ç¤¢¤¡¦ŽÐ
					¤³¤³¤ÇÆþÎÏ¤ò´þµÑ¤¹Ž¤¡¦*/
					/* when using "-rejectshort", and input was shorter than
					specified, reject the input here */
					if (jconf->reject.rejectshortlen > 0) {
						if (seclen * 1000.0 < jconf->reject.rejectshortlen) {
							result_error(recog, J_RESULT_STATUS_REJECT_SHORT);
							goto end_recog;
						}
					}

					/**********************************************/
					/* acoustic analysis and encoding of speech[] */
					/**********************************************/
					jlog("STAT: ### speech analysis (waveform -> MFCC)\n");
					/* CMN will be computed for the whole buffered input */
					if (wav2mfcc(recog->speech, recog->speechlen, recog) == FALSE) {
						/* error end, end stream */
						ret = -1;
						/* tell failure */
						result_error(recog, J_RESULT_STATUS_FAIL);
						goto end_recog;
					}

					/* if terminate signal has been received, cancel this input */
					if (recog->process_want_terminate) {
						result_error(recog, J_RESULT_STATUS_TERMINATE);
						goto end_recog;
					}

				}
			}

			/******************************************************/
			/* 1st-pass --- backward search to compute heuristics */
			/******************************************************/
			{
				/* prepare for outprob cache for each HMM state and time frame */
				/* assume all MFCCCalc has params of the same sample num */
				for(am=recog->amlist;am;am=am->next) {
					outprob_prepare(&(am->hmmwrk), am->mfcc->param->samplenum);
				}
			}

			/* if terminate signal has been received, cancel this input */
			if (recog->process_want_terminate) {
				result_error(recog, J_RESULT_STATUS_TERMINATE);
				goto end_recog;
			}

			/* execute computation of left-to-right backtrellis */
			if (get_back_trellis(recog) == FALSE) {
				jlog("ERROR: fatal error occured, program terminates now\n");
				return -1;
			}


			/* END OF BUFFERED 1ST PASS */

		}

		/**********************************/
		/* end processing of the 1st-pass */
		/**********************************/
		/* on-the-fly 1st pass processing will join here */

		/* -rejectshort »ØŽÄ¡¦¡¦, ÆþÎÏ¤¬»ØŽÄ¡¦þ´Ö°Ê²¼¤Ç¤¢¤¡¦ÐÃµº÷¼ºÇÔ¤È¤·¤ŽÆ */
		/* Âè£²¥Ñ¥¹¤ò¼Â¹Ô¤»¤º¤Ë¤³¤³¤Ç½ªÎ»¤¹Ž¤¡¦*/
		/* when using "-rejectshort", and input was shorter than the specified
		length, terminate search here and output recognition failure */
		if (jconf->reject.rejectshortlen > 0) {
			mseclen = (float)recog->mfcclist->param->samplenum * (float)jconf->input.period * (float)jconf->input.frameshift / 10000.0;
			if (mseclen < jconf->reject.rejectshortlen) {
				result_error(recog, J_RESULT_STATUS_REJECT_SHORT);
				goto end_recog;
			}
		}
#ifdef POWER_REJECT
		if (power_reject(recog)) {
			result_error(recog, J_RESULT_STATUS_REJECT_POWER);
			goto end_recog;
		}
#endif

		/* if terminate signal has been received, cancel this input */
		if (recog->process_want_terminate) {
			result_error(recog, J_RESULT_STATUS_TERMINATE);
			goto end_recog;
		}

		/* if GMM is specified and result are to be rejected, terminate search here */
		if (jconf->reject.gmm_reject_cmn_string != NULL) {
			if (! gmm_valid_input(recog)) {
				result_error(recog, J_RESULT_STATUS_REJECT_GMM);
				goto end_recog;
			}
		}

		/* for instances with "-1pass", copy 1st pass result as final */
		/* execute stack-decoding search */
		/* they will be skipepd in the next pass */
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			/* skip if 1st pass was failed */
			if (r->result.status < 0) continue;
			/* already stored on word recognition, so skip this */
			if (r->lmvar == LM_DFA_WORD) continue;
			if (r->config->compute_only_1pass) {
				if (verbose_flag) {
					jlog("%02d %s: \"-1pass\" specified, output 1st pass result as a final result\n", r->config->id, r->config->name);
				}
				/* prepare result storage */
				result_sentence_malloc(r, 1);
				/* finalize result when no hypothesis was obtained */
				pass2_finalize_on_no_result(r, TRUE);
			}
		}

		/***********************************************/
		/* 2nd-pass --- forward search with heuristics */
		/***********************************************/

#if !defined(PASS2_STRICT_IWCD) || defined(FIX_35_PASS2_STRICT_SCORE)    
		/* adjust trellis score not to contain outprob of the last frames */
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			/* if [-1pass] is specified, skip 2nd pass */
			if (r->config->compute_only_1pass) continue;
			/* if search already failed on 1st pass, skip 2nd pass */
			if (r->result.status < 0) continue;
			if (! r->am->hmminfo->multipath) {
				bt_discount_pescore(r->wchmm, r->backtrellis, r->am->mfcc->param);
			}
#ifdef LM_FIX_DOUBLE_SCORING
			if (r->lmtype == LM_PROB) {
				bt_discount_lm(r->backtrellis);
			}
#endif
		}
#endif

		/* execute stack-decoding search */
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			/* if [-1pass] is specified, just copy from 1st pass result */
			if (r->config->compute_only_1pass) continue;
			/* if search already failed on 1st pass, skip 2nd pass */
			if (r->result.status < 0) continue;
			/* prepare result storage */
			if (r->lmtype == LM_DFA && r->config->output.multigramout_flag) {
				result_sentence_malloc(r, r->config->output.output_hypo_maxnum * multigram_get_all_num(r->lm));
			} else {
				result_sentence_malloc(r, r->config->output.output_hypo_maxnum);
			}
			/* do 2nd pass */
			if (r->lmtype == LM_PROB) {
				wchmm_fbs(r->am->mfcc->param, r, 0, 0);
			} else if (r->lmtype == LM_DFA) {
				if (r->config->output.multigramout_flag) {
					/* execute 2nd pass multiple times for each grammar sequencially */
					/* to output result for each grammar */
					MULTIGRAM *m;
					boolean has_success = FALSE;
					for(m = r->lm->grammars; m; m = m->next) {
						if (m->active) {
							jlog("STAT: execute 2nd pass limiting words for gram #%d\n", m->id);
							wchmm_fbs(r->am->mfcc->param, r, m->cate_begin, m->dfa->term_num);
							if (r->result.status == J_RESULT_STATUS_SUCCESS) {
								has_success = TRUE;
							}
						}
					}
					r->result.status = (has_success == TRUE) ? J_RESULT_STATUS_SUCCESS : J_RESULT_STATUS_FAIL;
				} else {
					/* only the best among all grammar will be output */
					wchmm_fbs(r->am->mfcc->param, r, 0, r->lm->dfa->term_num);
				}
			}
		}

		/* do forced alignment if needed */
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			/* if search failed on 2nd pass, skip this */
			if (r->result.status < 0) continue;
			/* do needed alignment */
			do_alignment_all(r, r->am->mfcc->param);
		}

		/* output result */
		callback_exec(CALLBACK_RESULT, recog);

	
		/* clear work area for output */
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			clear_result(r);
		}

#ifdef DEBUG_VTLN_ALPHA_TEST
		if (r->am->mfcc->para->vtln_alpha == 1.0) {
			/* if vtln parameter remains default, search for VTLN parameter */
			vtln_alpha(recog, r);
		}
#endif

end_recog:
		/**********************/
		/* end of recognition */
		/**********************/

		/* update CMN info for next input (in case of realtime wave input) */
		if (jconf->input.type == INPUT_WAVEFORM && jconf->decodeopt.realtime_flag) {
			for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
				if (mfcc->param->samplenum > 0) {
					RealTimeCMNUpdate(mfcc, recog);
				}
			}
		}

		process_segment_last = recog->process_segment;
		if (jconf->decodeopt.segment) { /* sp-segment mode */
			/* param is now shrinked to hold only the processed input, and */
			/* the rests are holded in (newly allocated) "rest_param" */
			/* if this is the last segment, rest_param is NULL */
			/* assume all segmentation are synchronized */
			recog->process_segment = FALSE;
			for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
				if (mfcc->rest_param != NULL) {
					/* process the rest parameters in the next loop */
					recog->process_segment = TRUE;
					free_param(mfcc->param);
					mfcc->param = mfcc->rest_param;
					mfcc->rest_param = NULL;
				}
			}
		}



		if (verbose_flag) jlog("\n");
		jlog_flush();

		if (jconf->decodeopt.segment) { /* sp-segment mode */
			if (recog->process_segment == TRUE) {
				if (verbose_flag) jlog("STAT: <<<restart the rest>>>\n\n");
			} else {
				/* input has reached end of stream, terminate program */
				if (ret <= 0 && ret != -2) break;
			}
		} else {			/* not sp-segment mode */
			/* input has reached end of stream, terminate program */
			if (ret <= 0 && ret != -2) break;
		}

		/* recognition continues for next (silence-aparted) segment */

	} /* END OF STREAM LOOP */

	/* close the stream */
	if (jconf->input.type == INPUT_WAVEFORM) {
		if (adin_end(recog->adin) == FALSE) return -1;
	}
	if (jconf->input.speech_input == SP_MFCMODULE) {
		if (mfc_module_end(recog->mfcclist) == FALSE) return -1;
	}

	/* return to the opening of input stream */

	return(0);

}

/** 
* <EN>
* @brief  Recognize an input stream.
*
* This function repeat recognition process for the whole input stream,
* using segmentation and detection if required.  It ends when the
* whole input has been processed.
*
* When a recognition stop is requested from application, the following
* callbacks will be called in turn: CALLBACK_EVENT_PAUSE,
* CALLBACK_PAUSE_FUNCTION, CALLBACK_EVENT_RESUME.  After finishing executing
* all functions in these callbacks, recognition will restart.
* If you have something to be processed while recognition stops,
* you should write the function as callback to CALLBACK_PAUSE_FUNCTION.
* Note that recognition will restart immediately after all functions
* registered in CALLBACK_PAUSE_FUNCTION has been finished.
* 
* </EN>
* <JA>
* @brief  ÆþÎÏ¥¹¥È¥ê¡¼¥à¤ÎÇ§¼±¤ò¹Ô¤¦
*
* ÆþÎÏ¥¹¥È¥ê¡¼¥à¤ËÂÐ¤·¤Æ
* ¡ÊÉ¬Í×¤Ç¤¢Ž¤¡¦Ð¡Ë¶è´Ö¸¡½Ð¤ç·AD¤ò¹Ô¤¤¤Ê¤¬¤éÇ§¼±¤ò·«Ž¤¡¦Ö¤·¹Ô¤Ã¤Æ¤¤¤Ž¯. 
* ÆþÎÏ¤¬½ªÃ¼¤ËÃ£¤¹Ž¤¡¦«¤¢¤¡¦¤¤Ï¥¨¥é¡¼¤Ç½ªÎ»¤¹¤¡¦ 
*
* ¥¢¥×Ž¥¡¦±¡¼¥·¥ç¥ó¤«¤éÇ§¼±¤ÎÃæÃÇ¤ò¥¡¦¯¥¨¥¹¥È¤µ¤¡¦¿¤È¤­¤Ï¡Ž¤
* CALLBACK_EVENT_PAUSE¡¤CALLBACK_PAUSE_FUNCTION,
* CALLBACK_EVENT_RESUME ¤Î½ç¤Ë¸Æ¤ó¤À¤¢¤ÈÇ§¼±¤ËÌáŽ¤¡¦ ¤³¤Î¤¿¤á¡¤
* Ç§¼±¤òÃæÃÇ¤µ¤»¤Æ¤¤Ž¤¡¦Ö¤Ë¹Ô¤¦½èÍý¤Ï¡Ž¤CALLBACK_PAUSE_FUNCTION
* ¤ËÅÐÏ¿¤·¤Æ¤ª¤¯É¬Í×¤¬¤¢Ž¤¡¦ CALLBACK_PAUSE_FUNCTION ¤Ë
* ÅÐÏ¿¤µŽ¤¡¦Æ¤¤¤¡¦´¤Æ¤Î½èÍý¤¬½ªÎ»¤·¤¿¤éÇ§¼±¤ò¼«Æ°Åª¤ËºÆ³«¤¹¤¡¦Î¤ŽÇ
* ÃúÌÕ¤¹Ž¤¡¦³¤ŽÈ. 
* 
* </JA>
* 
* @param recog [i/o] engine instance
* 
* @return 0 when finished recognizing all the input stream to the end,
* or -1 on error.
* 
* @callgraph
* @callergraph
* @ingroup engine
*/
int
	j_recognize_stream(Recog *recog)
{
	int ret;

	do {

		ret = j_recognize_stream_core(recog);

		switch(ret) {
		case 1:	      /* paused by a callback (stream will continue) */
			/* call pause event callbacks */
			//callback_exec(CALLBACK_EVENT_PAUSE, recog);
			/* call pause functions */
			/* block until all pause functions exits */
			if (! callback_exist(recog, CALLBACK_PAUSE_FUNCTION)) {
				jlog("WARNING: pause requested but no pause function specified\n");
				jlog("WARNING: engine will resume now immediately\n");
			}
			//callback_exec(CALLBACK_PAUSE_FUNCTION, recog);
			/* after here, recognition will restart for the rest input */
			/* call resume event callbacks */
			//callback_exec(CALLBACK_EVENT_RESUME, recog);
			break;
		case 0:			/* end of stream */
			/* go on to the next input */
			break;
		case -1: 		/* error */
			jlog("ERROR: an error occured while recognition, terminate stream\n");
			return -1;
		}
	} while (ret == 1);		/* loop when paused by callback */

	return 0;
}


/** 
* <EN>
* @brief  Execute recognition.
*
* This function repeats recognition sequences until the input stream
* reached its end.  It detects speech segment (if needed), recognize
* the detected segment, output result, and go back to the first.
*
* This function will be stopped and exited if reached end of stream
* (mostly in case of file input), some error has been occured, or
* termination requested from application by calling
* j_request_pause() and j_request_terminate().
* 
* </EN>
* <JA>
* @brief  ²»À¼Ç§¼±¤Î¼Â¹Ô. 
*
* ¤³¤Î´Ø¿ô¤ÏÆþÎÏ¥¹¥È¥ê¡¼¥à¤¬½ªŽ¤¡¦¡¦Þ¤Ç²»À¼Ç§¼±¤ò·«¤¡¦Ö¤Ž¹. 
* É¬Í×¤Ç¤¢Ž¤¡¦ÐÆþÎÏÂÔ¤Á¤ò¹Ô¤Ã¤Æ¶è´Ö¤ò¸¡½Ð¤·¡¤²»À¼Ç§¼±¤ò¹Ô¤¤¡¤·¡¦Ì¤¡¦ * ½ÐÎÏ¤·¤Æ¤Õ¤¿¤¿¤ÓÆþÎÏÂÔ¤Á¤ËÌáŽ¤¡¦ 
*
* ÆþÎÏ¥¹¥È¥ê¡¼¥à¤ò½ªŽ¤¡¦ê¤Þ¤ÇÇ§¼±¤¹¤¡¦«¡¤¥¨¥é¡¼¤¬À¸¤¸¤¿¤È¤­¤Ë½ªÎ»¤¹¤¡¦ 
*
* ¤¢Ž¤¡¦¤¤Ï¡¤Ç§¼±½èÍýÃæ¤Ë¡Ž¤j_request_pause() Ž¤¡¦j_request_terminate() ¤¬
* ¥¢¥×¥ê¤«¤é¸Æ¤ÐŽ¤¡¦¿¾¡¦ç¡¤Ç§¼±½èÍý¤ÎÀÚ¤¡¦Ü¤Ç½ªÎ»¤¹¤¡¦ 
* 
* </JA>
* 
* @param recog [i/o] engine instance
* 
* @return 1 when stopped by application request, 0 when reached end of stream,
* or -1 when an error occured.  Note that the input stream can still continues
* when 1 is returned.
* 
*/
static int
	j_recognize_stream_core_naichichi(Recog *recog)
{
	Jconf *jconf;
	int ret;
	float seclen, mseclen;
	RecogProcess *r;
	MFCCCalc *mfcc;
	PROCESS_AM *am;
	PROCESS_LM *lm;
	boolean ok_p;
	boolean process_segment_last;
	boolean pass2_p;

	jconf = recog->jconf;

	if (jconf->input.type == INPUT_WAVEFORM || jconf->input.speech_input == SP_MFCMODULE) {
		for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
			param_init_content(mfcc->param);
		}
	}

	/* if no process instance exist, start with terminated */
	if (recog->process_list == NULL) {
		jlog("STAT: no recog process, engine inactive\n");
		j_request_pause(recog);
	}

	/* update initial recognition process status */
	for(r=recog->process_list;r;r=r->next) {
		if (r->active > 0) {
			r->live = TRUE;
		} else if (r->active < 0) {
			r->live = FALSE;
		}
		r->active = 0;
	}

	/******************************************************************/
	/* do recognition for each incoming segment from the input stream */
	/******************************************************************/
	while (1) {

start_recog:

		/*************************************/
		/* Update recognition process status */
		/*************************************/
		for(r=recog->process_list;r;r=r->next) {
			if (r->active > 0) {
				r->live = TRUE;
				jlog("STAT: SR%02d %s now active\n", r->config->id, r->config->name);
			} else if (r->active < 0) {
				r->live = FALSE;
				jlog("STAT: SR%02d %s now inactive\n", r->config->id, r->config->name);
			}
			r->active = 0;
		}
		/* check if any process is live */
		if (recog->process_active) {
			ok_p = FALSE;
			for(r=recog->process_list;r;r=r->next) {
				if (r->live) ok_p = TRUE;
			}
			if (!ok_p) {		/* no process is alive */
				/* make whole process as inactive */
				jlog("STAT: all recog process inactive, pause engine now\n");
				j_request_pause(recog);
			}
		}

		/* Check whether process status was changed while in the last run */
		if (recog->process_online != recog->process_active) {
			recog->process_online = recog->process_active;
		}
		/* reset reload flag here */
		j_reset_reload(recog);

		if (!recog->process_active) {
			/* now sleeping, return */
			/* in the next call, we will resume from here */
			return 1;
		}
		/* update process status */
		if (recog->process_online != recog->process_active) {
			recog->process_online = recog->process_active;
		}

		/*********************************************************/
		/* check for grammar to change, and rebuild if necessary */
		/*********************************************************/
		for(lm=recog->lmlist;lm;lm=lm->next) {
			if (lm->lmtype == LM_DFA) {
				multigram_update(lm); /* some modification occured if return TRUE*/
			}
		}
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			if (r->lmtype == LM_DFA && r->lm->global_modified) {
				multigram_build(r);
			}
		}
		for(lm=recog->lmlist;lm;lm=lm->next) {
			if (lm->lmtype == LM_DFA) lm->global_modified = FALSE;
		}

		ok_p = FALSE;
		for(r=recog->process_list;r;r=r->next) {
			if (!r->live) continue;
			if (r->lmtype == LM_DFA) {
				if (r->lm->winfo == NULL ||
					(r->lmvar == LM_DFA_GRAMMAR && r->lm->dfa == NULL)) {
						/* make this instance inactive */
						r->active = -1;
						ok_p = TRUE;
				}
			}
		}
		if (ok_p) {			/* at least one instance has no grammar */
			goto start_recog;
		}


		/******************/
		/* start 1st pass */
		/******************/
		{

			/********************************************/
			/* REALTIME ON-THE-FLY DECODING OF 1ST-PASS */
			/********************************************/
			/* store, analysis and search in a pipeline  */
			/* main function is RealTimePipeLine() at realtime-1stpass.c, and
			it will be periodically called for each incoming input segment
			from the AD-in function adin_go().  RealTimePipeLine() will be
			called as a callback function from adin_go() */
			/* after this part, directly jump to the beginning of the 2nd pass */

			if (recog->process_segment) {
				/*****************************************************************/
				/* short-pause segmentation: process last remaining frames first */
				/*****************************************************************/
				/* last was segmented by short pause */
				/* the margin segment in the last input will be re-processed first,
				and then the speech input will be processed */
				/* process the last remaining parameters */
				ret = RealTimeResume(recog);
				if (ret < 0) {		/* error end in the margin */
					jlog("ERROR: failed to process last remaining samples on RealTimeResume\n"); /* exit now! */
					return -1;
				}
				if (ret != 1) {	/* if segmented again in the margin, not process the rest */
					/* last parameters has been processed, so continue with the
					current input as normal */
					/* process the incoming input */
					/* get speech and process it on real-time */
					ret = adin_go(RealTimePipeLine, callback_check_in_adin, recog);

					if (ret < 0) {		/* error end in adin_go */
						if (ret == -2 || recog->process_want_terminate) {
							/* terminated by callback */
							RealTimeTerminate(recog);
							/* reset param */
							for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
								param_init_content(mfcc->param);
							}
							/* execute callback at end of pass1 */
							if (recog->triggered) {
								/* output result terminate */
								result_error(recog, J_RESULT_STATUS_TERMINATE);
							}
							goto end_recog; /* cancel this recognition */
						}
						jlog("ERROR: an error occured at on-the-fly 1st pass decoding\n");          /* exit now! */
						return(-1);
					}
				}

			} else {

				/***********************************************************/
				/* last was not segmented, process the new incoming input  */
				/***********************************************************/
				/* end of this input will be determined by either end of stream
				(in case of file input), or silence detection by adin_go(), or
				'TERMINATE' command from module (if module mode) */
				/* prepare work area for on-the-fly processing */
				if (RealTimePipeLinePrepare(recog) == FALSE) {
					jlog("ERROR: failed to prepare for on-the-fly 1st pass decoding\n");
					return (-1);
				}
				/* process the incoming input */
				ret = adin_go(RealTimePipeLine, callback_check_in_adin, recog);

				if (ret < 0) {		/* error end in adin_go */
					if (ret == -2 || recog->process_want_terminate) {	
						/* terminated by callback */
						RealTimeTerminate(recog);
						/* reset param */
						for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
							param_init_content(mfcc->param);
						}
						/* execute callback at end of pass1 */
						if (recog->triggered) {
							/* output result terminate */
							result_error(recog, J_RESULT_STATUS_TERMINATE);
						}
						goto end_recog;
					}
					jlog("ERROR: an error occured at on-the-fly 1st pass decoding\n");          /* exit now! */
					return(-1);
				}
			}
			/******************************************************************/
			/* speech stream has been processed on-the-fly, and 1st pass ends */
			/******************************************************************/
			if (ret == 1 || ret == 2) {		/* segmented */
#ifdef HAVE_PTHREAD
				/* check for audio overflow */
				if (recog->adin->enable_thread && recog->adin->adinthread_buffer_overflowed) {
					jlog("Warning: input buffer overflow: some input may be dropped, so disgard the input\n");
					result_error(recog, J_RESULT_STATUS_BUFFER_OVERFLOW);
					/* skip 2nd pass */
					goto end_recog;
				}
#endif
				/* check for long input */
				for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
					if (mfcc->f >= recog->real.maxframelen) {
						jlog("Warning: MFCC(2) too long input (> %d frames), segment it now\n", recog->real.maxframelen);
						change_count_zc_e(&(recog->adin->zc),1); /* */
						jlogreport(502);
						goto end_recog;
					}
				}
			}
			/* last procedure of 1st-pass */
			if (RealTimeParam(recog) == FALSE) {
				jlog("ERROR: fatal error occured, program terminates now\n");
				return -1;
			}

			/* output segment status */
			/*
			if (recog->adin->adin_cut_on && (jconf->input.speech_input == SP_RAWFILE || jconf->input.speech_input == SP_STDIN || jconf->input.speech_input == SP_MEMORY)) {
				seclen = (float)recog->adin->last_trigger_sample / (float)jconf->input.sfreq;
				jlog("STAT: triggered: [%d..%d] %.2fs from %02d:%02d:%02.2f\n", recog->adin->last_trigger_sample, recog->adin->last_trigger_sample + recog->adin->last_trigger_len, (float)(recog->adin->last_trigger_len) / (float)jconf->input.sfreq, (int)(seclen / 3600), (int)(seclen / 60), seclen - (int)(seclen / 60) * 60);
			}
			*/

			/* if terminate signal has been received, discard this input */
			/*
			if (recog->process_want_terminate) {
				result_error(recog, J_RESULT_STATUS_TERMINATE);
				goto end_recog;
			}
			*/

			/* END OF ON-THE-FLY INPUT AND DECODING OF 1ST PASS */
		}

		/**********************************/
		/* end processing of the 1st-pass */
		/**********************************/
		/* on-the-fly 1st pass processing will join here */

		/* -rejectshort »ØŽÄ¡¦¡¦, ÆþÎÏ¤¬»ØŽÄ¡¦þ´Ö°Ê²¼¤Ç¤¢¤¡¦ÐÃµº÷¼ºÇÔ¤È¤·¤ŽÆ */
		/* Âè£²¥Ñ¥¹¤ò¼Â¹Ô¤»¤º¤Ë¤³¤³¤Ç½ªÎ»¤¹Ž¤¡¦*/
		/* when using "-rejectshort", and input was shorter than the specified
		length, terminate search here and output recognition failure */
		/*
		if (jconf->reject.rejectshortlen > 0) {
			mseclen = (float)recog->mfcclist->param->samplenum * (float)jconf->input.period * (float)jconf->input.frameshift / 10000.0;
			if (mseclen < jconf->reject.rejectshortlen) {
				result_error(recog, J_RESULT_STATUS_REJECT_SHORT);
				goto end_recog;
			}
		}
		*/

		/***********************************************/
		/* 2nd-pass --- forward search with heuristics */
		/***********************************************/
		r=recog->process_list;
		if (!r->live || r->result.status < 0 ){
			goto end_recog;
		}

		/* adjust trellis score not to contain outprob of the last frames */
		{
			bt_discount_pescore(r->wchmm, r->backtrellis, r->am->mfcc->param);
		}

		/* execute stack-decoding search */
		{
			result_sentence_malloc(r, r->config->output.output_hypo_maxnum);
			/* only the best among all grammar will be output */
			wchmm_fbs(r->am->mfcc->param, r, 0, r->lm->dfa->term_num);
		}

		/* do forced alignment if needed */
		{
			/* do needed alignment */
			do_alignment_all(r, r->am->mfcc->param);
		}

		/* output result */
		callback_exec(CALLBACK_RESULT, recog);
	
		/* clear work area for output */
		{
			clear_result(r);
		}

end_recog:
		/**********************/
		/* end of recognition */
		/**********************/

		/* update CMN info for next input (in case of realtime wave input) */
		{
			for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
				if (mfcc->param->samplenum > 0) {
					RealTimeCMNUpdate(mfcc, recog);
				}
			}
		}

		process_segment_last = recog->process_segment;
		if (jconf->decodeopt.segment) { /* sp-segment mode */
			/* param is now shrinked to hold only the processed input, and */
			/* the rests are holded in (newly allocated) "rest_param" */
			/* if this is the last segment, rest_param is NULL */
			/* assume all segmentation are synchronized */
			recog->process_segment = FALSE;
			for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {
				if (mfcc->rest_param != NULL) {
					/* process the rest parameters in the next loop */
					recog->process_segment = TRUE;
					free_param(mfcc->param);
					mfcc->param = mfcc->rest_param;
					mfcc->rest_param = NULL;
				}
			}
		}

		if (jconf->decodeopt.segment) { /* sp-segment mode */
			if (recog->process_segment == TRUE) {
				if (verbose_flag) jlog("STAT: <<<restart the rest>>>\n\n");
			} else {
				/* input has reached end of stream, terminate program */
				if (ret <= 0 && ret != -2) break;
			}
		} else {			/* not sp-segment mode */
			/* input has reached end of stream, terminate program */
			if (ret <= 0 && ret != -2) break;
		}

		/* recognition continues for next (silence-aparted) segment */

	} /* END OF STREAM LOOP */

	/* close the stream */
	if (adin_end(recog->adin) == FALSE) return -1;

	/* return to the opening of input stream */

	return(0);

}

/** 
* <EN>
* @brief  Recognize an input stream.
*
* This function repeat recognition process for the whole input stream,
* using segmentation and detection if required.  It ends when the
* whole input has been processed.
*
* When a recognition stop is requested from application, the following
* callbacks will be called in turn: CALLBACK_EVENT_PAUSE,
* CALLBACK_PAUSE_FUNCTION, CALLBACK_EVENT_RESUME.  After finishing executing
* all functions in these callbacks, recognition will restart.
* If you have something to be processed while recognition stops,
* you should write the function as callback to CALLBACK_PAUSE_FUNCTION.
* Note that recognition will restart immediately after all functions
* registered in CALLBACK_PAUSE_FUNCTION has been finished.
* 
* </EN>
* <JA>
* @brief  ÆþÎÏ¥¹¥È¥ê¡¼¥à¤ÎÇ§¼±¤ò¹Ô¤¦
*
* ÆþÎÏ¥¹¥È¥ê¡¼¥à¤ËÂÐ¤·¤Æ
* ¡ÊÉ¬Í×¤Ç¤¢Ž¤¡¦Ð¡Ë¶è´Ö¸¡½Ð¤ç·AD¤ò¹Ô¤¤¤Ê¤¬¤éÇ§¼±¤ò·«Ž¤¡¦Ö¤·¹Ô¤Ã¤Æ¤¤¤Ž¯. 
* ÆþÎÏ¤¬½ªÃ¼¤ËÃ£¤¹Ž¤¡¦«¤¢¤¡¦¤¤Ï¥¨¥é¡¼¤Ç½ªÎ»¤¹¤¡¦ 
*
* ¥¢¥×Ž¥¡¦±¡¼¥·¥ç¥ó¤«¤éÇ§¼±¤ÎÃæÃÇ¤ò¥¡¦¯¥¨¥¹¥È¤µ¤¡¦¿¤È¤­¤Ï¡Ž¤
* CALLBACK_EVENT_PAUSE¡¤CALLBACK_PAUSE_FUNCTION,
* CALLBACK_EVENT_RESUME ¤Î½ç¤Ë¸Æ¤ó¤À¤¢¤ÈÇ§¼±¤ËÌáŽ¤¡¦ ¤³¤Î¤¿¤á¡¤
* Ç§¼±¤òÃæÃÇ¤µ¤»¤Æ¤¤Ž¤¡¦Ö¤Ë¹Ô¤¦½èÍý¤Ï¡Ž¤CALLBACK_PAUSE_FUNCTION
* ¤ËÅÐÏ¿¤·¤Æ¤ª¤¯É¬Í×¤¬¤¢Ž¤¡¦ CALLBACK_PAUSE_FUNCTION ¤Ë
* ÅÐÏ¿¤µŽ¤¡¦Æ¤¤¤¡¦´¤Æ¤Î½èÍý¤¬½ªÎ»¤·¤¿¤éÇ§¼±¤ò¼«Æ°Åª¤ËºÆ³«¤¹¤¡¦Î¤ŽÇ
* ÃúÌÕ¤¹Ž¤¡¦³¤ŽÈ. 
* 
* </JA>
* 
* @param recog [i/o] engine instance
* 
* @return 0 when finished recognizing all the input stream to the end,
* or -1 on error.
* 
* @callgraph
* @callergraph
* @ingroup engine
*/
int
	j_recognize_stream_naichichi(Recog *recog)
{
	int ret;

	do {

		ret = j_recognize_stream_core_naichichi(recog);

		switch(ret) {
		case 1:	      /* paused by a callback (stream will continue) */
			/* call pause event callbacks */
			//callback_exec(CALLBACK_EVENT_PAUSE, recog);
			/* call pause functions */
			/* block until all pause functions exits */
			if (! callback_exist(recog, CALLBACK_PAUSE_FUNCTION)) {
				jlog("WARNING: pause requested but no pause function specified\n");
				jlog("WARNING: engine will resume now immediately\n");
			}
			//callback_exec(CALLBACK_PAUSE_FUNCTION, recog);
			/* after here, recognition will restart for the rest input */
			/* call resume event callbacks */
			//callback_exec(CALLBACK_EVENT_RESUME, recog);
			break;
		case 0:			/* end of stream */
			/* go on to the next input */
			break;
		case -1: 		/* error */
			jlog("ERROR: an error occured while recognition, terminate stream\n");
			return -1;
		}
	} while (ret == 1);		/* loop when paused by callback */

	return 0;
}



//we need SPEEEEEEEEEEEED!!!!!!!!!!	
int
j_recognize_mfcc_yobikake(Recog *recog,HTK_Param* mfccparam,unsigned int yobikake_startfream,unsigned int yobikake_endfream)
{
	RecogProcess *r;
	MFCCCalc *mfcc;
	PROCESS_AM *am;
	PROCESS_LM *lm;
	int i;
		{

			/******************/
			/* buffered input */
			/******************/


				/*************************/
				/* buffered speech input */
				/*************************/

					/* compute mfcc from speech file for each mfcc instances */
					for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {

						/* set miscellaneous parameters */
						mfcc->param->header.samplenum = mfccparam->samplenum;
						mfcc->param->header.wshift = 0;
						mfcc->param->header.sampsize = mfccparam->veclen * sizeof(VECT); /* not compressed */
						mfcc->param->header.samptype = F_MFCC;
						mfcc->param->veclen = mfccparam->veclen;
						mfcc->param->samplenum = yobikake_endfream-yobikake_startfream;
						mfcc->param->mroot = NULL;
						/* memory share! */
						mfcc->param->parvec = mfccparam->parvec + yobikake_startfream;
					}

			/******************************************************/
			/* 1st-pass --- backward search to compute heuristics */
			/******************************************************/
			{
				/* prepare for outprob cache for each HMM state and time frame */
				/* assume all MFCCCalc has params of the same sample num */
				for(am=recog->amlist;am;am=am->next) {
					outprob_prepare(&(am->hmmwrk), am->mfcc->param->samplenum);
				}
			}

			/* execute computation of left-to-right backtrellis */
			if (get_back_trellis(recog) == FALSE) {
				jlog("ERROR: fatal error occured, program terminates now\n");
				return -1;
			}

		}

		/**********************************/
		/* end processing of the 1st-pass */
		/**********************************/

		/***********************************************/
		/* 2nd-pass --- forward search with heuristics */
		/***********************************************/
		r=recog->process_list;
		if (!r->live || r->result.status < 0 ){
			return -1;
		}


#if !defined(PASS2_STRICT_IWCD) || defined(FIX_35_PASS2_STRICT_SCORE)    
		/* adjust trellis score not to contain outprob of the last frames */
		{
			bt_discount_pescore(r->wchmm, r->backtrellis, r->am->mfcc->param);
		}
#endif

		/* execute stack-decoding search */
		{
			/* prepare result storage */
			result_sentence_malloc(r, r->config->output.output_hypo_maxnum);
			/* do 2nd pass */
			/* only the best among all grammar will be output */
			wchmm_fbs(r->am->mfcc->param, r, 0, r->lm->dfa->term_num);
		}

		/* do forced alignment if needed */
		{
			/* do needed alignment */
			do_alignment_all(r, r->am->mfcc->param);
		}

		/* output result */
		callback_exec(CALLBACK_RESULT, recog);
	
		/* clear work area for output */
		{
			clear_result(r);
		}


		/**********************/
		/* end of recognition */
		/**********************/


	return(0);
}

/* end of file */
