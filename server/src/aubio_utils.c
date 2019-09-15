/*
  Copyright (C) 2003-2013 Paul Brossier <piem@aubio.org>

  This file is part of aubio.

  aubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/

/**

  This file includes some tools common to all examples. Code specific to the
  algorithm performed by each program should go in the source file of that
  program instead.

*/

#include "aubio_utils.h"
#ifdef HAVE_JACK
#include "aubio_jackio.h"
#endif /* HAVE_JACK */
//#include "aubio_parse_args.h"

int APA(verbose) = 0;
int APA(usejack) = 0;
// input / output
char_t *APA(sink_uri) = NULL;
char_t *APA(source_uri) = NULL;
// general stuff
uint_t APA(samplerate) = 0;
uint_t APA(buffer_size) = 1024;
uint_t APA(hop_size) = 240;
// onset stuff
char_t * APA(onset_method) = "default";
smpl_t APA(onset_threshold) = 0.0; // will be set if != 0.
// pitch stuff
char_t * APA(pitch_unit) = "default";
char_t * APA(pitch_method) = "default";
smpl_t APA(pitch_tolerance) = 0.0; // will be set if != 0.
// tempo stuff
char_t * APA(tempo_method) = "default";
// more general stuff
smpl_t APA(silence_threshold) = -90.;
uint_t APA(mix_input) = 0;

uint_t APA(force_overwrite) = 0;

//
// internal memory stuff
aubio_source_t *APA(this_source) = NULL;
aubio_sink_t *APA(this_sink) = NULL;
fmat_t *APA(imat);
fvec_t *APA(obuf);

/* settings */
int APA(blocks) = 0;

extern void usage (FILE * stream, int exit_code);
extern int parse_args (int argc, char **argv);

#if HAVE_JACK
aubio_jack_t *jack_setup;
#endif

void APA(examples_common_init) (int argc, char **argv,char *client_name,int use_jack);
void APA(examples_common_del) (void);
void APA(examples_common_process) (aubio_process_func_t process_func,
    aubio_print_func_t print);

void APA(examples_common_init) (int argc, char **argv,char *client_name,int use_jack)
{

  /* parse command line arguments */
  APA(parse_args) (argc, argv);

  if (!APA(usejack)) {
    debug ("Opening files ...\n");
    APA(this_source) = new_aubio_source ((char_t*)APA(source_uri), APA(samplerate), APA(hop_size));
    if (APA(this_source) == NULL) {
      errmsg ("Error: could not open input file %s\n", APA(source_uri));
      exit (1);
    }
    if (APA(samplerate) == 0) {
      APA(samplerate) = aubio_source_get_samplerate(APA(this_source));
    }
    if (APA(sink_uri) != NULL) {
      uint_t sink_exists = (access(APA(sink_uri), F_OK) == 0 );
      if (!APA(force_overwrite) && sink_exists) {
        errmsg ("Error: output file %s already exists, use -f to overwrite.\n",
            APA(sink_uri));
        exit (1);
      }
      APA(this_sink) = new_aubio_sink ((char_t*)APA(sink_uri), APA(samplerate));
      if (APA(this_sink) == NULL) {
        errmsg ("Error: could not create output file %s\n", APA(sink_uri));
        exit (1);
      }
    }
#ifdef HAVE_JACK
  } else {
    if (use_jack) {
      debug ("Jack init ...\n");
      jack_setup = new_aubio_jack (APA(hop_size), 2, 1, 0, 1,client_name);
      APA(samplerate) = aubio_jack_get_samplerate (jack_setup);
      APA(source_uri) = "jack";
      }
    else {
      APA(samplerate)=48000;
      APA(source_uri)="alsa";
      }
#endif
  }
  APA(imat) = new_fmat(2,APA(hop_size)); /* stereo */
  APA(obuf) = new_fvec (APA(hop_size));

}

extern int tempo_hop_size;



void APA(examples_common_del) (void)
{
  del_fmat (APA(imat));
  del_fvec (APA(obuf));
  aubio_cleanup ();
  fflush(stderr);
  fflush(stdout);
}


void APA(examples_common_process) (aubio_process_func_t process_func,
    aubio_print_func_t print)
{

  uint_t read = 0;
  if (APA(usejack)) {

#if HAVE_JACK
    debug ("Jack activation ...\n");
    aubio_jack_activate (jack_setup, process_func);
    debug ("Processing (Ctrl+C to quit) ...\n");
    pause ();
    aubio_jack_close (jack_setup);
#else
    usage (stderr, 1);
    outmsg ("Compiled without jack output, exiting.\n");
#endif

  } else {

    uint_t total_read = 0;
    APA(blocks) = 0;

    do {
      aubio_source_do_multi (APA(this_source), APA(imat), &read);
      process_func (APA(imat), APA(obuf));
//      if (read != 240) {
      if (read ==0) {
        fprintf(stderr,"error read %d\n",read);
	}
      // print to console if verbose or no output given
      if (APA(verbose) || APA(sink_uri) == NULL) {
        print();
      }
      if (APA(this_sink)) {
        aubio_sink_do (APA(this_sink), APA(obuf), APA(hop_size));
      }
      APA(blocks)++;
      total_read += read;
    } while (read == APA(hop_size));

    verbmsg ("read %.2fs (%d samples in %d blocks of %d) from %s at %dHz\n",
        total_read * 1. / APA(samplerate),
        total_read, APA(blocks), APA(hop_size), APA(source_uri), APA(samplerate));

    del_aubio_source (APA(this_source));
    del_aubio_sink   (APA(this_sink));

  }
}

void
APA(send_noteon) (int pitch, int velo)
{
  smpl_t mpitch = floor (aubio_freqtomidi (pitch) + .5);
#if HAVE_JACK
  jack_midi_event_t ev;
  ev.size = 3;
  ev.buffer = malloc (3 * sizeof (jack_midi_data_t)); // FIXME
  ev.time = 0;
  if (APA(usejack)) {
    ev.buffer[2] = velo;
    ev.buffer[1] = mpitch;
    if (velo == 0) {
      ev.buffer[0] = 0x80;      /* note off */
    } else {
      ev.buffer[0] = 0x90;      /* note on */
    }
    aubio_jack_midi_event_write (jack_setup, (jack_midi_event_t *) & ev);
  } else
#endif
  if (velo == 0) {
    verbmsg ("%f\n", APA(blocks) * APA(hop_size) / (float) APA(samplerate));
  } else {
    verbmsg ("%f\t%f\t", mpitch, APA(blocks) * APA(hop_size) / (float) APA(samplerate));
  }
}

