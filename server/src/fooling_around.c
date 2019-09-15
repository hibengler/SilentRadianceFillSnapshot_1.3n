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


#include "aubio_utils.h"
#define PROG_HAS_ONSET 1
#define PROG_HAS_TEMPO 1

#define PROG_HAS_OUTPUT 0
#define PROG_HAS_JACK 1
#include "aubio_parse_args.h"



#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"



aubio_onset_t *o;
aubio_tempo_t *tempo;
fvec_t *onset;
smpl_t is_onset;
fvec_t * tempo_out;


#include "transmitter.h"

OpusEncoder *opusencoder=NULL;



unsigned char real_compressed_buffer1[4000];
unsigned char real_compressed_buffer2[4000];
unsigned char real_compressed_buffer3[4000];
unsigned char real_compressed_buffer4[4000];
unsigned char real_compressed_buffer5[4000];
unsigned char real_compressed_buffer6[4000];
unsigned char *compressed_buffers[6] = {real_compressed_buffer1,real_compressed_buffer2,real_compressed_buffer3,
  real_compressed_buffer4,real_compressed_buffer5,real_compressed_buffer6};
int compressed_length1=0;
int compressed_length2=0;
int compressed_length3=0;
int compressed_length4=0;
int compressed_length5=0;
int compressed_length6=0;
unsigned int frame_number=0;
unsigned int flags=0;


static smpl_t tempo_combdata[8000];
static int tempo_length=0;

int tempo_buffer_size = 1024;
int tempo_hop_size=240;



aubio_wavetable_t *wavetable;

void process_block1(fmat_t * imat, fvec_t *obuf);

void process_block(fmat_t *imat, fvec_t *obuf)
{
flags = flags & 0xffffff00;
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


  aubio_onset_do (o, ibuf, onset);
  is_onset = fvec_get_sample(onset, 0);

#ifdef dfjkdfjldf

  if (tempo_length + imat->length < tempo_hop_size) {
    memcpy(tempo_combdata+tempo_length,combdata,imat->length*sizeof(smpl_t));
    tempo_length += imat->length;
    }
  else {
    /* move tempo_combdata over, add what will fit, then call tempo stuff, then fit the rest */
    memcpy(tempo_combdata+tempo_length,combdata,(tempo_hop_size-tempo_length)*sizeof(smpl_t));
    ibuf->data = tempo_combdata;
    ibuf->length = tempo_hop_size; 
    aubio_tempo_do (tempo, ibuf, tempo_out);
    uint is_beat=0;
    uint_t is_silence=0;
    is_beat = fvec_get_sample (tempo_out, 0);
    if (silence_threshold != -90.)
       is_silence = aubio_silence_detection(ibuf, silence_threshold);

    if (is_beat) {
      flags |=2; /* tempo yeah */
      fprintf(stderr,"b");
      }
    
    memcpy(tempo_combdata,combdata+tempo_hop_size-tempo_length,(imat->length - (tempo_hop_size-tempo_length))*sizeof(smpl_t));
    tempo_length = tempo_length+ imat->length - tempo_hop_size;	
    }
#endif
//  aubio_tempo_do (tempo, ibuf, tempo_out);
    uint is_beat=0;
    uint_t is_silence=0;
//    is_beat = fvec_get_sample (tempo_out, 0);
    if (silence_threshold != -90.)
//       is_silence = aubio_silence_detection(ibuf, silence_threshold);
    if (is_beat) {
      flags |=2; /* tempo yeah */
      fprintf(stderr,"b");
      }
  
    
  if ( is_onset ) {
     flags |= 1;
    fprintf(stderr,",");
     }
   float s=aubio_db_spl(ibuf);
   boop(imat,compressed_length1,compressed_buffers[0],
              compressed_length2,compressed_buffers[1],
	      compressed_length3,compressed_buffers[2],
	      compressed_length4,compressed_buffers[3],
	      compressed_length5,compressed_buffers[4],
	      compressed_length6,compressed_buffers[5],
	      frame_number,s,flags);

{ /* generate opus */
float interleave[10000];
/* so opus does only 240 bytes - so we have 16*4 additional samples we need to add to the packet */
  compressed_length6=compressed_length5;
  compressed_length5=compressed_length4;
  compressed_length4=compressed_length3;
  compressed_length3=compressed_length2;
  compressed_length2=compressed_length1;
  unsigned char *t = compressed_buffers[5];
  compressed_buffers[5] = compressed_buffers[4];
  compressed_buffers[4] = compressed_buffers[3];
  compressed_buffers[3] = compressed_buffers[2];
  compressed_buffers[2] = compressed_buffers[1];
  compressed_buffers[1] = compressed_buffers[0];
  compressed_buffers[0] = t;
  
  
  int i;
  for (i=0;i<imat->length;i++) {
    interleave[i+i] =imat->data[0][i];
    interleave[i+i+1]=imat->data[1][i];
    }
    

  int packet_size=  opus_encode_float(opusencoder,interleave,240,compressed_buffers[0],4000);
  compressed_length1 = packet_size;
  
  } /* opus stuff */
flags = flags <<8;
frame_number++;
//reboop(); /* tried - and no - it doesnt help */
}


