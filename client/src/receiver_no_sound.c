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

 
//define SR_NOSOUND
#include <stdio.h>
#include "glue.h"
#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif
#include "processor.h"
#include "packet_summary.h"
#include <math.h>



typedef struct {
    double r;       //  [0, 1]
    double g;       //  [0, 1]
    double b;       //  [0, 1]
} rgb;                     
																		       
																		       
//   inconsistent pattern with rgbToHSV (like this approach better) but it's basically working for now http$
rgb hsv2rgb(double H, double S, double V)
{
    rgb RGB;
    double /* H = HSV.h, S = HSV.s, V = HSV.v, */
            P, Q, T,
            fract;
     H *= 360;//adapt for 0-1 hue input
    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);
 
    P = V*(1. - S);
    Q = V*(1. - S*fract);   
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
        RGB = (rgb){.r = V, .g = T, .b = P};
    else if (1. <= H && H < 2.)
        RGB = (rgb){.r = Q, .g = V, .b = P};   
    else if (2. <= H && H < 3.)
        RGB = (rgb){.r = P, .g = V, .b = T};
    else if (3. <= H && H < 4.)
        RGB = (rgb){.r = P, .g = Q, .b = V};   
    else if (4. <= H && H < 5.)
        RGB = (rgb){.r = T, .g = P, .b = V};
    else if (5. <= H && H < 6.)
        RGB = (rgb){.r = V, .g = P, .b = Q};
    else
        RGB = (rgb){.r = 0., .g = 0., .b = 0.};

    return RGB;
}

     





int pitch_to_rgb_value(float pitch) {
   float plog = logf(pitch)*1.44269504088884957607f;  /* first constant is 1/log(2) because ln(x)/ln(2)...*/
    int plogi = (int) plog;
    pitch = plog- (float)plogi;
/*
Old way:
    while (pitch>440.) pitch  = pitch*0.125;
    while (pitch<=220.) pitch  = pitch*2.0;   
pitches are now in the range of 256 to 511 - subtract 255 and we can use the pitch colors to set the color
*/
    
    
      
    rgb xrgb;
    xrgb=hsv2rgb(pitch,1.f,0.90f);
    int r,g,b;
    r = (xrgb.r*255);
    g = (xrgb.g*255);
    b = (xrgb.b*255);
    if (r>255) r=255; if (r<0) r=0;    
    if (g>255) g=255; if (g<0) g=0;    
    if (b>255) b=255; if (b<0) b=0;    

    return (r<<16)|(g<<8)|b;
}





int last_frame_number=0;

int show_stat(int fn) {
int index = (packet_summary.start_index + fn - packet_summary.start_frame) % PACKET_SUMMARY_SIZE;
if (index<0) index += PACKET_SUMMARY_SIZE;
struct packet *p = packet_summary.packets+index;


int commanded_background_color;
{
    int r,g,b;
    float *color=packet_summary.commanded_background_color;
    r = (color[0]*255);
    g = (color[1]*255);
    b = (color[2]*255);
    if (r>255) r=255; if (r<0) r=0;    
    if (g>255) g=255; if (g<0) g=0;    
    if (b>255) b=255; if (b<0) b=0;    

    commanded_background_color = (r<<16)|(g<<8)|b;
    }
    
    
if (fn==last_frame_number+1) {
  fprintf(stdout,"current	%d %ld %ld %d %f %6.6x ",fn,
  							packet_summary.current_sample_time.tv_sec,
                                                        packet_summary.current_sample_time.tv_usec,
							packet_summary.beat_interval_in_milliseconds,
							packet_summary.where_in_tempo,
							commanded_background_color);

  }
else {
  fprintf(stdout,"old	%d 0 0 0 0 000000 ",fn);
  }
if (p->has_statistics) {
  fprintf(stdout,"%f %d %f %x %d %d %6.6x\n"
  ,p->pitch,p->microseconds,p->db_level,p->folded_flags,p->has_beat,p->has_onset,pitch_to_rgb_value(p->pitch));
  }
else {
  fprintf(stdout,"0 0 0 0 0 0 000000\n");
  }
}


void show_stats() {
if (!((packet_summary.state==PACKET_STATE_GOOD)||(packet_summary.state==PACKET_STATE_WARNING))) { return;}
int frame_number = packet_summary.current_frame;
if (packet_summary.current_frame == last_frame_number) return;
if (packet_summary.current_frame > last_frame_number+1) {
  if (packet_summary.no_gap_frame > last_frame_number) {
    last_frame_number = packet_summary.no_gap_frame;
    }
  }

int i;
for (i=last_frame_number;i<=packet_summary.current_frame;i++) {
  show_stat(i);
  }
last_frame_number=packet_summary.current_frame;

}









