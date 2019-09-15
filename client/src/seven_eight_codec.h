#ifndef SEVEN_EIGHT_CODEC_H
#define SEVEN_EIGHT_CODEC_H




extern  unsigned char seven_to_eight_last_buf[8]; /* the codec cannot always complet the task, so we might buffer up to 8 chars
    It is just because 8 to the hotdog and "7" to the bun */

extern int seven_to_eight_res_count; /* default to = 0;*/
   /* if using this, be sure to reset this if there is a disription in the byte stream */
    
    
    
extern int seven_eight_locked_eight; /* default =0;*/
extern int seven_eight_locked_seven; /* default =0;*/
extern int do_we_know_if_we_are_7_bit(void);
extern int we_are_7_bit(unsigned char *data,int len,char *optional_content_type);  /*an initial cheat so we can detect 7 bit or 8 bit automatically */


/* this code is very tricky. ha! */
extern int seven_to_8(unsigned char *destination,unsigned char *osource,int len7);
/* it remembers state because you cannot always do a multiple of 7 buffers */
/* destination and source can be the same buffer *IF - destination is source-8.  */

extern int seven_to_8_break(char *outbuffer);


extern void reset_seven_eight_low_level(void);
#endif
