#include <stdio.h>
#include <math.h>

/* random number generator implementation */
#include "pcg_variants.h"
#include "entropy.h"

#include "glauber_dynamics.h"

/* use two different rngs for the exponential clocks and the vertex choosing
 * which ensures their independence, also use them globally for this file */
pcg32_random_t exponential_rng, uniform_rng, update_rng;

/* get an exponential random variable */
static double exponential_rand(double lambda_rate){
        /* first generate a 'double' using the recipe in the docs for the C
         * implementation */
        double unif_dbl = ldexp(pcg32_random_r(&exponential_rng), -32); 
        /* for the following line consider that -log(U)/lambda_rate with U
         * uniform on [0, 1) gives an exponential distribution */
        return - log(unif_dbl)/lambda_rate;
}

/* 
 * For the implementation of poisson clocks on every vertex use that the
 * time between events of a Poisson point process of rate 1 is the exponential
 * distribution with rate 1. Furthermore, for n vertices with independent
 * exponential distribution clocks {exp_i}_{i\in [0,...,n-1]} on them the
 * next time something fires has distribution min({exp_i}) which is again
 * exponentially distributed with rate n (i.e. the sum of rates).
 * 
 * Hence, we can take a single exponential time and then choose the vector
 * uniformly which is less computatinally intensive than putting an exponential
 * clock on every vertex and managing their order.
 *
 * NOTE: The RNG is initialized only here so glauber_dynamics has to be called
 * as the first and only function of this file.
 */
void glauber_dynamics(graph *init_state,
                      update_rule graph_update,
                      int threshold_time){
        double t = 0; /* time parameter */

        /* start RNG initialization */
        uint64_t seeds1[2], seeds2[2], seeds3[2];
        entropy_getbytes((void*)seeds1, sizeof(seeds1));
        entropy_getbytes((void*)seeds2, sizeof(seeds2));
        entropy_getbytes((void*)seeds3, sizeof(seeds3));
        pcg32_srandom_r(&exponential_rng, seeds1[0], seeds1[1]);
        pcg32_srandom_r(&uniform_rng, seeds2[0], seeds2[1]);
        pcg32_srandom_r(&update_rng, seeds3[0], seeds3[1]);
        /* end RNG initialization */

        while (t < threshold_time){
                t += exponential_rand((double) init_state->n);

                /* it generates strictly smaller than bound so init_state->n is
                 * fine. */
                int chosen_vertex_index = pcg32_boundedrand_r(&uniform_rng,
                                                              init_state->n);

                /* use the passed graph_update rule to update the graph state */
                graph_update(init_state, chosen_vertex_index, &update_rng);
        }

}

int main(){
        graph *torus = graph_construct_torus(2, 2, 1);
        glauber_dynamics(torus, polya_update, 100);
        draw_torus(torus, 2, 2);
        graph_free(torus);
}