/* current transmissison is adding sr01 to sr00 16 bytes.  bad */
int show_binary_sr00() {  
int i=0;
int frame=0;
int frame_count = 0;



int j = recvStringLen-12;
     unsigned int frame_number_start = packetbuffer[j] + (packetbuffer[j+1]<<8)
	        + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j=j+4;
     
     unsigned int header_frame_number = packetbuffer[j] + (packetbuffer[j+1]<<8)
	        + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j = j + 4;
     
for (i=0;i<recvStringLen;) {
  int length=  (packetbuffer[i+1]<<8) + packetbuffer[i];
  i +=2;
  if (length) {
    frame_count++;
    i += length+16;
    }
  else {
    break;
    }
  }
     
     fprintf(stdout," frame %d holds frames from %d to %d  with %d packets",header_frame_number,
           frame_number_start-frame_count+1,frame_number_start,frame_count);
 
     fprintf(stdout," %c%c%c%c\n",packetbuffer[j],packetbuffer[j+1],packetbuffer[j+2],packetbuffer[j+3]);
      j += 4;
     
     
int current_frame = frame_number_start;
for (i=0;i<recvStringLen;) {
  int length=  (packetbuffer[i+1]<<8) + packetbuffer[i];
  if (length) {
    fprintf(stdout," frame %d length %d ",current_frame,length);
    }
  else {
    fprintf(stdout," frame %d header %d ",frame_number_start,header_frame_number);
    }
  i +=2;
  if (length) {
    frame_count++;
    int j;
    for (j=i;j<i+length;j++) {
      if (j>=recvStringLen) {
        fprintf(stdout,"AAAAA-out of space as pos %d\n",j);
        break;
        }
      fprintf(stdout,"%2.2x",packetbuffer[j]);
      }
    i=j;
    fprintf(stdout,"\n");
    
     float dummy_pitch=0.0f;
     unsigned char *pt = (unsigned char *)&dummy_pitch;
     pt[0] = packetbuffer[j++] ;
     pt[1] = packetbuffer[j++];
     pt[2] = packetbuffer[j++];
     pt[3] = packetbuffer[j++];
     
     unsigned int microseconds = packetbuffer[j] + (packetbuffer[j+1]<<8)
	        + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j = j + 4;
     int dblevel1 =  packetbuffer[j] + (packetbuffer[j+1]<<8)
             + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     float dblevel = (float)(dblevel1)*0.001;
     j = j + 4;
     unsigned int flags = packetbuffer[j] + (packetbuffer[j+1]<<8)
             + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j = j + 4;
    
     fprintf(stdout," frame %d pitch %f  microseconds %d dblevel %f  flags %x\n",
           current_frame,dummy_pitch,microseconds,dblevel,flags);
    current_frame--;

    i=j;
    }    
  else {
    int l2;
    
  wowwow: 
  
    
    if (recvStringLen-i>40) { 
        l2=packetbuffer[i];i++;
        fprintf(stdout,"\n  command %d %c ",l2,packetbuffer[i]);
      int j = i;
      int ll = l2;
      for (j=j;j<i+ll;j++) {
        fprintf(stdout,"%2.2x",packetbuffer[j]);
	if ((j-i)%64 ==63) fprintf(stdout,"\n		");
	}
      i=i+l2;
      goto wowwow;
      }
    else {
  
    /* checksum */
//#define WORLD__checksum_ " checksum "
    printf(WORLD__checksum_);
    int j;
    for (j=i;j<i+12;j++) {
      if (j>=recvStringLen) {
        fprintf(stdout,"AAAAA-out of space as pos %d\n",j);
        break;
        }
      fprintf(stdout,"%2.2x",packetbuffer[j]);
      }
    break;
    i=j;
    }
        
    }
  }    
fprintf(stdout,"\n");
}





