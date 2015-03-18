/***************************************************************************//**
 * @file graph.h
 * @author Dorian Weber
 * @brief Interface definition of a directed graph class.
 * @sa graph.c
 ******************************************************************************/

#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "list.h"

/* forward declaration of opaque structure */
typedef struct s_graph graph;

/**@brief Creates a new graph node.
 * @param[in] name  specifies the name of this node
 * @return pointer to a graph node
 */
extern graph* newGraph(const char* name);

/**@brief Frees all memory for this node.
 * @param[in] src  graph node
 */
extern void deleteGraph(graph* src);

/**@brief Connects two graphs in one direction.
 * @param[in] src   source node that marks the starting point of the edge
 * @param[in] dest  target node that marks the end point of the edge
 */
extern void graphConnect(graph* src, const graph* dest);

/**@brief Sets a color value for a certain graph node.
 * @param[in] src    graph node to color
 * @param[in] color  the color
 */
extern void graphColorNode(graph* src, unsigned long color);

/**@brief Returns a color value of a graph.
 * @param[in] src  node to read the color from
 * @return the color
 */
extern unsigned long graphGetColorNode(graph* src);

/**@brief Returns the nodes name.
 */
extern const char* graphGetNameNode(graph* src);

/**@brief Returns a list of all the other graphs that the node is connected to.
 */
extern list* graphGetConnections(graph* src);

#endif
