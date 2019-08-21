/** \file edge.h
 *  \brief Define the edge struct and functions relating to it. */

/**\typedef edge
 * \brief typedef of the \ref edge struct.
 *
 * \struct edge edge.h "lib/weightedgraph/include/edge.h"
 * \brief Edge struct to be used as edges in the graph structs.
 */
typedef struct edge {
        int v1; /**< \brief The integer of vertex in \ref graph of one end of edge. */
        int v2; /**< \brief The integer of vertex in \ref graph of the other end of edge. */
        double weight; /**< \brief The weight for this particular edge. */
} edge;

/** \brief Allocate a new edge with the corresponding entries for \ref edge.
 *  \param v1 the integer that will be in edge->v1.
 *  \param v2 the integer that will be in edge->v2.
 *  \param weight the integer that will bein edge->weight.
 *  \return The pointer to the new edge.
 */
edge *edge_new(int v1, int v2, int weight);

/** \brief Free the memory taken by the \ref edge.
 *  \param e edge to be freed. */
void edge_free(edge *e);
