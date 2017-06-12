/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                         Copyright (c) 1996                            */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*                       Author :  Paul Taylor                           */
/*                       Date   :  April 1996                            */
/*-----------------------------------------------------------------------*/
/*                           Vector class                                */
/*                                                                       */
/*=======================================================================*/

#ifndef __EST_TVector_H__
#define __EST_TVector_H__

#include <iostream>
using namespace std;
#include "EST_bool.h"
#include "EST_rw_status.h"

#include "instantiate/EST_TVectorI.h"

template<class T> class EST_TMatrix;
template<class T> class EST_TList;
class EST_String;

/* A constants to make it clearer what is going on when we pass `-1'
  * meaning `current size' or `all the rest'
  */

extern const int EST_CURRENT;
extern const int EST_ALL;

/* When set bounds checks (safe but slow) are done on vector access */
#ifndef TVECTOR_BOUNDS_CHECKING
#    define TVECTOR_BOUNDS_CHECKING 0
#endif

#if TVECTOR_BOUNDS_CHECKING
#define A_CHECK a_check
#else
#define A_CHECK a_no_check
#endif

#define INLINE inline

/* This doesn't work as I thought so I have disabled it for now.
 */

#if defined(__GNUC__) && 0
#    define fast_a_v_gcc(C) \
	( *((T *)\
	      (((char (*) [sizeof(T)*p_column_step])p_memory) + (C))\
	      ))
#    define fast_a_v_x(C) (fast_a_v_gcc(C))
#else
#    define fast_a_v_x(C) (fast_a_v(C))
#endif


/**@name Template vector

    This serves as a base class for a vector
     of type <type>T</type>.  This acts as a higher level
     version of a normal C array as defined as <type>float *x</type> etc.

     The vector can be resized after declaration, access can be 
     with or without bounds checking.  Round brackets denote read-only
     access (for consts) while square brackets are for read-write access.
     In both cases references are returned.

     The standard operators () and [] should be thought of as 
     having no bounds checking, though they may do so optionally
     as a compile time option.  The methods <method>a_check</method> and 
     <method>a_nocheck</method> provide explicit boundary checking/nonchecking,
     both const and non-const versions are provided.

     Access through () and [] are guaranteed to be as fast as standard
     C arrays (assuming a reasonable optimizing compiler).  

     <programlisting>
     EST_FVector x(10);
     int i;

     for (i=0; i < x.length(); ++i)
        x[i] = sqrt((float)i);
     
     x.resize(20);

     for (i=10; i < x.length(); ++i)
        x[i] = sqrt((float)i);

     </programlisting>

     To instantiate a template for a a vector of type {FooBar}

     <programlisting>
     #include "../base_class/EST_TVector.cc"
     // If you want List to vector conversion (and defined a TList)
     #include "../base_class/EST_Tvectlist.cc"
       
     template class EST_TVector<FooBar>;
     template ostream& operator << 
          (ostream &st, const EST_TVector<FooBar> &v);
     </programlisting>

     The EST library already has template vector instantiations for
     <type>int</type>, <type>float</type>, <type>double</type> and
     <docppRef linkend='EST_String'>.  Also types are defined for them
     in <docppRef linkend='EST_types.h'> as <docppRef
     linkend='EST_IVector'>, <docppRef linkend='EST_FVector'>,
     <docppRef linkend='EST_DVector'> and <docppRef
     linkend='EST_StrVector'> for <type>int</type>s,
     <type>float</type>s, <type>doubles</type>s and <docppRef
     linkend='EST_String'>s respectively.

  * @see matrix_example */
//@{
template <class T> 
class EST_TVector 
{
  // protected:
public:
  /** Pointer to the start of the vector. 
    * The start of allocated memory is p_memory-p_offset.
    */
  T *p_memory; 

  /// Visible shape
  unsigned int p_num_columns;

  /// How to access the memory
  unsigned int p_offset;
  unsigned int p_column_step;
  
  bool p_sub_matrix;

  
  /// The memory access rule, in one place for easy reference
  INLINE unsigned int vcell_pos(unsigned int c,
			        unsigned int cs) const
    {return cs==1?c:c*cs;}

