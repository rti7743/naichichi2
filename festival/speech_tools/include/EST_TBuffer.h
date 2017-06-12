 /************************************************************************/
 /*                                                                      */
 /*                Centre for Speech Technology Research                 */
 /*                     University of Edinburgh, UK                      */
 /*                       Copyright (c) 1996,1997                        */
 /*                        All Rights Reserved.                          */
 /*                                                                      */
 /*  Permission is hereby granted, free of charge, to use and distribute */
 /*  this software and its documentation without restriction, including  */
 /*  without limitation the rights to use, copy, modify, merge, publish, */
 /*  distribute, sublicense, and/or sell copies of this work, and to     */
 /*  permit persons to whom this work is furnished to do so, subject to  */
 /*  the following conditions:                                           */
 /*   1. The code must retain the above copyright notice, this list of   */
 /*      conditions and the following disclaimer.                        */
 /*   2. Any modifications must be clearly marked as such.               */
 /*   3. Original authors' names are not deleted.                        */
 /*   4. The authors' names are not used to endorse or promote products  */
 /*      derived from this software without specific prior written       */
 /*      permission.                                                     */
 /*                                                                      */
 /*  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK       */
 /*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     */
 /*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  */
 /*  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE    */
 /*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   */
 /*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  */
 /*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         */
 /*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      */
 /*  THIS SOFTWARE.                                                      */
 /*                                                                      */
 /*************************************************************************/

/**@name EST_TBuffer.h
  * Extending buffers, ie arrays which grow as needed. 
  * I got fed up of writing equivalent code all over the place.
  * 
  * @see EST_TBuffer
  * @author Richard Caley <rjc@cstr.ed.ac.uk>
  * @version $Id: EST_TBuffer.h,v 1.4 2004/09/29 08:24:17 robert Exp $
  */
//@{
#ifndef __EST_TBUFFER_H__
#define __EST_TBUFFER_H__

#include "EST_bool.h"

/// How many chunks of memory to keep around for re-use.
#define TBUFFER_N_OLD (10)

/// Initial size for buffers created with no size specified.
#define TBUFFER_DEFAULT_SIZE 0

/// Amount to increment buffer size by.
#define TBUFFER_DEFAULT_STEP 100

/** Structure to remember old buffers for re-use.
  * @see EST_TBuffer.h 
  */
struct old_tbuffer { void *mem; unsigned int size; };

/// Memory of old buffers
extern struct old_tbuffer EST_old_buffers[TBUFFER_N_OLD];


/** Extending buffer class.
  * <p>
  * This class provides the convenience of arrays which change in size
  * at run time rather more efficiently than the full EST_TVector class
  * would.
  * <p>
  * Buffers can only be expanded and when a buffer is no longer needed
  * (i.e. when the variable goes out of scope) the memory is not deleted,
  * rather it is saved for re-use as a new buffer.
  * <p>
  * A typical use would be a buffer to hold a windowed section of a signal
  * inside a signal processing loop where the size of the window changes from
  * iteration to iteration.
  * 
  * @see EST_TBuffer.h, Associated definitions.
  * @see EST_TVector, class to use for more general uses.
  * @see lpc_analyse, example of use.
  */

template<class TYPE>
class EST_TBuffer {

private:
  /// Pointer to memory.
  TYPE *p_buffer;
  /// Current size.
  unsigned int p_size;
  /// Amount to grow by (if negative it is a percentage).
  int p_step;		

private:
  /// Setup code.
  void init(unsigned int size, int step);
  /// Expand the buffer.
  void expand_to(unsigned int req_size, bool cpy);
  /// Expand and set some of it to a known value.
  void expand_to(unsigned int req_size, const TYPE &set_to, int howmany);
  
public:

  /** Create with size and increment. 
    * Increment can be negative for percentage growth.
    * <p>
    * Tries to use a buffer from EST_old_buffers[] if there is one which
    * is suitable
    * @see EST_old_buffers
    */
  EST_TBuffer(unsigned int size=TBUFFER_DEFAULT_SIZE, int step=TBUFFER_DEFAULT_STEP);

  /// Destructor. Places the memory in EST_old_buffers[] for re-use if there is room.
  ~EST_TBuffer(void);

