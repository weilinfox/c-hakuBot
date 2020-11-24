#ifndef JSON_H
#define JSON_H

#include "errorMsg.h"
#include <json-glib/json-glib.h>

#define MAX_ERROR_MSG_LEN 256

enum jsonDataType {TYPE_INT64, TYPE_STRING};

int getJsonValue (const char *jsonData, void **resultPointer, int dataType, const char *memberName);


#endif
