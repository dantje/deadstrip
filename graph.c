/***************************************************************************//**
 * @file graph.c
 * @author Dorian Weber
 * @brief Implementation of the directed graph class interface.
 ******************************************************************************/

#include "graph.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* *************************************************************** structures */

struct s_graph
{
	unsigned long color;
	char* name;
	list* con;
};

/* ******************************************************* exported functions */

graph* newGraph(const char* name)
{
	graph* res = (graph*) malloc(sizeof(graph));
	
	res->color = 0;
	res->name = strdup(name);
	res->con = newList();
	
	return res;
}

void deleteGraph(graph* src)
{
	assert(src);
	
	deleteList(src->con);
	free(src->name);
	free(src);
}

void graphConnect(graph* src, const graph* dest)
{
	assert(src && dest);
	
	
	/* check for double connections - could surely be optimized a bit */
	listStart(src->con);
	while (listNext(src->con))
	{
		if (listGet(src->con) == dest)
			return;
	}
	/* check passed, so add that new connection */
	listAdd(src->con, dest);
}

void graphColorNode(graph* src, unsigned long color)
{
	assert(src);
	src->color = color;
}

unsigned long graphGetColorNode(graph* src)
{
	assert(src);
	return src->color;
}

const char* graphGetNameNode(graph* src)
{
	assert(src);
	return src->name;
}

list* graphGetConnections(graph* src)
{
	assert(src);
	return src->con;
}
