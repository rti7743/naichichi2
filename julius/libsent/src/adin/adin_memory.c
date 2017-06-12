/**
 * @file   adin_memory.c
 *
 * <JA>
 * </JA>
 *
 *
 * 
 */
#include <sent/stddefs.h>
#include <sent/speech.h>
#include <sent/adin.h>

struct adinopt_memory {
	char* g_memory;
	unsigned long g_length;
	int   g_currentPosstion;
};
typedef struct adinopt_memory ADINOPT;

void*
adin_memory_standby(int freq, void *arg)
{
	ADINOPT * adinopt = (ADINOPT*) malloc(sizeof(ADINOPT));
	memset(adinopt,0,sizeof(ADINOPT));

	if (arg == NULL) return adinopt;
	if ( (((int*)arg)[0]) != 0x12341234 ) return NULL;

	adinopt->g_memory = (unsigned char*) (((int*)arg)[1]);
	adinopt->g_length = (unsigned long)  (((int*)arg)[2]);

	return adinopt;
}

/** 
 * @brief  Begin reading audio data from a file.
 *
 * If listfile was specified in adin_file_standby(), the next filename
 * will be read from the listfile.  Otherwise, the
 * filename will be obtained from stdin.  Then the file will be opened here.
 * 
 * @param filename [in] file name to open or NULL for prompt
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_memory_begin(void * adinoptTHIS,char *dummy)
{
	ADINOPT* adinopt = (ADINOPT*)adinoptTHIS;
	//memory 自体は、adin_memory_standby で事前に渡す必要がある.
	if ( adinopt->g_memory == NULL )
	{
		return FALSE;
	}
	adinopt->g_currentPosstion = 0;

	jlog("Stat: adin_memory: input memory length: %d\n", adinopt->g_length);

	return TRUE;
}

/** 
 * Try to read @a sampnum samples and returns actual sample num recorded.
 * 
 * @param buf [out] samples obtained in this function
 * @param sampnum [in] wanted number of samples to be read
 * 
 * @return actural number of read samples, -1 if EOF, -2 if error.
 */
int
adin_memory_read(void * adinoptTHIS,SP16 *buf, int sampnum)
{
	ADINOPT* adinopt = (ADINOPT*)adinoptTHIS;
	unsigned long readsize = sampnum * sizeof(SP16);
	if (adinopt->g_length - adinopt->g_currentPosstion < readsize )
	{
		if (adinopt->g_currentPosstion >= adinopt->g_length)
		{
			return -1; //EOF
		}
		readsize = adinopt->g_length - adinopt->g_currentPosstion;
	}
	memcpy(buf,adinopt->g_memory+adinopt->g_currentPosstion ,readsize);
	adinopt->g_currentPosstion += readsize;
	
	return readsize / sizeof(SP16);
}

/** 
 * End recording.
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_memory_end(void * adinoptTHIS)
{
  /* nothing needed */
  return TRUE;
}

/** 
 * 
 * A tiny function to get current input raw speech file name.
 * 
 * @return string of current input speech file.
 * 
 */
char *
adin_memory_get_current_filename(void * adinoptTHIS)
{
  return "<MEMORY>";
}

