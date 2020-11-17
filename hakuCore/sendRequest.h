#ifndef SENDREQUEST_H
#define SENDREQUEST_H

#include <curl/curl.h>

#define OUT_OF_RANGE_ERROR -1
#define SET_PARMS_ERROR -2
#define BUFFER_ERROR -3
#define CURL_INIT_ERROR -4
#define UNKOWN_ERROR -10

#define MAX_PARMS_SIZE 2048

typedef struct SResponceData {
	size_t length;
	char *data;
} SRespData;

CURLcode getData (SRespData *respData, const char *url, int num, ...);

#endif
