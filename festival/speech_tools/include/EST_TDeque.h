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


#ifndef __EST_TDEQUE_H__
#define __EST_TDEQUE_H__

#include "EST_TVector.h"
#include "instantiate/EST_TDequeI.h"

/** Double ended queue.
  * 
  * @author Richard Caley <rjc@cstr.ed.ac.uk>
  * @version $Id: EST_TDeque.h,v 1.3 2006/07/19 21:52:12 awb Exp $
  */

template <class T>
class EST_TDeque {
private:
  EST_TVector<T> p_vector;
  int p_increment;
  int p_back;
  int p_front;

  // Make the structure bigger.
  void expand(void);

public:
  EST_TDeque(unsigned int capacity, unsigned int increment);
  EST_TDeque(unsigned int capacity);
  EST_TDeque(void);

  /// Used to fill empty spaces when possible.
  static const T *Filler;

  /// Empty it out.
  void clear(void);

  /// Is there anything to get?
  bool is_empty(void) const;

  /// print picture of state. Mostly useful for debugging.
  ostream &print(ostream &s) const;

  /**@name stack
    * 
    * An interface looking like a stack.
    */
  //@{
  void push(T &item);
  T &pop(void);
  T &nth(int i);
  //@}

  /**@name inverse stack
    * 
    * The other end as a stack.
    */
  //@{
  void back_push(T& item);
  T &back_pop(void);
  //@}

  /**@name queue
    * 
    * An interface looking like a queue.
    */
  //@{
  void add(T& item) { push(item); }
  T &next() { return back_pop(); }
  T &last() { return pop(); }
  //@}

  /**@name perl
    * 
    * For people who think in perl
    */
  //@{
  void unshift(T& item) { back_push(item); }
  T &shift() { return back_pop(); }
  //@}

  friend ostream& operator << (ostream &st, const EST_TDeque< T > &deq)
    {
        return deq.print(st);
    }
};

#include "EST_error.h"

template <class T>
EST_TDeque<T>::EST_TDeque(unsigned int capacity, unsigned int increment)
  : p_vector(capacity)
{
  p_increment = increment;
  p_front=0;
  p_back=0;
}

template <class T>
EST_TDeque<T>::EST_TDeque(unsigned int capacity)
 : p_vector(capacity)
{
  p_increment = 10;
  p_front=0;
  p_back=0;
}

template <class T>
EST_TDeque<T>::EST_TDeque()
{
  p_vector.resize(10);
  p_increment = 10;
  p_front=0;
  p_back=0;
}


template <class T>
ostream &EST_TDeque<T>::print(ostream &s) const
{
  s << "{" << p_vector.n() << "|";

  if (p_front >= p_back)
    {
      for(int i0=0; i0<p_back; i0++)
	s << "<>" << "//";
      for(int i=p_back; i<p_front; i++)
	s << p_vector(i) << "//";
      for(int in=p_front; in <p_vector.n(); in++)
	s << "<>" << "//";
    }
  else
    {
      for(int ii=0; ii<p_front; ii++)
	s << p_vector(ii) << "//";
      for(int i0=p_front; i0<p_back; i0++)
	s << "<>" << "//";
      for(int i=p_back; i<p_vector.n(); i++)
	s << p_vector(i) << "//";
    }

  return s << "}";
}

template <class T>
void EST_TDeque<T>::expand()
{
  EST_TVector<T> tmp(p_vector);

  if (p_back==0)
    // special case for pure stack
    p_vector.resize(p_vector.n()+p_increment, true);
  else
    {
      p_vector.resize(p_vector.n()+p_increment, false);
      
      if (p_front >= p_back)
	for(int i=p_back, j=0; i<p_front; i++, j++)
	  p_vector[j] = tmp[i];
      else
	{
	  int j=0;
	  for(int i=p_back; i<tmp.n(); i++, j++)
	    p_vector[j] = tmp[i];
	  
	  for(int ii=0; ii<p_front; ii++, j++)
	    p_vector[j] = tmp[ii];
    
	  p_back=0;
	  p_front=j;
	}
    }
}


template <class T>
bool EST_TDeque<T>::is_empty() const
{
  return p_front==p_back;
}

template <class T>
void EST_TDeque<T>::clear()
{
  p_front=p_back=0;
  for(int i=0; i<p_vector.n(); i++)
    p_vector[i]=*Filler;
}

template <class T>
void EST_TDeque<T>::push(T& it)
{
  int next_front= p_front+1;
  if (next_front >= p_vector.n())
    next_front= 0;

  if (next_front==p_back)
    {
      expand();
      push(it);
    }
  else
    {
      p_vector[p_front] = it;
      p_front=next_front;
    }
}

template <class T>
T &EST_TDeque<T>::pop()
{
  if (is_empty())
    EST_error("empty stack!");
  
  p_front--;
  if (p_front < 0)
    p_front=p_vector.n()-1;
  
  return p_vector[p_front];
}

template <class T>
T &EST_TDeque<T>::nth(int n)
{
  if (is_empty())
    EST_error("empty stack!");
  
  int pos = p_front-1-n;

  if (p_front < p_back)
    {
      if (pos < 0)
	{
	  pos += p_vector.n();
	  if (pos < p_back)
	    EST_error("looking too far up stack!");
	}
    }
  else
    if (pos < p_back)
      EST_error("looking too far up stack!");

  return p_vector[pos];
}

template <class T>
void EST_TDeque<T>::back_push(T& it)
{
  int next_back = p_back-1;

  if (next_back < 0)
    next_back = p_vector.n()-1;

  if (next_back == p_front)
    {
      expand();
      back_push(it);
    }
  else
    {
      p_vector[p_back=next_back] = it;
    }
}

template <class T>
T &EST_TDeque<T>::back_pop()
{
  if (is_empty())
    EST_error("empty stack!");
  
  int old_back = p_back;
  p_back++;
  if (p_back >= p_vector.n())
    p_back=0;
  
  return p_vector[old_back];
}



#endif

