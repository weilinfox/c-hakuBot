#include "plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

const char *PLUGIN_DIR_NAME = "plugin";
const char *PLUGIN_FUNC_NAME = "func";
const char *PLUGIN_INIT_NAME = "init_func";
const char *PLUGIN_FREE_NAME = "end_func";
so_name_tree_t *soNameTreeHead = NULL;

/*virtualize a trie tree with a binary tree*/

int init_so_file_tree (void)
{
	soNameTreeHead = (so_name_tree_t*)malloc(sizeof(so_name_tree_t));
	if (soNameTreeHead == NULL)
		return MALLOC_ERROR;
	soNameTreeHead->soFile = NULL;
	soNameTreeHead->pyModule = NULL;
	soNameTreeHead->LNode = soNameTreeHead->RNode = soNameTreeHead->FNode = NULL;
	soNameTreeHead->data = 0;
	fprintf(stdout, "File tree init finished\n");
	return init_python_plugin();
}

so_name_tree_t* open_plugin (/*so_name_tree_t *head,*/ const char *name)
{
	so_name_tree_t *node = soNameTreeHead, *lastNode, *newNode = NULL;
	int i;
	char *fileName = (char*)malloc(sizeof(char)*MAX_SOFILE_NAME_LEN);

	sprintf(fileName, "%s/%s.so", PLUGIN_DIR_NAME, name);

	fprintf(stdout, "Start to find the string. %s\n", name);
	/*find the string*/
	for (i = 0; name[i]; i++) {
		/*find the right node*/
		if (node->RNode) {
			fprintf(stdout, "have right node: %c\n", node->RNode->data);
			lastNode = node;
			node = node->RNode;
		} else {
			/*no right node*/
			fprintf(stdout, "no right node\n");
			break;
		}

		/*search the left node*/
		while (node) {
			fprintf(stdout, "search left node: %c\n", node->data);
			if (node->data == name[i])
				break;
			else
				node = node->LNode;
		}

		/*no such node == name[i]*/
		if (node == NULL) {
			/*fallback to last right node*/
			fprintf(stdout, "no such left node\n");
			node = lastNode;
			break;
		}
	}

	/* if no such string add it to the tree
	 * else return pointer
	 */
	if (name[i] || (!node->soFile && !node->pyModule)) {
		fprintf(stdout, "Add the string to the tree.\n");
		for ( ; name[i]; i++) {
			if (node->RNode == NULL) {
				/*new right node*/
				fprintf(stdout, "new right node\n");
				node->RNode = (so_name_tree_t*)malloc(sizeof(so_name_tree_t));
				if (node->RNode == NULL)
					return (so_name_tree_t*)MALLOC_ERROR;
				else if (!newNode)
					newNode = node->RNode;
				node->RNode->FNode = node;
				node = node->RNode;
				node->data = name[i];
				node->LNode = node->RNode = NULL;
				node->soFile = NULL;
				node->pyModule = NULL;
			} else {
				/*new left node*/
				fprintf(stdout, "new left node\n");
				node = node->RNode;
				while (node->LNode)
					node = node->LNode;
				node->LNode = (so_name_tree_t*)malloc(sizeof(so_name_tree_t));
				if (node->LNode == NULL)
					return (so_name_tree_t*)MALLOC_ERROR;
				else if (!newNode)
					newNode = node->LNode;
				node->LNode->FNode = node;
				node = node->LNode;
				node->data = name[i];
				node->RNode = node->LNode = NULL;
				node->soFile = NULL;
				node->pyModule = NULL;
			}
		}

		/*open the so file*/
		fprintf(stdout, "Open the so file\n");
		node->soFile = (so_file_t*)malloc(sizeof(so_file_t));
		node->soFile->handle = dlopen(fileName, RTLD_LAZY);
		fprintf(stdout, "so file handle: %p\n", node->soFile->handle);
		if (node->soFile->handle)
			node->soFile->func = (func_t)dlsym(node->soFile->handle, PLUGIN_FUNC_NAME);
		if (node->soFile->handle && node->soFile->func) {
			init_func_t tmpFunc = (init_func_t)dlsym(node->soFile->handle, PLUGIN_INIT_NAME);
			if (tmpFunc) tmpFunc();
			return node;
		}
		if (node->soFile->handle)
			dlclose(node->soFile->handle);
		free(node->soFile);
		node->soFile = NULL;
		
		/*open python plugin*/
		sprintf(fileName, "%s/%s.py", PLUGIN_DIR_NAME, name);
		if (!access(fileName, R_OK)) {	
			fprintf(stdout, "Open the py plugin\n");
			node->pyModule = (py_module_t*)malloc(sizeof(py_module_t));
			node->pyModule->pyModule = node->pyModule->pyFunc = NULL;
			node->pyModule->pyModule = PyImport_ImportModule(name);
			if (node->pyModule->pyModule)
				node->pyModule->pyFunc = PyObject_GetAttrString(node->pyModule->pyModule, PLUGIN_FUNC_NAME);
			if (node->pyModule->pyFunc) return node;
		}

		/*no such plugin*/
		fprintf(stderr, "No such plugin.\n");
		if (node->pyModule && node->pyModule->pyModule) Py_CLEAR(node->pyModule->pyModule);
		if (node->pyModule) free(node->pyModule);
		node->pyModule = NULL;

		free(fileName);

		/*free file name*/
		fprintf(stdout, "Free the string now.\n");
		while (newNode) {
			if (node->FNode == NULL || node->soFile || node->pyModule || node->LNode || node->RNode)
				break;
			lastNode = node;
			node = node->FNode;
			fprintf(stdout, "free %c ", lastNode->data);
			free(lastNode);
			if (node->LNode == lastNode) {
				node->LNode = NULL;
				fprintf(stdout, "Left\n");
			} else {
				node->RNode = NULL;
				fprintf(stdout, "Right\n");
			}
		}

		return NULL;
	} else {
		/*get this plugin*/
		fprintf(stdout, "Find the string.\n");
		return node;
	}
}

