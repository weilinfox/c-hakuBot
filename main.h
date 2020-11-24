#ifndef MAIN_H
#define MAIN_H

#include "hakuCore/errorMsg.h"
#include "hakuCore/request.h"
#include "hakuCore/server.h"
#include "hakuCore/json.h"

#define CONFIG_FILE "config.json"
#define CONFIG_FILE_LEN 1024

#define INIT_CONFIG 1

const char* initConfigData = "{\n"
	"\t\"URL\": \"127.0.0.1\",\n"
	"\t\"PORT\": 8000\n"
	"}\n";


#endif
