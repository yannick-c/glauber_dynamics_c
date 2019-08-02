#include <glib.h>
#include <stdio.h>

#include "vertex.h"

typedef struct {
        vertex *v;
        edge *e1, *e2;
} vfixture;

void vertex_setup(vfixture *vf, gconstpointer test_data){
        vf->v = vertex_new();
        *vf->v = (vertex){.dim=1, .local_weight=2}; /* local weight of vf->e1 defined 2 lines down */
        /* add an edge from the start */
        vf->e1 = edge_new(0,1,2); /* use non trivial weights */
                                  /* v1 and v2 are different since edge.c and
                                   * vertex.c check for non-existence of
                                   * self-edges, the values have no meaning */
        vf->v->edges=realloc(vf->v->edges, sizeof(edge*));
        vf->v->edges[0]=vf->e1;
        vf->e2 = edge_new(0,1,3);
}

/* quick for loop to avoid double freeing edges */
int static vertex_contains_edge(vertex *v, edge *e){
        for (int i=0; i<v->dim; i++){
                if (v->edges[i] == e){
                        return 1;
                }
        }
        return 0;
}
void vertex_teardown(vfixture *vf, gconstpointer test_data){
        vertex_free(vf->v);
        if(!vertex_contains_edge(vf->v, vf->e1)){
                edge_free(vf->e1);
        }
        if(!vertex_contains_edge(vf->v, vf->e2)){
                edge_free(vf->e2);
        }
}

void test_add_good_edge(vfixture *vf, gconstpointer ignored){
        /* add e2 to v and check that all the values are correctly adapted */
        vertex_add_edge_to_neighbourhood(vf->v, vf->e2);
        g_assert_cmpint(vf->v->dim, ==, 2);
        g_assert_cmpint(vf->v->local_weight, ==, 5); /* 2 + 3 = 5 */
        g_assert(vf->v->edges[0] == vf->e1); /* compare pointers */
        g_assert(vf->v->edges[1] == vf->e2); /* compare pointers */
}

int main(int argc, char **argv){
        g_test_init(&argc, &argv, NULL);
        g_test_add("/set1/goodedge test", vfixture, NULL,
                   vertex_setup, test_add_good_edge, vertex_teardown);
        return g_test_run();
}