void free_so_file_node (so_name_tree_t *head)
{
	/*free so file tree and close all .so*/
	if (head == NULL) return;
	so_name_tree_t *rNode, *lNode = head;
	so_name_tree_t *lastNode = NULL;

	fprintf(stdout, "Find node with data: %c\n", head->data);
	while (lNode) {
		free(lastNode);
		if (lNode->soFile) {
			fprintf(stdout, "Close so file.\n");
			init_func_t tmpFunc = (init_func_t)dlsym(lNode->soFile->handle, PLUGIN_FREE_NAME);
			if (tmpFunc) tmpFunc();
			dlclose(lNode->soFile->handle);
			free(lNode->soFile);
		} else if (lNode->pyModule) {
			fprintf(stdout, "Close py plugin.\n");
			Py_CLEAR(lNode->pyModule->pyFunc);
			Py_CLEAR(lNode->pyModule->pyModule);
			free(lNode->pyModule);
		}
		if (lNode->RNode) {
			fprintf(stdout, "Find R node\n");
			free_so_file_node(lNode->RNode);
		}
		lastNode = lNode;
		lNode = lNode->LNode;
	}
	free(lastNode);
}

void free_so_file_tree ()
{
	fprintf(stdout, "Free the so file tree...\n");
	free_so_file_node(soNameTreeHead);
	soNameTreeHead = NULL;
	fprintf(stdout, "Finished!\n");
}

int init_python_plugin (void)
{
	char *sysCom = (char*)malloc(sizeof(char) * 64);
	sprintf(sysCom, "sys.path.append('./%s')", PLUGIN_DIR_NAME);
	PyRun_SimpleString("import sys");
	PyRun_SimpleString(sysCom);
	free(sysCom);
	return NO_ERROR;
}

char* run_python_func(PyObject *func, event_t *newEvent)
{
	char *returnMsg = NULL;
	if (!func || !newEvent) return NULL;

	char *cStringMsg = NULL;

	PyObject* pArgs = Py_BuildValue("{s:s,s:s,s:i,s:i,s:i,s:s,s:s}", \
		"post_type", newEvent->eventType, \
		"message_type", newEvent->eventName, \
		"user_id", newEvent->userId, \
		"group_id", newEvent->groupId, \
		"self_id", newEvent->selfId, \
		"raw_message", newEvent->eventMessage, \
		"message", newEvent->eventMessage \
		);

	fprintf(stdout, "Call py function...");
	PyObject *pReply = PyObject_CallFunction(func, "O", pArgs);
	fprintf(stdout, "finished\n");

	PyArg_Parse(pReply, "z", &cStringMsg);
	if (cStringMsg == NULL) return NULL;
	returnMsg = (char*)malloc(sizeof(char)*(strlen(cStringMsg)+1));
	strcpy(returnMsg, cStringMsg);
	fprintf(stdout, "Plugin returned %s\n", returnMsg);

	Py_CLEAR(pArgs);
	Py_CLEAR(pReply);

	return returnMsg;
}

/*
PyObject* get_python_plugin (char *name)
{
	//char *fileName = (char*)malloc(sizeof(char)*MAX_SOFILE_NAME_LEN);
	PyObject *pModule = NULL;
	//sprintf(fileName, "%s.%s", PLUGIN_DIR_NAME, name);

	fprintf(stdout, "Try to open python plugin: %s .\n", name);
	pModule = PyImport_ImportModule(name);
	//free(fileName);

	return pModule;
}


char* run_python_plugin(PyObject *plugin, event_t *newEvent)
{
	char *returnMsg = NULL;
	if (!plugin || !newEvent) return NULL;

	fprintf(stdout, "Try to find python function\n");
	PyObject *pFunc = PyObject_GetAttrString(plugin, PLUGIN_FUNC_NAME);
	if (pFunc) {
		char *cStringMsg = NULL;

		PyObject* pArgs = Py_BuildValue("{s:s,s:s,s:i,s:i,s:i,s:s,s:s}", \
			"post_type", newEvent->eventType, \
			"message_type", newEvent->eventName, \
			"user_id", newEvent->userId, \
			"group_id", newEvent->groupId, \
			"self_id", newEvent->selfId, \
			"raw_message", newEvent->eventMessage, \
			"message", newEvent->eventMessage \
			);

		fprintf(stdout, "Call function.\n");
		PyObject *pReply = PyObject_CallFunction(pFunc, "O", pArgs);

		PyArg_Parse(pReply, "z", &cStringMsg);
		returnMsg = (char*)malloc(sizeof(char)*(strlen(cStringMsg)+1));
		strcpy(returnMsg, cStringMsg);
		fprintf(stdout, "Plugin returned %s\n", returnMsg);

		Py_CLEAR(pArgs);
		Py_CLEAR(pReply);
	}

	Py_CLEAR(pFunc);

	return returnMsg;
}
*/
