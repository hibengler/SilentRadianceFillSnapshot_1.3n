#include <stdio.h>
#include <stdlib.h>
#include "srx_format.h"



char inbuf[2000000];
char outbuf[1000000];
char outbuf2[1000000];
int main()  {
int first_read=1;
int v2=0;
while (gets(inbuf)) {
  if (first_read) v2=srx_is_comment(inbuf);
  first_read=0;
  if (srx_is_comment(inbuf)) {continue;}
  if (v2) {
    srx_100_to_srx_200(outbuf,inbuf,1000000);
    }
  else {
    srx_expand(outbuf,inbuf,1000000);
    }
  srx_compress(outbuf2,outbuf,1000000);
  puts(outbuf2);
  }
exit(0);
}
