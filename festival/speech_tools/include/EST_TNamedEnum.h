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
 /************************************************************************/
 /*                 Author: Richard Caley (rjc@cstr.ed.ac.uk)            */
 /*                   Date: Fri Feb 28 1997                              */
 /************************************************************************/
 /*                                                                      */
 /* A template class which allows names (const char *s) to be            */
 /* associated with enums, providing conversion.                         */
 /*                                                                      */
 /* EST_TValuesEnum is the obvious generalisation to associating         */
 /* things other than const char * with each value.                      */
 /*                                                                      */
 /* EST_T{Named/Valued}EnumI can include extra information for each      */
 /* enum element.                                                        */
 /*                                                                      */
 /* This should be rewritten as something other than linear search. At   */
 /* least sort them.                                                     */
 /*                                                                      */
 /************************************************************************/

#ifndef __EST_TNAMEDENUM_H__
#define __EST_TNAMEDENUM_H__

#include <cstring>

using namespace std;

#include "EST_String.h"
#include "EST_rw_status.h"

#define NAMED_ENUM_MAX_SYNONYMS (10)

// Used in the type of tables with no info field.

typedef char NO_INFO;

// struct used to define the mapping.

template<class ENUM, class VAL, class INFO> 
struct EST_TValuedEnumDefinition {
public:
    ENUM token; 
    VAL values[NAMED_ENUM_MAX_SYNONYMS];
    INFO info;
} ;

// This is the most general case, a mapping from enum to some other type
// with extra info.

template<class ENUM, class VAL, class INFO> class EST_TValuedEnumI  {

protected:
  int ndefinitions;
  ENUM p_unknown_enum;
  VAL p_unknown_value;
  EST_TValuedEnumDefinition<ENUM,VAL,INFO> *definitions;

  virtual int eq_vals(VAL v1, VAL v2) const {return v1 == v2; };
  // This is only a void * because INFO can`t manage to get the
  // parameter declaration in the definition past gcc with the actual type.
  void initialise(const void *defs);
  void initialise(const void *defs, ENUM (*conv)(const char *));
  void initialise(void) {ndefinitions=0; definitions=NULL;};
  void initialise(ENUM unknown_e, VAL unknown_v) {initialise(); p_unknown_enum=unknown_e; p_unknown_value = unknown_v;};

protected:
  EST_TValuedEnumI(void) {initialise();};

public:
  EST_TValuedEnumI(EST_TValuedEnumDefinition<ENUM,VAL,INFO> defs[]) 
	{initialise((const void *)defs); };
  EST_TValuedEnumI(EST_TValuedEnumDefinition<const char *,VAL,INFO> defs[], ENUM (*conv)(const char *)) 
	{initialise((const void *)defs, conv); };
  virtual ~EST_TValuedEnumI(void);

  int n(void) const;

  ENUM token(VAL value) const;
  ENUM token(int n) const { return nth_token(n); }
  ENUM nth_token(int n) const;
  VAL value(ENUM token, int n=0) const;
  INFO &info(ENUM token) const;

  ENUM unknown_enum(void) const {return p_unknown_enum;};
  VAL  unknown_value(void) const {return p_unknown_value;};
  int  valid(ENUM token) const { return !eq_vals(value(token),p_unknown_value); };
};

// This is a special case for names. This saves typing and also
// takes care of the fact that strings need their own compare function.

template<class ENUM, class INFO> class EST_TNamedEnumI  : public EST_TValuedEnumI<ENUM, const char *, INFO> {

protected:
  EST_TNamedEnumI(void) : EST_TValuedEnumI<ENUM, const char *, INFO>() {};
  int eq_vals(const char *v1, const char *v2) const {return strcmp(v1,v2) ==0; };
public:

  EST_TNamedEnumI(EST_TValuedEnumDefinition<ENUM,const char *,INFO> defs[])
	{this->initialise((const void *)defs); };
  EST_TNamedEnumI(EST_TValuedEnumDefinition<const char *,const char *,INFO> defs[], ENUM (*conv)(const char *))
	{this->initialise((const void *)defs, conv); };
  const char *name(ENUM tok, int n=0) const {return value(tok,n); };

};

