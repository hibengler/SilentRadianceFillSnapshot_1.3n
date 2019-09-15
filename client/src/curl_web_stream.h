#ifndef CURL_WEB_STREAM_H
#define CURL_WEB_STREAM_H

#include "glue.h"
#include <stdio.h>



#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <errno.h>

#ifdef _WIN32
#include "win64_network_receiver.h"
#else
#include "network_receiver.h"
#endif



extern int we_are_streaming_web;

void init_web_stream(char *url);
int get_packet_from_web_stream(void);
void shutdown_web_stream(void);

char *current_web_stream_url(void);


#endif
