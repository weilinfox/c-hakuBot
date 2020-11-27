#include "plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

so_name_tree_t *soNameTreeHead = NULL;

/*virtualize a trie tree with a binary tree*/

int init_so_file_tree (void)
{
	soNameTreeHead = (so_name_tree_t*)malloc(sizeof(so_name_tree_t));
	if (soNameTreeHead == NULL)
		return MALLOC_ERROR;
	soNameTreeHead->soFile = NULL;
	soNameTreeHead->LNode = soNameTreeHead->RNode = soNameTreeHead->FNode = NULL;
	soNameTreeHead->data = 0;
	return NO_ERROR;
}

so_file_t* open_so_file (so_name_tree_t *head, const char *name)
{
	so_name_tree_t *node = head, *lastNode;
	int i;
	char *fileName = (char*)malloc(sizeof(char)*MAX_SOFILE_NAME_LEN);

	sprintf(fileName, "plugins/%s.so", name);

	/*find the string*/
	for (i = 0; name[i]; i++) {
		/*find the right node*/
		if (node->RNode) {
			lastNode = node;
			node = node->RNode;
		} else {
			/*no more right node*/
			break;
		}

		/*search the left node*/
		while (node) {
			if (node->data == name[i])
				break;
			else
				node = node->LNode;
		}

		/*no such node == name[i]*/
		if (node == NULL) {
			/*fallback to last right node*/
			node = lastNode;
			break;
		}
	}

	/* if no such string add it to the tree
	 * else return pointer
	 */
	if (name[i]) {
		for ( ; name[i]; i++) {
			if (node->RNode == NULL) {
				/*new right node*/
				node->RNode = (so_name_tree_t*)malloc(sizeof(so_name_tree_t));
				if (node->RNode == NULL)
					return (so_file_t*)MALLOC_ERROR;
				node->RNode->FNode = node;
				node = node->RNode;
				node->data = name[i];
				node->LNode = node->RNode = NULL;
				node->soFile = NULL;
			} else {
				/*new left node*/
				node = node->RNode;
				while (node->LNode)
					node = node->LNode;
				node->LNode = (so_name_tree_t*)malloc(sizeof(so_name_tree_t));
				if (node->LNode == NULL)
					return (so_file_t*)MALLOC_ERROR;
				node->LNode->FNode = node;
				node = node->LNode;
				node->data = name[i];
				node->RNode = node->LNode = NULL;
				node->soFile = NULL;
			}
		}

		/*open the so file*/
		node->soFile = (so_file_t*)malloc(sizeof(so_file_t));
		node->soFile->handle = dlopen(fileName, RTLD_LAZY);
		if (node->soFile->handle)
			node->soFile->func = (func_t*)dlsym(node->soFile->handle, "func");
		if (node->soFile->handle && node->soFile->func)
			return node->soFile;

		/*open failed*/
		if (node->soFile->handle)
			dlclose(node->soFile->handle);
		free(node->soFile);
		node->soFile = NULL;

		/*free file name*/
		while (1) {
			if (node->soFile || node->LNode || node->RNode)
				break;
			lastNode = node;
			node = node->FNode;
			free(lastNode);
			if (node->LNode == lastNode)
				node->LNode == NULL;
			else
				node->RNode == NULL;
		}

		return NULL;
	} else {
		/*get this plugin*/
		return node->soFile;
	}
}

void free_so_file_tree (so_name_tree_t *head)
{
	/*free so file tree and close all .so*/
	if (head == NULL) return;
	so_name_tree_t *rNode, *lNode = head;
	so_name_tree_t *lastNode;

	while (lNode) {
		if (lNode->RNode) {
			free_so_file_tree(lNode->RNode);
		} else {
			lastNode = lNode;
			lNode = lNode->LNode;
		}
		if (lastNode->soFile) {
			dlclose(lastNode->soFile->handle);
		}
		free(lastNode);
	}
}
