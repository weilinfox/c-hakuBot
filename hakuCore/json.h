#ifndef JSON_H
#define JSON_H

#include <json-glib/json-glib.h>

#define MAX_ERROR_MSG_LEN 256

#define RESULT_NONEMPTY_ERROR -1
#define JSON_PARSE_ERROR -2
#define NO_SUCH_MEMBER_ERROR -3

int getJsonValue (const char *jsonData, char **resultText, const char *memberName);


#endif
