#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	char URL[] = "http://api.heerdev.top:4995/nemusic/random";

	CURLcode res;
	resp_data data = {
		.data = NULL,
		.length = 0
	};
	

	res = getData(&data, URL, 0);

	printf("%d\n", res);
	//printf("Origin data: %s\n", data.data);
	if (res) return -2;

	char *text = NULL;

	res = getJsonValue(data.data, &text, "text");

	printf("getJsonValue return with %d\n", res);
	//printf("in main text pointer is: %p", text);
	printf("Get Msg:\n%s\n", text);

	free(text);

	curl_global_cleanup();

	return 0;
}