void process_print (void)
{
  if ( is_onset ) {
    outmsg ("%f\n", aubio_onset_get_last_s (o) );
  }
}

int main(int argc, char **argv) {
  examples_common_init(argc,argv,"transmitter");
//  char* av[]={"a","172.23.6.255","12937"};
//  char* av[]={"a","172.23.244.255","6969"};
//  char* av[]={"a","10.70.177.182","6969"};
  char* av[]={"a","10.70.255.255","6969"};
//  char* av[]={"a","10.69.69.255","12937"};
//  if (argc<=3) {
//    fprintf(stderr,"usage: %s <ip> <port> <secondip> <secondport>\n",argv[0]);
//    exit(-1);
//    }
 
//  strcpy(av[1],argv[1]);
//  strcpy(av[2],argv[2]);
  fprintf(stderr,"Transmitting on %s port %s\n",av[1],av[2]);
  init_internet(3,av);
  verbmsg ("using source: %s at %dHz\n", source_uri, samplerate);
  verbmsg ("onset method: %s, ", onset_method);
  verbmsg ("buffer_size: %d, ", buffer_size);
  verbmsg ("hop_size: %d, ", hop_size);
  verbmsg ("silence: %f, ", silence_threshold);
  verbmsg ("threshold: %f\n", onset_threshold);

  o = new_aubio_onset (onset_method, buffer_size, hop_size, samplerate);
  if (onset_threshold != 0.)
    aubio_onset_set_threshold (o, onset_threshold);
  if (silence_threshold != -90.)
    aubio_onset_set_silence (o, silence_threshold);

  onset = new_fvec (1);
  
   tempo_buffer_size = 1024;
   tempo_hop_size=240;
  verbmsg ("tempo method: %s, ", tempo_method);
  verbmsg ("buffer_size: %d, ", tempo_buffer_size);
  verbmsg ("hop_size: %d, ", tempo_hop_size);
  verbmsg ("threshold: %f\n", onset_threshold);


  tempo_out = new_fvec(2);
//  tempo = new_aubio_tempo(tempo_method, tempo_buffer_size, tempo_hop_size, samplerate);
//  if (onset_threshold != 0.) aubio_tempo_set_threshold (tempo, onset_threshold);
      

  wavetable = new_aubio_wavetable (samplerate, hop_size);
  aubio_wavetable_set_freq ( wavetable, 2450.);
  //aubio_sampler_load (sampler, "/archives/sounds/woodblock.aiff");
  //aubio_sampler_load (sampler, "/archives/sounds/woodblock.aiff");
  
  
  { /* opus */
    int err;
    int application=OPUS_APPLICATION_RESTRICTED_LOWDELAY;
    opusencoder = opus_encoder_create((opus_int32)(48000),2,application,&err); /* it was 44100, but android phones failed if not 48k
       for the superpowered stuff */
    if (err != OPUS_OK)
       {
          fprintf(stderr, "Cannot create encoder: %s\n", opus_strerror(err));
          return EXIT_FAILURE;
       }
    opus_encoder_ctl(opusencoder, OPUS_SET_BITRATE(100000));
    opus_encoder_ctl(opusencoder,OPUS_SET_COMPLEXITY(5));
    opus_encoder_ctl(opusencoder,OPUS_SET_SIGNAL(OPUS_APPLICATION_RESTRICTED_LOWDELAY));
    
    compressed_buffers[0]=real_compressed_buffer1;
    compressed_buffers[1]=real_compressed_buffer2;
    compressed_buffers[2]=real_compressed_buffer3;
    compressed_buffers[3]=real_compressed_buffer4;
    compressed_buffers[4]=real_compressed_buffer5;
    compressed_buffers[5]=real_compressed_buffer6;
    compressed_length1=0;
    compressed_length2=0;
    compressed_length3=0;
    compressed_length4=0;
    compressed_length5=0;
    compressed_length6=0;
    frame_number=0;
    flags=0;
    }

  examples_common_process((aubio_process_func_t)process_block, process_print);

  del_aubio_tempo (tempo);
  del_aubio_onset (o);
  del_fvec (onset);
  del_fvec (tempo_out);

  examples_common_del();
  
  return 0;
}









aubio_tempo_t * tempo;
fvec_t * tempo_out;
smpl_t is_beat = 0;
uint_t is_silence = 0.;