  /// Current available space.
  unsigned int length(void) const {return p_size;}

  /// Set to the given value. By default sets all values.
  void set(const TYPE &set_to, int howmany=-1);

  /**@name Methods to make sure there is enough space. */
  //@{

  /// Extend if needed, copying existing data.
  void ensure(unsigned int req_size)	
    {if (req_size > p_size) expand_to(req_size, (bool)TRUE);}	

  /// Make sure there is enough space, copying if requested.
  void ensure(unsigned int req_size, bool copy)	
  {if (req_size > p_size) expand_to(req_size, copy,-1);}	

  /// Make sure there is enough space, setting to a known value.
  void ensure(unsigned int req_size, const TYPE &set_to, int howmany=-1)	
    {if (req_size > p_size) expand_to(req_size, set_to, howmany);}	
  //@}

  /**@name Access to the memory itself. */
  //@{

  /// Simple access as a pointer.
  TYPE *b(void) {return p_buffer;}
  /// Read-only access when the EST_TBuffer is a constant
  const TYPE *b(void) const {return p_buffer;}

  /// operator () is simple access
  const TYPE &operator() (unsigned int i) const { return p_buffer[i];}

  TYPE &operator[] (unsigned int i) { return p_buffer[i];}
  const TYPE &operator[] (unsigned int i) const { return p_buffer[i];}

  //@}
};

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "EST_unix.h"

template<class T>
EST_TBuffer<T>::EST_TBuffer(unsigned int size, int step)
{
  p_buffer = NULL;
  init(size, step);
}

template<class T>
EST_TBuffer<T>::~EST_TBuffer(void)
{
  // save the buffer if we have a slot
  for(int i=0; i<TBUFFER_N_OLD; i++)
    if (EST_old_buffers[i].mem == NULL)
      {
	EST_old_buffers[i].mem = p_buffer;
	EST_old_buffers[i].size = p_size*sizeof(T);
	p_buffer = NULL;
	p_size =0;
	break;
      }

  if (p_buffer)
    {
      delete[] p_buffer;
      p_buffer = NULL;
      p_size = 0;
    }
}

template<class T>
void EST_TBuffer<T>::init(unsigned int size, int step)
{
  for(int i=0; i<TBUFFER_N_OLD; i++)
    if (EST_old_buffers[i].size/sizeof(T) >= size)
      {
	p_buffer = (T *)EST_old_buffers[i].mem;
	p_size = EST_old_buffers[i].size/sizeof(T);
	EST_old_buffers[i].mem = NULL;
	EST_old_buffers[i].size = 0;
	break;
      }

  if (p_buffer == NULL)
    {
    p_buffer = new T[size];
    p_size = size;
    }
  p_step = step;
}

template<class T>
void EST_TBuffer<T>::expand_to(unsigned int req_size, bool copy)
{
  if (req_size > p_size)
    {
      unsigned int new_size = p_size;

      while(new_size < req_size)
	if (p_step >0)
	  new_size += p_step;
	else
	  new_size = (int)(new_size*(float)(-p_step)/100.0);

      T * new_buffer = new T[new_size];

      if (copy)
	memcpy(new_buffer, p_buffer, p_size*sizeof(T));

      delete[] p_buffer;
      p_buffer = new_buffer;
      p_size = new_size;
    }
}

template<class T>
void EST_TBuffer<T>::expand_to(unsigned int req_size, const T &set_to, int howmany)
{
  if (req_size > p_size)
    {
      unsigned int new_size = p_size;

      while(new_size < req_size)
	if (p_step >0)
	  new_size += p_step;
	else
	  new_size = (int)(new_size*(float)(-p_step)/100.0);

      T * new_buffer = new T[new_size];

      if (howmany<0)
	howmany=new_size;
      for(int i=0; i<howmany; i++)
	new_buffer[i] = set_to;

      delete[] p_buffer;
      p_buffer = new_buffer;
      p_size = new_size;
    }
}

template<class T>
void EST_TBuffer<T>::set(const T &set_to, int howmany)
{
  if (howmany < 0)
    howmany = p_size;

  for(int i=0; i<howmany; i++)
    p_buffer[i] = set_to;
}


#endif

//@}
