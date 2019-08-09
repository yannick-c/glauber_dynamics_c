#include "weightedgraph.h"

/* do a glauber evolution on init_state using the graph_update function for a
 * maximum of max_time */
void glauber_evolution(graph *init_state,
                       void (*graph_update)(graph, vertex),
                       int max_time);
