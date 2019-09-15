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






#ifndef PREFERENCES_H
#define PREFERENCES_H 1

/* preferences - uses uthash */
#include "uthash.h"

#define PREFERENCES_KEYSIZE 400
struct preferences {
  char name[PREFERENCES_KEYSIZE];
  char *value;
  UT_hash_handle hh;
};

extern char *getPreference(struct preferences **hash_tablep,char *name);
extern char *getPreferenceOrDefault(struct preferences **hash_tablep,char *name,char *default_value);
extern void setPreference(struct preferences **hash_tablep,char *name,char *value);
extern void clearPreference(struct preferences **hash_tablep,char *name);
extern void clearAllPreferences(struct preferences **hash_tablep);

/* note - you cannot delete anything while doing the iteration. It is not safe. */
void startPreferenceIteration(struct preferences **hash_tablep);
struct preferences *nextPreferenceIteration(struct preferences **hash_tablep);

#endif
