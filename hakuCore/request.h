#ifndef SENDREQUEST_H
#define SENDREQUEST_H

#include "errorMsg.h"
#include <curl/curl.h>

#define MAX_PARMS_SIZE 2048

typedef struct {
	size_t length;
	char *data;
} resp_data;

CURLcode getData (resp_data *respData, const char *url, int num, .../*parm name, parm data*/);

#endif
