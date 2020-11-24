#ifndef SERVER_H
#define SERVER_H

#include "errorMsg.h"
#include <uv.h>

typedef struct {
	uv_write_t wbuf;
	uv_buf_t buf;
} write_buf_t;

int new_server(void);
int set_server_data(const char* addr, int port, int backlog);

#endif