void process_block1save(fmat_t * imat, fvec_t *obuf) {

  
flags = flags & 0xffffff00;
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
    
  aubio_tempo_do (tempo, ibuf, tempo_out);
  is_beat = fvec_get_sample (tempo_out, 0);
  if (silence_threshold != -90.)
    is_silence = aubio_silence_detection(ibuf, silence_threshold);
  if ( !usejack && ! sink_uri ) return;
  fvec_zeros (obuf);
  if ( is_beat && !is_silence ) {
    aubio_wavetable_play ( wavetable );
    outmsg("%f\n", aubio_tempo_get_last_s(tempo) );
  } else {
    aubio_wavetable_stop ( wavetable );
  }
  if (mix_input)
    aubio_wavetable_do (wavetable, ibuf, obuf);
  else
    aubio_wavetable_do (wavetable, obuf, obuf);
}




void process_block1(fmat_t * imat, fvec_t *obuf) {

  
flags = flags & 0xffffff00;
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
    
  aubio_onset_do (o, ibuf, onset);
  is_onset = fvec_get_sample(onset, 0);
  
  aubio_tempo_do (tempo, ibuf, tempo_out);
  is_beat = fvec_get_sample (tempo_out, 0);
  if (silence_threshold != -90.)
    is_silence = aubio_silence_detection(ibuf, silence_threshold);
  if ( !usejack && ! sink_uri ) return;
  fvec_zeros (obuf);
  if ( is_beat && !is_silence ) {
    aubio_wavetable_play ( wavetable );
    fprintf(stderr,"B");
  } else {
    aubio_wavetable_stop ( wavetable );
  }
  if (is_onset) {
    fprintf(stderr,".");    
    }
  if (mix_input)
    aubio_wavetable_do (wavetable, ibuf, obuf);
  else
    aubio_wavetable_do (wavetable, obuf, obuf);
}

void process_print1 (void) {
  if ( is_beat && !is_silence ) {
    outmsg("f %f\n", aubio_tempo_get_last_s(tempo) );
  }
}






int main2(int argc, char **argv) {
  // override general settings from utils.c


  char* av[]={"a","10.70.255.255","6969"};
  init_internet(3,av);





  examples_common_init(argc,argv,"transmitter");

  verbmsg ("using source: %s at %dHz\n", source_uri, samplerate);

  verbmsg ("tempo method: %s, ", tempo_method);
  verbmsg ("buffer_size: %d, ", buffer_size);
  verbmsg ("hop_size: %d, ", hop_size);
  verbmsg ("threshold: %f\n", onset_threshold);
  o = new_aubio_onset (onset_method, buffer_size, hop_size, samplerate);
  if (onset_threshold != 0.)
    aubio_onset_set_threshold (o, onset_threshold);
  if (silence_threshold != -90.)
    aubio_onset_set_silence (o, silence_threshold);
  onset = new_fvec (1);


  tempo_buffer_size = 1024;
  tempo_hop_size = 240;
  verbmsg ("tempo_buffer_size: %d, ", tempo_buffer_size);
  verbmsg ("tempo_hop_size: %d, ", tempo_hop_size);
  tempo_out = new_fvec(2);
  tempo = new_aubio_tempo(tempo_method, tempo_buffer_size, tempo_hop_size, samplerate);
  if (onset_threshold != 0.) aubio_tempo_set_threshold (tempo, onset_threshold);

  wavetable = new_aubio_wavetable (samplerate, hop_size);
  aubio_wavetable_set_freq ( wavetable, 2450.);
  //aubio_sampler_load (sampler, "/archives/sounds/woodblock.aiff");

  
  { /* opus */
    int err;
    int application=OPUS_APPLICATION_RESTRICTED_LOWDELAY;
    opusencoder = opus_encoder_create((opus_int32)(48000),2,application,&err); /* it was 44100, but android phones failed if not 48k
       for the superpowered stuff */
    if (err != OPUS_OK)
       {
          fprintf(stderr, "Cannot create encoder: %s\n", opus_strerror(err));
          return EXIT_FAILURE;
       }
    opus_encoder_ctl(opusencoder, OPUS_SET_BITRATE(100000));
    opus_encoder_ctl(opusencoder,OPUS_SET_COMPLEXITY(5));
    opus_encoder_ctl(opusencoder,OPUS_SET_SIGNAL(OPUS_APPLICATION_RESTRICTED_LOWDELAY));
    
    compressed_buffers[0]=real_compressed_buffer1;
    compressed_buffers[1]=real_compressed_buffer2;
    compressed_buffers[2]=real_compressed_buffer3;
    compressed_buffers[3]=real_compressed_buffer4;
    compressed_buffers[4]=real_compressed_buffer5;
    compressed_buffers[5]=real_compressed_buffer6;
    compressed_length1=0;
    compressed_length2=0;
    compressed_length3=0;
    compressed_length4=0;
    compressed_length5=0;
    compressed_length6=0;
    frame_number=0;
    flags=0;
    }

//  examples_common_process((aubio_process_func_t)process_block, process_print);

  examples_common_process((aubio_process_func_t)process_block1,process_print1);

  del_aubio_tempo(tempo);
  del_aubio_wavetable (wavetable);
  del_fvec(tempo_out);

  examples_common_del();
  return 0;
}


