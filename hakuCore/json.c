#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getTextCb (JsonObject *obj, const gchar *key, JsonNode *node, gpointer userData)
{
	gchar *data = (gchar*)userData;
	GType type = json_node_get_value_type(node);
	if (g_type_is_a(type, G_TYPE_STRING)) {
		data = (gchar*)json_node_get_string(node);
	}
}

int getJsonValue(const char *data, char **result, const char *member)
{
	if (*result != NULL) return RESULT_NONEMPTY_ERROR;
	JsonParser *jsonParser = json_parser_new();
	GError *error = NULL;
	json_parser_load_from_data(jsonParser, data, strlen(data), &error);

	if (error) {
		*result = (char*)malloc(sizeof(char)*MAX_ERROR_MSG_LEN);
		snprintf(*result, MAX_ERROR_MSG_LEN-2, "Json parse Error: %s", error->message);
		g_error_free(error);
		g_object_unref(jsonParser);
		return JSON_PARSE_ERROR;
	}

	char *text = NULL;
	JsonNode *jsonRoot = json_parser_get_root(jsonParser);
	JsonObject *rootObject = json_node_get_object(jsonRoot);
	//printf("Has this member? %d\n", json_object_has_member(rootObject, "text"));
	if (json_object_has_member(rootObject, member)) {
		//JsonObject *memberObject = json_object_get_object_member(rootObject, "text");
		//printf("member object get pointer: %p\n", memberObject);
		//json_object_foreach_member(memberObject, getTextCb, text);
		//getTextCb();
		//GList *valueList = json_object_get_values(rootObject);
		//printf("Length of valueList: %d\n", g_list_length(valueList));
		text = (char*)json_object_get_string_member(rootObject, member);
		//printf("text pointer is: %p\ndata is: %s\n", text, text);
		//text = (char*)g_list_nth_data(valueList, 0);
		*result = (char*)malloc(sizeof(char)*(strlen(text)+2));
		//printf("result pointer is: %p\n", *result);
		strcpy(*result, text);
		//printf("length is: %ld\n", strlen(*result));
		//printf("data is: %s\n", *result);
	} else {
		g_object_unref(jsonParser);
		return NO_SUCH_MEMBER_ERROR;
	}

	g_object_unref(jsonParser);

	return 0;
}

