#include "hakuMind.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

hakuLive hakuSelf;
time_list_node_t *messageTimeListHead = NULL;	/*List to record msg time*/
time_list_node_t *messageTimeListTail = NULL;
int64_t messageNumPerSecond = 0;		/*message num per second*/
int64_t messageNumPerSecondLog = -1;		/*for log message*/
int64_t masterId[MASTER_NUM_MAX];		/*id of administrators*/
int64_t blockId[MASTER_NUM_MAX];		/*id of blocklist*/
int masterNum = 0;
int blockNum = 0;
char *QUIT_MSG_FILE = "quit.txt";
extern int quitFlag;
char hakuVersion[] = "由狸赋予的版本号0.0.3 weilinfox 2021-1-2";

void set_quit_flag (int flag)
{
	quitFlag = flag;
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

int haveSubstr (const char* str, const char* substr)
{
/*	size_t strLen = strlen(str);
	size_t substrLen = strlen(substr);
	size_t i, j;
	int flag = 0;
#ifdef DEBUG_HAKUMIND
	fprintf(stdout, "strLen %ld, substrLen %ld\n", strLen, substrLen);
#endif
	if (strLen < substrLen) return 0;
	for (i = 0; !flag && i <= strLen - substrLen; i++) {
		for (j = 0; i + j < strLen && j < substrLen; j++) {
			if (substr[j] != str[i+j])
				break;
		}
		if (j == substrLen)
			flag = 1;
	}
#ifdef DEBUG_HAKUMIND
	fprintf(stdout, "Find finished\n");
#endif
*/
	return strstr(str, substr) != NULL;
}

void awake_haku (char index)
{
	FILE *quitMsgFile = fopen(QUIT_MSG_FILE, "r");
	char *quitMsg = (char*)malloc(sizeof(char) * 4096);
	char *msgPoint = NULL;
	new_event_t *quitEvent = NULL;

	hakuSelf.selfId = hakuSelf.heartBeat = 0;
	hakuSelf.lastHeartBeat = 0;
	hakuSelf.wakeTime = time(NULL);
	hakuSelf.index = index;

	messageTimeListHead = (time_list_node_t*)malloc(sizeof(time_list_node_t));
	messageTimeListTail = messageTimeListHead;
	messageTimeListTail->next = NULL;
	messageNumPerSecond = 0;

	fprintf(stdout, "Haku wake up with index: %c\n", hakuSelf.index);

	if (quitMsgFile) {
		fread(quitMsg, sizeof(char), 4095, quitMsgFile);
		msgPoint = strchr(quitMsg, '\n');
		//fprintf(stdout, "%s\n%s\n", quitMsg, msgPoint);
		if (msgPoint) {
			*msgPoint = '\0';
			//fprintf(stdout, "\t%s%s\n", quitMsg, msgPoint+2);
			quitEvent = httpMsgToEvent(msgPoint + 2);
			if (!quitEvent->error) {
				if (!strcmp(quitMsg, "UPDATE")) reply_message(quitEvent, "小白升级完成~");
				else if (!strcmp(quitMsg, "SLEEP")) reply_message(quitEvent, "小白已经从睡梦中醒来~");
			}
		}
		fclose(quitMsgFile);
	}
	free(quitMsg);
	free(quitEvent);
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

int haku_master_attach(int64_t id)
{
	if (masterNum >= MASTER_NUM_MAX)
		return OUT_OF_RANGE_ERROR;
	else
		masterId[masterNum++] = id;
#ifdef DEBUG_HAKUMIND
	fprintf(stdout, "Attached master id: %ld\n", masterId[masterNum-1]);
#endif

	return 0;
}

int haku_block_attach(int64_t id)
{
	if (blockNum >= MASTER_NUM_MAX)
		return OUT_OF_RANGE_ERROR;
	else
		blockId[blockNum++] = id;
#ifdef DEBUG_HAKUMIND
	fprintf(stdout, "Attached block id: %ld\n", blockId[blockNum-1]);
#endif

	return 0;
}

char* catch_inside_command (const event_t *newEvent)
{
	char *replyMsg;
	int i;
	time_t timeNow = time(NULL);
	int isMaster = 0;
	
	/*check master*/
	for (i = 0; i < masterNum; i++) {
		if (masterId[i] == newEvent->userId) {
			isMaster = 1;
			break;
		}
	}

	/*inside command*/
	if (haveSubstr(newEvent->eventMessage, "hakuzousu") || \
			haveSubstr(newEvent->eventMessage, "白蔵主") || \
			haveSubstr(newEvent->eventMessage, "白藏主")) {
		if (!isMaster) {
			replyMsg = (char*)malloc(sizeof(char)*32);
			snprintf(replyMsg, 31, "何人竟敢直呼吾名？");
			return replyMsg;
		}
		if (haveSubstr(newEvent->eventMessage, "日志")) {
			replyMsg = (char*)malloc(sizeof(char)*64);
			snprintf(replyMsg, 63, "流量: %ld\n心跳: %ld\n小白已经正常运行:\n%ld分%ld秒", messageNumPerSecond, (int64_t)hakuSelf.heartBeat*60/((int64_t)(timeNow-hakuSelf.wakeTime))+1, (int64_t)(timeNow-hakuSelf.wakeTime)/60, (int64_t)(timeNow-hakuSelf.wakeTime)%60);
			return replyMsg;
		} else if (haveSubstr(newEvent->eventMessage, "休息")) {
			replyMsg = (char*)malloc(sizeof(char)*32);
			snprintf(replyMsg, 31, "_QUIT__FLAG__BY__INUYASHA_");
			set_quit_flag(GEN_SLEEP_FLAG);
			return replyMsg;
		} else if (haveSubstr(newEvent->eventMessage, "退出")) {
			replyMsg = (char*)malloc(sizeof(char)*32);
			snprintf(replyMsg, 31, "_QUIT__FLAG__BY__INUYASHA_");
			set_quit_flag(GEN_QUIT_FLAG);
			return replyMsg;
		} else if (haveSubstr(newEvent->eventMessage, "升级")) {
			replyMsg = (char*)malloc(sizeof(char)*32);
			snprintf(replyMsg, 31, "_QUIT__FLAG__BY__INUYASHA_");
			set_quit_flag(GEN_UPDATE_FLAG);
			return replyMsg;
		} else if (haveSubstr(newEvent->eventMessage, "版本") || haveSubstr(newEvent->eventMessage, "version")) {
			replyMsg = (char*)malloc(sizeof(char)*128);
			snprintf(replyMsg, 127, "小白的内核信息如下:\n%s", hakuVersion);
			return replyMsg;
		} else {
			replyMsg = (char*)malloc(sizeof(char)*32);
			snprintf(replyMsg, 31, "[CQ:face,id=175]");
			return replyMsg;
		}
	} else if (haveSubstr(newEvent->eventMessage, "haku") || \
			haveSubstr(newEvent->eventMessage, "小白")) {
		if (haveSubstr(newEvent->eventMessage, "日志")) {
			replyMsg = (char*)malloc(sizeof(char)*64);
			snprintf(replyMsg, 63, "流量: %ld\n心跳: %ld\n小白已经正常运行:\n%ld分%ld秒", messageNumPerSecond, (int64_t)hakuSelf.heartBeat*60/((int64_t)(timeNow-hakuSelf.wakeTime))+1, (int64_t)(timeNow-hakuSelf.wakeTime)/60, (int64_t)(timeNow-hakuSelf.wakeTime)%60);
			return replyMsg;
		} else if (haveSubstr(newEvent->eventMessage, "休息")) {
			replyMsg = (char*)malloc(sizeof(char)*80);
			if (isMaster) {
				snprintf(replyMsg, 79, "_QUIT__FLAG__BY__INUYASHA_");
				set_quit_flag(GEN_SLEEP_FLAG);
			} else snprintf(replyMsg, 79, "狸并没有赋予汝此权限，回应你就是小白最大的耐心。");
			return replyMsg;
		} else if (haveSubstr(newEvent->eventMessage, "退出")) {
			replyMsg = (char*)malloc(sizeof(char)*80);
			if (isMaster) {
				snprintf(replyMsg, 79, "_QUIT__FLAG__BY__INUYASHA_");
				set_quit_flag(GEN_QUIT_FLAG);
			} else snprintf(replyMsg, 79, "狸并没有赋予汝此权限，回应你就是小白最大的耐心。");
			return replyMsg;
		} else if (haveSubstr(newEvent->eventMessage, "升级")) {
			replyMsg = (char*)malloc(sizeof(char)*80);
			if (isMaster) {
				snprintf(replyMsg, 79, "_QUIT__FLAG__BY__INUYASHA_");
				set_quit_flag(GEN_UPDATE_FLAG);
			} else snprintf(replyMsg, 79, "狸并没有赋予汝此权限，回应你就是小白最大的耐心。");
			return replyMsg;
		} else if (isMaster && (haveSubstr(newEvent->eventMessage, "版本") || haveSubstr(newEvent->eventMessage, "version"))) {
			replyMsg = (char*)malloc(sizeof(char)*128);
			snprintf(replyMsg, 127, "小白的内核信息如下:\n%s", hakuVersion);
			return replyMsg;
		} else {
			replyMsg = (char*)malloc(sizeof(char)*32);
			if (isMaster) snprintf(replyMsg, 31, "[CQ:face,id=175]");
			else snprintf(replyMsg, 31, "[CQ:face,id=179]");
			return replyMsg;
		}
	}
	return NULL;
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

#ifdef DEBUG_HAKUMIND
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
#endif

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
				res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "group_id");
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

char* parse_plugin_command(event_t *newEvent)
{
	char *command = (char*)malloc(sizeof(char)*MAX_SOFILE_NAME_LEN);
	char *respstr = NULL;
	int i;
	if ((newEvent->eventMessage)[0] != hakuSelf.index) {
		free(command);
		return NULL;
	}

	for (i = 1; i < MAX_SOFILE_NAME_LEN - 2 && (newEvent->eventMessage)[i] && \
		((newEvent->eventMessage[i]) == '_' || isdigit((newEvent->eventMessage)[i]) || isalpha((newEvent->eventMessage)[i])); i++) {
		command[i-1] = (newEvent->eventMessage)[i];
	}
	if (i < MAX_SOFILE_NAME_LEN - 2) {
		/* .so plugins */
		command[i-1] = '\0';
#ifdef DEBUG_HAKUMIND
		fprintf(stdout, "Get plugin name: %s\n", command);
#endif
		so_name_tree_t *pluginp = open_plugin(command);
		if (pluginp > 0) {
#ifdef DEBUG_HAKUMIND
			fprintf(stdout, "Caught this plugin.\n");
#endif
			if (pluginp->soFile) {
				/* so plugins */
				respstr = (pluginp->soFile->func)(newEvent);
			} else if (pluginp->pyModule) {
				/* python plugins */
				respstr = run_python_func(pluginp->pyModule->pyFunc, newEvent);
			} else {
				/* error */
				fprintf(stderr, "open_plugin returned a invalid pointer!!!!\n");
			}
		}
	}
	free(command);

#ifdef DEBUG_HAKUMIND
	fprintf(stdout, "return %p\n", respstr);
#endif

	return respstr;
}

int new_thread(const char *msg)
{
	new_event_t* newEvent = httpMsgToEvent(msg);
	int res = newEvent->error;
	char *replyMsg;
	time_t timeNow = time(NULL);

	if (res) {
		fprintf(stderr, "error Code %ld\n", newEvent->error);
	} else {
#ifdef DEBUG_HAKUMIND
		fprintf(stdout, "No Error.\n");
		fprintf(stdout, "eventType: %s\n", newEvent->eventType);
		fprintf(stdout, "eventName: %s\n", newEvent->eventName);
		fprintf(stdout, "eventMessage: %s\n", newEvent->eventMessage);
		fprintf(stdout, "eventTime: %lld\n", (long long)newEvent->eventTime);
		fprintf(stdout, "eventInterval: %lld\n", (long long)newEvent->eventInterval);
		fprintf(stdout, "groupId: %ld, userId %ld, selfId %ld\n", newEvent->groupId, newEvent->userId, newEvent->selfId);
		fprintf(stdout, "return Code %ld\n", newEvent->error);
#endif

		if (!strcmp(newEvent->eventType, "QUIT")) {
			free(newEvent);
			return QUIT_FLAG;
		}

		if (!strcmp(newEvent->eventType, "message")) {
			fprintf(stdout, "Raw_message: %s\n", msg);
			fprintf(stdout, "Get new message: %s\nGroupId %lld; UserId %lld\n", newEvent->eventMessage, newEvent->groupId, newEvent->userId);

			if (hakuSelf.selfId == 0)
				hakuSelf.selfId = newEvent->selfId;

			/*add new message to list*/
			messageNumPerSecond++;
			messageTimeListTail->next = (time_list_node_t*)malloc(sizeof(time_list_node_t));
			messageTimeListTail->next->time = timeNow;
			messageTimeListTail->next->next = NULL;
			messageTimeListTail = messageTimeListTail->next;

			/*catch inside command*/
			replyMsg = catch_inside_command(newEvent);
			if (replyMsg && strcmp(replyMsg, "_QUIT__FLAG__BY__INUYASHA_")) {
				fprintf(stdout, "Get reply message.\n");
				reply_message(newEvent, replyMsg);
			} else if (replyMsg) {
				FILE *quitMsgFile = fopen(QUIT_MSG_FILE, "w");
				char *quitMsg = strstr(msg, "\r\n\r\n");
				free(replyMsg);
				switch (quitFlag) {
					case GEN_QUIT_FLAG:
						fprintf(quitMsgFile, "QUIT\n\n%s", msg);
						reply_message(newEvent, "小白将和元一同退出。");
						break;
					case GEN_UPDATE_FLAG:
						fprintf(quitMsgFile, "UPDATE\n\n%s", msg);
						reply_message(newEvent, "小白将请求元升级小白。");
						break;
					case GEN_SLEEP_FLAG:
						fprintf(quitMsgFile, "SLEEP\n\n%s", msg);
						reply_message(newEvent, "小白去休息了，狸也要早早休息哦~");
						break;
				}
				fclose(quitMsgFile);
				return QUIT_FLAG;
			}
			free(replyMsg);

			/*parse plugin command*/
			replyMsg = parse_plugin_command(newEvent);
			if (replyMsg) {
				fprintf(stdout, "Get plugin msg.\n");
				reply_message(newEvent, replyMsg);
			}
			free(replyMsg);
		} else if (!strcmp(newEvent->eventType, "meta_event")) {
			if (!strcmp(newEvent->eventName, "heartbeat")) {
				hakuSelf.heartBeat++;
				hakuSelf.lastHeartBeat = newEvent->eventTime;
				if (hakuSelf.selfId == 0)
					hakuSelf.selfId = newEvent->selfId;

				if (messageNumPerSecond != messageNumPerSecondLog) {
					messageNumPerSecondLog = messageNumPerSecond;
					fprintf(stdout, "messageNumPerSecond: %ld\n", messageNumPerSecond);
				}

				/*remove timeout message*/
				time_list_node_t *listHead = messageTimeListHead->next;
				time_list_node_t *listNode = NULL;
#ifdef DEBUG_HAKUMIND
				fprintf(stdout, "Delta time: %d\n", listHead?(int)(timeNow-listHead->time):0);
#endif
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
		}
	}
	free(newEvent);

	return res;
}


