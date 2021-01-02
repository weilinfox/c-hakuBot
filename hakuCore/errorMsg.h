#ifndef ERRORMSG_H
#define ERRORMSG_H

//#define DEBUG

/*gen code*/
#define GEN_UPDATE_FLAG 1010
#define GEN_SLEEP_FLAG 1009
#define GEN_QUIT_FLAG 0

/*General*/
#define NO_ERROR 0
#define WARNING -20
#define POINTER_NONEMPTY_ERROR -1
#define OUT_OF_RANGE_ERROR -2
#define MALLOC_ERROR -21
#define MULTIPLE_ERRORS -3
#define UNKOWN_ERROR -4

/*Files*/
#define INIT_CONFIG_ERROR -5

/*Server*/
#define SERVER_ALREADY_STARTED -6
#define SERVER_DATA_ERROR -7

/*Curl*/
#define CURL_GLOBAL_INIT_ERROR -19
#define CURL_INIT_ERROR -8
#define CURL_BUFFER_ERROR -9
#define SET_PARMS_ERROR -10

/*Python*/
#define PYTHON_INIT_ERROR -23

/*Http*/
#define UNKOWN_METHOD -11
#define METHOD_LENGTH_EXCEED -12
#define PATH_LENGTH_EXCEED -13
#define PROTOCOL_LENGTH_EXCEED -14
#define HEADER_PARSE_ERROR -15
#define DATA_PARSE_ERROR -16

/*Json*/
#define JSON_PARSE_ERROR -17
#define NO_SUCH_MEMBER_ERROR -18

/*Api*/
#define SEND_MESSAGE_ERROR -22

/*Other*/
#define ERROR_CODE_MAX -23

char* get_error_msg(int errorCode);

#endif
