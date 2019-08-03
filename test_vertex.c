#include <glib.h>
#include <stdio.h>

#include "vertex.h"

typedef struct {
        vertex *v;
        edge *e1, *e2, *e3;
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
        vf->e2 = edge_new(0,2,3);
        vf->e3 = edge_new(0,3,4);
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
        if(!vertex_contains_edge(vf->v, vf->e1)){
                edge_free(vf->e1);
        }
        if(!vertex_contains_edge(vf->v, vf->e2)){
                edge_free(vf->e2);
        }
        vertex_free(vf->v);
}

void test_add_good_edge(vfixture *vf, gconstpointer ignored){
        /* add e2 to v and check that all the values are correctly adapted */
        vertex_add_edge_to_neighbourhood(vf->v, vf->e2);
        g_assert_cmpint(vf->v->dim, ==, 2);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight+vf->e2->weight);
        g_assert_true(vf->v->edges[0] == vf->e1); /* compare pointers */
        g_assert_true(vf->v->edges[1] == vf->e2); /* compare pointers */
}

void test_add_double_edge(vfixture *vf, gconstpointer ignored){
        /* add e1 again and check that this does not change the dimension
         * Note that this is not supposed to give errors */
        vertex_add_edge_to_neighbourhood(vf->v, vf->e1);
        g_assert_cmpint(vf->v->dim, ==, 1);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight);
        g_assert_true(vf->v->edges[0] == vf->e1); /* compare pointers */
        g_assert_false(vf->v->edges[1] == vf->e1); /* this should be invalid memory so
                                                      most likely not equal to e1 */
}

void test_add_self_edge(vfixture *vf, gconstpointer ignored){
        /* edit e2 in a way that it becomes a self-edge and check that trying
         * to add that gives an assertion error since this should never be
         * allowed. */

        /* read the manual for subprocess and this particular recipe */
         if (g_test_subprocess()){
             vf->e2->v2 = vf->e2->v1;
             vertex_add_edge_to_neighbourhood(vf->v, vf->e2);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

void test_remove_edge(vfixture *vf, gconstpointer ignored){
        /* remove e1 and check that the vertex is empty after that */
        vertex_rm_edge_from_neighbourhood(vf->v, vf->e1);
        g_assert_cmpint(vf->v->dim, ==, 0);
        g_assert_cmpint(vf->v->local_weight, ==, 0);
        g_assert_false(vf->v->edges[0] == vf->e1); /* vf->v->edges[0] should be invalid */
}

void test_remove_edge_large(vfixture *vf, gconstpointer ignored){
        /* this is needed to executed the insides of the for-loop in
         * rm_i_th_edge */

        /* add_edge has been tested above, so can safely be used */
        vertex_add_edge_to_neighbourhood(vf->v, vf->e2);
        vertex_add_edge_to_neighbourhood(vf->v, vf->e3);
        vertex_rm_edge_from_neighbourhood(vf->v, vf->e2);
        g_assert_cmpint(vf->v->dim, ==, 2);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight + vf->e3->weight);
        g_assert_true(vf->v->edges[1] == vf->e3); /* vf->v->edges[0] should be invalid */
}

void test_remove_nonexistent_edge(vfixture *vf, gconstpointer ignored){
        /* remove e2 and check that nothing changes, since e2 is not contained */
        vertex_rm_edge_from_neighbourhood(vf->v, vf->e2);
        g_assert_cmpint(vf->v->dim, ==, 1);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight);
        g_assert_true(vf->v->edges[0] == vf->e1);
        g_assert_false(vf->v->edges[1] == vf->e1); /* vf->v->edges[0] should be invalid */
        g_assert_false(vf->v->edges[1] == vf->e2); /* vf->v->edges[0] should be invalid */
}
        
int main(int argc, char **argv){
        g_test_init(&argc, &argv, NULL);
        /* Tests for vertex_add_edge_to_neighbourhood */
        g_test_add("/set1/good edge", vfixture, NULL,
                   vertex_setup, test_add_good_edge, vertex_teardown);
        g_test_add("/set1/double edge", vfixture, NULL,
                   vertex_setup, test_add_double_edge, vertex_teardown);
        g_test_add("/set1/self edge", vfixture, NULL,
                   vertex_setup, test_add_self_edge, vertex_teardown);

        /* Tests for vertex_rm_edge_from_neighbourhood */
        g_test_add("/set2/remove existing edge", vfixture, NULL,
                   vertex_setup, test_remove_edge, vertex_teardown);
        g_test_add("/set2/remove existing edge large set", vfixture, NULL,
                   vertex_setup, test_remove_edge_large, vertex_teardown);
        g_test_add("/set2/remove non-existing edge", vfixture, NULL,
                   vertex_setup, test_remove_nonexistent_edge, vertex_teardown);
        return g_test_run();
}
