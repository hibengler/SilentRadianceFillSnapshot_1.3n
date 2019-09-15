/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler

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

/* this is a gutted version of curl_web_stream for emscriptem */

#include "curl_web_stream_empty.h"
#include "processor.h"
#include "seven_eight_codec.h"

int curl_already_initted=0;
int we_are_streaming_web=0;
unsigned long bytes_streamed=0;

/* goto guts to get to the guts of this */


/*---------------------------------------------------------------------------------------------------*/
/* guts */

/* note - this might be re-called by the receive loop based on various errors
because we sometimes get a recvfrrom failed error on android, when switching
networks */
void init_web_stream(char *url) {
we_are_streaming_web=1; 
bytes_streamed=0;

return;
}



int stream_slide_until_sr_indicator(char *startbuf,int number_in_buf)
{
}


int get_packet_from_web_stream() {
return(0);
}



void shutdown_web_stream() {
}
