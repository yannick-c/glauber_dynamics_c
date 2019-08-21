/** \file test_weightedgraph.c
 * \brief Glib testing based test code for \ref weightedgraph.h.*/

#include <glib.h>

#include "weightedgraph.h"

/** \brief Fixture around the \ref graph struct. */
struct gfixture{
        graph *g; /**< \brief The main graph object of the tests. */
};


/** \brief Setup function which initializes a gfixture. */
void graph_setup(struct gfixture *gf, gconstpointer test_data){
        gf->g = graph_new();
}

/** \brief Teardown function for gfixture. */
void graph_teardown(struct gfixture *gf, gconstpointer test_data){
        graph_free(gf->g);
}

/** \brief Test that \ref graph_add_n_vertices works as expected */
void test_add_n_vertices(struct gfixture *gf, gconstpointer ignored){
        graph_add_n_vertices(gf->g, 5);
        g_assert_cmpint(gf->g->n, ==, 5);
        g_assert_cmpint(gf->g->m, ==, 0);
        for (int i=0; i<5; i++){
                /* compare dim of the vertices as a makeshift way to confirm
                 * that we have indeed vertices in g->vertices */
                g_assert_cmpint(gf->g->vertices[i]->dim, ==, 0);
        }
}

/** \brief Extended setup which directly adds 5 vertices.
 *
 * This setup can only follow after \ref test_add_n_vertices since only now we
 * know that it works as expected.*/
void graph_setup_w_5_vertices(struct gfixture *gf, gconstpointer test_data){
        gf->g = graph_new();
        graph_add_n_vertices(gf->g, 5);
}

/** \brief Test that \ref graph_add_edge works as expected. */
void test_add_new_edge(struct gfixture *gf, gconstpointer ignored){
        graph_add_edge(gf->g, 2, 3, 4);
        g_assert_cmpint(gf->g->m, ==, 1);
        g_assert_cmpint(gf->g->edges[0]->v1, ==, 2);
        g_assert_cmpint(gf->g->edges[0]->v2, ==, 3);
        g_assert_cmpint(gf->g->edges[0]->weight, ==, 4);
}

/** \brief Check that adding the same edge twice only changes the graph once
 * (i.e. no double edge added). */
void test_add_existing_edge(struct gfixture *gf, gconstpointer ignored){
        graph_add_edge(gf->g, 2, 3, 4);
        graph_add_edge(gf->g, 2, 3, 4);
        g_assert_cmpint(gf->g->m, ==, 1);
        g_assert_cmpint(gf->g->edges[0]->v1, ==, 2);
        g_assert_cmpint(gf->g->edges[0]->v2, ==, 3);
        g_assert_cmpint(gf->g->edges[0]->weight, ==, 4);
}

/** \brief Test trying to connect a vertex v1 that does not exist in
 * \ref graph_add_edge fails. */
