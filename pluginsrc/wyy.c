#include "hakuCore/request.h"
#include "hakuCore/json.h"
#include "hakuCore/api.h"

#include <stdio.h>

char* func (event_t *newEvent)
{
	//char URL[] = "https://www.ruanyifeng.com/blog/2015/02/make.html";
	//char URL[] = "https://v1.hitokoto.cn";
	char URL[] = "http://api.heerdev.top:4995/nemusic/random";
	//CURL *easyConnect = NULL;
	CURLcode res;
	resp_data *data = (resp_data*)malloc(sizeof(resp_data));
	data->data = NULL;
	data->length = 0;
	//data.length = 0;
	//data.data = NULL;
	
	//curl_global_init(CURL_GLOBAL_DEFAULT);

	//easyConnect = curl_easy_init();
	//res = curl_easy_setopt(easyConnect, CURLOPT_URL, URL);
	//res = curl_easy_perform(easyConnect);
	//curl_easy_cleanup(easyConnect);
	
	//res = getData(&data, URL, 2, "c", "a");
	res = getData(data, URL, 0);
	//fprintf(stdout, "%s\n", curl_easy_strerror(res));
	fprintf(stdout, "%u ", res);
	if (res != CURLE_OK) return NULL;
	fprintf(stdout, "%u ", res);
	fprintf(stdout, "%s\n", data->data);

	void *text = NULL;
	res = getJsonValue(data->data, &text, TYPE_STRING, "text");

	//printf("%d\n", res);
	//printf("%s", data.data);
	//printf("%s\n", errorMsg);
	//free(text);

	//curl_global_cleanup();
	fprintf(stdout, "code: %u", res);
	if (res) fprintf(stderr, "wyy %s\n", (char*)text);
	else return (char*)text;
}

