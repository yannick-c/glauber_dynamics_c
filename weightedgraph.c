#define _GNU_SOURCE //cause stdio.h to include asprintf

#define MAX_PENWIDTH 1
#define DECREASE_RATE 10 // exponent by which pendwidth decreases with edge weight
#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> // malloc

#include "weightedgraph.h"

/**
 * Basic undirected graph struct implementation with edge weights using
 * adjacency lists represented as  variable length arrays.
 *
 * No order is imposed on the adjacency lists!
 **/

graph *graph_new(){
        graph *g = malloc(sizeof(graph));
        *g = (graph) {.n=0, .m=0};
        return g;
}

void graph_free(graph *g){
        for(int i=0; i<g->n; i++){
                vertex *v = g->vertices[i];
                /**
                 * use the fact that vertex_free frees the memory location for all
                 * edges contained in it so remove the edge from the other
                 * vertex.
                 **/
                for (int j=0; j<v->dim; j++){
                        edge *e = v->edges[j];
                        int to_cleanse = e->v1;
                        if (e->v2 != i){
                                /* if v2 is the vertex that is not i then v1==i
                                 * and we have to remove the edge from v2 not
                                 * from v1 */
                                to_cleanse = e->v2;
                        }
                        vertex_rm_edge_from_neighbourhood(g->vertices[to_cleanse], e);
                }
                vertex_free(g->vertices[i]);
        }
        free(g->vertices);
        free(g->edges);
        free(g);
}

void graph_add_n_vertices(graph *g, int to_add){
        /* update g->n only at the end and use that it contains the old value */
        g->vertices = realloc(g->vertices, sizeof(g->vertices) + to_add*sizeof(vertex));
        for (int i=g->n; i<to_add; i++){
                g->vertices[i] = vertex_new();
        }
        g->n += to_add;
}

void graph_add_edge(graph *g, int v1, int v2, int weight){
        /* check that the vertices are possible for the graph */
        g_assert(v1 < g->n);
        g_assert(v2 < g->n);

        /* check that the connection does not exist */
        for (int i=0; i<g->vertices[v1]->dim; i++){
                if (g->vertices[v1]->edges[i]->v1 == v2 ||
                    g->vertices[v1]->edges[i]->v2 == v2){
                        return;
                }
        }

        /* update g->m only at the end and use that it contains the old value */
        g->edges = realloc(g->edges, (g->m+1) * sizeof(edge*));
        edge *new_edge = edge_new(v1, v2, weight);
        g->edges[g->m] = new_edge;

        vertex_add_edge_to_neighbourhood(g->vertices[v1], new_edge);
        vertex_add_edge_to_neighbourhood(g->vertices[v2], new_edge);
        g->m++;
}

/**
 * utility function to find edge connecting v to vertex with label dst
 *
 * Returns NULL pointer if no edge present.
 **/
edge *graph_find_connecting_edge(vertex *v, int dst){
        for (int i=0; i<v->dim; i++){
                if (v->edges[i]->v1 == dst ||
                    v->edges[i]->v2 == dst){
                        return v->edges[i];
                }
        }
        return NULL;
}

void graph_rm_edge(graph *g, int v1, int v2){
        /* check that the vertices are possible for the graph */
        g_assert(v1 < g->n);
        g_assert(v2 < g->n);

        /**
         * Since we need the correct memory location we cannot just create a
         * new edge instance, but need to loop through the edges and find the
         * actual edge.
         **/
        edge *connecting_edge = graph_find_connecting_edge(g->vertices[v1], v2); 
        if (connecting_edge){
                /* only if the connecting edge is not NULL */
                vertex_rm_edge_from_neighbourhood(g->vertices[v1], connecting_edge);
                vertex_rm_edge_from_neighbourhood(g->vertices[v2], connecting_edge);
                free(connecting_edge);
                g->m--;
        }
}

/* write a pow function for integers since the math.h pow uses exp(log(x))
 * which is not the most precise way of calculating it for integers. */
int static int_pow(int base, int exponent){
        int result = 1;
        while (exponent){
                if (exponent & 1){
                        result *= base;
                }
                exponent /= 2;
                base *= base;
        }
        return result;
}

