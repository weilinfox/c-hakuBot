#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	set_server_data("0.0.0.0", 8000);
	int res = new_server();

	printf("%d\n", res);

	curl_global_cleanup();

	return 0;
}