/* current transmissison is adding sr01 to sr00 16 bytes.  bad */
int show_binary_sr01() {  
int i=0;
int frame=0;
int frame_count = 0;

int j = recvStringLen-28;
     unsigned int frame_number_start = packetbuffer[j] + (packetbuffer[j+1]<<8)
	        + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j=j+4;
     float dummy_pitch=0.0f;
     unsigned char *pt = (unsigned char *)&dummy_pitch;
     pt[0] = packetbuffer[j++] ;
     pt[1] = packetbuffer[j++];
     pt[2] = packetbuffer[j++];
     pt[3] = packetbuffer[j++];
     
     unsigned int header_frame_number = packetbuffer[j] + (packetbuffer[j+1]<<8)
	        + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j = j + 4;
     unsigned int microseconds = packetbuffer[j] + (packetbuffer[j+1]<<8)
	        + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j = j + 4;
     int dblevel1 =  packetbuffer[j] + (packetbuffer[j+1]<<8)
             + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     float dblevel = (float)(dblevel1)*0.001;
     j = j + 4;
     unsigned int flags = packetbuffer[j] + (packetbuffer[j+1]<<8)
             + (packetbuffer[j+2]<<16) + (packetbuffer[j+3]<<24);
     j = j + 4;
     
     
for (i=0;i<recvStringLen;) {
  int length=  (packetbuffer[i+1]<<8) + packetbuffer[i];
  i +=2;
  if (length) {
    frame_count++;
    i += length;
    }
  else {
    break;
    }
  }
     
     fprintf(stdout," frame %d pitch %f  microseconds %d dblevel %f  flags %x  from %d to %d packets %d",
           header_frame_number,dummy_pitch,microseconds,dblevel,flags,frame_number_start-frame_count+1,frame_number_start,frame_count);
 
     fprintf(stdout," %c%c%c%c\n",packetbuffer[j],packetbuffer[j+1],packetbuffer[j+2],packetbuffer[j+3]);
      j += 4;

      int current_frame = frame_number_start;      
for (i=0;i<recvStringLen;) {
  int length=  (packetbuffer[i+1]<<8) + packetbuffer[i];
  i +=2;
  if (length) {
    frame_count++;
    fprintf(stdout," frame %d length %d ",current_frame,length);
    int j;
    for (j=i;j<i+length;j++) {
      if (j>=recvStringLen) {
        fprintf(stdout,"AAAAA-out of space as pos %d\n",j);
        break;
        }
      fprintf(stdout,"%2.2x",packetbuffer[j]);
      }
    i=j;
    fprintf(stdout,"  ");
//    for (j=i;j<i+16;j++) {
//      if (j>=recvStringLen) {
//        fprintf(stdout,"BBBBB-out of space as pos %d\n",j);
//        break;
//        }
//      fprintf(stdout,"%2.2x",packetbuffer[j]);
//      }
//    i=j;
    fprintf(stdout,"\n");
    current_frame--;
    }    
  else { 
  
  wowwow: 
    if (recvStringLen-i>40) {     
    int l2;
    l2=packetbuffer[i];i++;
      fprintf(stdout,"\n  command %d %c ",l2,packetbuffer[i]);
      int j = i;
      int ll = l2;
      for (j=j;j<i+ll;j++) {
        fprintf(stdout,"%2.2x",packetbuffer[j]);
	if ((j-i)%64 ==63) fprintf(stdout,"\n		");
	}
      i=i+l2;
      goto wowwow;
      }
    else {
  
  /* checksum */
//#define WORLD__frame__pct_d_checksum_ " frame %d checksum "
    printf(WORLD__frame__pct_d_checksum_,header_frame_number);
    int j;
    for (j=i;j<i+12;j++) {
      if (j>=recvStringLen) {
        fprintf(stdout,"mmmm-out of space as pos %d\n",j);
        break;
        }
      fprintf(stdout,"%2.2x",packetbuffer[j]);
      }
    i=j;
    
    
    
//#define WORLD_receiver_no_sound__crud_ " crud "
    printf(WORLD_receiver_no_sound__crud_);
    
    for (j=i;j<(recvStringLen-28);j++) {
      if (j>=recvStringLen) {
        fprintf(stdout,"nnnnnn-out of space as pos %d\n",j);
        break;
        }
      fprintf(stdout,"%2.2x",packetbuffer[j]);
      }
    i=j;
    
        
    /* the frame number is the frame of the first frame, and then the rest are less than
     it goes backwards
     20 19 18 17 16 15 */
    
    
    
     if (i+28>recvStringLen) {
        fprintf(stdout,"CC-out of space as pos %d\n",i+28);
        break;
        }
     }
     break;
    }
  }    
fprintf(stdout,"\n");
}






int show_binary() {
//extern unsigned char packetbuffer[PACKETMAXSIZE];
//extern int recvStringLen; /* Length of received string */
if (recvStringLen==0) { return 0;}
if (packetbuffer[recvStringLen-1]=='1') {
  return show_binary_sr01();
  }
else {
  return show_binary_sr00();
  }
}  






int main(int argc,char *argv[]) {
logit("usage: %s [-t]\n	where\n	-t - troubleshooting mode\n",argv[0]);
int mode=0;
//#define WORLD_receiver_no_sound__minus_t "-t"
if ((argc>=2)&&(strcmp(argv[1],WORLD_receiver_no_sound__minus_t)==0)) {
  mode=1;   /* 1 for troublshooting mode */
  argv++;
  argc--;
  }
//#define WORLD_receiver_no_sound__minus_b "-b"
else if ((argc>=2)&&(strcmp(argv[1],WORLD_receiver_no_sound__minus_b)==0)) {
  mode=2;   /* 1 for troublshooting mode */
  argv++;
  argc--;
  }
NativeInit(argc,argv);
while (1) {
  if (get_internet_receiver_packet()) {
    if (mode) {
      show_binary();
      }
    if ((mode==0)||(mode==2)) {
//      fprintf(stdout,"\n");
      process_packet();
      compute_packet_summary();
      show_stats();
      }
//  NativeWork();
    }
  }
NativeFinish();
}
