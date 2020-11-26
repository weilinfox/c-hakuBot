#include "hakuMind.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

hakuLive hakuSelf;
time_list_node_t *messageTimeListHead = NULL;	/*List to record msg time*/
time_list_node_t *messageTimeListTail = NULL;
int64_t messageNumPerSecond = 0;			/*message num per second*/

void awaken_haku ()
{
	hakuSelf.selfId = hakuSelf.heartBeat = 0;
	hakuSelf.lastHeartBeat = 0;
	hakuSelf.wakeTime = time(NULL);

	messageTimeListHead = (time_list_node_t*)malloc(sizeof(time_list_node_t));
	messageTimeListTail = messageTimeListHead;
	messageTimeListTail->next = NULL;
	messageNumPerSecond = 0;
}

void haku_sleep()
{
	time_list_node_t *listHead = messageTimeListHead, *listNode;
	while (listHead) {
		listNode = listHead;
		listHead = listHead->next;
		fprintf(stdout, "Free node with time: %ld\n", listNode->time);
		free(listNode);
	}
}

size_t strToInt (const char* msg)
{
	size_t ans = 0, i = 0;
	while (msg[i]) {
		ans *= 10;
		ans += msg[i] - '0';
		i++;
	}
	return ans;
}

new_event_t* httpMsgToEvent (const char* msg)
{	
	new_event_t *newEvent = (new_event_t*)malloc(sizeof(new_event_t));
	http_header_t httpData;
	
	size_t pos = 0;
	size_t len = strlen(msg);
	size_t i;
	int errorFlag;

	newEvent->error = NO_ERROR;

	/*parse http data*/
	httpData.headerNum = 0;
	httpData.dataLen = 0;
	httpData.httpData = NULL;

	while (msg[pos] == ' ') pos++;
	for (i = 0; msg[pos+i] != ' ' && i < HTTP_METHOD_LEN; i++) {
		httpData.httpMethod[i] = msg[pos+i];
	}
	if (i == HTTP_METHOD_LEN) {
		newEvent->error = METHOD_LENGTH_EXCEED;
		return newEvent;
	}
	httpData.httpMethod[i] = '\0';
	pos += i;

	if (strcmp(httpData.httpMethod, "POST") && \
		strcmp(httpData.httpMethod, "GET")) {
		newEvent->error = UNKOWN_METHOD;
		return newEvent;
	}

	while (msg[pos] == ' ') pos++;
	for (i = 0; msg[pos+i] != ' ' && i < HTTP_PATH_LEN; i++) {
		httpData.httpPath[i] = msg[pos+i];
	}
	if (i == HTTP_METHOD_LEN) {
		newEvent->error = PATH_LENGTH_EXCEED;
		return newEvent;
	}
	httpData.httpPath[i] = '\0';
	pos += i;

	while (msg[pos] == ' ') pos++;
	for (i = 0; msg[pos+i] != '\r'&& i < HTTP_PROTOCOL_LEN; i++) {
		httpData.httpProtocol[i] = msg[pos+i];
	}
	if (i == HTTP_PROTOCOL_LEN) {
		newEvent->error = PROTOCOL_LENGTH_EXCEED;
		return newEvent;
	}
	httpData.httpProtocol[i] = '\0';
	pos += i + 2;

	errorFlag = 0;
	while (msg[pos] != '\r' && httpData.headerNum < HTTP_HEADER_NUM) {
		httpData.httpHeader[httpData.headerNum] = (char*)malloc(sizeof(char)*HTTP_HEADER_LEN);
		httpData.httpHeaderData[httpData.headerNum] = (char*)malloc(sizeof(char)*HTTP_HEADERDATA_LEN);
		for (i = 0; msg[pos+i] != ':' && i < HTTP_HEADER_LEN; i++) {
			httpData.httpHeader[httpData.headerNum][i] = msg[pos+i];
		}
		if (i == HTTP_HEADER_LEN) {
			errorFlag = 1;
			httpData.headerNum++;
			break;
		}
		httpData.httpHeader[httpData.headerNum][i] = '\0';
		pos += i + 2;
		for (i = 0; msg[pos+i] != '\r' && i < HTTP_HEADERDATA_LEN; i++) {
			httpData.httpHeaderData[httpData.headerNum][i] = msg[pos+i];
		}
		if (i == HTTP_HEADERDATA_LEN) {
			errorFlag = 1;
			httpData.headerNum++;
			break;
		}
		httpData.httpHeaderData[httpData.headerNum][i] = '\0';
		pos += i + 2;

		if (!strcmp(httpData.httpHeader[httpData.headerNum], "Content-Length"))
			httpData.dataLen = strToInt(httpData.httpHeaderData[httpData.headerNum]);

		httpData.headerNum++;
	}
	if (errorFlag || httpData.headerNum == HTTP_HEADER_NUM) {
		for (i = 0; i < httpData.headerNum; i++) {
			free(httpData.httpHeader[i]);
			free(httpData.httpHeaderData[i]);
		}
		newEvent->error = HEADER_PARSE_ERROR;
		return newEvent;
	}
	pos += 2;

	if (httpData.dataLen) {
		httpData.httpData = (char*)malloc(sizeof(char)*(httpData.dataLen+2));
		for (i = 0; msg[pos+i] && i < httpData.dataLen; i++) {
			httpData.httpData[i] = msg[pos+i];
		}
		httpData.httpData[i] = '\0';

		if (i != httpData.dataLen) {
			for (i = 0; i < httpData.headerNum; i++) {
				free(httpData.httpHeader[i]);
				free(httpData.httpHeaderData[i]);
			}
			free(httpData.httpData);
			newEvent->error = DATA_PARSE_ERROR;
			return newEvent;
		}
	}

	fprintf(stdout, "\n\nParse result START:\n");
	fprintf(stdout, "Method: %s\nPath: %s\nProtocol: %s\n", httpData.httpMethod, httpData.httpPath, httpData.httpProtocol);
	for (i = 0; i < httpData.headerNum; i++) {
		fprintf(stdout, "%s: %s\n", httpData.httpHeader[i], httpData.httpHeaderData[i]);
	}
	if (httpData.httpData == NULL) {
		fprintf(stdout, "No data: %ld\n", httpData.dataLen);
	} else {
		fprintf(stdout, "Get data: %ld\n", httpData.dataLen);
		fprintf(stdout, "%s\n", httpData.httpData);
	}
	fprintf(stdout, "Parse result END.\n\n");

	/*generate new go-cqhttp event*/
	void *jsonData = NULL;
	int res;

	res = getJsonValue(httpData.httpData, &jsonData, TYPE_STRING, "post_type");
	if (res == NO_ERROR) {
		strncpy(newEvent->eventType, (char*)jsonData, EVENT_NAME_LEN-1);
		free(jsonData);
		jsonData = NULL;
		if (!strcmp(newEvent->eventType, "message")) {
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_STRING, "message_type");
			if (res) {
				newEvent->error = res;
			} else {
				strncpy(newEvent->eventName, (char*)jsonData, EVENT_NAME_LEN-1);
			}
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_STRING, "raw_message");
			if (res) {
				newEvent->error = res;
			} else {
				strncpy(newEvent->eventMessage, (char*)jsonData, EVENT_MESSAGE_LEN);
			}
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "self_id");
			if (res) {
				newEvent->error = res;
			} else {
				newEvent->selfId = *(int64_t*)jsonData;
			}
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "user_id");
			if (res) {
				newEvent->error = res;
			} else {
				newEvent->userId = *(int64_t*)jsonData;
			}
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "time");
			if (res) {
				newEvent->error = res;
			} else {
				newEvent->eventTime = *(time_t*)jsonData;
			}
			free(jsonData);
			jsonData = NULL;
			if (!strcmp(newEvent->eventName, "group")) {
				res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "user_id");
				if (res) {
					newEvent->error = res;
				} else {
					newEvent->groupId = *(int64_t*)jsonData;
				}
				free(jsonData);
				jsonData = NULL;
			} else {
				newEvent->groupId = 0;
			}
		} else if (!strcmp(newEvent->eventType, "meta_event")) {
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_STRING, "meta_event_type");
			if (res) {
				newEvent->error = res;
			} else {
				strncpy(newEvent->eventName, (char*)jsonData, EVENT_NAME_LEN-1);
			}
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "interval");
			if (res) {
				newEvent->error = res;
			} else {
				newEvent->eventInterval = *(int64_t*)jsonData;
			}
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "self_id");
			if (res) {
				newEvent->error = res;
			} else {
				newEvent->selfId = *(int64_t*)jsonData;
			}
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "time");
			if (res) {
				newEvent->error = res;
			} else {
				newEvent->eventTime = *(int64_t*)jsonData;
			}
			free(jsonData);
			jsonData = NULL;

		}
	} else {
		/*Quit command*/
		if (!strcmp(httpData.httpPath, "/QUIT")) {
			newEvent->error = 0;
			snprintf(newEvent->eventType, EVENT_NAME_LEN-1, "QUIT");
		}
	}

	for (int i = 0; i < httpData.headerNum; i++) {
		free(httpData.httpHeader[i]);
		free(httpData.httpHeaderData[i]);
	}
	free(httpData.httpData);

	return newEvent;
}

