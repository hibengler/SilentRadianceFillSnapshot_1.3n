#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define STDIN 0  // file descriptor for standard input

unsigned long filenumber = 0l;
unsigned long last_filenumber = 0l;

char url[4000];
#define MAX_NUMBER_OF_PIPES 2



FILE *nextpipefile = NULL;
FILE *pipefile=NULL;
volatile int pipe_file_no;



#define PIPE_CUTOFF_POINT 10485760

#define BUFFER_SIZE 4096

int buffer_len;
unsigned char buffer[BUFFER_SIZE];




time_t start_time;









void open_pipe() {
char calling[20000];
fprintf(stderr,"url is now %s\n",url);
sprintf(calling,"curl --raw -S  -T -  -H \"Transfer-Encoding: chunked\"   "
                "'%s?filename=%16.16ld69%16.16ld'",url,start_time,filenumber);
//sprintf(calling,"curl --raw -S 2>>/dev/null >/dev/null -T -  -H \"Transfer-Encoding: chunked\"   "
//                "'%s?filename=%16.16ld69%16.16ld'",url,start_time,filenumber);
fprintf(stderr,"command: %s\n",calling);
pipefile = popen(calling,"w");
if (!pipefile ) {
  fprintf(stderr,"ERROR - %d\n",errno);
  exit(1);
  }
pipe_file_no = fileno(pipefile);
//  fcntl(pipe_stuff[pipeno].pipe_file_no, F_SETFL, O_NONBLOCK);
}



void open_next_pipe() {
char calling[20000];
fprintf(stderr,"nextpipe url is now %s\n",url);
long fnum2;
fnum2=((filenumber+(PIPE_CUTOFF_POINT-1))/PIPE_CUTOFF_POINT) *PIPE_CUTOFF_POINT;
sprintf(calling,"curl --raw -S  -T -  -H \"Transfer-Encoding: chunked\"   "
                "'%s?filename=%16.16ld69%16.16ld'",url,start_time,fnum2);
fprintf(stderr,"command: %s\n",calling);
nextpipefile = popen(calling,"w");
if (!nextpipefile ) {
  fprintf(stderr,"next ERROR - %d\n",errno);
  exit(1);
  }
//pipe_file_no = fileno(pipefile);
//  fcntl(pipe_stuff[pipeno].pipe_file_no, F_SETFL, O_NONBLOCK);
}


int main(int argc,char *argv[]) {
start_time = time(NULL);
//fcntl(STDIN, F_SETFL, O_NONBLOCK);

if (argc!=2) {
  fprintf(stderr,"Usage: %s url\n  where url is the url to upload to.\n",argv[0]);
  exit(-1);
  }
if (strlen(argv[1])>200) {
  fprintf(stderr,"This url is too long\n");
  exit(-1);
  }
  
strcpy(url,argv[1]);


filenumber = 0l;

fprintf(stderr,"url is %s\n",url);


while (1) {
  int actual_size=BUFFER_SIZE;
  if (filenumber % BUFFER_SIZE) {
    actual_size = BUFFER_SIZE - (filenumber % BUFFER_SIZE);
    }
  buffer_len = fread(&buffer,1,actual_size,stdin);
  if (buffer_len!= actual_size) {
    if (feof(stdin)) {
      fprintf(stderr,"\neof\n");
      exit(1);
      }
    int err;
    if (err = ferror(stdin)) {
      fprintf(stderr,"stdin error %d\n",err);
      clearerr(stdin);
      }
    fprintf(stderr,"\neof\n");
    exit(1);
    } /* if we had an error */
  else {
    if (!pipefile) {
      if (nextpipefile) {
        pipefile=nextpipefile;
	nextpipefile=NULL;
	}
      else {
        open_pipe();
	}
      }
    int written =fwrite(buffer,1,actual_size,pipefile);
    if (written != actual_size) {
      fprintf(stderr,"curl failed\n");
      if(pipefile) pclose(pipefile);      
      }
    last_filenumber = filenumber;
    filenumber += written;
    if (((filenumber +131072) % PIPE_CUTOFF_POINT) ==0) {
      open_next_pipe();
      }
    if ((filenumber % PIPE_CUTOFF_POINT) ==0) {
      if(pipefile) pclose(pipefile);
      pipefile = NULL;
      if (nextpipefile) {
        pipefile=nextpipefile; 
	nextpipefile=NULL;
	}
      else {
        open_pipe();
	}
      }
    }
  }
return(0);
}