// Now the simple cases with no extra information

template<class ENUM, class VAL> class EST_TValuedEnum : public EST_TValuedEnumI<ENUM,VAL,NO_INFO> { 
public:
  EST_TValuedEnum(EST_TValuedEnumDefinition<ENUM,VAL,NO_INFO> defs[]) 
	{this->initialise((const void *)defs);};
  EST_TValuedEnum(EST_TValuedEnumDefinition<const char *,VAL,NO_INFO> defs[], ENUM (*conv)(const char *)) 
	{this->initialise((const void *)defs, conv);};
};


template<class ENUM> class EST_TNamedEnum : public EST_TNamedEnumI<ENUM,NO_INFO> { 
private:
  EST_read_status priv_load(EST_String name, EST_TNamedEnum *definitive);
  EST_write_status priv_save(EST_String name, EST_TNamedEnum *definitive, char quote) const;
public:
  EST_TNamedEnum(ENUM undef_e, const char *undef_n = NULL) 
	{this->initialise(undef_e, undef_n);};
  EST_TNamedEnum(EST_TValuedEnumDefinition<ENUM,const char *,NO_INFO> defs[]) 
	{this->initialise((const void *)defs);};
  EST_TNamedEnum(EST_TValuedEnumDefinition<const char *,const char *,NO_INFO> defs[], ENUM (*conv)(const char *)) 
	{this->initialise((const void *)defs, conv);};

  EST_read_status load(EST_String name) { return priv_load(name, NULL); };
  EST_read_status load(EST_String name, EST_TNamedEnum &definitive) { return priv_load(name, &definitive); };
  EST_write_status save(EST_String name, char quote='"') const { return priv_save(name, NULL, quote); };
  EST_write_status save(EST_String name, EST_TNamedEnum &definitive, char quote='"') const { return priv_save(name, &definitive, quote); };

};

#include "instantiate/EST_TNamedEnumI.h"


#include <cstdlib>
#include <iostream>
#include <cstdio>
#include "EST_walloc.h"
#include "EST_TNamedEnum.h"

// This only takes a void * because I can't manage to get the
// parameter declaration in the definition past gcc with the actual type.

template<class ENUM, class VAL, class INFO> 
void EST_TValuedEnumI<ENUM,VAL,INFO>::initialise(const void *vdefs)
{
  int n=0;
  typedef EST_TValuedEnumDefinition<ENUM,VAL,INFO> defn;
  const defn *defs = (const defn *)vdefs;

  for(n=1; defs[n].token != defs[0].token; n++)
    ;

  this->ndefinitions = n;
  this->definitions = new defn[n];

  this->definitions[0] = defs[0];
  for(n=1; defs[n].token != defs[0].token; n++)
    this->definitions[n] = defs[n];

  this->p_unknown_enum = defs[n].token;
  this->p_unknown_value = defs[n].values[0];
}

template<class ENUM, class VAL, class INFO> 
void EST_TValuedEnumI<ENUM,VAL,INFO>::initialise(const void *vdefs, ENUM (*conv)(const char *))
{
  int n=0;
  //  const struct EST_TValuedEnumDefinition<const char *,VAL,INFO> *defs = (const struct EST_TValuedEnumDefinition<const char *,VAL,INFO> *)vdefs;

  typedef EST_TValuedEnumDefinition<const char *,VAL,INFO> _EST_TMPNAME;
  const _EST_TMPNAME *defs = (const _EST_TMPNAME *)vdefs;

  // fprintf(stderr, "start setup\n");

  for(n=1; strcmp(defs[n].token, defs[0].token) != 0; n++)
    {
      //const char *a = defs[0].token;
      //      const char *b = defs[n].token;
      // fprintf(stderr, ": %d '%s' '%s'\n", n, defs[n].token, defs[0].token);
    }

  this->ndefinitions = n;
  typedef EST_TValuedEnumDefinition<ENUM,VAL,INFO> defn;
  this->definitions = new defn[n];

  this->definitions[0].token = conv(defs[0].token);  
  for(int i=0; i<NAMED_ENUM_MAX_SYNONYMS; i++)
    this->definitions[0].values[i] = defs[0].values[i];
  this->definitions[0].info = defs[0].info;
  for(n=1; strcmp(defs[n].token, defs[0].token) != 0; n++)
    {
      this->definitions[n].token = conv(defs[n].token);
      for(int i2=0; i2<NAMED_ENUM_MAX_SYNONYMS; i2++)
	this->definitions[n].values[i2] = defs[n].values[i2];
      this->definitions[n].info = defs[n].info;
    }

  this->p_unknown_enum = conv(defs[n].token);
  this->p_unknown_value = defs[n].values[0];
}

