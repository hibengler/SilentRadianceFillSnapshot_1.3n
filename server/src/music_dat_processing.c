#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "shm_communication.h"
#include <time.h>
#include <stdlib.h>

/* $Revision: 1.5 $ */
/* so we play the song right away by calling system */
/* then we try to get other info about the other songs */

#include "music_dat_processing.h"

//      some filename.mp3|unewscaped_filename|
//featured artist|sound_recording_title|isrc|album|title|marketing_label|track|song|ipc_s

struct song_info *song_search1(char *filename,struct song_info *pinfo) {
char fnamem[20000];
strcpy(fnamem,filename);
strcat(fnamem,"|");
int checklen=strlen(fnamem);

FILE *ifil=fopen("songs.dat","r");
if (ifil) {
  char line[20000];
  while ( fgets(line,  19999,ifil)) {
    if (strncmp(line,fnamem,checklen)==0) {
      strcpy(pinfo->search_line,line);
      char *p=pinfo->search_line;
      char *o=p;
      char *q=p;

      
      
GLOBB(pinfo->filename);      
GLOBB(pinfo->unescaped_filename);
GLOBB(pinfo->length);
GLOBB(pinfo->upc);
GLOBB(pinfo->featured_artist);
GLOBB(pinfo->sound_recording_title);
GLOBB(pinfo->isrc);
GLOBB(pinfo->album_title);
GLOBB(pinfo->marketing_label);
GLOBB(pinfo->track);
GLOBB(pinfo->title);
GLOBB(pinfo->year);
GLOBB(pinfo->genre);
GLOBB(pinfo->extra1);
GLOBB(pinfo->extra2);
GLOBB(pinfo->picture);
GLOBB(pinfo->picture_ipc);
GLOBB(pinfo->picture_width);
GLOBB(pinfo->picture_height);
GLOBB(pinfo->sha256);

      fclose(ifil);
      return pinfo;
      }    
    }      
  fclose(ifil);
  }
return NULL;
}



void song_add(char *filename) {
FILE *ofil=fopen("songs.dat","a");
if (!ofil) {
  fprintf(stderr,"cannot open dongs.dat for writing\n");
  exit(-1);
  }
fprintf(ofil,"%s|",filename);
char uesfilename[20000];

char uuecmd[2000];
sprintf(uuecmd,"echo %s",filename);
FILE *pf1a=popen(uuecmd,"r");
if (pf1a) {
  *uesfilename='\0';
  if (fgets(uesfilename,1999,pf1a));
  int l=strlen(uesfilename); if (l) uesfilename[l-1]='\0';
  fclose(pf1a);
  }
fprintf(ofil,"%s|",uesfilename);

double sec = get_seconds(filename);
fprintf(ofil,"%lf|",sec);

int use_gettags=0;
int use_id3info=0;
{
  int l=strlen(uesfilename);
  char *n = uesfilename + strlen(uesfilename);
  if (l>4) {
    n -=4;
    if ((strcmp(n,".mp3")==0)||(strcmp(n,".MP3")==0)) {
      use_id3info=1;
      }
    else if   ((strcmp(n,".OGG")==0)||(strcmp(n,".ogg")==0)) {
      use_gettags=1;
      }
    else if   ((strcmp(n,"flac")==0)||(strcmp(n,"FLAC")==0)) {
      use_gettags=1;
      }
      
    }
  }
  
    
char guess_command[2000];
char guess_data[2000];
if (use_id3info) {
  sprintf(guess_command,"{ echo %s; id3info %s; } | snobol4 $SR_BIN/guess_info.sno",filename,filename);
  }
if (use_gettags) {
  sprintf(guess_command,"{ echo %s; gettags %s; } | snobol4 $SR_BIN/guess_info.sno",filename,filename);
  }
else {
  sprintf(guess_command,"echo %s | snobol4 $SR_BIN/guess_info.sno",filename);
  }
fprintf(stderr,"\n%s\n",guess_command);
FILE *pf1s=popen(guess_command,"r");
*guess_data='\0';
if (pf1s) {
  if (fgets(guess_data,1999,pf1s)) {
      int l=strlen(guess_data); if (l) guess_data[l-1]='\0';
      }
  fclose(pf1s);
  } /* snobol guesser for label_cd_id,track_number | song_artist | song name | blank | CD or EP name | marketing label | track */
if (guess_data[0]) {
  fprintf(ofil,"%s|",guess_data);
  }
else {
  fprintf(ofil,"guess%c%c%c%c%c|||||||||||",rand()%26+'A',
    rand()%26+'A',rand()%26+'A',rand()%26+'A',rand()%26+'A');
  }

/* picture stuff */
fprintf(ofil,"||||");

char shacmd[2000];
char shacode[20000];
shacode[0]='\0';
sprintf(shacmd,"sha256sum -b %s",filename);
FILE *pf2 = popen(shacmd,"r");
if (pf2) {
  int c=fread(shacmd,1,64,pf2);
  shacmd[c]='\0';
  fclose(pf2);
  }
fprintf(ofil,"%s|\n",shacmd);
fclose(ofil);
}
  
    
struct song_info *song_search(char *filename,struct song_info *pinfo) {
struct song_info *s1;
s1 =song_search1(filename,pinfo);
if (!s1) {
  song_add(filename);
  s1 =song_search1(filename,pinfo);
  if (!s1) {
    fprintf(stderr,"song_add failed darn\n");
    exit(-1);
    }
  }  
return pinfo;
}



