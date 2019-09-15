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
#ifndef APA_ON
#define APA(a) a
#endif

extern int APA(verbose);
// input / output
extern int APA(usejack);
extern char_t *APA(source_uri);
extern char_t *APA(sink_uri);
// general stuff
extern uint_t APA(samplerate);
extern uint_t APA(buffer_size);
extern uint_t APA(hop_size);
// onset stuff
extern char_t * APA(onset_method);
extern smpl_t APA(onset_threshold);
// pitch stuff
extern char_t * APA(pitch_method);
extern char_t * APA(pitch_unit);
extern smpl_t APA(pitch_tolerance);
// tempo stuff
extern char_t * APA(tempo_method);
// more general stuff
extern smpl_t APA(silence_threshold);
extern uint_t APA(mix_input);

extern uint_t APA(force_overwrite);

// functions defined in utils.c
extern void APA(examples_common_init) (int argc, char **argv,char *client_name,int use_jack);
extern void APA(examples_common_del) (void);
extern void APA(examples_common_process) (aubio_process_func_t process_func,
    aubio_print_func_t print);
int APA(parse_args) (int argc, char **argv);

// internal stuff
extern int APA(blocks);

extern fvec_t *APA(ibuf);
extern fvec_t *APA(obuf);

const char *APA(prog_name);


void APA(usage_aubio) (FILE * stream, int exit_code)
{
  fprintf (stream, "usage_aubio: %s [ options ] \n", APA(prog_name));
  fprintf (stream,
      "       -i      --input            input file\n"
#ifdef PROG_HAS_OUTPUT
      "       -o      --output           output file\n"
#endif
      "       -r      --samplerate       select samplerate\n"
      "       -B      --bufsize          set buffer size\n"
      "       -H      --hopsize          set hopsize\n"
#ifdef PROG_HAS_ONSET
      "       -O      --onset            select onset detection algorithm\n"
      "       -t      --onset-threshold  set onset detection threshold\n"
#endif /* PROG_HAS_ONSET */
#ifdef PROG_HAS_PITCH
      "       -p      --pitch            select pitch detection algorithm\n"
      "       -u      --pitch-unit       select pitch output unit\n"
      "       -l      --pitch-tolerance  select pitch tolerance\n"
#endif /* PROG_HAS_PITCH */
      "       -s      --silence          select silence threshold\n"
#ifdef PROG_HAS_OUTPUT
      "       -m      --mix-input        mix input signal with output signal\n"
      "       -f      --force-overwrite  overwrite output file if needed\n"
#endif
#ifdef PROG_HAS_JACK
      "       -j      --jack             use Jack\n"
#endif
      "       -v      --verbose          be verbose\n"
      "       -h      --help             display this message\n"
      );
  exit (exit_code);
}

