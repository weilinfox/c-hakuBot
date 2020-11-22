#ifndef HAKUMAIN_H
#define HAKUMAIN_H

#include <stdio.h>

#define QUIT_FLAG -1009

#define HTTP_METHOD_LEN 8
#define HTTP_PATH_LEN 128
#define HTTP_PROTOCOL_LEN 16
#define HTTP_HEADER_NUM 32
#define HTTP_HEADER_LEN 32
#define HTTP_HEADERDATA_LEN 256

#define UNKOWN_METHOD -1
#define METHOD_LENGTH_EXCEED -2
#define PATH_LENGTH_EXCEED -3
#define PROTOCOL_LENGTH_EXCEED -4
#define HEADER_PARSE_ERROR -5
#define DATA_PARSE_ERROR -6

typedef struct {
	char httpMethod[HTTP_METHOD_LEN];
	char httpPath[HTTP_PATH_LEN];
	char httpProtocol[HTTP_PROTOCOL_LEN];
	char *httpHeader[HTTP_HEADER_NUM];
	char *httpHeaderData[HTTP_HEADER_NUM];
	char *httpData;
	size_t headerNum;
	size_t dataLen;
} http_header_t;

int new_thread(const char *httpMsg);

#endif