void test_add_invalid_v1_vertex_edge(struct gfixture *gf, gconstpointer ignored){
         if (g_test_subprocess()){
                 graph_add_edge(gf->g, 6, 1, 1);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

/** \brief Test trying to connect a vertex v2 that does not exist in
 * \ref graph_add_edge fails. */
void test_add_invalid_v2_vertex_edge(struct gfixture *gf, gconstpointer ignored){
         if (g_test_subprocess()){
                 graph_add_edge(gf->g, 1, 6, 1);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

/** \brief Setup the gfixture with 5 vertices and an edge connecting 2
 *  vertices.
 *
 *  This had to be done after testing \ref graph_add_edge.*/
void graph_setup_w_5_vert_1_edge(struct gfixture *gf, gconstpointer ignored){
        gf->g = graph_new();
        graph_add_n_vertices(gf->g, 5);
        graph_add_edge(gf->g, 2, 3, 1);
}

/** \brief Test that removing the valid edge with \ref graph_rm_edge removes the
 * edge. */
void test_remove_valid_edge(struct gfixture *gf, gconstpointer ignored){
        graph_rm_edge(gf->g, 2, 3);
        g_assert_cmpint(gf->g->m, ==, 0);
        g_assert_false(vertex_find_connecting_edge(gf->g->vertices[2], 3));
}

/** \brief Test that removing a non existent edge does not change the graph. */
void test_remove_invalid_edge(struct gfixture *gf, gconstpointer ignored){
        graph_rm_edge(gf->g, 0, 1);

        g_assert_cmpint(gf->g->m, ==, 1);

        g_assert_cmpint(gf->g->edges[0]->v1, ==, 2);
        g_assert_cmpint(gf->g->edges[0]->v2, ==, 3);
        g_assert_cmpint(gf->g->edges[0]->weight, ==, 1);
}

/** \brief Assert that invalid v1 to \ref graph_rm_edge fails. */
void test_rm_invalid_v1_vertex_edge(struct gfixture *gf, gconstpointer ignored){
        /* try removing an edge where v1 is larger than the amount of vertices */
         if (g_test_subprocess()){
                 graph_rm_edge(gf->g, 6, 1);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

/** \brief Assert that invalid v2 to \ref graph_rm_edge fails. */
void test_rm_invalid_v2_vertex_edge(struct gfixture *gf, gconstpointer ignored){
        /* try removing an edge where v2 is larger than the amount of vertices */
         if (g_test_subprocess()){
                 graph_rm_edge(gf->g, 1, 6);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

/** \brief Construct a torus and explicitly check the existence of all the
 * edges.
 *
 *
 * Explicitly check for all the edges to be present, the torus should
 * look like this
 *   |   |   |
 * - 0 - 1 - 2 -
 *   |   |   |
 * - 3 - 4 - 5 -
 *   |   |   |
 * - 6 - 7 - 8 -
 *   |   |   |
 * with boundaries being connected */
void test_graph_construct_torus(struct gfixture *gf, gconstpointer ignored){
        /* since graph_construct_torus mallocs a new graph free the old one */
        free(gf->g);

        /* construct a 3x3 (i.e. 3 vertices long in 2 dimensions) torus */
        gf->g = graph_construct_torus(3, 2, 1);
        g_assert_cmpint(gf->g->n, ==, 9);
        g_assert_cmpint(gf->g->m, ==, 18);
        
        /* first make sure every vertex has the same dimension 4 */
        for (int i=0; i<gf->g->n; i++){
                g_assert_cmpint(gf->g->vertices[i]->dim, ==, 4);
        }

        /* order is always, top right bottom left */
        /* vertex 1 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[0], 6));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[0], 1));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[0], 3));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[0], 2));

        /* vertex 2 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[1], 7));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[1], 2));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[1], 4));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[1], 0));

        /* vertex 3 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[2], 8));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[2], 0));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[2], 5));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[2], 1));

        /* vertex 4 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[3], 0));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[3], 4));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[3], 6));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[3], 5));

        /* vertex 5 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[4], 1));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[4], 5));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[4], 7));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[4], 3));

        /* vertex 6 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[5], 2));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[5], 3));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[5], 8));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[5], 4));

        /* vertex 7 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[6], 3));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[6], 7));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[6], 0));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[6], 8));

        /* vertex 8 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[7], 4));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[7], 8));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[7], 1));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[7], 6));

        /* vertex 9 */
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[8], 5));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[8], 6));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[8], 2));
        g_assert_true(vertex_find_connecting_edge(gf->g->vertices[8], 7));
}

/** \brief Add all the tests to the test runner. */
int main(int argc, char **argv){
        g_test_init(&argc, &argv, NULL);
        /* Tests for graph_add_n_vertices */
        g_test_add("/add_n_vertices/add n vertices", struct gfixture, NULL,
                   graph_setup, test_add_n_vertices, graph_teardown);

        /* Tests for  graph_add_edge */
        g_test_add("/graph_add_edge/add new edge", struct gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_new_edge, graph_teardown);
        g_test_add("/graph_add_edge/add existing edge", struct gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_existing_edge, graph_teardown);
        g_test_add("/graph_add_edge/test invalid v1 vertex adding", struct gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_invalid_v1_vertex_edge, graph_teardown);
        g_test_add("/graph_add_edge/test invalid v2 vertex adding", struct gfixture, NULL,
                   graph_setup_w_5_vertices, test_add_invalid_v2_vertex_edge, graph_teardown);

        /* Tests for graph_rm_edge */
        g_test_add("/graph_rm_edge/remove existing edge", struct gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_remove_valid_edge, graph_teardown);
        g_test_add("/graph_rm_edge/remove non-existing edge", struct gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_remove_invalid_edge, graph_teardown);
        g_test_add("/graph_rm_edge/remove invalid v1 edge", struct gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_rm_invalid_v1_vertex_edge,
                   graph_teardown);
        g_test_add("/graph_rm_edge/remove invalid v2 edge", struct gfixture, NULL,
                   graph_setup_w_5_vert_1_edge, test_rm_invalid_v2_vertex_edge,
                   graph_teardown);

        /* Test for torus construction */
        g_test_add("/graph_construct_torus/construct 3x3 torus", struct gfixture, NULL,
                   graph_setup, test_graph_construct_torus, graph_teardown);

        return g_test_run();
}
