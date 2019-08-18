#include "pcg_variants.h"
#include "weightedgraph.h"

/**
 * This file is used to outline the possible update rules to graphs once the
 * Poisson clock on a vertex has rung, the functions should be of the form
 *
 * void graph_update(graph *state, int vertex_index, double alpha, pcg32_random_t *rng)
 *
 * and update the pointed to graph using the random value given by rng in case
 * it is needed.
 **/

typedef void (*update_rule)(graph*, int, double, pcg32_random_t*);

update_rule polya_update;
