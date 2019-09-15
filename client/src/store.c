/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License   
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

// store.c
#include "glue.h"
#include <stdio.h>
#include <string.h>

extern char *current_web_stream_url();

/* p - platform variable:
  p=l - is the client type for linux
  p=i - client is ios
  p=w - client is windowz
  p=a - cleint is android
  sending this to the stopre helps it adjus to the client.
  */  

// $Revision: 1.10 $
#ifdef LINUX_CLIENT
//#define WORLD_store_l "l"
const char *platform_extention_folder=WORLD_store_l;
#endif
#ifdef __APPLE__
//#define WORLD_store_i "i"
const char *platform_extention_folder=WORLD_store_i;
#endif
#ifdef __WIN32
//#define WORLD_store_w "w"
const char *platform_extention_folder=WORLD_store_w;
#endif
#ifdef ANDROID
//#define WORLD_store_a "a"
const char *platform_extention_folder=WORLD_store_a;
#endif

/* l - language_code
      l=en - english
      l=es - spanish
      l=hi - Hindi
      
*/

//#define WORLD_LANGUAGE_CODE "en"
const char *language_code=WORLD_LANGUAGE_CODE;
//#define WORLD_STORE_BASE_URL "https://www.silentradiance.com"
static const char *store_base_url = WORLD_STORE_BASE_URL;

char store_munged_url[10000];
char *compute_store_munged_url(char *type) 
// type would be windowshop or buynow
{
char *cur=current_web_stream_url();
if (!cur) {
  // t=type (window or buy now)
  // p=platform id
  // l= language code
  // g= affiliate tag (will have g=r) for cheap reggaeton which will make the reggaeton sites 0.69 as a discount
  // There also is a c= (for consumbale_id)


// set this for Reggaeton69 clients
#ifdef PROGRAM_LANGUAGE_VARIANT_IS_reggaeton69_pr_es
#define URL_TAG "&g=6"
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_resplandor_pr_es
#define URL_TAG "&g=r"
#endif

// Lili gets a special tag so she gets 3 points
#ifdef PROGRAM_LANGUAGE_VARIANT_IS_lele_radiance_en_us
#define URL_TAG "&g=lele"
#endif

#ifndef URL_TAG  
#define URL_TAG ""
#endif

  sprintf(store_munged_url,"%s/?t=%s&p=%s&l=%s%s",store_base_url,type,platform_extention_folder,language_code,URL_TAG);
  }
else {
  strcpy(store_munged_url,cur);
  char *p;
  p = store_munged_url+strlen(cur);
  if (p!=store_munged_url) p--;
  /* convert .pl/.cgi to nothing */
  
  /* first get rid of extra slash */
  if ((p!=store_munged_url)&&(*p=='/')) { 
    p--;
    }
  while ((p!=store_munged_url)&&(*p!='/')) {
    p--;
    }
//#define WORLD__qm_t_equals__pct_s_amp_p_equals__pct_s_amp_l_equals__pct_s "?t=%s&p=%s&l=%s"
  sprintf(p,"?t=%s&p=%s&l=%s%s",type,platform_extention_folder,language_code,URL_TAG);  
  }
logit("mung %s\n",store_munged_url);
return store_munged_url;
}
