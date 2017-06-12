//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: mecab.cpp,v 1.17 2011-02-07 08:51:14 uratec Exp $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation

/* ----------------------------------------------------------------- */
/*           The Japanese TTS System "Open JTalk"                    */
/*           developed by HTS Working Group                          */
/*           http://open-jtalk.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2008-2011  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#ifndef MECAB_CPP
#define MECAB_CPP

#include <stdlib.h>
#include <string.h>

#include "mecab.h"

#ifdef __cplusplus
#define MECAB_CPP_START extern "C" {
#define MECAB_CPP_END   }
#else
#define MECAB_CPP_START
#define MECAB_CPP_END
#endif                          /* __CPLUSPLUS */

MECAB_CPP_START;

MecabThreadFeature* Mecab_analysis(mecab_t* m, char *str)
{
  int i = 0;
  mecab_node_t *head;
  mecab_node_t *node;

  head = (mecab_node_t *) mecab_sparse_tonode(m, str);
  if(head == NULL) return NULL;

  MecabThreadFeature* mf = (MecabThreadFeature*)malloc(sizeof(MecabThreadFeature));
  if (!mf) return NULL;
  mf->feature = NULL;
  mf->size = 0;

  for (node = head; node != NULL; node = node->next) {
    if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE)
      mf->size++;
  }
  mf->feature = (char **) calloc(mf->size, sizeof(char *));
  for (node = head; node != NULL; node = node->next) {
    if(node->stat != MECAB_BOS_NODE && node->stat != MECAB_EOS_NODE){
      mf->feature[i] = (char *) calloc(node->length + strlen(node->feature) + 2,sizeof(char));
      strcpy(mf->feature[i],"");
      strncat(mf->feature[i],node->surface,node->length);
      strcat(mf->feature[i],",");
      strcat(mf->feature[i],node->feature);
      i++;
    }
  }

  return mf;
}

void Mecab_print(MecabThreadFeature* mf)
{
  int i;
  
  for(i = 0;i < mf->size;i++)
    printf("%s\n",mf->feature[i]);
}

int Mecab_get_size(MecabThreadFeature* mf)
{
	return mf->size;
}

char **Mecab_get_feature(MecabThreadFeature* mf)
{
  return mf->feature;
}
void Mecab_refresh(MecabThreadFeature* mf)
{
  int i;
  if (mf == NULL) return ;
  if(mf->feature != NULL){
    for(i = 0;i < mf->size;i++)
      free(mf->feature[i]);
    free(mf->feature);
    mf->feature = NULL;
    mf->size = 0;
  }
  free(mf);
}

MECAB_CPP_END;

#endif                          /* !MECAB_CPP */
