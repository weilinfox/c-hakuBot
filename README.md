# c-hakuBot
A new version of hakuBot writen by C.

## DEP

### ubuntu

+ libcurl-dev
+ libuv-dev
+ libjson-glib-dev
+ python3-dev

### fedora

+ libcurl-devel
+ libuv-devel
+ json-glib-devel
+ python3-devel

## BUILD DEP

+ make
+ pkg-config
+ gcc

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
int haku_block_attach(int64_t id);
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
int init_python_plugin (void);
so_file_t* open_so_file (const char *name);
void free_so_file_tree (void);
PyObject* get_python_plugin (char *name);
char* run_python_plugin (PyObject *pyPlugin, event_t *newEvent);
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

## 插件

插件中应该至少包含三个函数，分别用于初始化插件，实现插件功能和卸载插件。鉴于动态链接库和python模块的实现，插件被设计成只载入一次和只在退出时卸载，在插件中对数据进行短暂的保存成为可能。

+ init\_func()

对插件进行初始化，包括一些插件中的全局变量以及指针的初始化。只在初次载入时调用一次。

+ func(event\_t *)

插件的主体功能，每次调用插件都将调用这个函数。

+ end\_func()

释放插件占用的资源。

+ python

python的设计告诉我们不需要为他们设计专门的方法来初始化之，因此只有一个 ``func()`` 。它和之前用python编写的小白获得的参数格式是一致的，不同的只是只能同过返回一个str来实现消息的返回。因此很大程度上它是向下兼容的。

## reference

[glib-compiling](https://developer.gnome.org/glib/stable/glib-compiling.html)
[Json object](https://developer.gnome.org/json-glib/stable/json-glib-JSON-Object.html)
[Json node](https://developer.gnome.org/json-glib/stable/json-glib-JSON-Node.html)
[Type information](https://developer.gnome.org/gobject/stable/gobject-Type-Information.html)
[Error Reporting](https://developer.gnome.org/glib/stable/glib-Error-Reporting.html)

