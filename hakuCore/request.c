#include "request.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

size_t writeToString (char *ptr, size_t size, size_t nmemb, void *userdata)
{
	resp_data *data = (resp_data*)userdata;
	size_t allocSize = (nmemb + data->length + 5) * size;
	size_t i;

	if (data->length)
		data->data = (char*)realloc(data->data, allocSize);
	else
		data->data = (char*)malloc(allocSize);
	/*snprintf(data, dataSize, "%s", ptr);*/
	for (i = 0; i < nmemb; i++) {
		(data->data)[data->length + i] = ptr[i];
	}
	data->length += nmemb;
	(data->data)[data->length] = '\0';

	return size * nmemb;
}

CURLcode getData (resp_data *respData, const char *url, int num, ...)
{
	int length, i, j;
	char *urlParms, *parms;
	va_list arg_ptr;
	CURL *easyConnect = NULL;
	CURLcode res = UNKOWN_ERROR;

	if (num & 0x01) return SET_PARMS_ERROR;
	if (respData->length || respData->data) return CURL_BUFFER_ERROR;
	
	urlParms = (char*)malloc(sizeof(int) * (MAX_PARMS_SIZE + 5));
	length = 0;

	/*Parameters*/
	va_start(arg_ptr, num);
	for (i = 0; i < num/2; i++) {
		urlParms[length++] = '?';
		parms = va_arg(arg_ptr, char*);
		for (j = 0; parms[j]; j++) {
			urlParms[length++] = parms[j];
			if (length > MAX_PARMS_SIZE) {
				va_end(arg_ptr);
				free(urlParms);
				return OUT_OF_RANGE_ERROR;
			}
		}
		urlParms[length++] = '=';
		parms = va_arg(arg_ptr, char*);
		for (j = 0; parms[j]; j++) {
			urlParms[length++] = parms[j];
			if (length > MAX_PARMS_SIZE) {
				va_end(arg_ptr);
				free(urlParms);
				return OUT_OF_RANGE_ERROR;
			}
		}
	}
	va_end(arg_ptr);
	urlParms[length] = '\0';

	easyConnect = curl_easy_init();
	if (easyConnect) {
		curl_easy_setopt(easyConnect, CURLOPT_URL, url);
		curl_easy_setopt(easyConnect, CURLOPT_HTTPGET, urlParms);
		curl_easy_setopt(easyConnect, CURLOPT_TIMEOUT, 60);
		curl_easy_setopt(easyConnect, CURLOPT_CONNECTTIMEOUT, 10);
		curl_easy_setopt(easyConnect, CURLOPT_WRITEFUNCTION, writeToString);
		curl_easy_setopt(easyConnect, CURLOPT_WRITEDATA, respData);
		/*curl_easy_setopt(easyConnect, CURLOPT_ERRORBUFFER, errorData);*/
		res = curl_easy_perform(easyConnect);
		if (res != CURLE_OK) {
			const char *errorMsg = curl_easy_strerror(res);
			size_t len = strlen(errorMsg);

			free(respData->data);
			respData->length = len;
			respData->data = (char*)malloc((len+1) * sizeof(char));
			strcpy(respData->data, errorMsg);
		}
	} else {
		return CURL_INIT_ERROR;
	}
	curl_easy_cleanup(easyConnect);

	return res;
}