int
APA(parse_args) (int argc, char **argv)
{
  const char *options = "hv"
    "i:r:B:H:"
#ifdef PROG_HAS_JACK
    "j"
#endif /* PROG_HAS_JACK */
#ifdef PROG_HAS_OUTPUT
    "o:"
#endif /* PROG_HAS_OUTPUT */
#ifdef PROG_HAS_ONSET
    "O:t:"
#endif /* PROG_HAS_ONSET */
#ifdef PROG_HAS_PITCH
    "p:u:l:"
#endif /* PROG_HAS_PITCH */
    "s:mf";
  int next_option;
  struct option long_options[] = {
    {"help",                  0, NULL, 'h'},
    {"verbose",               0, NULL, 'v'},
    {"input",                 1, NULL, 'i'},
    {"samplerate",            1, NULL, 'r'},
    {"bufsize",               1, NULL, 'B'},
    {"hopsize",               1, NULL, 'H'},
#ifdef PROG_HAS_JACK
    {"jack",                  0, NULL, 'j'},
#endif /* PROG_HAS_JACK */
#ifdef PROG_HAS_OUTPUT
    {"output",                1, NULL, 'o'},
#endif /* PROG_HAS_OUTPUT */
#ifdef PROG_HAS_ONSET
    {"onset",                 1, NULL, 'O'},
    {"onset-threshold",       1, NULL, 't'},
#endif /* PROG_HAS_ONSET */
#ifdef PROG_HAS_PITCH
    {"pitch",                 1, NULL, 'p'},
    {"pitch-unit",            1, NULL, 'u'},
    {"pitch-tolerance",       1, NULL, 'l'},
#endif /* PROG_HAS_PITCH */
    {"silence",               1, NULL, 's'},
    {"mix-input",             0, NULL, 'm'},
    {"force-overwrite",       0, NULL, 'f'},
    {NULL,                    0, NULL, 0}
  };
  APA(prog_name) = argv[0];
  if (argc < 1) {
    APA(usage_aubio) (stderr, 1);
    return -1;
  }
  do {
    next_option = getopt_long (argc, argv, options, long_options, NULL);
    switch (next_option) {
      case 'h':                /* help */
        APA(usage_aubio) (stdout, 0);
        return -1;
      case 'v':                /* verbose */
        APA(verbose) = 1;
        break;
      case 'j':
        APA(usejack) = 1;
        break;
      case 'i':
        APA(source_uri) = optarg;
        break;
      case 'o':
        APA(sink_uri) = optarg;
        break;
      case 'f':                /* force_overwrite flag */
        APA(force_overwrite) = 1;
        break;
      case 'r':
        APA(samplerate) = atoi (optarg);
        break;
      case 'B':
        APA(buffer_size) = atoi (optarg);
        break;
      case 'H':
        APA(hop_size) = atoi (optarg);
        break;
      case 'O':                /*onset method */
        APA(onset_method) = optarg;
        break;
      case 't':                /* threshold value for onset */
        APA(onset_threshold) = (smpl_t) atof (optarg);
        break;
      case 'p':
        APA(pitch_method) = optarg;
        break;
      case 'u':
        APA(pitch_unit) = optarg;
        break;
      case 'l':
        APA(pitch_tolerance) = (smpl_t) atof (optarg);
        break;
      case 's':                /* silence threshold */
        APA(silence_threshold) = (smpl_t) atof (optarg);
        break;
      case 'm':                /* mix_input flag */
        APA(mix_input) = 1;
        break;
      case '?':                /* unknown options */
        APA(usage_aubio) (stderr, 1);
        break;
      case -1:                 /* done with options */
        break;
      default:                 /*something else unexpected */
        fprintf (stderr, "Error parsing option '%c'\n", next_option);
        abort ();
    }
  }
  while (next_option != -1);

  // if unique, use the non option argument as the source
  if ( APA(source_uri) == NULL ) {
    if (argc - optind == 1) {
      APA(source_uri) = argv[optind];
    } else if ( argc - optind > 1 ) {
      errmsg ("Error: too many non-option arguments `%s'\n", argv[argc - 1]);
      APA(usage_aubio) ( stderr, 1 );
    }
  } else if ( argc - optind > 0 ) {
    errmsg ("Error: extra non-option argument %s\n", argv[optind]);
    APA(usage_aubio) ( stderr, 1 );
  }

  // if no source, show a message
  if (APA(source_uri) == NULL) {
#ifdef PROG_HAS_JACK
#if HAVE_JACK
    verbmsg("No input source given, using jack\n");
    APA(usejack) = 1;
#else
    errmsg("Error: no arguments given (and no available audio input)\n");
    APA(usage_aubio) ( stderr, 1 );
#endif /* HAVE_JACK */
#else
    errmsg("Error: no arguments given\n");
    APA(usage_aubio) ( stderr, 1 );
#endif /* PROG_HAS_JACK */
  }

  if ((sint_t)APA(hop_size) < 1) {
    errmsg("Error: got hop_size %d, but can not be < 1\n", APA(hop_size));
    APA(usage_aubio) ( stderr, 1 );
  } else if ((sint_t)APA(buffer_size) < 2) {
    errmsg("Error: got buffer_size %d, but can not be < 2\n", APA(buffer_size));
    APA(usage_aubio) ( stderr, 1 );
  } else if ((sint_t)APA(buffer_size) < (sint_t)APA(hop_size)) {
    errmsg("Error: hop size (%d) is larger than win size (%d)\n",
        APA(hop_size), APA(buffer_size));
    APA(usage_aubio) ( stderr, 1 );
  }

  if ((sint_t)APA(samplerate) < 0) {
    errmsg("Error: got samplerate %d, but can not be < 0\n", APA(samplerate));
    APA(usage_aubio) ( stderr, 1 );
  }

  return 0;
}
