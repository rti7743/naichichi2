/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                      Copyright (c) 1995,1996                          */
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
/*                   Author :  Paul Taylor                               */
/*                   Date   :  January 1995                              */
/*-----------------------------------------------------------------------*/
/*              Key/Value list template class                            */
/*                                                                       */
/*=======================================================================*/
#ifndef __EST_TKVL_H__
#define __EST_TKVL_H__

#include <cmath>

using namespace std;

#include "EST_TList.h"
#include "instantiate/EST_TKVLI.h"
#include "EST_TIterator.h"

class EST_String;


/** Templated Key-Value Item. Serves as the items in the list of the
EST_TKVL class.  */
template<class K, class V> class EST_TKVI {
 public:
    K k;
    V v;

    inline bool operator==(const EST_TKVI<K,V> &i){
	return( (i.k == k) && (i.v == v) );
    }

    friend  ostream& operator << (ostream& s, EST_TKVI<K,V> const &i)
        {  return s << i.k << "\t" << i.v << "\n"; }
};


/** Templated Key-Value list. Objects of type EST_TKVL contain lists which
are accessed by a key of type {\bf K}, which returns a value of type
{\bf V}. */
template<class K, class V> class EST_TKVL {
 private:
    EST_Litem *find_pair_key(const K &key) const;
    EST_Litem *find_pair_val(const V &val) const;
 public:
    /**@name Constructor functions */
    //@{
    /// default constructor
    EST_TKVL() {;}
    /// copy constructor
    EST_TKVL(const EST_TKVL<K, V> &kv);
    //@}

    /// default value, returned when there is no such entry.
    static V *default_val;

    /// default value, returned when there is no such entry.
    static K *default_key;

    /// Linked list of key-val pairs. Don't use
    /// this as it will be made private in the future
    EST_TList< EST_TKVI<K,V> > list;	

    /// number of key value pairs in list
    const int length() const {return list.length();} 

    /// Return First key value pair in list
    EST_Litem * head() const {return list.head();};

    /// Empty list.
    void clear();
    
    /**@name Access functions.
     */
    //@{
    /// return value according to key (const)
    const V &val(const K &rkey, bool m=0) const;
    /// return value according to key (non-const)
    V &val(const K &rkey, bool m=0);
    /// return value according to ptr
    const V &val(EST_Litem *ptr, bool m=0) const;     
    /// return value according to ptr
    V &val(EST_Litem *ptr, bool m=0);     
    /// value or default 
    const V &val_def(const K &rkey, const V &def) const;

    /// find key, reference by ptr 
    const K &key(EST_Litem *ptr, int m=1) const;
    /// find key, reference by ptr 
    K &key(EST_Litem *ptr, int m=1);

    /// return first matching key, referenced by val
    const K &key(const V &v, int m=1) const;

    /** change key-val pair. If no corresponding entry is present, add
      to end of list.
      */
    int change_val(const K &rkey,const V &rval); 
    /** change key-val pair. If no corresponding entry is present, add
      to end of list.*/
    int change_val(EST_Litem *ptr,const V &rval); // change key-val pair.
    /// change name of key pair.
    int change_key(EST_Litem *ptr,const K &rkey);

    /// add key-val pair to list
    int add_item(const K &rkey,const V &rval, int no_search = 0);

    /// remove key and val pair from list
    int remove_item(const K &rkey, int quiet = 0);

    //@}
    
    /// Returns true if key is present.
    const int present(const K &rkey) const;

    /// apply function to each pair
    void map(void (*func)(K&, V&));
    
    friend ostream& operator << (ostream& s, EST_TKVL<K,V> const &l)
    {EST_Litem *p; 
        for (p = l.list.head(); p ; p = p->next()) 
            s << l.list(p).k << "\t" << l.list(p).v << endl; 
        return s;
    } 
    
    /// full copy of KV list.
    EST_TKVL<K, V> & operator =  (const EST_TKVL<K,V> &kv);
    /// add kv after existing list.
    EST_TKVL<K, V> & operator += (const EST_TKVL<K,V> &kv); 
    /// make new concatenated list
    EST_TKVL<K, V>   operator +  (const EST_TKVL<K,V> &kv); 

  // Iteration support

protected:
  struct IPointer {  EST_Litem *p; };

  void point_to_first(IPointer &ip) const { ip.p = list.head(); }
  void move_pointer_forwards(IPointer &ip) const { ip.p = ip.p->next(); }
  bool points_to_something(const IPointer &ip) const { return ip.p != NULL; }
  EST_TKVI<K, V> &points_at(const IPointer &ip) { return list(ip.p); }

  friend class EST_TIterator< EST_TKVL<K, V>, IPointer, EST_TKVI<K, V> >;
  friend class EST_TStructIterator< EST_TKVL<K, V>, IPointer, EST_TKVI<K, V> >;
  friend class EST_TRwIterator< EST_TKVL<K, V>, IPointer, EST_TKVI<K, V> >;
  friend class EST_TRwStructIterator< EST_TKVL<K, V>, IPointer, EST_TKVI<K, V> >;

public:
  typedef EST_TKVI<K, V> Entry;
  typedef EST_TStructIterator< EST_TKVL<K, V>, IPointer, Entry> Entries;
  typedef EST_TRwStructIterator< EST_TKVL<K, V>, IPointer, Entry> RwEntries;

  // Iteration support

protected:
  struct IPointer_k {  EST_Litem *p; };

  void point_to_first(IPointer_k &ip) const { ip.p = list.head(); }
  void move_pointer_forwards(IPointer_k &ip) const { ip.p = ip.p->next(); }
  bool points_to_something(const IPointer_k &ip) const { return ip.p != NULL; }
  K &points_at(const IPointer_k &ip) { return list(ip.p).k; }

  friend class EST_TIterator< EST_TKVL<K, V>, IPointer_k, K >;
  friend class EST_TRwIterator< EST_TKVL<K, V>, IPointer_k, K >;

public:
  typedef K KeyEntry;
  typedef EST_TIterator< EST_TKVL<K, V>, IPointer_k, KeyEntry> KeyEntries;
  typedef EST_TRwIterator< EST_TKVL<K, V>, IPointer_k, KeyEntry> KeyRwEntries;

};

