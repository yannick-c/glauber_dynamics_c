#include <glib.h>

#include "weightedgraph.h"

typedef struct {
        graph *g;
} gfixture;


void graph_setup(gfixture *gf, gconstpointer test_data){
        gf->g = graph_new();
}

void graph_teardown(gfixture *gf, gconstpointer test_data){
        graph_free(gf->g);
}

/* test first the adding of vertices */
void test_add_n_vertices(gfixture *gf, gconstpointer ignored){
        graph_add_n_vertices(gf->g, 5);
        g_assert_cmpint(gf->g->n, ==, 5);
        g_assert_cmpint(gf->g->m, ==, 0);
        for (int i=0; i<5; i++){
                /* compare dim of the vertices as a makeshift way to confirm
                 * that we have indeed vertices in g->vertices */
                g_assert_cmpint(gf->g->vertices[i]->dim, ==, 0);
        }
}

/* have a new setup with some initial vertices with which to call all following
 * tests */
void graph_setup_w_5_vertices(gfixture *gf, gconstpointer test_data){
        gf->g = graph_new();
        graph_add_n_vertices(gf->g, 5);
}

void test_add_new_edge(gfixture *gf, gconstpointer ignored){
        graph_add_edge(gf->g, 2, 3, 4);
        g_assert_cmpint(gf->g->m, ==, 1);
        g_assert_cmpint(gf->g->edges[0]->v1, ==, 2);
        g_assert_cmpint(gf->g->edges[0]->v2, ==, 3);
        g_assert_cmpint(gf->g->edges[0]->weight, ==, 4);
}

void test_add_existing_edge(gfixture *gf, gconstpointer ignored){
        /* same as previous but adding the same edge twice */
        graph_add_edge(gf->g, 2, 3, 4);
        graph_add_edge(gf->g, 2, 3, 4);
        g_assert_cmpint(gf->g->m, ==, 1);
        g_assert_cmpint(gf->g->edges[0]->v1, ==, 2);
        g_assert_cmpint(gf->g->edges[0]->v2, ==, 3);
        g_assert_cmpint(gf->g->edges[0]->weight, ==, 4);
}