  INLINE unsigned int vcell_pos(unsigned int c) const
    {
      return vcell_pos(c, 
		      p_column_step);
    }

  INLINE unsigned int vcell_pos_1(unsigned int c) const
    {
      return c;
    }

  /// quick method for returning \(x[n]\)
  INLINE const T &fast_a_v(int c) const { return p_memory[vcell_pos(c)]; }

  INLINE T &fast_a_v(int c) { return p_memory[vcell_pos(c)]; }

  INLINE const T &fast_a_1(int c) const { return p_memory[vcell_pos_1(c)]; }
  INLINE T &fast_a_1(int c) { return p_memory[vcell_pos_1(c)]; }

  /// Get and set values from array
  void set_values(const T *data, int step, int start_c, int num_c);
  void get_values(T *data, int step, int start_c, int num_c) const;
    
  /// private copy function, called from all other copying functions.
  void copy(const EST_TVector<T> &a); 
  /// just copy data, no resizing, no size check.
  void copy_data(const EST_TVector<T> &a); 

  /// resize the memory and reset the bounds, but don't set values.
  void just_resize(int new_cols, T** old_vals);

  /// sets data and length to default values (0 in both cases).
  void default_vals();

public:
  ///default constructor
  EST_TVector(); 

  /// copy constructor
  EST_TVector(const EST_TVector<T> &v); 

  /// "size" constructor - make vector of size n.
  EST_TVector(int n); 

  /// construct from memory supplied by caller
  EST_TVector(int, 
	      T *memory, int offset=0, int free_when_destroyed=0);

  /// destructor.
  ~EST_TVector();

  /// default value, used for filling matrix after resizing
  static const T *def_val;

  /** A reference to this variable is returned if you try and access
    * beyond the bounds of the matrix. The value is undefined, but you
    * can check for the reference you get having the same address as
    * this variable to test for an error.
    */
  static T *error_return;

  /** resize vector. If <expr>set=1</expr>, then the current values in
      the vector are preserved up to the new length <parameter>n</parameter>. If the
      new length exceeds the old length, the rest of the vector is
      filled with the <variable>def_val</variable>
  */
  void resize(int n, int set=1); 

  /** For when you absolutely have to have access to the memory.
    */
  const T * memory() const { return p_memory; }
  T * memory(){ return p_memory; }

  /**@name access
    * Basic access methods for vectors.
    */
  //@{

  /// number of items in vector.
  INLINE int num_columns() const {return p_num_columns;}
  /// number of items in vector.
  INLINE int length() const {return num_columns();}
  /// number of items in vector.
  INLINE int n() const {return num_columns();}

  /// read-only const access operator: without bounds checking
  INLINE const T &a_no_check(int n) const { return fast_a_v_x(n); }
  /// read/write non-const access operator: without bounds checking
  INLINE T &a_no_check(int n) { return fast_a_v_x(n); }
  /// read-only const access operator: without bounds checking
  INLINE const T &a_no_check_1(int n) const { return fast_a_1(n); }
  /// read/write non-const access operator: without bounds checking
  INLINE T &a_no_check_1(int n) { return fast_a_1(n); }

  // #define pp_a_no_check(V,N) (pp_fast_a(V,N))

  /// read-only const access operator: with bounds checking
  const T &a_check(int n) const;
  /// read/write non-const access operator: with bounds checking
  T &a_check(int n);

  const T &a(int n) const { return A_CHECK(n); }
  T &a(int n) { return A_CHECK(n); }

  /// read-only const access operator: return reference to nth member
  const T &operator () (int n) const {return A_CHECK(n);}

  // PT
  // /// non const access operator: return reference to nth member
  //  T &operator () (int n) const {return a(n);}

  /// read/write non const access operator: return reference to nth member
  T &operator [] (int n) { return A_CHECK(n); }

  //@}

  void set_memory(T *buffer, int offset, int columns,
		  int free_when_destroyed=0);

  /// assignment operator
  EST_TVector &operator=(const EST_TVector &s);

  /// Fill entire array will value <parameter>v</parameter>.
  void fill(const T &v);

