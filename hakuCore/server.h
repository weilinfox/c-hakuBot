#ifndef SERVER_H
#define SERVER_H

#include <uv.h>

#define SERVER_ALREADY_STARTED -1
#define SERVER_DATA_ERROR -2

typedef struct {
	uv_write_t wbuf;
	uv_buf_t buf;
} write_buf_t;

int new_server(void);
int set_server_data(const char* addr, int port, int backlog);

#endif
