#ifndef GLIST_H_
#define GLIST_H_

#include "graph.h"

/* Linked list of graphs module.
 * Describes the GraphList struct that is used to implement O, P. */

/* Node that contains a graph pointer */
typedef struct _GraphNode {
	Graph* G;
    struct _GraphNode *next;
} GraphNode;

/* Struct represents a linked list that contains graphs */
typedef struct _GraphList {

	/* Pointer to first element in list (actually a stack) */
	GraphNode *current_graph_node;

	/* Removes and returns graph from list beginning */
	GraphNode*	(*deque)(struct _GraphList *L);

	/* Frees all resources of glist,
	 * AND ALL GRAPHS\GRAPH_NODES CONTAINED IN IT */
	void	(*free_glist)(struct _GraphList *L);

	/* Add graph to list beginning */
	void	(*enque)(struct _GraphList *L, GraphNode *new_graph);

} GraphList;

/* Allocates memory to a new glist */
GraphList* allocateGraphList();

/* Frees all resources of glist,
*  AND ALL GRAPHS\GRAPH_NODES CONTAINED IN IT */
void freeGraphList(GraphList *L);

/* Removes and returns graph from list beginning */
GraphNode* deque(GraphList *L);

/* Add graph to list beginning */
void enque(GraphList *L, GraphNode *new_graph);

#endif /* GLIST_H_ */