template<class ENUM, class VAL, class INFO>
EST_TValuedEnumI<ENUM,VAL,INFO>::~EST_TValuedEnumI(void)
{
  if (this->definitions)
     delete[] this->definitions;
}

template<class ENUM, class VAL, class INFO> 
int EST_TValuedEnumI<ENUM,VAL,INFO>::n(void) const
{ 
return this->ndefinitions; 
}

template<class ENUM, class VAL, class INFO>
VAL EST_TValuedEnumI<ENUM,VAL,INFO>::value (ENUM token, int n) const
{
  int i;

  for(i=0; i<this->ndefinitions; i++)
    if (this->definitions[i].token == token)
      return this->definitions[i].values[n];

  return this->p_unknown_value;
}

template<class ENUM, class VAL, class INFO>
INFO &EST_TValuedEnumI<ENUM,VAL,INFO>::info (ENUM token) const
{
  int i;

  for(i=0; i<this->ndefinitions; i++)
    if (this->definitions[i].token == token)
      return this->definitions[i].info;

  cerr << "Fetching info for invalid entry\n";
  abort();

  static INFO dummyI;
  return dummyI;
}

template<class ENUM, class VAL, class INFO>
ENUM EST_TValuedEnumI<ENUM,VAL,INFO>::nth_token (int n) const
{
  if (n>=0 && n < this->ndefinitions)
    return this->definitions[n].token;

  return this->p_unknown_enum;
}

template<class ENUM, class VAL, class INFO> 
ENUM EST_TValuedEnumI<ENUM,VAL,INFO>::token (VAL value) const
{
    int i,j;

    for(i=0; i<this->ndefinitions; i++)
	for(j=0; j<NAMED_ENUM_MAX_SYNONYMS && this->definitions[i].values[j] ; j++)
	    if (eq_vals(this->definitions[i].values[j], value))
		return this->definitions[i].token;

    return this->p_unknown_enum;
}

