/** \file update_rules.h
 * \brief Define a general update_rule class and contains examples for them.
 *
 * This file is used to outline the possible update rules to graphs once the
 * Poisson clock on a vertex has rung, the functions should be of the form
 *
 *      void graph_update(graph *state, int vertex_index, double alpha, pcg32_random_t *rng)
 *
 * and update the pointed to graph using the random value given by rng in case
 * it is needed.
 **/

#include "pcg_variants.h"
#include "weightedgraph.h"

/** \brief The update_rule function type to which all update rules should
 * adhere.
 *
 * \param graph The input graph to update.
 * \param int The index of the vertex to update (i.e. whose clock 'rang').
 * \param double The intrinsic alpha parameter of the model.
 * \param pcg32_random_t The RNG governing the randomness in the model.
 * \see glauber_dynamics */
typedef void (*update_rule)(graph*, int, double, pcg32_random_t*);

/** \brief The polya update rule leading to polya competition on the graph.
 *
 * For details see the paper by Yannick Couzinié and Christian Hirsch and the
 * references therein. Essentially the update rules are like polya urns on
 * every vertex reinforced with a power law which is governed by the double
 * parameter in the update rule (i.e. double alpha = 1 is linear
 * reinforcement). */
update_rule polya_update;
