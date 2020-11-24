#ifndef API_H
#define API_H

#include "request.h"
#include "errorMsg.h"

int set_api_data(const char *url, int64_t port, const char* access_token);
void clear_api_data(void);

#define MAX_ADDRESS_SIZE 64
#define QQ_ID_LEN 16

int send_private_message(const char* msg, int64_t qq_id, int auto_escape);
int send_group_message(const char* msg, int64_t group_id, int auto_escape);
int get_friend_list(char* returnData[]);
int get_group_list(char* returnData[]);
int get_group_member_list(int64_t group_id, char* returnData[]);

#endif
