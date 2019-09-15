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

/*Srx format:

srxV1 - no comments implies 200 frames
a comment implies 100 frames

# - comment on first char

- - option


- tab - 8 spaced in tab
- ^a[A-Z] - compress spaces
- ^b[a-z] - double compress spaces


To work the file ->
# -> ignore






old sr


*/


int srx_is_comment(char *input) {
if (!input) return 0;
if (!(*input)) return 0;
if (*input == '#') return 1;
return 0;
}




static inline void srx_compress_spaces(char *output,int *pl,int maxlen,int spaces) {
if (spaces<3) {
  while (spaces) {
    output[(*pl)++] = ' ';
    if ((*pl)>=maxlen) {
      output[maxlen-1]='\0';
      return;
      }
    spaces--;
    }
  return ;
  }
/* heres the compress */
while (spaces) {
  output[(*pl)++] = '\001';
  if ((*pl)>=maxlen) {
    output[maxlen-1]='\0';
    return;
    }
  int size = spaces;
  if (size>25) size=25;
  output[(*pl)++] = ('@' + (char)(size));
  if ((*pl)>=maxlen) {
    output[maxlen-1]='\0';
    return ;
    }
  spaces -=size;
  }
  return;
}






char *srx_compress(char *output,char *input,int maxlen) {
int l=0;
int spaces=0;
if (*input=='#') { output[l]='\0'; return output; }
while (*input) {
  char ch = *input;
  if (ch>='!') {
    srx_compress_spaces(output,&l,maxlen,spaces);
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    spaces=0;
    output[l++] = ch;
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    input++;
    continue;
    }    
  if (ch=='\t') {
    int t = l % 8;
    if (t==0) t+=8;
    spaces+=t;
    input++;
    continue;
    }
  if (ch=='\001') {
    srx_compress_spaces(output,&l,maxlen,spaces);
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    spaces=0;
    
    input++;    
    if (!(*input)) break;
    output[l++] = ch;
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    output[l++] = *input;
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    input++;
    continue;   
    }
  spaces++;
  input++;
  continue;
  }
srx_compress_spaces(output,&l,maxlen,spaces);
if (l>=maxlen) {
  output[maxlen-1]='\0';
  return output;
  }
spaces=0;
  
output[l++] = '\0';
return output;
}




char *srx_expand(char *output,char *input,int maxlen) {
int l=0;
if (*input=='#') { output[l]='\0'; return output; }
while (*input) {
  char ch = *input;
  if (ch>='!') {
    output[l++] = ch;
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    input++;
    continue;
    }    
  if (ch=='\t') {
    int t=l%8;
    if (t==0) t +=8;
    while (t) {
      output[l++] = ' ';
      if (l>=maxlen) {
        output[maxlen-1]='\0';
        return output;
        }
      t--;
      }
    input++;
    continue;
    }
  if (ch=='\001') {
    input++;    
    if (!(*input)) break;
    int t=((unsigned char )(*input))&0x1f;
    while (t) {
      output[l++] = ' ';
      if (l>=maxlen) {
        output[maxlen-1]='\0';
        return output;
        }
      t--;
      }
    input++;
    continue;   
    }
  output[l++] = ch;
  if (l>=maxlen) {
    output[maxlen-1]='\0';
    return output;
    }
  input++;
  continue;
  }
output[l++] = '\0';
return output;
}







char *srx_100_to_srx_200(char *output,char *input,int maxlen) {
int l=0;
int extra=0;
if (*input=='#') { output[l]='\0'; return output; }
while (*input) {
  char ch = *input;
  if (ch>='!') {
    output[l++] = ch;
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    input++;
    extra++;
    continue;
    }    
  while (extra) {
    output[l++] = ' ';
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    extra--;
    }
  if (ch=='\t') {
    int t=l%8;
    if (t==0) t +=8;
    while (t) {
      output[l++] = ' ';
      if (l>=maxlen) {
        output[maxlen-1]='\0';
        return output;
        }
      output[l++] = ' ';
      if (l>=maxlen) {
        output[maxlen-1]='\0';
        return output;
        }
      t--;
      }
    input++;
    continue;
    }
  if (ch=='\001') {
    input++;    
    if (!(*input)) break;
    int t=(*input)&0x1f;
    while (t) {
      output[l++] = ' ';
      if (l>=maxlen) {
        output[maxlen-1]='\0';
        return output;
        }
      output[l++] = ' ';
      if (l>=maxlen) {
        output[maxlen-1]='\0';
        return output;
        }
      t--;
      }
    input++;
    continue;   
    }
  output[l++] = ch;
  if (l>=maxlen) {
    output[maxlen-1]='\0';
    return output;
    }
  output[l++] = ' ';
  if (l>=maxlen) {
    output[maxlen-1]='\0';
    return output;
    }
  input++;
  continue;
  }
output[l++] = '\0';
return output;
}





char *srx_200_to_srx_100(char *output,char *input,int maxlen) {
char bigbuf[10000];
srx_expand(bigbuf,input,10000);
input=bigbuf;
int l=0;
int borrow=0;
if (*input=='#') { output[l]='\0'; return output; }
while (*input) {
  char ch = *input;
  if (ch>='!') {
    output[l++] = ch;
    if (l>=maxlen) {
      output[maxlen-1]='\0';
      return output;
      }
    input++;
    borrow++;
    continue;
    }    
  if(borrow) {
    borrow--;
    input++;
    continue;  
    }
  
  output[l++] = ' ';
  if (l>=maxlen) {
    output[maxlen-1]='\0';
    return output;
    }
  input++;
  borrow++;
  continue;      
  } // while input characters
output[l++] = '\0';
return output;
}

