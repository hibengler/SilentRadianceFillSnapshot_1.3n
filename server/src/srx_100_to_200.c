#include <stdio.h>
#include <stdlib.h>
#include "srx_format.h"



char inbuf[2000000];
char outbuf[1000000];
int main()  {
while (gets(inbuf)) {
  srx_100_to_srx_200(outbuf,inbuf,1000000);
  puts(outbuf);
  }
exit(0);
}
