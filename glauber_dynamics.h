#include "pcg-c/include/pcg_variants.h"
#include "update_rules.h" // contains weightedgraph.h

/* do a glauber evolution on init_state using the graph_update function for a
 * maximum of threshold_time */
void glauber_dynamics(graph *init_state, update_rule graph_update, int threshold_time);