#include <cstdlib>
#include "EST_error.h"

template <class K, class V> EST_TKVL<K, V>::EST_TKVL(const EST_TKVL<K, V> &kv)
{
    list = kv.list;
}

template <class K, class V> 
void EST_TKVL<K, V>::clear(void)
{
    list.clear();
}

template <class K, class V> 
EST_Litem *EST_TKVL<K, V>::find_pair_key(const K &key) const
{
    EST_Litem *ptr;

    for (ptr = list.head(); ptr != 0; ptr= ptr->next())
	if (list.item(ptr).k == key)
	    return ptr;
    return 0;
}

template <class K, class V> 
EST_Litem *EST_TKVL<K, V>::find_pair_val(const V &val) const
{
    EST_Litem *ptr;

///    cout << "function list\n" << endl;

    for (ptr = list.head(); ptr != 0; ptr= ptr->next())
    {
//	cout << "ff: " << list.item(ptr).k << endl;
	if (list.item(ptr).v == val)
	    return ptr;
    }
    return 0;
}


// look for pointer kptr in list. If found, change its value to rval and
// return true, otherwise return false.

template <class K, class V> 
int EST_TKVL<K, V>::change_val(EST_Litem *kptr, const V &rval)
{
    if (list.index(kptr) == -1)
	return 0;
    else
    {
	list.item(kptr).v = rval;
	return 1;
    }
}

template <class K, class V> 
int EST_TKVL<K, V>::change_key(EST_Litem *kptr, const K &rkey)
{
    if (list.index(kptr) == -1)
	return 0;
    else
    {
	list.item(kptr).k = rkey;
	return 1;
    }
}

// look for key rkey in list. If found, change its value to rval and
// return true, otherwise return false.
template <class K, class V> 
int EST_TKVL<K, V>::change_val(const K &rkey,const V &rval)
{
    EST_Litem *ptr=find_pair_key(rkey);
    if (ptr == 0)
	return 0;
    else
    {
	list.item(ptr).v = rval;
	return 1;
    }
}

// NOTE: This _MUST_NOT_ change the EST_TKVL,  if it needs to, a separate
// const version will need to replace the dummy one below.

template<class K, class V> 
V &EST_TKVL<K, V>::val(const K &rkey, bool must)
{ 
    EST_Litem *ptr = find_pair_key(rkey);

    if (ptr == 0)
    {
	if (must)
	  EST_error("No value set for '%s'", error_name(rkey));

	return *default_val;
    }
    else
	return list.item(ptr).v;
}

