#ifndef PLUGIN_H
#define PLUGIN_H

#include "api.h"
#include "errorMsg.h"
#include <Python.h>

#define MAX_SOFILE_NAME_LEN 32

typedef char* (*func_t)(event_t*);
typedef void (*init_func_t)(void);

typedef struct {
	void* handle;
	func_t func;
} so_file_t;

typedef struct {
	PyObject *pyModule, *pyFunc;
} py_module_t;

typedef struct so_file_tree {
	char data;
	so_file_t* soFile;
	py_module_t* pyModule;
	struct so_file_tree *LNode, *RNode, *FNode;
} so_name_tree_t;

int init_so_file_tree (void);
int init_python_plugin (void);
so_name_tree_t* open_plugin (/*so_name_tree_t *headNode,*/ const char *name);
void free_so_file_tree (void/*so_name_tree_t *headNode*/);
//PyObject* get_python_plugin(char* name);
char* run_python_func(PyObject *pyFunc, event_t *newEvent);

#endif
