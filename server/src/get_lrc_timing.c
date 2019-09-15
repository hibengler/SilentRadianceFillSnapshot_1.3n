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
This tries to get lrc timing, and since timing for lrc is in hundreths of a second, we will do it in 2 packets of 
240 to equal 1 200th *2
......... - nothing
Z - tempo onset
o - onset of beat
tempo onset is estimated after a while - 4 four in a descent time.
*/

#include "aubio_utils.h"
#define PROG_HAS_ONSET 1
#define PROG_HAS_OUTPUT 0
#define PROG_HAS_JACK 1
#define PROG_HAS_TEMPO 1

#include "aubio_parse_args.h"

aubio_onset_t *o;
aubio_wavetable_t *wavetable;
fvec_t *onset;
aubio_tempo_t *tempo;
fvec_t * tempo_out;
smpl_t is_onset=0;
smpl_t is_beat = 0;
uint_t is_silence = 0;
smpl_t is_onseto=0;
smpl_t is_beato = 0;
uint_t is_silenceo = 0;

int process_block(fmat_t *imat, fvec_t *obuf)
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
  
  /* note - aubio 0.4.3 fixes a bug with this - so you are warned */
  aubio_tempo_do (tempo, ibuf, tempo_out);
  is_beat = fvec_get_sample (tempo_out, 0);
  if (silence_threshold != -90.)
    is_silence = aubio_silence_detection(ibuf, silence_threshold);
return 0;
}


int flipit=0;
int position=0;
int position_index;
int position_max_count=4;
float current_s=0;
int had_offset=0;
void process_print (void)
{
  flipit=1-flipit;  /* 100/sec versus 200/sec */
  if (flipit) {
    is_onseto=is_onset;
    is_beato=is_beat;
    is_silenceo=is_silence;
    return;
    }
  else {
    if (is_onseto) is_onset=is_onseto;
    if (is_beato)  is_beat = is_beato;
    if (is_silenceo) is_silence = is_silenceo;
    }
    
  float last_s =  aubio_onset_get_last_s (o);
//  fprintf(stderr,"last_s %f\n",last_s);
  if (had_offset==0){ 
    current_s += 0.005;
    }
  else {
      current_s += 0.005;
      }
  if (is_beat) {
    fprintf(stdout,"|");
    position_index++;
    if (position_index >=position_max_count) {
      position_index=0;
      fprintf(stdout,"\n");
      }
    }
  else 
  if ( is_onset ) {
    fprintf(stdout,".");
  }
  else {
    fprintf(stdout," ");
    }  
}

int main(int argc, char **argv) {
position_max_count = atoi(argv[0]); 
if ((position_max_count >=2) && (
     position_max_count <7)) {
  examples_common_init(argc-1,argv+1,"hi",0);
  }
else {
  position_max_count = 4;
  examples_common_init(argc,argv,"hi",0);
  }
//  char* av[]={"a","172.23.6.255","12937"};
//  char* av[]={"a","172.23.244.255","6969"};
//  char* av[]={"a","10.70.177.182","6969"};
//  char* av[]={"a","10.70.255.255","6969"};
//  char* av[]={"a","10.69.69.255","12937"};
  verbmsg ("using source: %s at %dHz\n", source_uri, samplerate);
  verbmsg ("onset method: %s, ", onset_method);
  verbmsg ("buffer_size: %d, ", buffer_size);
  verbmsg ("hop_size: %d, ", hop_size);
  verbmsg ("silence: %f, ", silence_threshold);
  verbmsg ("threshold: %f\n", onset_threshold);

/* note:
ONSET METHODS
Available methods are:
default  Default distance, currently hfc
Default: 'default' (currently set to hfc)
energy  Energy based distance
This function calculates the local energy of the input spectral frame.
hfc  High-Frequency content
This method computes the High Frequency Content (HFC) of the input
spectral frame. The resulting function is efficient at detecting
percussive onsets.
Paul Masri. Computer modeling of Sound for Transformation and Synthesis of
Musical Signal. PhD dissertation, University of Bristol, UK, 1996.
complex  Complex domain onset detection function
This function uses information both in frequency and in phase to determine
changes in the spectral content that might correspond to musical onsets.
It is best suited for complex signals such as polyphonic recordings.
Christopher Duxbury, Mike E. Davies, and Mark B. Sandler.  Complex domain
onset detection for musical signals. In Proceedings of the Digital Audio
Effects Conference, DAFx-03, pages 90-93, London, UK, 2003.
phase  Phase based onset detection function
This function uses information both in frequency and in phase to determine
changes in the spectral content that might correspond to musical onsets. It
is best suited for complex signals such as polyphonic recordings.
(continues on next page)
5.6.  Command line tools
27

(continued from previous page)
Juan-Pablo Bello, Mike P. Davies, and Mark B. Sandler.  Phase-based note
onset detection for music signals. In Proceedings of the IEEE International
Conference on Acoustics Speech and Signal Processing, pages 441444,
Hong-Kong, 2003.
specdiff  Spectral difference onset detection function
Jonhatan Foote and Shingo Uchihashi. The beat spectrum: a new approach to
rhythm analysis. In IEEE International Conference on Multimedia and Expo
(ICME 2001), pages 881884, Tokyo, Japan, August 2001.
kl  Kulback-Liebler onset detection function
Stephen Hainsworth and Malcom Macleod. Onset detection in music audio
signals. In Proceedings of the International Computer Music Conference
(ICMC), Singapore, 2003.
mkl  Modified Kulback-Liebler onset detection function
Paul Brossier, ``Automatic annotation of musical audio for interactive
systems'', Chapter 2, Temporal segmentation, PhD thesis, Centre for
Digital music, Queen Mary University of London, London, UK, 2006.
specflux  Spectral flux
Simon Dixon, Onset Detection Revisited, in ``Proceedings of the 9th
International Conference on Digital Audio Effects'' (DAFx-06), Montreal,
Canada, 2006.
*/

onset_method="specdiff";
  o = new_aubio_onset (onset_method, buffer_size, hop_size, samplerate);
  if (onset_threshold != 0.)
    aubio_onset_set_threshold (o, onset_threshold);
  if (silence_threshold != -90.)
    aubio_onset_set_silence (o, silence_threshold);

  onset = new_fvec (1);


  tempo_out = new_fvec(2);
  tempo = new_aubio_tempo(tempo_method, buffer_size, hop_size, samplerate);
//  if (onset_threshold != 0.) aubio_tempo_set_threshold (tempo, onset_threshold);

  examples_common_process((aubio_process_func_t)process_block, process_print);
  
  del_aubio_onset (o);
  del_fvec (onset);

  examples_common_del();
  return 0;
}