//command_arg = get_command_arg(command);

double get_seconds(char *filename) {
char idtext[2000];
sprintf(idtext,"mplayer 2>/dev/null -vo null -ao null -frames 0 -identify %s",filename);
double sec=0.;
FILE *pf = popen(idtext,"r");
if (pf) {
  char idline[2000];
  while (fgets(idline,1999,pf)) {
    int l=strlen(idline);
    if (l) idline[l-1]='\0';
    if (strncmp(idline,"ID_LENGTH=",10)==0) {
      sscanf(idline+10,"%lf",&sec);
      fclose(pf);
      return sec;
      }
    }
  }
return sec;
}


void parse_title(char *current_title,struct song_info *pinfo) {
if (pinfo->title[0]) {
  char *a=current_title;
  char *b=pinfo->title;
  while (*b) {
    if ((*b)=='\\') {
      if (b[1]=='n') {
        *a++ = '\n';
	b +=2;
	continue;
	}
      }
    *a++ = *b++;
    }
  *a++='\0';
  }
else {
  char *comma = "";
  if (pinfo->featured_artist) {
    strcpy(current_title,pinfo->featured_artist);
    comma = "\n";
    }
  if (pinfo->sound_recording_title) {
    strcat(current_title,comma);
    strcat(current_title,pinfo->sound_recording_title);
    }
  if (pinfo->album_title) {
    strcat(current_title,comma);
    strcat(current_title,pinfo->album_title); 
    if (pinfo->track) {
      strcat(current_title," ");
      if (pinfo->track[0]=='0') {
        strcat(current_title,pinfo->track+1);     
	}
      else {
        strcat(current_title,pinfo->track);     
	}
      }
    }
  if (pinfo->marketing_label) {
    strcat(current_title,comma);
    strcat(current_title,pinfo->marketing_label);
    }
  if (current_title[0]=='\0') {
    strcpy(current_title,pinfo->unescaped_filename);
    char *b = current_title+strlen(current_title)-1;
    if (((b-1)>=current_title)&&(b[-1]=='.')) b[-1]='\0';
    else if (((b-2)>=current_title)&&(b[-2]=='.')) b[-2]='\0';
    else if (((b-3)>=current_title)&&(b[-3]=='.')) b[-3]='\0';
    else if (((b-4)>=current_title)&&(b[-4]=='.')) b[-4]='\0';
    else if (((b-5)>=current_title)&&(b[-5]=='.')) b[-5]='\0';
    }
  if (current_title[0]=='\0') {    
    strcpy(current_title,"n/a");
    }
  }
}

   
char *get_filename_from_command(char *command) {
while ((command)&&(command[0]!= '\0')&&(command[0] != ' '))
  command++;
if ((*command)==' ') command++;
return command;
}   

void init_music_dat_processing() {
time_t t=time(NULL);
srandom((unsigned int)t);
}


/*
  $Log: music_dat_processing.c,v $
  Revision 1.5  2018/12/01 16:27:01  hib
  fixing radio servers to show song name

  Revision 1.4  2018/03/29 00:48:13  hib
  Getting the greatful dead tapes set up

  Revision 1.3  2018/02/01 20:48:20  hib
  Got dennis's songs to work.

  Revision 1.2  2018/02/01 18:00:29  hib
  changed it up so we can read dennis cds and we capture year, genre

  Revision 1.1  2018/01/22 12:09:42  hib
  fixing one_step_ahead

  Revision 1.4  2018/01/18 08:38:33  hib
  getting stuff ready for u-choose

  
*/
