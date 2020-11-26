#ifndef HAKUMAIN_H
#define HAKUMAIN_H

#include "errorMsg.h"
#include "json.h"
#include "api.h"
#include <stdio.h>

#define QUIT_FLAG -1009

#define HTTP_METHOD_LEN 8
#define HTTP_PATH_LEN 128
#define HTTP_PROTOCOL_LEN 16
#define HTTP_HEADER_NUM 32
#define HTTP_HEADER_LEN 32
#define HTTP_HEADERDATA_LEN 256

#define EVENT_NAME_LEN 16
#define EVENT_MESSAGE_LEN 8192
#define EVENT_DATA_LEN 32

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

typedef struct {
	char eventType[EVENT_NAME_LEN];
	char eventName[EVENT_NAME_LEN];
	char eventMessage[EVENT_MESSAGE_LEN];
	char eventOtherData[EVENT_DATA_LEN];
	time_t eventTime;
	time_t eventInterval;
	int64_t groupId;
	int64_t userId;
	int64_t selfId;
} new_event_t;

typedef struct {
	int64_t selfId;
	int64_t heartBeat;
	time_t lastHeartBeat;
	time_t wakeTime;
} hakuLive;

int new_thread(const char *httpMsg);

#endif
