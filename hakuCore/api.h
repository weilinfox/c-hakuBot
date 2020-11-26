#ifndef API_H
#define API_H

#include "request.h"
#include "errorMsg.h"

#define EVENT_NAME_LEN 16
#define EVENT_MESSAGE_LEN 8192
#define EVENT_DATA_LEN 32

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
	int64_t error;
} event_t, new_event_t;

int set_api_data(const char *url, int64_t port, const char* access_token);
void clear_api_data(void);

#define MAX_ADDRESS_SIZE 64
#define QQ_ID_LEN 16

int reply_message(const event_t *newEvent, const char* replyMessage);
int send_private_message(const char* msg, int64_t qq_id, int auto_escape);
int send_group_message(const char* msg, int64_t group_id, int auto_escape);
int get_friend_list(char* returnData[]);
int get_group_list(char* returnData[]);
int get_group_member_list(int64_t group_id, char* returnData[]);

#endif