template<class ENUM> 
EST_read_status EST_TNamedEnum<ENUM>::priv_load(EST_String name, EST_TNamedEnum<ENUM> *definitive)
{
  typedef EST_TValuedEnumDefinition<ENUM, const char *, NO_INFO> Defn;
#define LINE_LENGTH (1024)
  EST_String line(NULL, 'x', LINE_LENGTH);
  char *buffer = (char *)line;
  EST_String tokens[NAMED_ENUM_MAX_SYNONYMS+2];
  FILE *file;
  char quote = '\0';
  int have_unknown=0;
  int n=0;

  if ((file=fopen(name, "rb"))==NULL)
    return misc_read_error;

  if (this->definitions)
    delete[] this->definitions;

  this->ndefinitions= -1;
  this->definitions=NULL;

  buffer[LINE_LENGTH-1] = 'x';

  while (fgets(buffer, LINE_LENGTH, file))
    {
      if ( buffer[LINE_LENGTH-1] != 'x')
	{
	  cerr << "line too long .. '" << buffer << "'\n";
	  return wrong_format;
	}

      if (this->ndefinitions>=0 && quote != '\0' && buffer[0] == '=')
	{
	  // definition by number

	  if ( n>= this->ndefinitions)
	    {
	      cerr << "too many definitions\n";
	      return wrong_format;
	    }

	  int ntokens = split(line, tokens, NAMED_ENUM_MAX_SYNONYMS+2, RXwhite, '"');
	  this->definitions[n].token = (ENUM)atoi(tokens[0].after(0,1));

	  for(int i=1; i<ntokens; i++)
	      this->definitions[n].values[i-1] = wstrdup(tokens[i].unquote_if_needed(quote));
	  for(int j=ntokens-1 ; j< NAMED_ENUM_MAX_SYNONYMS; j++)
	    this->definitions[n].values[j]=NULL;

	  n++;
	}
      else if (have_unknown && this->ndefinitions>=0 && quote != '\0' && buffer[0] == quote)
	{
	  // definition by standard name
	  if (!definitive)
	    {
	      cerr << "can't use names in this definition\n";
	      return wrong_format;
	    }
	  if ( n>= this->ndefinitions)
	    {
	      cerr << "too many definitions\n";
	      return wrong_format;
	    }

	  int ntokens = split(line, tokens, NAMED_ENUM_MAX_SYNONYMS+2, RXwhite, quote);

	  this->definitions[n].token = definitive->token(tokens[0].unquote(quote));

	  for(int i=1; i<ntokens; i++)
	    this->definitions[n].values[i-1] = wstrdup(tokens[i].unquote_if_needed(quote));
	  for(int j=ntokens-1 ; j< NAMED_ENUM_MAX_SYNONYMS; j++)
	    this->definitions[n].values[j]=NULL;

	  n++;
	}
      else
	{
	  // parameter

	  int mlen;
	  int eq = line.search("=", 1, mlen);

	  if (eq <0)
	    {
	      cerr << "bad header line '" << line;
	      return wrong_format;
	    }

	  EST_String key(line.before(eq));

	  if (key == "quote")
	    {
	      quote = line[eq+1];
	      // cout << "quote = '" << quote << "'\n";
	    }
	  else if (key == "number")
	    {
	      this->ndefinitions=atoi(line.after(eq,1));
	      // cout << "n = '" << ndefinitions << "'\n";
	      this->definitions = new Defn[this->ndefinitions];
	      for(int i=0; i<this->ndefinitions; i++)
		this->definitions[i].values[0] =NULL;
	      n=0;
	    }
	  else if (key == "unknown")
	    {
	      this->p_unknown_enum=(ENUM)atoi(line.after(eq,1));
	      // cout << "unknown = '" << p_unknown_enum << "'\n";
	      have_unknown=1;
	    }
	  else
	    {
	      cerr << "bad header line '" << line;
	      return wrong_format;
	    }
	  
	}
    }


  fclose(file);

  return format_ok;
}

template<class ENUM> 
EST_write_status EST_TNamedEnum<ENUM>::priv_save(EST_String name, EST_TNamedEnum<ENUM> *definitive, char quote) const
{
  FILE *file;

  if ((file=fopen(name, "wb"))==NULL)
    return write_fail;

  fprintf(file, "unknown=%d\n", this->p_unknown_enum);
  fprintf(file, "quote=%c\n", quote);
  fprintf(file, "number=%d\n", this->ndefinitions);

  for(int i=0; i<this->ndefinitions; i++)
    if (this->definitions[i].values[0])
      {
	if (definitive)
	  fprintf(file, "%s ", (const char *)EST_String(definitive->name(this->definitions[i].token)).quote(quote));
	else
	  fprintf(file, "=%d ", (int)this->definitions[i].token);

	for(int j=0; j<NAMED_ENUM_MAX_SYNONYMS && this->definitions[i].values[j] != NULL; j++)
	  fprintf(file, "%s ", (const char *) EST_String(this->definitions[i].values[j]).quote_if_needed(quote));
      
	fputc('\n', file);
      }

  fclose(file);

  return write_ok;
}


#endif
