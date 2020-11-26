#include "hakuMind.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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



int new_thread (const char* msg)
{	
	http_header_t httpData;
	
	size_t pos = 0;
	size_t len = strlen(msg);
	size_t i;
	int errorFlag;

	httpData.headerNum = 0;
	httpData.dataLen = 0;
	httpData.httpData = NULL;

	while (msg[pos] == ' ') pos++;
	for (i = 0; msg[pos+i] != ' ' && i < HTTP_METHOD_LEN; i++) {
		httpData.httpMethod[i] = msg[pos+i];
	}
	if (i == HTTP_METHOD_LEN)
		return METHOD_LENGTH_EXCEED;
	httpData.httpMethod[i] = '\0';
	pos += i;

	if (strcmp(httpData.httpMethod, "POST") && \
		strcmp(httpData.httpMethod, "GET")) {
		return UNKOWN_METHOD;
	}

	while (msg[pos] == ' ') pos++;
	for (i = 0; msg[pos+i] != ' ' && i < HTTP_PATH_LEN; i++) {
		httpData.httpPath[i] = msg[pos+i];
	}
	if (i == HTTP_METHOD_LEN) {
		return PATH_LENGTH_EXCEED;
	}
	httpData.httpPath[i] = '\0';
	pos += i;

	while (msg[pos] == ' ') pos++;
	for (i = 0; msg[pos+i] != '\r'&& i < HTTP_PROTOCOL_LEN; i++) {
		httpData.httpProtocol[i] = msg[pos+i];
	}
	if (i == HTTP_PROTOCOL_LEN) {
		return PROTOCOL_LENGTH_EXCEED;
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
		return HEADER_PARSE_ERROR;
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
			return DATA_PARSE_ERROR;
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


	void *jsonData = NULL;
	char *msgType = NULL;
	char *message = NULL;
	int64_t id;
	int res;

	fprintf(stdout, "Echo process start.\n");
	res = getJsonValue(httpData.httpData, &jsonData, TYPE_STRING, "post_type");
	if (!res) {
		fprintf(stdout, "post_type is: %s\n", (char*)jsonData);
		if (!strcmp((char*)jsonData, "message")) {
			free(jsonData);
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_STRING, "message_type");
			fprintf(stdout, "message_type is: %s %d\n", (char*)jsonData, res);
			msgType = (char*)jsonData;
			jsonData = NULL;
			res = getJsonValue(httpData.httpData, &jsonData, TYPE_STRING, "raw_message");
			fprintf(stdout, "raw_message is: %s %d\n", (char*)jsonData, res);
			message = (char*)jsonData;
			jsonData = NULL;
			if (!strcmp(msgType, "group"))
				res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "group_id");
			else if (!strcmp(msgType, "private"))
				res = getJsonValue(httpData.httpData, &jsonData, TYPE_INT64, "user_id");
			else
				fprintf(stdout, "msgType: What's this??\n");
			if (jsonData) {
				fprintf(stdout, "ID is: %ld %d\n", *(int64_t*)jsonData, res);
				id = *(int64_t*)jsonData;
				free(jsonData);
				jsonData = NULL;

				fprintf(stdout, "Try to send message...\n");

				if (msgType[0] == 'p')
					res = send_private_message(message, id, 0);
				else
					res = send_group_message(message, id, 0);
				fprintf(stdout, "send_xxx_message returned: %d\n", res);
			}

		} else {
			fprintf(stdout, "Not a message.\n");
			free(jsonData);
			jsonData = NULL;
		}
	} else {
		fprintf(stderr, "post_type member returned error code: %d\n", res);
		fprintf(stderr, "Error jsonData is: %s\n", (char*)jsonData);
		free(jsonData);
		jsonData = NULL;
	}


	/*Quit command*/
	if (!strcmp(httpData.httpPath, "/QUIT")) {
		for (i = 0; i < httpData.headerNum; i++) {
			free(httpData.httpHeader[i]);
			free(httpData.httpHeaderData[i]);
		}
		free(httpData.httpData);
		return QUIT_FLAG;
	} else {
		return NO_ERROR;
	}
}
