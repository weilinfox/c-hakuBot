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

int getJsonValue(const char *data, void **result, int type, const char *member)
{
	if (result != NULL && *result != NULL) return POINTER_NONEMPTY_ERROR;
	if (data == NULL) {
		if (result == NULL)
			return JSON_PARSE_ERROR;
		if (type == TYPE_STRING) {
			*result = malloc(sizeof(char)*MAX_ERROR_MSG_LEN);
			snprintf((char*)*result, MAX_ERROR_MSG_LEN-2, "Json data is empty!");
			return JSON_PARSE_ERROR;
		} else if (type == TYPE_INT64) {
			*result = malloc(sizeof(gint64));
			**(gint64**)result = 1;
			return JSON_PARSE_ERROR;
		} else {
			return MULTIPLE_ERRORS;
		}
	}
	JsonParser *jsonParser = json_parser_new();
	GError *error = NULL;
	json_parser_load_from_data(jsonParser, data, strlen(data), &error);

	if (error) {
		fprintf(stderr, "Json parse error.\n");
		if (result == NULL) {
			g_error_free(error);
			g_object_unref(jsonParser);
			return JSON_PARSE_ERROR;
		}
		if (type == TYPE_STRING) {
			*result = malloc(sizeof(char)*MAX_ERROR_MSG_LEN);
			snprintf((char*)*result, MAX_ERROR_MSG_LEN-2, "Json parse Error: %s", error->message);
			g_error_free(error);
			g_object_unref(jsonParser);
			return JSON_PARSE_ERROR;
		} else if (type == TYPE_INT64) {
			*result = malloc(sizeof(gint64));
			**(gint64**)result = error->code;
			return JSON_PARSE_ERROR;
		} else {
			return MULTIPLE_ERRORS;
		}
	} else {
		free(error);
	}
	fprintf(stdout, "Json parse finished\n");
	
	/*search and get data*/
	JsonNode *rootNode = json_parser_get_root(jsonParser);
	JsonObject *rootObject = json_node_get_object(rootNode);
	if (json_object_has_member(rootObject, member)) {
		fprintf(stdout, "Json parse: find member\n");
		JsonNode *thisNode = json_object_get_member(rootObject, member);
		GType thisType = json_node_get_value_type(thisNode);
		//json_object_foreach_member(memberObject, getTextCb, text);
		if (type == TYPE_STRING && g_type_is_a(thisType, G_TYPE_STRING)) {
			if (result == NULL) {
				g_object_unref(jsonParser);
				return NO_ERROR;
			}
			char *text = NULL;
			text = (char*)json_node_get_string(thisNode);
			*result = malloc(sizeof(char)*(strlen(text)+2));
			strcpy((char*)*result, text);
		} else if (type == TYPE_INT64 && g_type_is_a(thisType, G_TYPE_INT64)) {
			if (result == NULL) {
				g_object_unref(jsonParser);
				return NO_ERROR;
			}
			*result = malloc(sizeof(gint64));
			**(gint64**)result = json_node_get_int(thisNode);
		} else {
			g_object_unref(jsonParser);
			return MULTIPLE_ERRORS;
		}
	} else {
		g_object_unref(jsonParser);
		fprintf(stderr, "Json parse: no such member\n");
		if (result == NULL)
			return NO_SUCH_MEMBER_ERROR;
		if (type == TYPE_STRING) {
			*result = malloc(sizeof(char)*16);
			strcpy((char*)*result, "No such member.");
		} else if (type == TYPE_INT64) {
			*result = malloc(sizeof(gint64));
			**(gint64**)result = NO_SUCH_MEMBER_ERROR;
		} else {
			return MULTIPLE_ERRORS;
		}
		return NO_SUCH_MEMBER_ERROR;
	}

	g_object_unref(jsonParser);

	return NO_ERROR;
}

