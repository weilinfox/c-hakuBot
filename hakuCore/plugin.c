#include "plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

const char *PLUGIN_DIR_NAME = "plugin";
const char *PLUGIN_FUNC_NAME = "func";
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
	fprintf(stdout, "File tree init finished\n");
	return NO_ERROR;
}

so_file_t* open_so_file (/*so_name_tree_t *head,*/ const char *name)
{
	so_name_tree_t *node = soNameTreeHead, *lastNode;
	int i;
	char *fileName = (char*)malloc(sizeof(char)*MAX_SOFILE_NAME_LEN);

	sprintf(fileName, "%s/%s.so", PLUGIN_DIR_NAME, name);

	fprintf(stdout, "Start to find the string.\n");
	/*find the string*/
	for (i = 0; name[i]; i++) {
		/*find the right node*/
		if (node->RNode) {
			fprintf(stdout, "have right node\n");
			lastNode = node;
			node = node->RNode;
		} else {
			/*no right node*/
			fprintf(stdout, "no right node\n");
			break;
		}

		/*search the left node*/
		while (node) {
			fprintf(stdout, "search left node\n");
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
	if (name[i]) {
		fprintf(stdout, "Add the string to the tree.\n");
		for ( ; name[i]; i++) {
			if (node->RNode == NULL) {
				/*new right node*/
				fprintf(stdout, "new right node\n");
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
				fprintf(stdout, "new left node\n");
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
		fprintf(stdout, "Open the so file\n");
		node->soFile = (so_file_t*)malloc(sizeof(so_file_t));
		node->soFile->handle = dlopen(fileName, RTLD_LAZY);
		fprintf(stdout, "so file handle: %p\n", node->soFile->handle);
		if (node->soFile->handle)
			node->soFile->func = (func_t)dlsym(node->soFile->handle, PLUGIN_FUNC_NAME);
		if (node->soFile->handle && node->soFile->func)
			return node->soFile;

		/*open failed*/
		fprintf(stderr, "No such plugin.\n");
		if (node->soFile->handle)
			dlclose(node->soFile->handle);
		free(node->soFile);
		node->soFile = NULL;

		/*free file name*/
		fprintf(stdout, "Free the string now.\n");
		while (1) {
			if (node->FNode == NULL || node->soFile || node->LNode || node->RNode)
				break;
			lastNode = node;
			node = node->FNode;
			fprintf(stdout, "free ");
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
		return node->soFile;
	}
}

void free_so_file_node (so_name_tree_t *head)
{
	/*free so file tree and close all .so*/
	if (head == NULL) return;
	so_name_tree_t *rNode, *lNode = head;
	so_name_tree_t *lastNode;

	while (lNode) {
		if (lNode->RNode) {
			free_so_file_node(lNode->RNode);
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

void free_so_file_tree ()
{
	fprintf(stdout, "Free the so file tree.\n");
	free_so_file_node(soNameTreeHead);
}
