#include "pcg-c/include/pcg_variants.h"
#include "weightedgraph.h"

/* do a glauber evolution on init_state using the graph_update function for a
 * maximum of max_time */
void glauber_dynamics(graph *init_state,
                       void (*graph_update)(graph*, int, pcg32_random_t *rng),
                       int max_time);
