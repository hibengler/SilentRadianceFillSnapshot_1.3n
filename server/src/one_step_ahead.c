#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include "shm_communication.h"
/* $Revision: 1.7 $ */
/* so we play the song right away by calling system */
/* then we try to get other info about the other songs */

#include "music_dat_processing.h"




int main(int argc,char *argv[]) 
{
char command[2000];
char current_command[2000];

init_music_dat_processing();

if (argc != 2) {
  fprintf(stderr,"Usage: %s bash_filename\n where bash_filename runs mplayer\n",argv[0]);
  fprintf(stderr,"\nThis program loads images and titles one step ahead of the files being played\n");
  exit(-1);
  }
FILE *bashf = fopen(argv[1],"r");
if (!bashf) {
  fprintf(stderr,"cannot open bash file\n");
  exit(-1);
  }
      
int first_time=1;
int picture_id=1;
int pid=0;

char text_command[2000];
strcpy(text_command,"t ");

char next_text_command[2000];
strcpy(next_text_command,"t ");

struct song_info current_info;
struct timeval current_start_time;


char next_sr_command[20000];
      char current_title[2000];
      char next_title[2000];
      char full_command[20000];
          double length;

while ( fgets(command,  1999,bashf)) {
  struct song_info next_info;
  

  int l=strlen(command);
  if (l) command[l-1]='\0';
  
  int seconds_left;
  
  
  
  if (!first_time) {
    sleep(1);
 
    char *filename = get_filename_from_command(command);
    
    song_search(filename,&next_info);
   
    parse_title(next_title,&next_info);
    
    sleep(1);  
    if ((next_info.picture[0]&&(next_info.picture_ipc[0])&&(next_info.picture_width[0])&&
               next_info.picture_height[0])) {
      char c[2000];
      fprintf(stderr,"sr_send_binary I %d \"%s\" \"%s\" %s %s 1 &\n",
         picture_id,next_info.picture,next_info.picture_ipc,
	 next_info.picture_width, next_info.picture_height);
      sprintf(c,"sr_send_binary I %d \"%s\" \"%s\" %s %s 1 &",
         picture_id,next_info.picture,next_info.picture_ipc,
	 next_info.picture_width, next_info.picture_height);
      system(c);      
      }
  

    int count = 40;
    do {
      int wstatus;
      int pidw;
      siginfo_t info;
      pidw = waitpid(pid,&wstatus,WNOHANG);  
      if (!pidw) {
        count++;
	if ((count%100)==0) {
          if (text_command[0]) {
  	    int x=silent_radiance_simple_command("default",text_command,strlen(text_command));
	    }
          struct timeval thistime;
          gettimeofday(&thistime,NULL);
	
          fprintf(stderr,"\ncm|%ld.%ld|%lf|%s|\n",thistime.tv_sec,thistime.tv_usec,length,current_command);
	  }
	/* sleep 100 milliseconds  then */
	struct timespec onehundredmilliseconds;
	onehundredmilliseconds.tv_sec = 0;
	onehundredmilliseconds.tv_nsec = 100000000;
	nanosleep(&onehundredmilliseconds,NULL);  
	
	}
      else {  
        pid=0;
	break;
	}
      } while (1);  
    } /* if it is not the first time, and we need to show the current stuff peridoically */
  
  pid=fork();
  if (pid==-1) {
    fprintf(stderr,"error forking\n");
    exit(-1);
    }
  else if (pid==0) {
    fclose(bashf);
    system(command);
    exit(0);
    }
  else {
    gettimeofday(&current_start_time,NULL);
    
    
    if (first_time) {
      char *filename = get_filename_from_command(command);
      
      song_search(filename,&current_info);
    
      if (current_info.length[0]) {length = atof(current_info.length);} else {length=0.;} 
      fprintf(stderr,"\nsm|%ld.%ld|%lf|%s\n",current_start_time.tv_sec,current_start_time.tv_usec,length,command);
      }
    else {
    

      fprintf(stderr,"\nfm|%ld.%ld|%lf|%s|\n",current_start_time.tv_sec,current_start_time.tv_usec,length,current_command);

    
      current_info = next_info;
    
      strcpy(current_title,next_title);
      

      if (current_info.length[0]) {length = atof(current_info.length);} else {length=0.;} 
      fprintf(stderr,"\nsm|%ld.%ld|%lf|%s\n",current_start_time.tv_sec,current_start_time.tv_usec,length,command);
      }	  
	          
    
    if (first_time) {
      first_time=0;
      
      parse_title(current_title,&current_info);
      strcpy(full_command,"t ");
      if (current_title[0]) {
        strcat(full_command,current_title);
	}
      strcpy(text_command,full_command);
      int x=silent_radiance_simple_command("default",full_command,strlen(full_command));
      
      
      if ((current_info.picture[0]&&(current_info.picture_ipc[0])&&(current_info.picture_width[0])&&
                 current_info.picture_height[0])) {
        char c[2000];
        fprintf(stderr,"sr_send_binary I %d \"%s\" \"%s\" %s %s 1 &\n",
           picture_id,current_info.picture,current_info.picture_ipc,  
  	   current_info.picture_width, current_info.picture_height);
        sprintf(c,"sr_send_binary I %d \"%s\" \"%s\" %s %s 1 &",
           picture_id,current_info.picture,current_info.picture_ipc,
	   current_info.picture_width, current_info.picture_height);
        system(c);
	
        sprintf(c,"sr_turn_on_image i %d 3 &",picture_id);
	picture_id = (picture_id)%2+1;
        system(c);      
        }
      }	
    else {
      
      strcpy(full_command,"t ");
      if (current_title[0]) {
        strcat(full_command,current_title);
	}
      strcpy(text_command,full_command);
      int x=silent_radiance_simple_command("default",full_command,strlen(full_command));
      
      if ((current_info.picture[0]&&(current_info.picture_ipc[0])&&(current_info.picture_width[0])&&
               current_info.picture_height[0])) {
        char c[2000];
        sprintf(c,"sr_turn_on_image i %d 3 &",picture_id);
	picture_id = (picture_id)%2+1;
        system(c);      
	}		
      }	      
      
    strcpy(current_command,command);
    }  /* if we spawned a process */
  command[0]='\0';
  }
if (pid) {
    int wstatus;
    waitpid(pid,&wstatus,0);  
  }
if (!first_time) {
  gettimeofday(&current_start_time,NULL);
  fprintf(stderr,"\nfm|%ld.%ld|%lf|%s|\n",current_start_time.tv_sec,current_start_time.tv_usec,length,current_command);
  }
fclose(bashf);
exit(0);  
}

/*
  $Log: one_step_ahead.c,v $
  Revision 1.7  2018/03/17 20:33:19  hib
  Got the text to show every 10 seconds, and cleaned up one_step_beyond alot
  This migh have had some issues with the pictures, actually.

  Revision 1.6  2018/02/01 18:00:29  hib
  changed it up so we can read dennis cds and we capture year, genre

  Revision 1.5  2018/01/21 21:01:33  hib
  guess_info - added all of spooks labels

  Revision 1.4  2018/01/18 08:38:33  hib
  getting stuff ready for u-choose

  
*/