template<class K, class V> 
const V &EST_TKVL<K, V>::val(const K &rkey, bool must) const
{ 
   return ((EST_TKVL<K, V> *)this)->val(rkey, must);
}

template<class K, class V> 
const V &EST_TKVL<K, V>::val_def(const K &rkey, const V &def) const
{ 
    EST_Litem *ptr = find_pair_key(rkey);
    if (ptr == 0)
	return def;
    else
	return list.item(ptr).v;
}

// NOTE: This _MUST_NOT_ change the EST_TKVL,  if it needs to, a separate
// const version will need to replace the dummy one below.

template<class K, class V> 
V &EST_TKVL<K, V>::val(EST_Litem *kptr, bool must)
{ 
    if (must == 0)
	return list.item(kptr).v;
    /* check kptr is one of mine */
    if (list.index(kptr) == -1)
    {
      if (must)
	EST_error("No value set in EST_TKVL");
      return *default_val;
    }
    else
	return list.item(kptr).v;
}


template<class K, class V> 
const V &EST_TKVL<K, V>::val(EST_Litem *kptr, bool must) const
{ 
  return ((EST_TKVL<K, V> *)this)->val(kptr, must);
}

// NOTE: This _MUST_NOT_ change the EST_TKVL,  if it needs to, a separate
// const version will need to replace the dummy one below.

template<class K, class V> 
K &EST_TKVL<K, V>::key(EST_Litem *kptr, int must)
{ 
    if (must == 0)
	return list.item(kptr).k;
    if (list.index(kptr) == -1)
      EST_error("No value set in EST_TKVL");

    return list.item(kptr).k;
}

template<class K, class V> 
const K &EST_TKVL<K, V>::key(EST_Litem *kptr, int must) const
{ 
  return ((EST_TKVL<K, V> *)this)->key(kptr, must);
}

template<class K, class V> 
const K &EST_TKVL<K, V>::key(const V &v, int must) const
{ 
    EST_Litem *ptr = find_pair_val(v);
    if (ptr == 0)
    {
	if (must)
	  EST_error("No value set for '%s'", error_name(v));

	return *default_key;
    }
    
    return list.item(ptr).k;
}

template<class K, class V> 
const int EST_TKVL<K, V>::present(const K &rkey) const
{ 
    if (find_pair_key(rkey) == 0)
	return 0;
    else
	return 1;
}

// map a function over the pairs

template<class K, class V>
void EST_TKVL<K,V>::map(void (*func)(K&, V&))
{
    EST_Litem *p;
    for(p=list.head(); p; p=p->next())
    {
	EST_TKVI<K,V> item = list.item(p);
	(*func)(item.k, item.v);
    }
}

// add item to list. By default, the list is searched to see if the
// item exists already. If so, its value is overwritten. This facility
// can be turned off by setting no_search = 1;

template<class K, class V> 
int EST_TKVL<K, V>::add_item(const K &rkey, const V &rval, int no_search)
{
    if (!no_search)
	if (change_val(rkey, rval)) // first see if key exists
	    return 1;
    
    EST_TKVI<K,V>  item;
    item.k = rkey;
    item.v = rval;
    
    list.append(item);
    return 1;
}

template<class K, class V> 
int EST_TKVL<K, V>::remove_item(const K &rkey, int quiet)
{
    EST_Litem *ptr = find_pair_key(rkey);
    const char *en;
    if (ptr == 0)
    {
	if (!quiet)
	{
	    en = error_name(rkey);
	    EST_warning("EST_TKVL: no item labelled '%s'", en);
	}
	return -1;
    }
    else 
    {
	list.remove(ptr);
	return 0;
    }
}

template<class K, class V> EST_TKVL<K, V> &EST_TKVL<K, V>::operator = 
(const EST_TKVL<K, V> &kv)
{
    list = kv.list;
    return *this;
}

template<class K, class V> EST_TKVL<K, V> &EST_TKVL<K, V>::operator += 
(const EST_TKVL<K, V> &kv)
{
    list += kv.list;
    return *this;
}

template<class K, class V> EST_TKVL<K, V> EST_TKVL<K, V>::operator + (const EST_TKVL<K, V> &kv)
{
    EST_TKVL<K, V> result;
    result = *this;
    result += kv;
    return result;
}



#endif				// __KVL_H__
