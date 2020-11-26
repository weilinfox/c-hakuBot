#include "api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *apiUrl = NULL;
char *accToken = NULL;
int64_t apiPort = 8000;

int set_api_data(const char *u, int64_t p, const char *t)
{
	if (apiUrl)
		free(apiUrl);
	if (accToken)
		free(accToken);
	apiUrl = (char*)malloc(sizeof(char)*(strlen(u)+2));
	accToken = (char*)malloc(sizeof(char)*(strlen(t)+2));
	if (!apiUrl || !accToken)
		return MALLOC_ERROR;
	strcpy(apiUrl, u);
	strcpy(accToken, t);
	apiPort = p;
	return NO_ERROR;
}

void clear_api_data(void)
{
	free(apiUrl);
	apiUrl = NULL;
	apiPort = 8000;
}

char* get_url(const char* dir)
{
	char *addr = (char*)malloc(sizeof(char)*MAX_ADDRESS_SIZE);
	snprintf(addr, MAX_ADDRESS_SIZE-1, "http://%s:%ld/%s", apiUrl, apiPort, dir);
	fprintf(stdout, "Get URL: %s\n", addr);
	return addr;
}

int send_private_message(const char* msg, int64_t qid, int auto_escape)
{
	char *ul = get_url("send_private_msg");
	char *id = (char*)malloc(sizeof(char)*QQ_ID_LEN);
	char tc;
	resp_data repData = {
		.data = NULL,
		.length = 0
	};
	int i, j;
	for (i = 0; qid > 0; i++) {
		id[i] = qid % 10 + '0';
		qid /= 10;
	}
	id[i] = '\0';
	for (j = 0; j < i/2; j++) {
		tc = id[j];
		id[j] = id[i-j-1];
		id[i-j-1] = tc;
	}
	fprintf(stdout, "Send message: %s to privateId: %s\n", msg, id);
	int res = (int)getData(&repData, ul, 8, "access_token", accToken, "user_id", id, "message", msg, "auto_escape", auto_escape?"True":"False");
	free(ul);
	free(id);
	if (res) {
		fprintf(stderr, "%s\n", repData.data);
		return SEND_MESSAGE_ERROR;
	} else {
		fprintf(stdout, "Cqhttp return msg: %s\n", repData.data);
		return NO_ERROR;
	}
}

int send_group_message(const char* msg, int64_t gid, int auto_escape)
{
	char *ul = get_url("send_group_msg");
	char *id = (char*)malloc(sizeof(char)*QQ_ID_LEN);
	char tc;
	resp_data repData = {
		.data = NULL,
		.length = 0
	};
	int i, j;
	for (i = 0; gid > 0; i++) {
		id[i] = gid % 10 + '0';
		gid /= 10;
	}
	id[i] = '\0';
	for (j = 0; j < i/2; j++) {
		tc = id[j];
		id[j] = id[i-j-1];
		id[i-j-1] = tc;
	}
	fprintf(stdout, "Send message: %s to groupId: %s\n", msg, id);
	int res = (int)getData(&repData, ul, 8, "access_token", accToken, "group_id", id, "message", msg, "auto_escape", auto_escape?"True":"False");
	free(ul);
	free(id);
	if (res) {
		fprintf(stderr, "%s\n", repData.data);
		return SEND_MESSAGE_ERROR;
	} else {
		fprintf(stdout, "Cqhttp return msg: %s\n", repData.data);
		return NO_ERROR;
	}
}

int reply_message(const event_t *newEvent, const char* msg)
{
	if (!strcmp(newEvent->eventName, "group")) {
		return send_group_message(msg, newEvent->groupId, 0);
	} else if (!strcmp(newEvent->eventName, "private")) {
		return send_private_message(msg, newEvent->userId, 0);
	} else {
		return MULTIPLE_ERRORS;
	}
}

int get_friend_list(char* data[])
{
	if (*data != NULL)
		return POINTER_NONEMPTY_ERROR;
	char *ul = get_url("get_friend_list");
	resp_data repData = {
		.data = NULL,
		.length = 0
	};
	int res = (int)getData(&repData, ul, 2, "access_token", accToken);
	free(ul);
	if (res) {
		fprintf(stderr, "%s\n", repData.data);
		return SEND_MESSAGE_ERROR;
	} else {
		fprintf(stdout, "Cqhttp return msg: %s\n", repData.data);
		*data = repData.data;
		return NO_ERROR;
	}
}

int get_group_list(char* data[])
{
	if (*data != NULL)
		return POINTER_NONEMPTY_ERROR;
	char *ul = get_url("get_group_list");
	resp_data repData = {
		.data = NULL,
		.length = 0
	};
	int res = (int)getData(&repData, ul, 2, "access_token", accToken);
	free(ul);
	if (res) {
		fprintf(stderr, "%s\n", repData.data);
		return SEND_MESSAGE_ERROR;
	} else {
		fprintf(stdout, "Cqhttp return msg: %s\n", repData.data);
		*data = repData.data;
		return NO_ERROR;
	}
}

int get_group_member_list(int64_t gid, char* data[])
{
	if (*data != NULL)
		return POINTER_NONEMPTY_ERROR;
	char *ul = get_url("get_group_member_list");
	char *id = (char*)malloc(sizeof(char)*QQ_ID_LEN);
	char tc;
	resp_data repData = {
		.data = NULL,
		.length = 0
	};
	int i, j;
	for (i = 0; gid > 0; i++) {
		id[i] = gid % 10 + '0';
		gid /= 10;
	}
	id[i] = '\0';
	for (j = 0; j < i/2; j++) {
		tc = id[j];
		id[j] = id[i-j-1];
		id[i-j-1] = tc;
	}
	int res = (int)getData(&repData, ul, 2, "user_id", id);
	free(ul);
	free(id);
	if (res) {
		fprintf(stderr, "%s\n", repData.data);
		return SEND_MESSAGE_ERROR;
	} else {
		fprintf(stdout, "Cqhttp return msg: %s\n", repData.data);
		*data = repData.data;
		return NO_ERROR;
	}
}

