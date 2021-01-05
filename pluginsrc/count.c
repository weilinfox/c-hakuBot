#include "hakuCore/request.h"
#include "hakuCore/json.h"
#include "hakuCore/api.h"

#include <stdio.h>

int a;

void init_func(void)
{
	a = 0;
}

char* func (event_t *newEvent)
{
	char *str;
	str = (char*)malloc(sizeof(char)*64);
	sprintf(str, "%d", a++);
	return str;
}

