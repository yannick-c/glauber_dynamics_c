/** \file vertex.h 
 * \brief Define the vertex struct and functions relating to it.
 *
 * The vertex struct is defined with its general memory management functions
 * and \ref edge adding and removing utitilities are defined.
 **/

#include "edge.h"

/** \typedef vertex
 * \brief The typedef of the \ref vertex struct.
 *
 * \struct vertex vertex.h lib/weightedgraph/include/vertex.h
 * \brief The vertex struct containing the neighbouring edges as pointers.
 * \see edge*/
typedef struct vertex {
                int dim;               /**< \brief The number of slots in array (local dimension)*/
                double local_weight;   /**< \brief The sum of all weights of connected edges */
                edge **edges;          /**< \brief The list of neighbour \ref edge instances as pointers */
} vertex;;

/** \brief Allocate the memory for a new vertex with no neighbours.
 * \return The pointer to the newly allocated vertex. */
vertex *vertex_new();

/** \brief Free the input \ref vertex pointer and all edges pointing to it.
 *
 * The edges are freed in this function since they would otherwise be invalid.
 * Invoke this function only after removing the edge from all other
 * neighbourhoods so that those will not contain an invalid neighbour (cf.
 * \ref graph_rm_edge).
 *
 * \param v The vertex to be freed.
 *
 * \todo Add graph_rm_vertex to \ref weightedgraph.h which automatically takes care
 * of removing edges from all neighbours and then frees the vertex.
 **/
void vertex_free(vertex *v);

/** \brief Add the \ref edge e to the neighbourhood v->edges.
 *
 * This also increments \ref vertex.dim and \ref vertex.local_weight accordingly.
 *
 * \param v Pointer to the \ref vertex whose neighbourhood should be extended.
 * \param e Pointer to the \ref edge to add to the neighbourhood of v.
 **/
void vertex_add_edge_to_neighbourhood(vertex *v, edge *e);

/** \brief Remove \ref edge e from neighbourhood of \ref vertex v.
 *
 * The function checks if e is contained in the neighbourhood of v and does
 * nothing if that is not the case. This also decrements \ref vertex.dim and
 * \ref vertex.local_weight accordingly.
 *
 * \param v Pointer to the \ref vertex whose nieghbourhood should be shrunk.
 * \param e Pointer to the \ref edge to remove fromthe neighbourhood of v. */
void vertex_rm_edge_from_neighbourhood(vertex *v, edge *e);

/** \brief Find the edge that connects \ref vertex v with the vertex of index
 * dst.
 *
 * \param v The vertex whose neighbourhood is to be searched.
 * \param dst The target vertex of the desired edge.
 * \returns NULL if no edge is found or a pointer to the connecting \ref edge.
 **/
edge *vertex_find_connecting_edge(vertex *v, int dst);
