/**
 * Define the vertex class containing all the edges incident to it.
 **/

#include "edge.h"

typedef struct vertex {
                int dim;               /* number of slots in array (local dimension)*/
                double local_weight;   /* sum of all weights of connected edges */
                edge **edges;          /* list of neighbours ordered by neighobur index */
} vertex;;

/* create a new vertex struct instance */
vertex *vertex_new();

/**
 * free the vertex and all edges pointing to it
 *
 * NOTE: Do not do this before removing the edges from all other vertices
 * neighbourhoods in the graph since otherwise they will be pointing to bad
 * memory adresses. Clean the other vertices first before freeing the one in
 * question.
 **/
void vertex_free(vertex *v);

/**
 * add the edge to the neighbourhood incrementing d and len accordingly if it
 * is not yet part of the neighbourhood
 **/
void vertex_add_edge_to_neighbourhood(vertex *v, edge *e);

/** remove an edge (if it exists) from neighbourhood decrementing d and len
 * accordingly **/
void vertex_rm_edge_from_neighbourhood(vertex *v, edge *e);
