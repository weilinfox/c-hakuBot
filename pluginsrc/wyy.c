#include "hakuCore/request.h"
#include "hakuCore/json.h"
#include "hakuCore/api.h"

#include <stdio.h>

char* func (event_t *newEvent)
{
	char URL[] = "http://api.heerdev.top:4995/nemusic/random";
	void *text = NULL;
	CURLcode res;
	resp_data *data = (resp_data*)malloc(sizeof(resp_data));
	data->data = NULL;
	data->length = 0;

	res = getData(data, URL, 0);

	fprintf(stdout, "%u ", res);
	if (res != CURLE_OK) {
		text = malloc(sizeof(char)*64);
		snprintf((char*)text, 63, "啊嘞嘞好像出错了，一定是wyy炸了不关小白！");
		return (char*)text;
	}

	res = getJsonValue(data->data, &text, TYPE_STRING, "text");

	if (res) {
		char *errorMsg = (char*)malloc(sizeof(char)*(strlen((char*)text)+30));
		sprintf(errorMsg, "好像返回了奇怪的东西: %s", (char*)text);
		return errorMsg;
	} else return (char*)text;
}