int new_thread(const char *msg)
{
	int res;
	new_event_t* newEvent = httpMsgToEvent(msg);
	res = newEvent->error;

	if (res) {
		fprintf(stderr, "errorCode %ld\n", newEvent->error);
	} else {
		fprintf(stdout, "No Error.\n");
		fprintf(stdout, "eventType: %s\n", newEvent->eventType);
		fprintf(stdout, "eventName: %s\n", newEvent->eventName);
		fprintf(stdout, "eventMessage: %s\n", newEvent->eventMessage);
		fprintf(stdout, "eventTime: %lld\n", (long long)newEvent->eventTime);
		fprintf(stdout, "eventInterval: %lld\n", (long long)newEvent->eventInterval);
		fprintf(stdout, "groupId: %ld, userId %ld, selfId %ld\n", newEvent->groupId, newEvent->userId, newEvent->selfId);
		fprintf(stdout, "returnCode %ld\n", newEvent->error);

		if (!strcmp(newEvent->eventType, "QUIT")) {
			free(newEvent);
			return QUIT_FLAG;
		}

		if (!strcmp(newEvent->eventType, "message")) {
			if (hakuSelf.selfId == 0)
				hakuSelf.selfId = newEvent->selfId;
			/*add new message*/
			messageNumPerSecond++;
			messageTimeListTail->next = (time_list_node_t*)malloc(sizeof(time_list_node_t));
			messageTimeListTail->next->time = newEvent->eventTime;
			messageTimeListTail->next->next = NULL;
			messageTimeListTail = messageTimeListTail->next;
		} else if (!strcmp(newEvent->eventType, "meta_event")) {
			if (!strcmp(newEvent->eventName, "heartbeat")) {
				hakuSelf.heartBeat++;
				hakuSelf.lastHeartBeat = newEvent->eventTime;
				if (hakuSelf.selfId == 0)
					hakuSelf.selfId = newEvent->selfId;

				fprintf(stdout, "messageNumPerSecond: %ld\n", messageNumPerSecond);

				/*remove timeout message*/
				time_list_node_t *listHead = messageTimeListHead->next;
				time_list_node_t *listNode = NULL;
				time_t timeNow = time(NULL);
				fprintf(stderr, "Delta time: %d\n", listHead?(int)(timeNow-listHead->time):0);
				while (listHead && timeNow - listHead->time >= 60) {
					listNode = listHead;
					listHead = listHead->next;
					fprintf(stderr, "Free node with time: %ld\n", listNode->time);
					free(listNode);
					messageNumPerSecond--;
				}
				messageTimeListHead->next = listHead;
				if (listHead == NULL)
					messageTimeListTail = messageTimeListHead;
			}
			free(newEvent);
		}
	}

	return res;
}


