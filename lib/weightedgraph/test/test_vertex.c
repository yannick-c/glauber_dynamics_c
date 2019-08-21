/** \file test_vertex.c
 * \brief glib testing based test code for \ref vertex.h. */
#include <glib.h>
#include <stdlib.h>

#include "vertex.h"

/** \brief The fixture used to test \ref vertex related functions. */
struct vfixture{
        vertex *v; /**< \brief The vertex to be used for testing */
        edge *e1; /**< \brief Edge to be used for testing. */
        edge *e2; /**< \brief Edge to be used for testing. */ 
        edge *e3; /**< \brief Edge to be used for testing. */
};

/** \brief The setup of the vfixture.
 *
 * The vfixtures is setup such that vfixture.e1 is contained in the
 * neighbourhood of vfixture.v and initializes vfixture.e2 and vfixture.e3.*/
void vertex_setup(struct vfixture *vf, gconstpointer test_data){
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

/* quick for loop to avoid double freeing edges just pure utility*/
int static vertex_contains_edge(vertex *v, edge *e){
        for (int i=0; i<v->dim; i++){
                if (v->edges[i] == e){
                        return 1;
                }
        }
        return 0;
}

/** \brief Teardown of the vfixture freeing all the memory.*/
void vertex_teardown(struct vfixture *vf, gconstpointer test_data){
        if(!vertex_contains_edge(vf->v, vf->e1)){
                edge_free(vf->e1);
        }
        if(!vertex_contains_edge(vf->v, vf->e2)){
                edge_free(vf->e2);
        }
        vertex_free(vf->v);
}

/** \brief Add e2 to v and check that all the values are correctly adapted. */
void test_add_good_edge(struct vfixture *vf, gconstpointer ignored){
        vertex_add_edge_to_neighbourhood(vf->v, vf->e2);
        g_assert_cmpint(vf->v->dim, ==, 2);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight+vf->e2->weight);
        g_assert_true(vf->v->edges[0] == vf->e1); /* compare pointers */
        g_assert_true(vf->v->edges[1] == vf->e2); /* compare pointers */
}

/** \brief Add e1 again and check that this does not change the dimension.
 *
 * Note that this is not supposed to give errors. */
void test_add_double_edge(struct vfixture *vf, gconstpointer ignored){
        vertex_add_edge_to_neighbourhood(vf->v, vf->e1);
        g_assert_cmpint(vf->v->dim, ==, 1);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight);
        g_assert_true(vf->v->edges[0] == vf->e1); /* compare pointers */
        g_assert_false(vf->v->edges[1] == vf->e1); /* this should be invalid memory so
                                                      most likely not equal to e1 */
}

/** \brief Check that adding self-edges gives errors.
 *
 * Edit e2 in a way that it becomes a self-edge and check that trying
 * to add that gives an assertion error since this should never be
 * allowed. */
void test_add_self_edge(struct vfixture *vf, gconstpointer ignored){
        /* read the manual for subprocess and this particular recipe */
         if (g_test_subprocess()){
             vf->e2->v2 = vf->e2->v1;
             vertex_add_edge_to_neighbourhood(vf->v, vf->e2);
         }
         g_test_trap_subprocess(NULL, 0, 0);
         g_test_trap_assert_failed();
}

/** \brief Remove e1 and check that the vertex is empty after that. */
void test_remove_edge(struct vfixture *vf, gconstpointer ignored){
        vertex_rm_edge_from_neighbourhood(vf->v, vf->e1);
        g_assert_cmpint(vf->v->dim, ==, 0);
        g_assert_cmpint(vf->v->local_weight, ==, 0);
        g_assert_false(vf->v->edges[0] == vf->e1); /* vf->v->edges[0] should be invalid */
}

/** \brief The large case needed to executed the insides of the for-loop in
 * rm_i_th_edge. (see \ref vertex_rm_edge_from_neighbourhood) */
void test_remove_edge_large(struct vfixture *vf, gconstpointer ignored){
        /* add_edge has been tested above, so can safely be used */
        vertex_add_edge_to_neighbourhood(vf->v, vf->e2);
        vertex_add_edge_to_neighbourhood(vf->v, vf->e3);
        vertex_rm_edge_from_neighbourhood(vf->v, vf->e2);
        g_assert_cmpint(vf->v->dim, ==, 2);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight + vf->e3->weight);
        g_assert_true(vf->v->edges[1] == vf->e3); /* vf->v->edges[1] should be invalid */
}

/**\brief Remove e2 and check that nothing changes, since e2 is not contained in
 * the neighbourhood*/
void test_remove_nonexistent_edge(struct vfixture *vf, gconstpointer ignored){
        vertex_rm_edge_from_neighbourhood(vf->v, vf->e2);
        g_assert_cmpint(vf->v->dim, ==, 1);
        g_assert_cmpint(vf->v->local_weight, ==, vf->e1->weight);
        g_assert_true(vf->v->edges[0] == vf->e1);
        g_assert_false(vf->v->edges[1] == vf->e1); /* vf->v->edges[1] should be invalid */
        g_assert_false(vf->v->edges[1] == vf->e2); /* vf->v->edges[1] should be invalid */
}

/** \brief Assert that finding an existing edge returns the correct output. */
void test_finding_existing_edge(struct vfixture *vf, gconstpointer ignored){
        g_assert_true(vertex_find_connecting_edge(vf->v, 1) == vf->e1);
}

/** \brief Assert that finding a non-existing edge returns NULL.*/
void test_finding_non_existing_edge(struct vfixture *vf, gconstpointer ignored){
        g_assert_false(vertex_find_connecting_edge(vf->v, 2));
}

/** \brief Add all the tests to the test runner. */
int main(int argc, char **argv){
        g_test_init(&argc, &argv, NULL);
        /* Tests for vertex_add_edge_to_neighbourhood */
        g_test_add("/set1/good edge", struct vfixture, NULL,
                   vertex_setup, test_add_good_edge, vertex_teardown);
        g_test_add("/set1/double edge", struct vfixture, NULL,
                   vertex_setup, test_add_double_edge, vertex_teardown);
        g_test_add("/set1/self edge", struct vfixture, NULL,
                   vertex_setup, test_add_self_edge, vertex_teardown);

        /* Tests for vertex_rm_edge_from_neighbourhood */
        g_test_add("/set2/remove existing edge", struct vfixture, NULL,
                   vertex_setup, test_remove_edge, vertex_teardown);
        g_test_add("/set2/remove existing edge large set", struct vfixture, NULL,
                   vertex_setup, test_remove_edge_large, vertex_teardown);
        g_test_add("/set2/remove non-existing edge", struct vfixture, NULL,
                   vertex_setup, test_remove_nonexistent_edge, vertex_teardown);

        /* Tests for vertex_find_connecting_edge */
        g_test_add("/vertex_find_connecting_edge/find existing", struct vfixture, NULL,
                   vertex_setup, test_finding_existing_edge, vertex_teardown);
        g_test_add("/vertex_find_connecting_edge/find non-existing edge", struct vfixture, NULL,
                   vertex_setup, test_finding_non_existing_edge,
                   vertex_teardown);
        return g_test_run();
}
