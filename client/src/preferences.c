/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2018 Hibbard M. Engler

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






#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "glue.h"
#include "preferences.h"

char *getPreference(struct preferences **hash_tablep,char *name) {
struct preferences *hash_table = *hash_tablep;
struct preferences *preference;
HASH_FIND_STR(hash_table,name,preference);
if (!preference) return(NULL);
return(preference->value);
}

char *getPreferenceOrDefault(struct preferences **hash_tablep,char *name,char *default_value)
{
char *x=getPreference(hash_tablep,name);
//logit("get %d %s:",(int)strlen(name),name);
if (!x) return(default_value);
if (!x[0]) return(default_value);
//logit("   %s",x);
return x;
}

void setPreference(struct preferences **hash_tablep,char *name,char *value) {
struct preferences *hash_table = *hash_tablep;
//logit("set %d %s: %s",(int)strlen(name),name,value);
/* replace if old */
struct preferences *old;
HASH_FIND_STR(hash_table,name,old);
if (old) {
  free(old->value);
  old->value = strdup(value);
  return;
  }

struct preferences *new;
new = malloc(sizeof(struct preferences));
if (!new) return; /* out of memory */
strncpy(new->name,name,399);
new->name[399]='\0';
new->value = strdup(value);
HASH_ADD_STR(hash_table,name,new);

*hash_tablep = hash_table;
}


void clearPreference(struct preferences **hash_tablep,char *name) {
struct preferences *hash_table = *hash_tablep;

struct preferences *target;
HASH_FIND_STR(hash_table,name,target);
if (!target) return;

free(target->value);
target->value=NULL;
HASH_DEL(hash_table,target);
free(target);

*hash_tablep = hash_table;
}


void clearAllPreferences(struct preferences **hash_tablep) {
struct preferences *hash_table = *hash_tablep;
struct preferences *target;
struct preferences *tmp;

HASH_ITER(hh,hash_table,target,tmp) {
  free(target->value);
  target->value=NULL;
  HASH_DEL(hash_table,target);
  free(target);
  }
*hash_tablep = hash_table;
}


static struct preferences *iterator;
void startPreferenceIteration(struct preferences **hash_tablep) {
struct preferences *hash_table = *hash_tablep;
iterator = hash_table;
}


struct preferences *nextPreferenceIteration(struct preferences **hash_tablep) {
struct preferences *oldint;
oldint = iterator;
if (iterator) iterator=iterator->hh.next;
return(oldint);
}
