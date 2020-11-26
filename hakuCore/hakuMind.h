#ifndef HAKUMAIN_H
#define HAKUMAIN_H

#include "errorMsg.h"
#include "json.h"
#include "api.h"
#include <stdio.h>

#define QUIT_FLAG 1009
#define MASTER_NUM_MAX 128

#define HTTP_METHOD_LEN 8
#define HTTP_PATH_LEN 128
#define HTTP_PROTOCOL_LEN 16
#define HTTP_HEADER_NUM 32
#define HTTP_HEADER_LEN 32
#define HTTP_HEADERDATA_LEN 256

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

typedef struct time_list_node {
	time_t time;
	struct time_list_node* next;
} time_list_node_t;

typedef struct {
	int64_t selfId;
	int64_t heartBeat;
	time_t lastHeartBeat;
	time_t wakeTime;
} hakuLive;

void awake_haku(void);
void haku_sleep(void);
int haku_master_attach(int64_t id);

int new_thread(const char *httpMsg);

#endif
