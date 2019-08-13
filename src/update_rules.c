#include <math.h>
#include <stdlib.h>

#include "update_rules.h"

/* See paper Yannick Couzinie and Christian Hirsch */
#define ALPHA 0.5
void static polya_update_func(graph *state,
                              int vertex_index,
                              pcg32_random_t *rng){
        vertex *chosen_vertex = state->vertices[vertex_index];
        /* generate a double (using the recipe in the docs) and choose the edge
         * by adding their weights (starting from the first in state->edges)
         * until the sum goes over the generated uniform value. This
         * corresponds to choosing an edge with probability
         * edge->weight/vertex->local_weight */
        double unif_dbl = ldexp(pcg32_random_r(rng), -32); 

        double weight_sum = 0;
        double scaled_local_weight = 0;
        /* calculate the normalization constant once for the vertex */
        for (int i=0; i < chosen_vertex->dim; i++){
                scaled_local_weight += pow(chosen_vertex->edges[i]->weight, ALPHA);
        }

        for (int i=0; i < chosen_vertex->dim; i++){
                edge *cur_edge = chosen_vertex->edges[i];
                double normal_weight = pow(cur_edge->weight, ALPHA)/scaled_local_weight;
                if (weight_sum < unif_dbl && (weight_sum + normal_weight) > unif_dbl){
                        /* this edge is chosen so increment its weights */
                        cur_edge->weight++;
                        chosen_vertex->local_weight++;
                        return;
                }
                weight_sum += normal_weight;
        }
        abort(); /* this for loop should definitely not run without hitting return */
}

update_rule polya_update = &polya_update_func;
