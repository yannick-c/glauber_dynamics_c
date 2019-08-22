/** \file weightedgraph.h
 * \brief Define the graph struct and functions relating to it.
 *
 * Any function not strictly acting only on either \ref edge or \ref vertex
 * instances but on both usually should be contained in here.*/

#include <stdio.h>
#include "vertex.h"

/** \typedef graph
 * \brief The typedef of the \ref graph struct.
 *
 * \struct graph weightedgraph.h lib/weightedgraph/include/weightedgraph.h
 * \brief The graph struct containing the pointers to edges and vertices. \see edge \see vertex*/
typedef struct graph {
        int n;                  /**< \brief The number of vertices in the graph.*/
        int m;                  /**< \brief The number of edges in the graph.*/
        vertex **vertices; /**< \brief List of \ref vertex pointers for vertices contained in the graph */
        edge **edges; /**< \brief List of \ref edge pointers for vertices contained in the graph */
} graph;

/** \brief Allocate memory for an empty graph.
 * \return Pointer to the newly allocated empty graph. */
graph *graph_new();

/** \brief Free all the memory contained in the graph.
 *
 * This will iterate through the vertices and free all the memory allocated to
 * edges and vertices contained in the graph.
 *
 * \param g The graph to be freed. */
void graph_free(graph *g);

/** \brief Add n newly created empty vertices to the graph. 
 *
 * The vertices have labels going from i+0 to i+n-1 where i is the initial
 * number of vertices in the graph.
 *
 * \param g The graph to which to add the n vertices.
 * \param n The number of vertices to add.
 * \see vertex_new */
void graph_add_n_vertices(graph *g, int n);

/** \brief Connect vertices with index v1 and v2 with an edge of weight weight.
 *
 * This will create a new edge and
 * add an edge to an existing graph between two vertices if the connection does
 * not already exist.
 **/
void graph_add_edge(graph *g, int v1, int v2, int weight);

/** \brief Searches for an \ref edge connecting v1 and v2 and removes it.
 *
 * This will only remove existing edges, if the edge does not exist nothing
 * happens.
 *
 * \param g The graph from which to remove the vertex.
 * \param v1 One end of the edge to remove.
 * \param v2 The other end of the edge to remove. */
void graph_rm_edge(graph *g, int v1, int v2);


/** \brief Allocate memory for a square lattice with periodic boundary
 * conditions (i.e. a torus).
 *
 * Construct a hypercube of length n and dimension d and connect the boundaries
 * appropriately whereby all the edges have initial weight init_weight. This
 * essentially corresponds to \\Z^d \\setminus n\\Z.
 *
 * NOTE: Do not use this with n<3 since then you forcibly will get double or
 * self-edges to fulfill periodic boundary conditions.
 *
 * \param n The amount of particles in one direction (before the periodically
 * connected boundaries are hit).
 * \param d The dimension of the graph.
 * \param init_weight The initial weight assigned to all edges.
 *
 * \returns A pointer to the graph corresponding to the d-dimensional n-torus.
 *
 * \todo Move this and draw_torus2png to its own header in libweightedgraph and
 * define a easily extendable general {graph_constructing_function,
 * graph_drawing_function} struct and make graphs choosable from the command
 * line.*/
graph *graph_construct_torus(int n, int d, int init_weight);

/** \brief Output a png rendering of draw_torus to out_stream.
 *
 * This works by generating a valid graphviz string out of draw_torus and then
 * invoking the graphviz C libraries to render those into png streams and
 * outputting them to oustream or stdout if outstream=NULL.
 *
 * The penwidth and decrease rate are needed to calculate how much the edge
 * weight influences the penwidth drawing. The formula goes like
 *
 *      penwidth * (edge_weight/max_weight_in_the_graph)^decrease_rate
 *
 * \param draw_torus The graph to be drawn (should correspond to the output of
 * \ref graph_construct_torus).
 * \param n The amount of particles in one direction (before the periodically
 * connected boundaries are hit) (same as \ref graph_construct_torus).
 * \param d The dimension of the graph (same as \ref graph_construct_torus).
 * \param duration The amount of times the frame is copied ot out_stream.
 * \param out_stream The opened file to which to copy the rendered png. Can be
 * set to NULL to automatically get stdout.
 * \param max_width Corresponds to width parameter in the graphviz image.
 * \param max_height Corresponds to height paramter in the graphviz image.
 * \param max_dpi corresponds to the dpi (i.e. resolution) in the graphviz
 * image.
 * \param penwidth Is the maximum penwidth in the graphviz image for the edges.
 * \param passed_time The parameter by which to divide the weights (i.e. the
 * time passed until now).
 *
 * \todo Add a drawing function for values d other than 2.
 *  
 * \todo Separate the string generation and png rendering to make the png
 * rendering reusable by other graphviz string generating drawing functions.
 *  
 * \todo Understand graphviz library better and avoid the string generation
 * step alltogether and directly build the graph itself.
 *
 * \todo Find a more elegant solution to the duration parameter than just
 * rendering the same frame duration amount of times. Ideally a way that allows
 * for double duration (i.e. generate one frame and assign it a duration on
 * sendover through the pipe) since the exponential times are doubles and not
 * integers..
 */
void draw_torus2png(graph *draw_torus, int n, int d, unsigned int duration,
                    FILE *out_stream, int max_width, int max_height, int max_dpi,
                    int penwidth, double passed_time);
