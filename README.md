# c-hakuBot
A new version of hakuBot writen by C.

## DEP
+ libcurl
+ libuv
+ libjson-glib


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

### hakuMain.h

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
```

```c
/*start a new parsing thread*/
/*httpMsg is origin http tcp data*/
/*return code : 0 - no error, QUIT_FLAG - quit server*/
int new_thread(const char *httpMsg);
```

### server.h

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

### api.h

```c
/*auto_escape usually be 0*/
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

