
#include <stdio.h>
#include <stdlib.h>
#include <aubio/aubio.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "glue.h"
#include "portaudio.h"
#include "processor.h"


#define SAMPLE_RATE  (48000)
#define FRAMES_PER_BUFFER (512)
#define NUM_CHANNELS    (2)

/* Select sample format. */
#define PA_SAMPLE_TYPE  paInt16

#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"





#define PROG_HAS_ONSET 1
#define PROG_HAS_TEMPO 1


int soundin_framenumber=0;
int soundin_offset_in_collectionhead=0;
float soundin_factor = 1.;



static     PaStreamParameters  inputParameters,
                        outputParameters;
static     PaStream*           stream;





/* aubio stuff */
 
int aubio_jack=0; /* 1 is standard aubio, 0 uses alsa instead of jack */
aubio_onset_t *aubio_o;
aubio_tempo_t *aubio_tempo;
aubio_pitch_t *aubio_pitchrec;
fvec_t *aubio_pitch;
fvec_t *aubio_onset;
smpl_t aubio_is_onset;
fvec_t *aubio_tempo_out;
unsigned int aubio_flags;
aubio_tempo_t * aubio_tempo;
fvec_t * aubio_tempo_out;
smpl_t aubio_is_beat = 0;
uint_t aubio_is_silence = 0.;


int soundin_init_aubio();








#ifdef _WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

#ifdef dontgotut
struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag = 0;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    tmpres /= 10;  /*convert into microseconds*/
    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}
#endif

















//short dummy=1;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
  const short *rptr = (const short*)inputBuffer;
  short *wptr;
  long framesToCalc;
  long i;
  int finished;

  (void) outputBuffer; /* Prevent unused variable warnings. */
  (void) timeInfo;
  (void) statusFlags;
  (void) userData;
  int input_index=0;
  int samples_left = framesPerBuffer;
  while (samples_left) {
    wptr = collections[collectionhead]+(soundin_offset_in_collectionhead+soundin_offset_in_collectionhead);
    int samples_to_do = samples_left;
    if (samples_to_do > (240-soundin_offset_in_collectionhead)) {
      samples_to_do =  (240-soundin_offset_in_collectionhead);
      }
    if( inputBuffer == NULL )
    {
      for( i=0; i<samples_to_do; i++ ) {
        *wptr++ = SAMPLE_SILENCE;  /* left */
        *wptr++ = SAMPLE_SILENCE;  /* right */
        }
      }
    else {
      for( i=0; i<samples_to_do; i++ )
        {
	float l= *rptr++;  /* left */
	float r= *rptr++;  /* right*/
	float l2,r2;
	l2 = l*soundin_factor;
	r2 = r*soundin_factor;
	if (l2>32767.) l2=32767.;
	if (l2<-32767.) l2=-32767.;
	if (r2>32767.) r2=32767.;
	if (r2<-32767.) r2=-32767.;
        *wptr++ = l2;
        *wptr++ = r2;
        }
      }
    samples_left = samples_left - samples_to_do;
    soundin_offset_in_collectionhead = soundin_offset_in_collectionhead +samples_to_do;
    if (soundin_offset_in_collectionhead==240) {
      collectionstat[collectionhead]->len = 960;
      collectionstat[collectionhead]->frame = soundin_framenumber;
      int v;
      char *vc = (char *)(&v);
      vc[0]='S';
      vc[1]='R';
      vc[2]='0';
      vc[3]='0';
      collectionstat[collectionhead]->version = v; /* sr00 */
      
      collectionhead= (collectionhead+1)%COLLECTION_COUNT;
      if (collection_size<240) collection_size++;
      samples_left,soundin_offset_in_collectionhead=0;
      soundin_framenumber++;
      }
    }  
    finished = 0;
    return finished;
}





int soundin_init() {
// this uses the structure of processor.c, but does not use processor.c directly, as we use aubio to handle processing 
soundin_framenumber=0;
soundin_offset_in_collectionhead=0;

    PaError             err = paNoError;

    init_processor_buffers(); /* processor.c */
    soundin_init_aubio(); 
    
    err = Pa_Initialize();
    if( err != paNoError ) goto done;
    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto done;
    }
    inputParameters.channelCount = 2;                    /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              recordCallback,
              NULL );
    if( err != paNoError ) goto done;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    
    return 0;
