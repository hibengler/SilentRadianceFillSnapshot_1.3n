#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "shm_communication.h"
/* $Revision: 1.1 $ */
/* so we play the song right away by calling system */
/* then we try to get other info about the other songs */

#include "music_dat_processing.h"




int main(int argc,char *argv[]) 
{
char command[2000];
char start_command[2000];
char current_command[2000];


int first_time=1;
int picture_id=1;
int pid=0;


int state=0;  // 0 - looking for an sm, 1 - looking for an fm
// in state 0 a fm is ignored


struct song_info current_info;
struct timeval current_start_time;
while ( fgets(command,  1999,stdin)) {
  int l=strlen(command);
  if (l) command[l-1]='\0';

  
  if (state==0) {  
    if (strncmp(command,"fm|",3)==0) {
      }
    if (strncmp(command,"sm|",3)==0) {
      strcpy(start_command,command);
      state=1;
      }
    }
  else if (state==1) {
    if (strncmp(command,"fm|",3)==0) {
      /* make sure fm matches sm */
      /* copy finish_command */
//      call got_one;
      state=0;
      }
    if (strncmp(command,"sm|",3)==0) {
      /* build missing finish command from start command and current start_command */
//      call gotone;
      strcpy(start_command,command);
      state=1;
      }
  }      
if (state==1) {
      /* build missing finish command from start command and current start_command */
//      call gotone;
  }   

exit(0);  
}

/*
  $Log: one_step_ahead_log_to_played_list.c,v $
  Revision 1.1  2018/01/22 12:09:42  hib
  fixing one_step_ahead

  Revision 1.4  2018/01/18 08:38:33  hib
  getting stuff ready for u-choose

  
*/
