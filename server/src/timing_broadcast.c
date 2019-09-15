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
/*

*/

#include "aubio_utils.h"
#define PROG_HAS_ONSET 1
#define PROG_HAS_OUTPUT 1
#define PROG_HAS_JACK 1
#include "aubio_parse_args.h"

aubio_onset_t *o;
aubio_wavetable_t *wavetable;
fvec_t *onset;
smpl_t is_onset;


#include "timing_transmitter.h"

void process_block(fmat_t *imat, fvec_t *obuf)
{
fvec_t *ibuf;
{ /* make ibuf form stereo matrix */
  fvec_t ibufx;
  ibuf = &ibufx;
  ibufx.length = imat->length;
  smpl_t combdata[8000];
  ibufx.data = combdata;
  int i;
  for (i=0;i<imat->length;i++) { 
    combdata[i] =(imat->data[0][i]+imat->data[1][i])*0.5;
    }
  } /* block to make a ibuf out of a imat */

  aubio_onset_do (o, ibuf, onset);
  is_onset = fvec_get_sample(onset, 0);
  if ( !usejack && ! sink_uri ) return;
  fvec_zeros(obuf);
  if ( is_onset ) {
//   broadcast_net(ibuf);
    float s=aubio_db_spl(ibuf);
    beep(s,1);
    aubio_wavetable_play ( wavetable );
//    boop(imat);
  } else {
    aubio_wavetable_stop ( wavetable );
    float s=aubio_db_spl(ibuf);
    beep(s,0);
//    boop(imat);
  }
  if (mix_input)
    aubio_wavetable_do (wavetable, obuf, obuf);
  else
    aubio_wavetable_do (wavetable, obuf, obuf);
}


void process_print (void)
{
  if ( is_onset ) {
    outmsg ("%f\n", aubio_onset_get_last_s (o) );
  }
}

int main(int argc, char **argv) {
  examples_common_init(argc,argv,"timing",1);
//  char* av[]={"a","172.23.6.255","12937"};
//  char* av[]={"a","172.23.244.255","6969"};
//  char* av[]={"a","10.70.177.182","6969"};
//  char* av[]={"a","10.70.255.255","6969"};
//  char* av[]={"a","10.69.69.255","12937"};
  char* av[]={"a","127.0.0.1","12937"};
//  if (argc<=3) {
//    fprintf(stderr,"usage: %s <ip> <port> <secondip> <secondport>\n",argv[0]);
//    exit(-1);
//    }
  strcmp(av[1],argv[1]);
  strcmp(av[2],argv[2]);
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

  wavetable = new_aubio_wavetable (samplerate, hop_size);
  aubio_wavetable_set_freq ( wavetable, 2450.);
  //aubio_sampler_load (sampler, "/archives/sounds/woodblock.aiff");

  examples_common_process((aubio_process_func_t)process_block, process_print);

  del_aubio_onset (o);
  del_aubio_wavetable (wavetable);
  del_fvec (onset);

  examples_common_del();
  return 0;
}



