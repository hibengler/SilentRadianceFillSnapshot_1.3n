


struct song_info {
  char search_line[20000];
  char *filename;
  char *unescaped_filename;
  char *length;
  char *upc;
  char *featured_artist;
  char *sound_recording_title;
  char *isrc;
  char *album_title;
  char *marketing_label;
  char *track;
  char *title;
  char *year; //
  char *genre;
  char *extra1;
  char *extra2; //
  char *picture;
  char *picture_ipc;
  char *picture_width;
  char *picture_height;
  char *sha256;
};

//      some filename.mp3|unewscaped_filename|
//featured artist|sound_recording_title|isrc|album|title|marketing_label|track|song|ipc_s

struct song_info *song_search1(char *filename,struct song_info *pinfo) ;


#define GLOBB(ll)      \
      o=p; \
      while ((*o)&&(*o != '|')) o++;	\
      if (*o=='|') 		\
        {*o='\0';	\
	 q=o+1;	\
	 }		\
	else {		\
	  q=o;		\
	  }		\
      (ll)=p;\
      p=q; 


void song_add(char *filename) ;

struct song_info *song_search(char *filename,struct song_info *pinfo) ;

double get_seconds(char *filename) ;


void parse_title(char *current_title,struct song_info *pinfo) ;
char *get_filename_from_command(char *command) ;

void init_music_dat_processing();
