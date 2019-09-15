/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017 Hibbard M. Engler

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

#ifndef _H_WORLD_INTERN
#define _H_WORLD_INTERN

// hard code for reggaeton69 and resplandor for android, anyways
//#define PROGRAM_LANGUAGE_VARIANT_IS_reggaeton69_pr_es 1

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_resplandor_pr_es
#define PROGRAM_LANGUAGE_VARIANT resplandor_pr_us
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_reggaeton69_pr_es
#define PROGRAM_LANGUAGE_VARIANT reggaeton69_pr_us
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_reggaeton_pr_es
#define PROGRAM_LANGUAGE_VARIANT reggaeton_pr_us
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_lele_radiance_en_us
#define PROGRAM_LANGUAGE_VARIANT lele_radiance_en_us
#endif

#ifndef PROGRAM_LANGUAGE_VARIANT
#define PROGRAM_LANGUAGE_VARIANT silentradiance_en_us
#define PROGRAM_LANGUAGE_VARIANT_IS_silentradiance_en_us 1
#endif



#ifdef PROGRAM_LANGUAGE_VARIANT_IS_silentradiance_en_us
#include "world_internationalization_en.h"
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_reggaeton69_pr_es
#include "world_internationalization_es69.h"
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_reggaeton_pr_es
#include "world_internationalization_rgt_es.h"
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_resplandor_pr_es
#include "world_internationalization_es.h"
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_lele_radiance_en_us
#include "world_internationalization_enle.h"
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_hui_guang_zh_zh
#include "world_internationalization_zh.h"
#endif

#ifdef PROGRAM_LANGUAGE_VARIANT_IS_dooree_disco_in_hi
#include "world_internationalization_hi.h"
#endif

#define APPNAME PROGRAM_LANGUAGE_VARIANT

#endif
