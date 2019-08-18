#include "pcg_variants.h"
#include "update_rules.h" // contains weightedgraph.h

/* config arguments struct */
typedef struct arguments {
	/* boolean options */
	int silent;

	/* actual int value options */
	int n, d, max_time, width, height, dpi, frame_density, penwidth, decrease_rate, do_init;
    double alpha;

	/* string options */
	char *init_fname, *output;
} arguments;

/* do a glauber evolution on init_state using the graph_update function for a
 * maximum of threshold_time */
void glauber_dynamics(graph *init_state, update_rule graph_update, int threshold_time, arguments *args);
