/** \file glauber_dynamics.h
 * \brief Contains main dynamics and main function.*/

#include "pcg_variants.h"
#include "update_rules.h" // contains weightedgraph.h

/** \typedef arguments
 * \brief Typedef of the \ref arguments struct.
 *
 * \struct arguments glauber_dynamics.h include/glauber_dynamics.h
 * \brief Struct containing the results of the inline command arguments.
 *
 * For details refer to
 * 
 *      ./glauber_dynamics --help
 *
 * The particular values have matching names so are not further documented
 * here.
 */
typedef struct arguments {
	int silent; /**< \brief Default: 0. */
    int do_init; /**< \brief Goes to 1 if -i is mentioned (even without fname)
                      Default: 0. */
	int n; /**< \brief Default: 10. */
    int d; /**< \brief Default: 2. */
    int max_time; /**< \brief Default: 10000. */
    int width; /**< \brief Default: 5. */
    int height; /**< \brief Default: 5. */
    int dpi; /**< \brief Default: 200. */
    int penwidth; /**< \brief Default: 10. */
    double alpha; /**< \brief Default: 0.5. */
    double frame_density; /**< \brief Default: 1. */

	/* string options */
	char *init_fname; /**< optional init_fname option. Default: init */
    char *output; /**< output fname. Default: final */
} arguments;

/** \brief Do a glauber evolution on init_state using the provided update rule.
 * 
 * \param init_state Pointer to the initial state, note that this is changed in
 * place and after running the function the pointer points to the evolved graph.
 * \param graph_update \ref update_rule which should be run on every vertex every exp(1)
 * time.
 * \param threshold_time Maximum time for which the system runs.
 * \param args \ref arguments from parsed command-line arguments.
 *
 * \see graph */
void glauber_dynamics(graph *init_state, update_rule graph_update, int threshold_time, arguments *args);