void test_add_invalid_v1_vertex_edge(gfixture *gf, gconstpointer ignored){
        /* try adding an edge where v1 is larger than the amount of vertices */
         if (g_test_subprocess()){
                 graph_add_edge(gf->g, 6, 1, 1);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

void test_add_invalid_v2_vertex_edge(gfixture *gf, gconstpointer ignored){
        /* try adding an edge where v2 is larger than the amount of vertices */
         if (g_test_subprocess()){
                 graph_add_edge(gf->g, 1, 6, 1);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

/* setup function to test edge removal */
void graph_setup_w_5_vert_1_edge(gfixture *gf, gconstpointer test_data){
        gf->g = graph_new();
        graph_add_n_vertices(gf->g, 5);
        graph_add_edge(gf->g, 2, 3, 1);
}

void test_remove_valid_edge(gfixture *gf, gconstpointer test_data){
        graph_rm_edge(gf->g, 2, 3);
        g_assert_cmpint(gf->g->m, ==, 0);
}

void test_remove_invalid_edge(gfixture *gf, gconstpointer test_data){
        graph_rm_edge(gf->g, 0, 1);

        g_assert_cmpint(gf->g->m, ==, 1);

        g_assert_cmpint(gf->g->edges[0]->v1, ==, 2);
        g_assert_cmpint(gf->g->edges[0]->v2, ==, 3);
        g_assert_cmpint(gf->g->edges[0]->weight, ==, 1);
}

void test_rm_invalid_v1_vertex_edge(gfixture *gf, gconstpointer ignored){
        /* try removing an edge where v1 is larger than the amount of vertices */
         if (g_test_subprocess()){
                 graph_rm_edge(gf->g, 6, 1);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

void test_rm_invalid_v2_vertex_edge(gfixture *gf, gconstpointer ignored){
        /* try removing an edge where v2 is larger than the amount of vertices */
         if (g_test_subprocess()){
                 graph_rm_edge(gf->g, 1, 6);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

void test_finding_existing_edge(gfixture *gf, gconstpointer ignored){
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[2], 3) == gf->g->edges[0]);
}

void test_finding_non_existing_edge(gfixture *gf, gconstpointer ignored){
        g_assert_false(graph_find_connecting_edge(gf->g->vertices[2], 1));
        g_assert_false(graph_find_connecting_edge(gf->g->vertices[1], 2));
}

void test_graph_construct_torus(gfixture *gf, gconstpointer ignored){
        /* since graph_construct_torus mallocs a new graph free the old one */
        free(gf->g);

        /* construct a 3x3 (i.e. 3 vertices long in 2 dimensions) torus */
        gf->g = graph_construct_torus(3, 2, 1);
        g_assert_cmpint(gf->g->n, ==, 9);
        g_assert_cmpint(gf->g->m, ==, 18);
        
        /* explicitly check for all the edges to be present, the torus should
         * look like this
         *   |   |   |
         * - 0 - 1 - 2 -
         *   |   |   |
         * - 3 - 4 - 5 -
         *   |   |   |
         * - 6 - 7 - 8 -
         *   |   |   |
         * with boundaries being connected */
        
        /* first make sure every vertex has the same dimension 4 */
        for (int i=0; i<gf->g->n; i++){
                g_assert_cmpint(gf->g->vertices[i]->dim, ==, 4);
        }

        /* order is always, top right bottom left */
        /* vertex 1 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[0], 6));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[0], 1));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[0], 3));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[0], 2));

        /* vertex 2 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[1], 7));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[1], 2));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[1], 4));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[1], 0));

        /* vertex 3 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[2], 8));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[2], 0));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[2], 5));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[2], 1));

        /* vertex 4 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[3], 0));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[3], 4));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[3], 6));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[3], 5));

        /* vertex 5 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[4], 1));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[4], 5));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[4], 7));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[4], 3));

        /* vertex 6 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[5], 2));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[5], 3));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[5], 8));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[5], 4));

        /* vertex 7 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[6], 3));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[6], 7));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[6], 0));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[6], 8));

        /* vertex 8 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[7], 4));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[7], 8));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[7], 1));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[7], 6));

        /* vertex 9 */
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[8], 5));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[8], 6));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[8], 2));
        g_assert_true(graph_find_connecting_edge(gf->g->vertices[8], 7));
}
int main(int argc, char **argv){
        g_test_init(&argc, &argv, NULL);
        /* Tests for graph_add_n_vertices */
        g_test_add("/add_n_vertices/add n vertices", gfixture, NULL,
                   graph_setup, test_add_n_vertices, graph_teardown);

        /* Tests for  graph_add_edge */
        g_test_add("/graph_add_edge/add new edge", gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_new_edge, graph_teardown);
        g_test_add("/graph_add_edge/add existing edge", gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_existing_edge, graph_teardown);
        g_test_add("/graph_add_edge/test invalid v1 vertex adding", gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_invalid_v1_vertex_edge, graph_teardown);
        g_test_add("/graph_add_edge/test invalid v2 vertex adding", gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_invalid_v2_vertex_edge, graph_teardown);

        /* Tests for graph_rm_edge */
        g_test_add("/graph_rm_edge/remove existing edge", gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_remove_valid_edge, graph_teardown);
        g_test_add("/graph_rm_edge/remove non-existing edge", gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_remove_invalid_edge, graph_teardown);
        g_test_add("/graph_rm_edge/remove invalid v1 edge", gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_rm_invalid_v1_vertex_edge,
                   graph_teardown);
        g_test_add("/graph_rm_edge/remove invalid v2 edge", gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_rm_invalid_v2_vertex_edge,
                   graph_teardown);

        /* Tests for graph_find_connecting_edge */
        g_test_add("/graph_find_connecting_edge/find existing", gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_finding_existing_edge, graph_teardown);
        g_test_add("/graph_find_connecting_edge/find non-existing edge", gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_finding_non_existing_edge,
                   graph_teardown);

        /* Test for torus construction */
        g_test_add("/graph_construct_torus/construct 3x3 torus", gfixture, NULL,
                   graph_setup, test_graph_construct_torus, graph_teardown);

        return g_test_run();
}
