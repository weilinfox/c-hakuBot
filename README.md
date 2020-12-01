# c-hakuBot
A new version of hakuBot writen by C.

## DEP
+ libcurl
+ libuv
+ libjson-glib

fedora:
+ libcurl-devel
+ libuv-devel
+ json-glib-devel


## API

### request.h

```c
typedef struct {
	size_t length;
	char *data;
} resp_data;
```

```c
/*send GET request and return CURLcode*/
/*store data in respData*/
CURLcode getData (resp_data *respData, const char *url, int num, ...);
```

### hakuMind.h

```c
typedef struct {
	char httpMethod[HTTP_METHOD_LEN];		/*GET/POST*/
	char httpPath[HTTP_PATH_LEN];			/*PATH*/
	char httpProtocol[HTTP_PROTOCOL_LEN];	/*HTTP/1.1*/
	char *httpHeader[HTTP_HEADER_NUM];		/*header data name*/
	char *httpHeaderData[HTTP_HEADER_NUM];	/*header data*/
	char *httpData;							/*data*/
	size_t headerNum;						/*num of header data*/
	size_t dataLen;							/*length of data*/
} http_header_t;


typedef struct time_list_node {
	time_t time;
	struct time_list_node* next;
} time_list_node_t;

typedef struct {
	int64_t selfId;
	int64_t heartBeat;
	time_t lastHeartBeat;
	time_t wakeTime;
} hakuLive;
```


```c
/*set haku data*/
void awake_haku(void);
void haku_sleep(void);
int haku_master_attach(int64_t id);
/*start a new parsing thread*/
/*httpMsg is origin http tcp data*/
/*return code : 0 - no error, QUIT_FLAG - quit server*/
int new_thread(const char *httpMsg);
```

### server.h

```c
typedef struct {
	uv_write_t wbuf;
	uv_buf_t buf;
} write_buf_t;
```

```c
/*start a new http server return uv_run result*/
int new_server(void);
/*set bind address, port and backlog*/
int set_server_data(const char* addr, int port, int backlog);
```

### json.h

```c
enum jsonDataType = {TYPE_INIT64, TYPE_STRING};
```


```c
int getJsonValue (const char *jsonData, void **resultPointer, int dataType, const char *memberName);
```

### plugin.h

```c
typedef char* (*func_t)(event_t*);

typedef struct {
	void* handle;
	func_t *func;
} so_file_t;

typedef struct so_file_tree {
	char data;
	so_file_t* soFile;
	struct so_file_tree *LNode, *RNode, *FNode;
} so_name_tree_t;
```

```c
int init_so_file_tree (void);
so_file_t* open_so_file (const char *name);
void free_so_file_tree (void);
```

### api.h

```c
typedef struct {
	char eventType[EVENT_NAME_LEN];
	char eventName[EVENT_NAME_LEN];
	char eventMessage[EVENT_MESSAGE_LEN];
	char eventOtherData[EVENT_DATA_LEN];
	time_t eventTime;
	time_t eventInterval;
	int64_t groupId;
	int64_t userId;
	int64_t selfId;
	int64_t error;
} event_t, new_event_t;
```

```c
int set_api_data(const char *url, int64_t port, const char* access_token);
void clear_api_data(void);
```


```c
/*auto_escape usually be 0*/
int reply_message(const event_t *newEvent, const char* replyMessage);
int send_private_message(const char* msg, int64_t qq_id, int auto_escape);
int send_group_message(const char* msg, int64_t group_id, int auto_escape);
int get_friend_list(char* returnData[]);
int get_group_list(char* returnData[]);
int get_group_member_list(int64_t group_id, char* returnData[]);
```


### errorMsg.h
Set error flags

## reference

[glib-compiling](https://developer.gnome.org/glib/stable/glib-compiling.html)
[Json object](https://developer.gnome.org/json-glib/stable/json-glib-JSON-Object.html)
[Json node](https://developer.gnome.org/json-glib/stable/json-glib-JSON-Node.html)
[Type information](https://developer.gnome.org/gobject/stable/gobject-Type-Information.html)
[Error Reporting](https://developer.gnome.org/glib/stable/glib-Error-Reporting.html)