done:
    Pa_Terminate();
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return err;
    
 }
 
 
 
int soundin_init_aubio() {

int buffer_size=1024;
int hop_size=240; /*oops - was 512 in many places. */
float aubio_onset_threshold=0;
float aubio_onset_silence_threshold= -90.;
float aubio_silence_threshold= -90.;
float onset_threshold = -90.;

//#define WORLD_soundin_portaudio_default "default"
aubio_o = new_aubio_onset (WORLD_soundin_portaudio_default, buffer_size, hop_size, SAMPLE_RATE);
if (aubio_onset_threshold != 0.)
  aubio_onset_set_threshold (aubio_o, aubio_onset_threshold);
if (aubio_silence_threshold != -90.)
  aubio_onset_set_silence (aubio_o, aubio_silence_threshold);
aubio_onset = new_fvec (1);

aubio_pitch = new_fvec(1);
//#define WORLD_soundin_portaudio_default "default"
aubio_pitchrec = new_aubio_pitch(WORLD_soundin_portaudio_default,buffer_size,hop_size,SAMPLE_RATE);
  /*
   if (pitch_tolerance != 0.)
    aubio_pitch_set_tolerance (o, pitch_tolerance);
  if (silence_threshold != -90.)
    aubio_pitch_set_silence (o, silence_threshold);
  aubio_pitch_set_unit(pitchrec,default);
  */
                    

aubio_tempo_out = new_fvec(2);
//#define WORLD_soundin_portaudio_default "default"
aubio_tempo = new_aubio_tempo(WORLD_soundin_portaudio_default, buffer_size, hop_size, SAMPLE_RATE);
if (onset_threshold != 0.) aubio_tempo_set_threshold (aubio_tempo, onset_threshold);

return 1;
}







/* this is similar to music_broadcast_server.c */
void process_block(fmat_t * imat, fvec_t *obuf,unsigned char *statistics) {
float aubio_silence_threshold= -90.;

aubio_flags = aubio_flags & 0xffffff00;
fvec_t *ibuf;
smpl_t combdata[8000];
{ /* make ibuf form stereo matrix */
  fvec_t ibufx;
  ibuf = &ibufx;
  ibufx.length = imat->length;



  ibufx.data = combdata;
  int i;
  for (i=0;i<imat->length;i++) {
    combdata[i] =(imat->data[0][i]+imat->data[1][i])*0.5;
    }
  } /* block to make a ibuf out of a imat */

aubio_onset_do (aubio_o, ibuf, aubio_onset);
aubio_is_onset = fvec_get_sample(aubio_onset, 0);


/* note - aubio 0.4.3 fixes a bug with this - so you are warned */
aubio_tempo_do (aubio_tempo, ibuf, aubio_tempo_out);
aubio_is_beat = fvec_get_sample (aubio_tempo_out, 0);
if (aubio_silence_threshold != -90.)
  aubio_is_silence = aubio_silence_detection(ibuf, aubio_silence_threshold);

aubio_pitch_do (aubio_pitchrec, ibuf, aubio_pitch);
float the_pitch;
the_pitch = fvec_get_sample(aubio_pitch, 0);

if (obuf) fvec_zeros (obuf); /* dont need for alsa */
if ( aubio_is_beat &&(!aubio_is_silence) ) {
  aubio_flags |= 2;
  }
if ( aubio_is_onset ) {
  aubio_flags |= 1;
   }

float db_level=aubio_db_spl(ibuf);
// PER_FRAME_OVERHEAD (16 bytes)
{ /* 16 extra bytes */
  int microseconds;
  struct timeval tv;
  gettimeofday(&tv,NULL);
  microseconds = tv.tv_sec & 1024; /* 277 hours*/
  microseconds = microseconds*1000000 + (tv.tv_usec);
  
  int l = 0;
  float dummy_pitch=the_pitch;
  unsigned char *pt = (unsigned char *)&the_pitch;
  statistics[l++]=pt[0];                                                        /* pitch */
  statistics[l++]=pt[1];
  statistics[l++]=pt[2];
  statistics[l++]=pt[3];
  statistics[l++]=microseconds&255;                                     /* microseconds - is modulo 1024 seconds */
  statistics[l++]= (microseconds>>8)&255;
  statistics[l++]= (microseconds>>16)&255;
  statistics[l++]= (microseconds>>24)&255;
  pt = (unsigned char *) &db_level;                                    /* db level */
  statistics[l++]=pt[0];
  statistics[l++]=pt[1];
  statistics[l++]=pt[2];
  statistics[l++]=pt[3];
  statistics[l++]=aubio_flags&255;                                           /* flags */
  statistics[l++]= (aubio_flags>>8)&255;
  statistics[l++]= (aubio_flags>>16)&255;
  statistics[l++]= (aubio_flags>>24)&255;
  }   


 

}


