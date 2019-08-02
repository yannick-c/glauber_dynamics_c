#include <glib.h>

#include "vertex.h"

typedef struct {
        vertex *v;
        edge *e1, *e2;
} vfixture;

void vertex_setup(vfixture *vf, gconstpointer test_data){
        vf->v = vertex_new();
        *vf->v = (vertex){.dim=1, .local_weight=1};
        /* add an edge from the start */
        vf->e1 = edge_new(0,0,2); /* use non trivial weights */
        vf->v->edges=realloc(vf->v->edges, sizeof(edge*));
        vf->v->edges[0]=vf->e1;
        vf->e2 = edge_new(0,0,3);
}

void vertex_teardown(vfixture *vf, gconstpointer test_data){
        vertex_free(vf->v);
        if(vf->e1){ edge_free(vf->e1); }
        if(vf->e2){ edge_free(vf->e2); }
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
