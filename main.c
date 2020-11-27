#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *url = NULL;
int64_t port = 8000;
int64_t sendport = 8001;
char *token = NULL;
int64_t backlog = 0;
int64_t masterQid[MASTER_NUM_MAX];
int mstNum;

int data_preload(void)
{
	FILE *configFile = NULL;
	char *configData;
	int i = 0;
	char *tmpC, ch;

	printf("Reading data from %s\n", CONFIG_FILE);

	configFile = fopen(CONFIG_FILE, "r");
	if (!configFile) {
		configFile = fopen(CONFIG_FILE, "w");
		if (!configFile)
			return INIT_CONFIG_ERROR;
		fprintf(configFile, "%s\n", initConfigData);
		fclose(configFile);
		return INIT_CONFIG;
	}

	configData = (char*)malloc(sizeof(char)*CONFIG_FILE_LEN);
	while ((ch = fgetc(configFile)) != EOF && i < CONFIG_FILE_LEN-1) {
		configData[i++] = ch;
	}
	configData[i] = '\0';
	fclose(configFile);
	configFile = NULL;

	void *voidUrl = NULL;
	void *voidPort = NULL;
	void *voidSPort = NULL;
	void *voidToken = NULL;
	void *voidData = NULL;
	int res = NO_ERROR;

	/*URL*/
	res = getJsonValue(configData, &voidUrl, TYPE_STRING, "URL");
	if (res) {
		free(voidUrl);
		voidUrl = NULL;
		return res;
	}
	/*PORT*/
	res = getJsonValue(configData, &voidPort, TYPE_INT64, "PORT");
	if (res) {
		free(voidPort);
		free(voidUrl);
		voidUrl = voidPort = NULL;
		return res;
	}
	/*SEND_PORT*/
	res = getJsonValue(configData, &voidSPort, TYPE_INT64, "SEND_PORT");
	if (res) {
		free(voidPort);
		free(voidUrl);
		free(voidSPort);
		voidUrl = voidPort = voidSPort = NULL;
		return res;
	}
	/*TOKEN*/
	res = getJsonValue(configData, &voidToken, TYPE_STRING, "TOKEN");
	if (res) {
		free(voidPort);
		free(voidUrl);
		free(voidSPort);
		free(voidToken);
		voidUrl = voidPort = voidSPort = voidToken = NULL;
		return res;
	}
	/*MASTER_ID*/
	mstNum = 0;
	tmpC = (char*)malloc(sizeof(char)*16);
	while (mstNum < MASTER_NUM_MAX) {
		sprintf(tmpC, "MASTER%d", mstNum);
		res = getJsonValue(configData, &voidData, TYPE_INT64, tmpC);
		if (!res) {
			masterQid[mstNum] = *(int64_t*)voidData;
			free(voidData);
			voidData = NULL;
		} else {
			fprintf(stderr, "Failed to get MASTER%d. Code: %ld\n", mstNum, *(int64_t*)voidData);
			break;
		}
		mstNum++;
	}
	free(voidData);
	voidData = NULL;
	free(tmpC);
	/*BACKLOG*/
	res = getJsonValue(configData, &voidData, TYPE_INT64, "BACKLOG");
	if (res) {
		fprintf(stderr, "Failed to get backlog value. Code: %ld\n", *(int64_t*)voidData);
		free(voidData);
		voidData = NULL;
		res = WARNING;
	} else {
		backlog = *(int64_t*)voidData;
	}

	url = (char*)voidUrl;
	port = *(int64_t*)voidPort;
	sendport = *(int64_t*)voidSPort;
	token = (char*)voidToken;

	fprintf(stdout, "Server will bind with: %s:%ld, backlog is %ld\n", url, port, backlog);
	fprintf(stdout, "Request will be send to port: %ld\n", sendport);
	fprintf(stdout, "Access-token is: %s\n", token);


	free(voidData);
	free(configData);

	return res;
}

int global_init(void)
{
	printf("Starting global init\n");
	int res = NO_ERROR;
	res = (int)curl_global_init(CURL_GLOBAL_DEFAULT);
	if (res) return CURL_GLOBAL_INIT_ERROR;
	res = data_preload();
	if (res) return res;
}

void global_cleanup(void)
{
	clear_api_data();
	haku_sleep();
	curl_global_cleanup();
	/*global varieble*/
	free(url);
	printf("Global clearup finished.\n");
}

int main()
{
	int res, i;

	res = global_init();
	if (res == WARNING) {
		printf("Global init returned a warning flag.\n");
	} else if (res) {
		global_cleanup();
		fprintf(stderr, "Global init error with return code: %d\n", res);
		return res;
	}

	set_server_data(url, port, backlog);
	set_api_data(url, sendport, token);
	init_so_file_tree();
	awake_haku();	/*awaken haku~*/
	for (i = 0; i < mstNum; i++) {
		res = haku_master_attach(masterQid[i]);
		if (res) break;
	}

	res = new_server();

	fprintf(stdout, "Server returned code: %d\n", res);

	global_cleanup();

	return 0;
}

