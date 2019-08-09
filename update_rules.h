#include "pcg-c/include/pcg_variants.h"
#include "weightedgraph.h"

/**
 * This file is used to outline the possible update rules to graphs once the
 * Poisson clock on a vertex has rung, the functions should be of the form
 *
 * void graph_update(graph *state, int vertex_index, pcg32_random_t *rng)
 *
 * and update the pointed to graph using the random value given by rng in case
 * it is needed.
 **/

void polya_update(graph *state, int vertex_index, pcg32_random_t *rng);
