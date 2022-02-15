#define _GNU_SOURCE //cause stdio.h to include asprintf

#include <argp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> // malloc and free for file handling
#include <string.h> //strcmp

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
                      int threshold_time,
					  arguments *args){
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

        double prev_frame = 0; // when the previous frame was drawn
        while (t < threshold_time){
                double cur_time = exponential_rand((double) args->n);
                t += cur_time;

                /* it generates strictly smaller than bound so init_state->n is
                 * fine. */
                int chosen_vertex_index = pcg32_boundedrand_r(&uniform_rng,
                                                              init_state->n);

                /* use the passed graph_update rule to update the graph state */
                graph_update(init_state, chosen_vertex_index, args->alpha,
							 &update_rng);

                if (!args->silent && t-prev_frame>args->frame_density){
						draw_torus2png(init_state, args->n, args->d, round((t-prev_frame)),
									   NULL, args->width, args->height, args->dpi,
									   args->penwidth, t);
                        prev_frame=t;
                }
        }

}

/** begin initial argument parsing code **/
const char *argp_program_version = "glauber_dynamics 0.9";
const char *argp_program_bug_address = "yannick.couzinie@uniroma3.it";

/* Program documentation. */
static char doc[] =
  "General simulation code which simulates Poisson point process clocks "\
  "put on vertices which, when ringing, perform a certain update which can "\
  "easily be adapted by writing a custom update rule. Graphs are output using "\
  "graphviz and the code naturally facilitates piping into ffmpeg for rendering "\
  "videos.";

static struct argp_option options[] = {
		  {"alpha",			'a',	"double",	0,					"Set the alpha parameter for the update rules. The default is 0.5."},
		  {"num",			'n',	"int",		0,						"Set the number of vertices per dimension (i.e. on torus we have n^d vertices). "\
							      										"The default is 10."},	
		  {"dim",			'd',	"int",		0,						"Set the dimension of the lattice. The default is 2."},
		  {"max-time",		'm',	"int",		0,						"Maximum time to run the simulation for. The default is 10000."},
          {"quiet",			'q',	0, 			0,					 	"Do not output video frames to stdout." },
		  {"silent", 		's',	0,			OPTION_ALIAS},
		  {"init-frame", 	'i',	"FILENAME",	OPTION_ARG_OPTIONAL,	"Save a frame of the system state after 10 time steps in init.png "\
						         										"or in FILENAME.png if supplied."},
          {"output",		'o', 	"FILENAME", 0,  					"Output the final state to FILENAME.png instead of final.png." },
		  {"width",			'w',	"int",		0, 						"Set the max width for the frames in inches (graphviz option). The default is 5."},
		  {"height",		'h',	"int",		0, 						"Set the max height for the frames in inches (graphviz option). The default is 5."},
		  {"dpi",			'r',	"int",		0, 						"Set the max dpi (dots per inch, i.e. resolution) for the frames (graphviz option). "\
							    				   						"The default is 200."},
		  {"frame-density",	'f',	"int",		0, 						"Save a frame only every frame-density time units. Serves as coarse graining to reduce "\
							    				   						"output video size. The default is 1 (i.e. approximately every full time unit a frame is saved)."},
		  {"max-penwidth",	'p',	"int",		0, 						"Maximum penwidth used to draw the highest weighted edge (graphviz option). The default is 10."},
                  { 0 }
};

/* use that the strto functions return the remaining string part after parsing
 * and check that that part is empty otherwise return error messages. Only to
 * be used inside parse_opt */
void static check_input(char *remaining_str, char *error_msg, struct argp_state *state){
        /* use that non-empty strings have length >0 and are thus true */
        if (strlen(remaining_str)){
                argp_error(state, "%s", error_msg);
        }
}

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
        /* Get the input argument from argp_parse, which we
         * know is a pointer to our arguments structure. */
        struct arguments *args = state->input;

        char *remaining_str; /* use this to check the input */

		switch (key){
				case 'q': case 's':
				  		args->silent = 1;
						break;
				case 'a':
						args->alpha = strtod(arg, &remaining_str);
                        check_input(remaining_str,
                                    "False input for alpha (a), only input doubles. Example: -a 0.5.",
                                    state);
						break;
				case 'f':
						args->frame_density = strtod(arg, &remaining_str);
                        check_input(remaining_str,
                                    "False input for frame-density (f), only doubles. "
                                    "Example: -f 1.",
                                    state);
						break;
				case 'n':
						args->n = (int) strtol(arg, &remaining_str, 10);
                        check_input(remaining_str,
                                    "False input for num (n), only input integers. Example: -n 10.",
                                    state);
						break;
				case 'd':
						args->d = (int) strtol(arg, &remaining_str, 10);
                        check_input(remaining_str,
                                    "False input for dim (d), only input integers. Example: -d 2.",
                                    state);
						break;
				case 'm':
						args->max_time = (int) strtol(arg, &remaining_str, 10);
                        check_input(remaining_str,
                                    "False input for max_time (m), only input integers. Example: -m 10000.",
                                    state);
						break;
				case 'i':
                        args->do_init = 1;
						args->init_fname = arg;
						break;
				case 'o':
				  		args->output = arg;
						break;
				case 'w':
						args->width = (int) strtol(arg, &remaining_str, 10);
                        check_input(remaining_str,
                                    "False input for width (w), only input integers. Example: -w 5.",
                                    state);
						break;
				case 'h':
						args->height = (int) strtol(arg, &remaining_str, 10);
                        check_input(remaining_str,
                                    "False input for height (h), only input integers. Example: -h 5.",
                                    state);
						break;
				case 'r':
						args->dpi = (int) strtol(arg, &remaining_str, 10);
                        check_input(remaining_str,
                                    "False input for dpi (r), only input integers. Example: -r 200.",
                                    state);
						break;
				case 'p':
						args->penwidth = (int) strtol(arg, &remaining_str, 10);
                        check_input(remaining_str,
                                    "False input for penwidth (p), only input integers. Example: -p 1.",
                                    state);
						break;
				default:
						return ARGP_ERR_UNKNOWN;
				}
				return 0;
		}

static struct argp argp = { options, parse_opt, NULL, doc };

int main(int argc, char **argv){
		arguments args;
        args.silent=0;
		args.init_fname="DO NOT INIT";
        args.do_init=0;
		args.output="final.png";
		args.alpha=0.5;
		args.n=10;
		args.d=2;
		args.max_time=10000;
		args.width=5;
		args.height=5;
		args.dpi=200;
		args.frame_density=1.0;
		args.penwidth=10;

		argp_parse (&argp, argc, argv, 0, 0, &args);

        graph *torus = graph_construct_torus(args.n, args.d, 1);

		if (args.do_init){
				glauber_dynamics(torus, polya_update, 10, &args);
				if (args.init_fname == NULL){
						args.init_fname = "init.png";
				}
				FILE *init_state = fopen(args.init_fname, "w");
				draw_torus2png(torus, args.n, args.d, 1, init_state,
							   args.width, args.height, args.dpi,
							   args.penwidth, 10);
				fclose(init_state);
		}
				
        glauber_dynamics(torus, polya_update, args.max_time-10, &args);

        FILE *final_state = fopen(args.output, "w");
		draw_torus2png(torus, args.n, args.d, 1, final_state,
					   args.width, args.height, args.dpi,
					   args.penwidth, args.max_time);
        fclose(final_state);

        graph_free(torus);
}
