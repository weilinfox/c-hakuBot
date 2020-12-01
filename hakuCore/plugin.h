#ifndef PLUGIN_H
#define PLUGIN_H

#include "api.h"
#include "errorMsg.h"

#define MAX_SOFILE_NAME_LEN 32

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

int init_so_file_tree (void);
so_file_t* open_so_file (/*so_name_tree_t *headNode,*/ const char *name);
void free_so_file_tree (void/*so_name_tree_t *headNode*/);

#endif