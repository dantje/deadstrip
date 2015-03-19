/***************************************************************************//**
 * @file list.c
 * @author Dorian Weber
 * @brief Implementation of the linked list.
 ******************************************************************************/

#include "list.h"

#include <stdlib.h>
#include <assert.h>

/* *************************************************************** structures */

typedef struct s_element
{
	void* data;
	struct s_element* next;
} element;

struct s_list
{
	element *top, *curr;
};

/* ******************************************************* exported functions */

list* newList()
{
	list* res = (list*) malloc(sizeof(list));
	res->curr = res->top = (element*) calloc(1, sizeof(element));
	return res;
}

void deleteList(list* src)
{
	assert(src);
	
	while ((src->curr = src->top->next))
	{
		free(src->top);
		src->top = src->curr;
	}

	free(src->top);
	free(src);
}

void listAdd(list* src, const void* data)
{
	element* nE = (element*) malloc(sizeof(element));
	
	assert(src);
	
	nE->data = (void*) data;
	nE->next = src->curr->next;
	src->curr = src->curr->next = nE;
}

void listRemove(list* src)
{
	element* tmp;
	
	assert(src);
	
	tmp = src->curr;
	
	if (src->top == tmp)
		tmp = tmp->next;
	if (src->top->next == tmp)
		src->top->next = tmp->next;
	
	src->curr = tmp->next;
	free(tmp);
}

void listSet(list* src, const void* data)
{
	assert(src);
	assert(src->curr != src->top);
	
	src->curr->data = (void*) data;
}

void* listGet(list* src)
{
	assert(src);
	return src->curr->data;
}

int listNext(list* src)
{
	assert(src);
	if (src->curr->next)
	{
		src->curr = src->curr->next;
		return 1;
	}
	return 0;
}

void listStart(list* src)
{
	assert(src);
	src->curr = src->top;
}

int listCount(list* src)
{
	int res = 0;
	element* tmp;
	
	assert(src);
	
	tmp = src->top;
	
	while ((tmp = tmp->next))
		++res;
	
	return res;
}

int listIsEmpty(list* src)
{
	assert(src);
	if (src->top->next)
		return 0;
	return 1;
}
