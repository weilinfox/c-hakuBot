#include "hakuCore/sendRequest.h"

#include <stdio.h>

int main()
{
	//char URL[] = "https://www.ruanyifeng.com/blog/2015/02/make.html";
	//char URL[] = "https://v1.hitokoto.cn";
	char URL[] = "http://api.heerdev.top:4995/nemusic/random";
	//CURL *easyConnect = NULL;
	CURLcode res;
	SRespData data = {
		.data = NULL,
		.length = 0
	};
	//data.length = 0;
	//data.data = NULL;
	
	curl_global_init(CURL_GLOBAL_ALL);

	//easyConnect = curl_easy_init();
	//res = curl_easy_setopt(easyConnect, CURLOPT_URL, URL);
	//res = curl_easy_perform(easyConnect);
	//curl_easy_cleanup(easyConnect);
	
	//res = getData(&data, URL, 2, "c", "a");
	res = getData(&data, URL, 0);

	curl_global_cleanup();
	printf("%s", data.data);
	//printf("%s\n", errorMsg);
	printf("\n%d\n", res);
	printf("%s\n", curl_easy_strerror(res));

	return 0;
}

