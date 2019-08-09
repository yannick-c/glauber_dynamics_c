#include <glib.h>
#include <stdlib.h> // malloc

#include "vertex.h"

vertex *vertex_new(){
        vertex *out = malloc(sizeof(vertex));
        *out = (vertex){.dim=0,
                        .local_weight=0}; // initalize with no standard value for v->edges
        return out;
}

void vertex_free(vertex *v){
        for(int i=0; i<v->dim; i++){
                edge_free(v->edges[i]);
        }
        if(v->edges){
                free(v->edges);
        }
        free(v);
}

void vertex_add_edge_to_neighbourhood(vertex *v, edge *e){
        /* check that we are not trying to add a self-edge
         *
         * NOTE: This does not check for different edges connecting the same
         * vertices (should not happen when used with weightedgraphs). */
        g_assert(e->v1 != e->v2);        

        /* check that the edge is not already contained */
        for(int i=0; i<v->dim; i++){
                if (v->edges[i] == e){
                        return;
                }
        }
        v->dim++;
        v->local_weight += e->weight;
        v->edges = realloc(v->edges, sizeof(edge*)*v->dim);
        v->edges[v->dim-1] = e;
}

/**
 * make a new edges-array for the incoming vertex removing the i-th edge but
 * not freeing it.
 * 
 * Note that this is a pure utility function (hence static, i.e. limited to the
 * file) for vertex_rm_edge_from_neighbourhood.
 *
 * CAUTION: Always assign have this on the right side of an assignment to
 * v->edges since it frees the original v->edges.
 **/
edge static **rm_i_th_edge(vertex const *v, int to_remove){
        edge **out;
        edge **old_edges = v->edges;
        out = malloc(sizeof(edge*)*(v->dim-1));
        for (int i=0; i<v->dim-1; i++){
                /* skip the i-th index in assigning */
                if (i<to_remove) { out[i] = old_edges[i]; }
                else if (i>=to_remove) { out[i] = old_edges[i+1]; }
        }
        free(old_edges);
        return out;
}

void vertex_rm_edge_from_neighbourhood(vertex *v, edge *e){
        for(int i=0; i<v->dim; i++){
                /* compare memory adresses */
                if(v->edges[i] == e){
                        v->edges = rm_i_th_edge(v, i);
                        v->dim--;
                        v->local_weight -= e->weight;
                        return;
                }
        }
}
