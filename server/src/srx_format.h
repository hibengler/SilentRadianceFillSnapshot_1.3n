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
Srx format:


Well, the . | and spaces show the beats and song onsets - I though that this would align to the text - not so good.

So the non .| lines contain the actual text - and multiline is for up to 4 voices.

OK,  but dealing with 200 samples a second makes files fucking huge. - like 800 characters across
And that overloads musib_broadcast some times, and shit


The srx v1 format was

|              .              .             .              |              .              .             .              |
 In                            the                         town                          where
 							   town      _o       town
|              .              .             .              |              .              .             .              |

 
so V1.2 kinda dies.   
Also the srplayer seg faults
Also, clips some of the unised clips because >3000 in launched SR1.2
also, tabs did not work in the | . area
also, 800 was too small.

So Srx v2 format has a comment on the first line - which indicates it is 100 frames per second instead of 200

This gets expanded to 200 on packet_summary and internally in srx.

Then there is compression and expansion.   Tabs still work.


so we will read 100 or 200 and convert into 200 frames then compress before going over broadcast
and expand in broadcast to 200.


   
     

srxV1 - no comments implies 200 frames
a comment implies 100 frames

# - comment on first char

- - option


- tab - 8 spaced in tab
- ^a[A-Z] - compress spaces
- ^b[a-z] - double compress spaces


To work the file ->
# -> ignore






old sr


*/

/* used to see if srx is V1 (200) or v2(100) */
extern int srx_is_comment(char *input);



/* compress - converts tabs and spaces to compressed ctrla|size format*/
extern char *srx_compress(char *output,char *input,int maxlen);


/* srx_expand - expand compressed format to nice format - without the tabs */
extern char *srx_expand(char *output,char *input,int maxlen);



/* nice way to compress the files so they are editable */
extern char *srx_200_to_srx_100(char *output,char *input,int maxlen);

/* way top expand srv2*/
extern char *srx_100_to_srx_200(char *output,char *input,int maxlen);


/* so to read file:

int first_read=1;
int v2=0;
while (readline)
  if (first_read) v2=srx_is_comment(line);
  first_read=0;
  if (v2) {
    srx_100_to_srx_200(outbuf,line);
    }
  else
    srx_expand(output,line);
    }
   
To write files -
srx_compress(compressed,output,10000);
...
put on broadcast
  
      
*/