graph *graph_construct_torus(int n, int d, int init_weight){
        graph *out = graph_new();
        int vertex_count = int_pow(n, d);
        graph_add_n_vertices(out, vertex_count);

        for (int i=0; i<vertex_count; i++){
                for (int j=0; j<d; j++){
                        /* the offset ensures that we dont modulo down to the
                         * first cases all the time but only close the
                         * boundaries */
                        int offset = i - (i % int_pow(n,j+1));
                        /* connect it to  the next vertex in the next
                         * dimensions, i.e. in 2 dimensions the lower one and
                         * the right one take care of the periodic boundary conditions. */
                        int connect_to = (i + (int_pow(n,j))) % int_pow(n,j+1) + offset; 
                        graph_add_edge(out, i, connect_to, init_weight);
                }
        }
        return out;
}

/* print a dot graph file (see graphviz) to stdout such that it can be piped
 * into the dot command the input has to be the output of graph_construct_torus
 * with the same inputs n and d.*/
void draw_torus(graph *draw_torus, int n, int d){
        /* only d==2 printing case has been handled */
        g_assert(d==2);
        /* calculate the max weight first */
        double max_weight=0;
        for (int i=0; i<draw_torus->m; i++){
                max_weight = fmax(max_weight, draw_torus->edges[i]->weight);
        }

        printf("graph {\n");
        printf("node [shape=point, style=dot, width=.1, height=.1, label=None];\n");
        printf("rankdir=LR;\n");
        /* find all the horizontal vertices that are invisible and serve as
         * docking points for edges to 'loop' around to the other end (i.e.
         * open edges) */
        for (int i=0; i<n; i++){
                printf("H%i [style=invis];\n", n*i); /* analogously for horizontal connections */
                printf("H%i [style=invis];\n", n*i+(n-1));
        }

        for (int i=0; i<n; i++){
                printf("V%i [style=invis];\n", i); /* first line needs vertical connections to the top */
                printf("V%i [style=invis];\n", i+int_pow(n, d)-n); /* and last line */
        }

        /* Do the horizontal connections */ 
        for (int i=0; i < n; i++){

                edge *looping_edge = graph_find_connecting_edge(draw_torus->vertices[n*i], n*i+n-1);
                double looping_weight_ratio=MAX_PENWIDTH*pow(looping_edge->weight/max_weight, DECREASE_RATE);
                printf("H%i -- %i[penwidth=%f];\n", n*i, n*i, looping_weight_ratio);

                for (int j=1; j < n; j++){
                        int cur_vertex_index = n*i+j;

                        edge *connecting_edge = graph_find_connecting_edge(draw_torus->vertices[cur_vertex_index-1],
                                                                           cur_vertex_index);
                        double weight_ratio = MAX_PENWIDTH*pow(connecting_edge->weight/max_weight, DECREASE_RATE);

                        printf("%i -- %i[penwidth=%f];\n", cur_vertex_index-1, cur_vertex_index, weight_ratio);
                }
                printf("%i -- H%i[penwidth=%f];\n", n*i+n-1, n*i+n-1, looping_weight_ratio);
        }

        /* Now do the vertical connections and define their ranks as same */ 
        for (int i=0; i < n; i++){
                char *same_rank;
                asprintf(&same_rank, "V%i, %i", i, i);
                
                
                edge *looping_edge = graph_find_connecting_edge(draw_torus->vertices[i], i+n*(n-1));
                double looping_weight_ratio=MAX_PENWIDTH*pow(looping_edge->weight/max_weight, DECREASE_RATE);
                printf("V%i -- %i[penwidth=%f];\n", i, i, looping_weight_ratio);

                for (int j=1; j < n; j++){
                        int prev_vertex_index=i+n*(j-1);
                        int cur_vertex_index=i+n*j;

                        edge *connecting_edge = graph_find_connecting_edge(draw_torus->vertices[prev_vertex_index],
                                                                           cur_vertex_index);
                        double weight_ratio = MAX_PENWIDTH*pow(connecting_edge->weight/max_weight, DECREASE_RATE);

                        printf("%i -- %i[penwidth=%f];\n", prev_vertex_index, cur_vertex_index, weight_ratio);
                        asprintf(&same_rank, "%s, %i", same_rank, cur_vertex_index);
                }
                printf("%i -- V%i[penwidth=%f];\n", i+n*(n-1), i+n*(n-1), looping_weight_ratio);
                asprintf(&same_rank, "%s, %i, V%i", same_rank, i+n*(n-1), i+n*(n-1));
                printf("{ rank=same; %s};\n", same_rank);
        }
        printf("}");
}