/* also borrowed from the server code music_broadcast_server.c */
void buffer_to_imat(unsigned char *b,fmat_t *imat) {
      int i;  
      float one_over_max = 1.0/32768.;
      for (i=0;i<240;i++) {
        unsigned short u;
        short s;
        u = b[0] + (b[1]<<8);
        s=u;
        imat->data[0][i] = ((float)s)* one_over_max;
        imat->data[0][i] *= 0.5;

        b +=2;

        u = b[0] + (b[1]<<8);
        s=u;
        imat->data[1][i] = ((float)s)* one_over_max;
        imat->data[1][i] *= 0.5;
        b +=2;
        }
} /* block to convert to imat format */







int soundin_process_aubio() {
int collection_index;
int sizeback;


int err;
//if( ( err = Pa_IsStreamActive( stream ) ) == 1 ) {
//  Pa_Sleep(5);
//  }

if (collection_size==0) return 0;

/* go backward to size or processed. */ 
int size_back=0;
collection_index=collectionhead-1;
if (collection_index<0) collection_index +=COLLECTION_COUNT;
while (collection_index != collectionhead) {
  if (collectionstat[collection_index]->len<=0) {
    collection_index = (collection_index+1) % COLLECTION_COUNT;
    break;
    }
  size_back++;
  if (size_back ==collection_size) {
    break;
    }
  collection_index=collection_index-1;
  if (collection_index<0) collection_index +=COLLECTION_COUNT;

  }

int count=0;
/* now go forward and process them */
while (collection_index != collectionhead) {

    unsigned char *buffer=(unsigned char *)(collections[collection_index]);
    fmat_t *imat =new_fmat(2,240);
    buffer_to_imat(buffer,imat);
    process_block(imat,NULL,(unsigned char *)(collectionstat[collection_index]->stats_area));
    del_fmat(imat);

    collectionstat[collection_index]->len=-960;

    
    int tail=soundringtail;
    { /* swap the buffers for the collections to the ring buffer */
             short *t;    
             t=(short *)soundring[tail];
             soundring[tail]=collections[collection_index];
             collections[collection_index]=t;
             if (!soundring[tail]) {
               logit("found bug %d \n",collection_index);
               }
             volatile struct processor_stat *tstat;
             tstat=soundstat[tail];soundstat[tail]=collectionstat[collection_index];collectionstat[collection_index]=tstat;
	     collectionstat[collection_index]->frame=0;
	     collectionstat[collection_index]->len=0;


             tail = (tail+1)% SOUNDRING_COUNT;   
             if (soundringhead==tail) {
//???              logit("Soundring overflow!!!!!!!!!!!");
                soundringhead = (soundringhead+1) % SOUNDRING_COUNT;
                }
             if (soundringfirst==tail) {
                soundringfirst = (soundringfirst+1) % SOUNDRING_COUNT;
                }
             soundringcurrent = soundringtail;  // fakes audio processing 
             soundringhead = soundringtail;  // fakes audio processing 
             soundringtail=tail;
             
             collection_size--;
             }
 
    /* maybe do something with the audio output */
    count++;
    collection_index = (collection_index+1) % COLLECTION_COUNT;
    }
return count;
}


 


int soundin_finish() {
    PaError             err = paNoError;
    if (stream) {
      err = Pa_CloseStream( stream );
      stream = NULL;
      }

done:
    Pa_Terminate();
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return err;
}



