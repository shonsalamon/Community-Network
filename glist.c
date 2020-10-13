#include "glist.h"
#include "validator.h"
#include <stdio.h>
#include <stdlib.h>

/* Allocates memory to a new GraphList */
GraphList* allocateGraphList() {
	GraphList *L;

	L = (GraphList*)malloc(sizeof(GraphList));
	checkPointerForNull(L, "FAILED TO MALLOC GLIST");

	L->deque = &deque;
	L->enque = &enque;
	L->free_glist = &freeGraphList;

	return L;
}

/* Frees all resources of GraphList,
   AND ALL GRAPHS\GRAPH_NODES CONTAINED IN IT. */
void freeGraphList(GraphList *L) {
	GraphNode *next, *current;

	current = L->current_graph_node;

	if (current != NULL) {
		next = current->next;
		while (next != NULL) {
			current->G->freeGraph(current->G);
			free(current);
			current = next;
			next = current->next;
		}
		current->G->freeGraph(current->G);
	}
	free(current);
	free(L);
}

/* Removes and returns graph from list beginning */
GraphNode* deque(GraphList *L) {
	GraphNode *removed;

	removed = L->current_graph_node;
	L->current_graph_node = L->current_graph_node->next;
	removed->next = NULL;

	return removed;
}

/* Add graph to list beginning */
void enque(GraphList *L, GraphNode *new_graph) {
	new_graph->next = L->current_graph_node;
	L->current_graph_node = new_graph;
}
