# c-hakuBot
A new version of hakuBot writen by C.

## DEP
+ libcurl
+ libuv
+ libhttp-parser
+ libjson-glib: req libgio


## API

### sendRequest.h

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

## hakuMain.h

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

## server.h

```c
/*start a new http server return uv_run result*/
int new_server(void);
/*set bind address, port and backlog*/
int set_server_data(const char* addr, int port, int backlog);
```

## json.h

```c
int getJsonValue (const char *jsonData, char **resultText, const char *memberName);
```

## reference

[glib-compiling](https://developer.gnome.org/glib/stable/glib-compiling.html)