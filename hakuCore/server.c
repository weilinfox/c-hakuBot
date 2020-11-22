#include "server.h"
#include "hakuMain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uv_loop_t *loop;
int serverLock = 0;
int serverPort = 8000;
char *serverAddr = NULL;
int serverBacklog = 0;

char *httpReply = "HTTP/1.0 200 OK\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Content-Length: 0\r\n"
		"Server: weilinfox-virHttp\r\n\n\n";

void on_close(uv_handle_t* handle)
{
	free(handle);
}

void new_buffer(uv_handle_t* handle, size_t size, uv_buf_t* buf)
{
	buf->base = (char*)malloc(size);
	buf->len = size;
}

void on_write(uv_write_t *req, int status)
{
	if (status) {
		fprintf(stdout, "Write Error %s\n", uv_strerror(status));
	}
	free(req);
}

void on_reply(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
	if (nread > 0) {
		write_buf_t *replyBuf = (write_buf_t*)malloc(sizeof(write_buf_t));
		replyBuf->buf = uv_buf_init(httpReply, strlen(httpReply));
		//printf("Connection got message: \n%s\n", buf->base);
		int res = uv_write((uv_write_t*)replyBuf, client, &replyBuf->buf, 1, on_write);
		if (res) {
			printf("Connection write Error with code: %d\n", res);
		}
		printf("Call new_thread here\n");
		res = new_thread(buf->base);
		if (res) {
			if (res == QUIT_FLAG) {
				printf("Quit server now.\n");
				uv_stop(loop);
			} else {
				printf("Quit new_thread with an Error code: %d\n", res);
			}
		}
		return;
	} else if (nread < 0) {
		if (nread != UV_EOF) {
			fprintf(stdout, "Read Error: %s\n", uv_err_name(nread));
		} else {
			fprintf(stdout, "Read End Nomally.\n");
		}
		uv_close((uv_handle_t*)client, on_close);
	}
	printf("Free buf->base here\n");
	free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status)
{
	if (status < 0) {
		printf("Error in listen callback with status: %d\n", status);
		return ;
	}
 
	uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(loop, client);

	if (uv_accept(server, (uv_stream_t*)client) == 0) {
		int res = uv_read_start((uv_stream_t*)client, new_buffer, on_reply);
		printf("Connection accepted with a read code: %d\n", res);
	} else {
		printf("Connection accept Error.\n");
		uv_close((uv_handle_t*)client, on_close);
	}
}

int set_server_data(const char* addrc, int p, int b)
{
	if (serverAddr)
		free(serverAddr);
	serverAddr = (char*)malloc(sizeof(char)*(strlen(addrc)+5));
	strcpy(serverAddr, addrc);
	serverPort = p;
	serverBacklog = b;
}

void quit_server()
{
	free(serverAddr);
	serverPort = 8000;
	serverAddr = NULL;
	serverLock = 0;
}

int new_server()
{
	struct sockaddr_in addr;

	if (serverLock) {
		printf("Server start Error: Server already started\n");
		return SERVER_ALREADY_STARTED;
	} else if (!serverAddr) {
		printf("Server start Error: Server address didn't set\n");
		return SERVER_DATA_ERROR;
	}
	serverLock = 1;
	
	loop = uv_default_loop();

	uv_tcp_t server;
	uv_tcp_init(loop, &server);

	uv_ip4_addr(serverAddr, serverPort, &addr);

	int res = uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
	
	if (res < 0) {
		printf("Bind Error at %s:%d .%s\n", serverAddr, serverPort, uv_strerror(res));
		return 0;
	}

	res = uv_listen((uv_stream_t*)&server, serverBacklog, on_new_connection);

	if (res) {
		printf("Listen Error on port %d: %s\n", serverPort, uv_strerror(res));
	} else {
		res = uv_run(loop, UV_RUN_DEFAULT);
		printf("UV_RUN returned code: %d\n", res);
	}
	
	//printf("free now?");
	//free(loop);
	//printf("free end?");

	quit_server();

	return res;
}