  /// Fill vector with default value
  void empty() { fill(*def_val); }

  /// is true if vectors are equal size and all elements are equal.
  int operator == (const EST_TVector &v) const;
  /// is true if vectors are not equal size or a single elements isn't equal.
  int operator != (const EST_TVector &v) const
    { return ! ((*this) == v); }

  /// Copy data in and out. Subclassed by SimpleVector for speed.

  void copy_section(T* dest, int offset=0, int num=-1) const;
  void set_section(const T* src, int offset=0, int num=-1);

  /// Create a sub vector.
  void sub_vector(EST_TVector<T> &sv, int start_c=0, int len=-1);
  /// print out vector.
    friend ostream& operator << (ostream &st, const EST_TVector<T> &m)
    {
        int i; 
        for (i = 0; i < m.n(); ++i) 
            st << m(i) << " "; st << endl; 
        return st;
    }

  /// Matrix must be friend to set up subvectors
  friend class EST_TMatrix<T>;

  void integrity() const;

};

//@}
/// assignment operator: fill track with values in list <parameter>s</parameter>.
template<class T>
extern EST_TVector<T> &set(EST_TVector<T> &v, const EST_TList<T> &s);

#include <iostream>
#include <fstream>
#include "../base_class/EST_matrix_support.h"
#include "EST_cutils.h"
#include "EST_error.h"

template<class T>
void EST_TVector<T>::default_vals()
{
  p_num_columns = 0;
  p_offset=0;
  p_column_step=0;

  p_memory = NULL;
  p_sub_matrix=FALSE;
}

template<class T>
EST_TVector<T>::EST_TVector()
{
    default_vals();
}

template<class T>
EST_TVector<T>::EST_TVector(int n)
{
    default_vals();
    resize(n);
}

template<class T>
EST_TVector<T>::EST_TVector(const EST_TVector<T> &in)
{
  default_vals();
  copy(in);
}

template<class T>
EST_TVector<T>::EST_TVector(int n,
			    T *memory, int offset, int free_when_destroyed)
{
  default_vals();

  set_memory(memory, offset, n, free_when_destroyed);
}

template<class T>
EST_TVector<T>::~EST_TVector()
{
  p_num_columns = 0;
  p_offset=0;
  p_column_step=0;

  if (p_memory != NULL && !p_sub_matrix)
    {
      delete [] (p_memory-p_offset);
      p_memory = NULL;
    }
}


template<class T>
void  EST_TVector<T>::fill(const T &v)
{
    for (int i = 0; i < num_columns(); ++i)
	fast_a_v(i) = v;
}

template<class T>
void EST_TVector<T>::set_memory(T *buffer, int offset, int columns, 
				int free_when_destroyed)
{
  if (p_memory != NULL && !p_sub_matrix)
    delete [] (p_memory-p_offset);
  
  p_memory = buffer-offset;
  p_offset=offset;
  p_num_columns = columns;
  p_column_step=1;
  p_sub_matrix = !free_when_destroyed;
}

template<class T>
void EST_TVector<T>::set_values(const T *data, 
				 int step,
				 int start_c, 
				 int num_c)
{
  for(int i=0, c=start_c, p=0; i<num_c; i++, c++, p+=step)
    a_no_check(c) = data[p];
}


template<class T>
void EST_TVector<T>::get_values(T *data, 
				 int step,
				 int start_c, 
				 int num_c) const
{
  for(int i=0, c=start_c, p=0; i<num_c; i++, c++, p+=step)
   data[p] = a_no_check(c);
}


template<class T>
void EST_TVector<T>::copy_data(const EST_TVector<T> &a)
{
  set_values(a.p_memory, a.p_column_step, 0, num_columns());
}

template<class T>
void EST_TVector<T>::copy(const EST_TVector<T> &a)
{
    resize(a.n(), FALSE);
    copy_data(a);
}

