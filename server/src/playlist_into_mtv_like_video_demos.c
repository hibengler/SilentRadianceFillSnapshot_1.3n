#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include "shm_communication.h"

/*
#define TARGET_VIDEO_BITRATE 7700
#define TARGET_AUDIO_BITRATE 256
*/

#define TARGET_VIDEO_BITRATE 4000
//#define TARGET_VIDEO_BITRATE 2000
#define TARGET_AUDIO_BITRATE 256

/* $Revision: 1.1 $ */
/* from revision 1.7 of one_step_ahead 
This reads the playlist and builds videos
Since it runs faster than the playing of songs, it can cruze through and make the videos
And we can rerun it - evey once in a while - to add advertisements to the end
*/
/* so we play the song right away by calling system */
/* then we try to get other info about the other songs */

#include "music_dat_processing.h"



char * handle_quotes(char *buffer,char *title) {
char *j = buffer;
*j++ = '\'';
int ch;
while (ch=*title) {
  if (ch=='\'') { /* '\''*/
    *j++ = '\'';
    *j++ = '\\';
    *j++ = '\'';
    }
  *j++ = *title++;
  }
*j++ = '\'';
*j = '\0';
return buffer;
}



char *directory_from_filename(char *buffer,char *filename) 
// assumes that there is an end single quote and a beginning single quote
{
char *j=buffer;
char *last_slash=NULL;
int ch;
while (ch=*filename) {
  if (ch=='/') {
    last_slash = j;
    }
  *j++ = *filename++;
  }
*j = '\0';
if (last_slash) {
  *last_slash++ = '\'';
  *last_slash = '\0';
  }
return buffer;
}


char *mp4_filename_from_filename(char *buffer,char *filename) 
// assumes that there is an end single quote and a beginning single quote
{
char *j=buffer;
int ch;
while (ch=*filename) {
  *j++ = *filename++;
  }
*j = '\0';
if (j>buffer) {j--;}
while (j>buffer) {
  if ((*j)=='.') break;
  j--;
  }
if (j!=buffer) {
  strcpy(j,".mp4\'");
  }
return buffer;
}


char *jpg_filename_from_filename(char *buffer,char *filename) 
// assumes that there is an end single quote and a beginning single quote
{
char *j=buffer;
int ch;
while (ch=*filename) {
  *j++ = *filename++;
  }
*j = '\0';
if (j>buffer) {j--;}
while (j>buffer) {
  if ((*j)=='.') break;
  j--;
  }
if (j!=buffer) {
  strcpy(j,".jpg\'");
  }
return buffer;
}


