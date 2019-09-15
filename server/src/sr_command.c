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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shm_communication.h"

int main(int argc,char *argv[]) {
if (argc<=1) {
  fprintf(stderr,"Usage: sr_command <command> where <command> is one command to run\n");
  exit(1);
  }
char buf[200000];
buf[0]='\0';
int i=1;
char *spaces = "";
while (i<argc) {
  strcat(buf,spaces);
  strcat(buf,argv[i]);
  spaces=" ";
  i++;
  }
int x=silent_radiance_simple_command("default",buf,strlen(buf));
if (x!=0) {
  if (x==-1) {
    fprintf(stderr,"Error: server is not up.\n");
    }
  fprintf(stderr, "Error %d\n",x);
  }
exit(x);
}