template<class T>
void EST_TVector<T>::just_resize(int new_cols, T** old_vals)
{
  T *new_m;
  
  if (num_columns() != new_cols || p_memory == NULL )
    {
      if (p_sub_matrix)
	EST_error("Attempt to resize Sub-Vector");

      if (new_cols < 0)
	EST_error("Attempt to resize vector to negative size: %d",
		  new_cols);

      new_m = new T[new_cols];

      if (p_memory != NULL)
      {
	if (old_vals != NULL)
	  *old_vals = p_memory;
	else if (!p_sub_matrix)
	  delete [] (p_memory-p_offset);
      }

      p_memory = new_m;
      //cout << "vr: mem: " << p_memory << " (" << (int)p_memory << ")\n";
      p_offset=0;
      p_num_columns = new_cols;
      p_column_step=1;
    }
  else
    *old_vals = p_memory;
}


template<class T>
void EST_TVector<T>::resize(int new_cols, int set)
{
  int i;
  T * old_vals = p_memory;
  int old_cols = num_columns();
  int old_offset = p_offset;
  int old_column_step = p_column_step;

  just_resize(new_cols, &old_vals);

  if (set)
    {
      int copy_c = 0;

      if (!old_vals)
	copy_c=0;
      else if (old_vals != p_memory)
	{
	  copy_c = Lof(num_columns(), old_cols);

	  for(i=0; i<copy_c; i++)
	      a_no_check(i) 
		= old_vals[vcell_pos(i,
				    old_column_step)];
	}
      else 
	copy_c = old_cols;
      
      for(i=copy_c; i<new_cols; i++)
	  {
		  if(def_val == NULL)
		  {
//			  a_no_check(i) =  NULL;
		  }
		  else
		  {
			  a_no_check(i) =  *def_val;
		  }
	  }
    }

  if (old_vals && old_vals != p_memory && !p_sub_matrix)
    delete [] (old_vals-old_offset);
}

template<class T>
EST_TVector<T> &EST_TVector<T>::operator=(const EST_TVector<T> &in)
{
    copy(in);
    return *this;
}

template<class T>
T &EST_TVector<T>::a_check(int n)
{
  if (!EST_vector_bounds_check(n, num_columns(), FALSE))
    return *error_return;

  return fast_a_v(n);
}

template<class T>
const T &EST_TVector<T>::a_check(int n) const
{
  return ((EST_TVector<T> *)this)->a(n);
}

template<class T>
int EST_TVector<T>::operator == (const EST_TVector<T> &v) const
{
    if (num_columns() != v.num_columns())
	return 0;

    for(int i=0; i<num_columns() ; i++)
      if (fast_a_v(i) != v.fast_a_v(i))
	return 0;
    return 1;
}

template<class T>
void EST_TVector<T>::copy_section(T* dest, int offset, int num) const
{
  if (num<0)
    num = num_columns()-offset;

  if (!EST_vector_bounds_check(num+offset-1, num_columns(), FALSE))
    return;
  

  for(int i=0; i<num; i++)
    dest[i] = a_no_check(offset+i);
}

template<class T>
void EST_TVector<T>::set_section(const T* src, int offset, int num)
{
  if (num<0)
    num = num_columns()-offset;

  if (!EST_vector_bounds_check(num+offset-1, num_columns(), FALSE))
    return;
  
  for(int i=0; i<num; i++)
    a_no_check(offset+i) = src[i];
}

template<class T>
void EST_TVector<T>::sub_vector(EST_TVector<T> &sv,
				int start_c, int len)
{
  if (len < 0)
    len = num_columns()-start_c;

  if (sv.p_memory != NULL && ! sv.p_sub_matrix)
    delete [] (sv.p_memory - sv.p_offset);

  sv.p_sub_matrix = TRUE;
  sv.p_offset = p_offset + start_c*p_column_step;
  sv.p_memory = p_memory - p_offset + sv.p_offset;
  sv.p_column_step=p_column_step;
  sv.p_num_columns = len;
}

template<class T>
void EST_TVector<T>::integrity() const
{
  cout << "integrity: p_memory=" << p_memory << endl;
  if(p_memory == (T *)0x00080102) 
    {
      cout << "fatal value!!!\n";
    }
}


template<class T> const T* EST_TVector<T>::def_val;


#undef A_CHECK
#endif