int main(int argc,char *argv[]) 
{
char command[2000];


init_music_dat_processing();

if (argc != 2) {
  fprintf(stderr,"Usage: %s bash_filename\n where bash_filename runs mplayer\n",argv[0]);
  fprintf(stderr,"\nThis program makes the music video demos\n");
  exit(-1);
  }
FILE *bashf = fopen(argv[1],"r");
if (!bashf) {
  fprintf(stderr,"cannot open bash file\n");
  exit(-1);
  }
      
int picture_id=1;
int pid=0;

char text_command[2000];
strcpy(text_command,"t ");


struct timeval start_time;


char sr_command[20000];
      char title[2000];
      char full_command[20000];
          double length;
	  char directory[2000];
	  char mp4_filename[2000];
	  char jpg_filename[2000];

while ( fgets(command,  1999,bashf)) {
  struct song_info info;
  
    
  
  int l=strlen(command);
  if (l) command[l-1]='\0';
  
  if (text_command[0]) {
    int x=silent_radiance_simple_command("default",text_command,strlen(text_command));
    }
  
  
  char *filename = get_filename_from_command(command);
  directory_from_filename(directory,filename);
  mp4_filename_from_filename(mp4_filename,filename);
  jpg_filename_from_filename(jpg_filename,filename);
    
  song_search(filename,&info);
   
  parse_title(title,&info);
    
  if (info.length[0]) {length = atof(info.length);} else {length=0.;} 

#ifdef dsghlghjkhflsdf    
    sleep(1);  
    if ((info.picture[0]&&(info.picture_ipc[0])&&(info.picture_width[0])&&
               info.picture_height[0])) {
      char c[2000];
      sprintf(c,"sr_send_binary I %d \"%s\" \"%s\" %s %s 1 &",
         picture_id,info.picture,info.picture_ipc,
	 info.picture_width, info.picture_height);
      system(c);      
      }
#endif

  char ad_file[100];
  int ww=(rand()) &3;
  ww++;
  sprintf(ad_file,"%3.3d",ww);
  
  fprintf(stdout,"srplayer_to_sirad %s >/tmp/a.sirad\n",filename);
  fprintf(stdout,"rm 2>/dev/null a.raw* a.rawrgb\n");
  char quoted_title[2000];
  handle_quotes(quoted_title,title);
  fprintf(stdout,"echo \"\n\n\n\" Part 1 doing %s\n",quoted_title);
    
  fprintf(stdout,"silentradiance_make_demo /tmp/a.sirad 40 15 %s\n",quoted_title);
  fprintf(stdout,"rm 2>/dev/null a.aac a_ns.mp4 a.mp4 a.rgb a.jpg\n");
  
  fprintf(stdout,"dd if=a.rawrgb of=a.rgb bs=691200 count=1\n");
  fprintf(stdout,"convert  -size 480x480 -depth 8 a.rgb a.jpg\n");
  
  
  fprintf(stdout,"cat >>a.rawsound  %s.rawsound\n",ad_file);
  fprintf(stdout,"use_raw_vid_for_layer_background a.rawrgb %s.rgba >>a.rawrgb\n",ad_file);
  

  fprintf(stdout,"echo \"\n\n\n\" Part 2 doing %s\n",quoted_title);
  fprintf(stdout,"avconv </dev/null  -f s16le -ar 48000 -ac 2 -i a.rawsound  -c:a aac -strict experimental "
   " -bsf:a aac_adtstoasc -b:a %dk  a.aac\n",TARGET_AUDIO_BITRATE);
/* note - one pass is here:
avconv -v debug -framerate 30 -f rawvideo -pix_fmt rgb24 -s 480x480 -i a.rawrgb \
  -c:v libx264   -pix_fmt yuv420p -profile:v main -level 3.1 -movflags +faststart -crf 28 a_ns.mp4
  # tried 23,26  ,28
  */
  fprintf(stdout,"avconv </dev/null -y -framerate 30 -f rawvideo -pix_fmt rgb24 -s 480x480 -i a.rawrgb "
    "-c:v libx264  -pass 1  -pix_fmt yuv420p -preset veryslow -b:v %dk -an -profile:v main -level 3.1 "
    "  -movflags +faststart -f mp4 /dev/null\n",TARGET_VIDEO_BITRATE);
  fprintf(stdout,"avconv </dev/null  -framerate 30 -f rawvideo -pix_fmt rgb24 -s 480x480 -i a.rawrgb "
    " -c:v libx264 -pass 2  -pix_fmt yuv420p -preset veryslow  -b:v %dk -an -profile:v main -level 3.1 "
    " -movflags +faststart a_ns.mp4\n",TARGET_VIDEO_BITRATE);
  fprintf(stdout,"avconv </dev/null -i a_ns.mp4 -i a.aac -acodec copy  -bsf:a aac_adtstoasc -vcodec copy a.mp4\n");
  fprintf(stdout,"echo \"\n\n\n\" Part 3 copying %s\n",quoted_title);

  fprintf(stdout,"mkdir -p video/%s\n",directory);
  fprintf(stdout,"cp a.mp4 video/%s\n",mp4_filename);
  fprintf(stdout,"cp a.jpg video/%s\n",jpg_filename);
  fprintf(stdout,"\n\n\n");
  
//	sleep(5);	          
    
    
// title has the tiele - we need to finesse it      
      
  }
fclose(bashf);
exit(0);  
}

/*
  $Log: playlist_into_mtv_like_video_demos.c,v $
  Revision 1.1  2019/05/20 03:42:38  hib
  wip

  Revision 1.2  2019/03/26 11:54:17  hib
  increase the quality

  Revision 1.1  2019/03/19 12:00:06  hib
  ok - need to randomize soson

  Revision 1.7  2018/03/17 20:33:19  hib
  
*/
