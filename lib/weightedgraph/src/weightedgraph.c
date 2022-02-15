#define _GNU_SOURCE 1

#include <glib.h>
#include <gvc.h> //graphviz
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

void graph_rm_edge(graph *g, int v1, int v2){
        /* check that the vertices are possible for the graph */
        g_assert(v1 < g->n);
        g_assert(v2 < g->n);

        /**
         * Since we need the correct memory location we cannot just create a
         * new edge instance, but need to loop through the edges and find the
         * actual edge.
         **/
        edge *connecting_edge = vertex_find_connecting_edge(g->vertices[v1], v2); 
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
        g_assert(n>2); /* if n=2 then you get connections like - 0 - 1 - which
                         is a double edge so no periodic boundary conditions are possible
                       */
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

/* This macro ensures that the string that was extended is freed and returns an
 * extended string. The inputs are the previous string as previous_str and
 * variable amounts of printf-like arguments
 * NOTE: main_string_containing_percents has to contain %s (basically where to
 * insert the original string, for usual concatenation put at beginning, has to
 * be the first % argument in any case).
 * NOTE: Use ##__VAR_ARGS__ to not include the comma if there are no
 * __VA_ARGS__ given, see gcc docs on variadic macros.*/
#define ConcatStr(previous_str, main_string_containing_percents, ...){                                  \
        char *tmp=previous_str;                                                                         \
        if (asprintf(&previous_str, main_string_containing_percents, previous_str, ##__VA_ARGS__)<0){   \
                perror("Could not concatenate strings, run again with gdb or report bug.");             \
                abort();                                                                                \
        }                                                                                               \
        free(tmp);                                                                                      \
}

/* get a png file as string stream */
void draw_torus2png(graph *draw_torus, int n, int d, unsigned int duration,
					FILE *out_stream, int max_width, int max_height, int max_dpi,
					int penwidth, double passed_time){
        /* only d==2 printing case has been handled */
        g_assert(d==2);

        /* if no output file has been specified, take stdout */
        if (out_stream == NULL){
                out_stream = stdout;
        }
        /* calculate the max weight first */
        double max_weight=0;
        for (int i=0; i<draw_torus->m; i++){
                max_weight = fmax(max_weight, draw_torus->edges[i]->weight);
        }
        char *graph_gv_str; // need to initialize for ConcatStr not to segfault

        if (asprintf(&graph_gv_str, "graph {\n") < 0){
                perror("Could not allocate string 'graph {\n' to graph_gv_str, check with gdb or report bug.");
                abort();
        }
        ConcatStr(graph_gv_str, "%ssize=\"%i,%i\";\ndpi=%i;\n",
                  max_width, max_height, max_dpi);
        ConcatStr(graph_gv_str, "%snode [shape=point, style=dot, width=.1, height=.1];\n");
        ConcatStr(graph_gv_str, "%srankdir=LR;\n");
        /* find all the horizontal vertices that are invisible and serve as
         * docking points for edges to 'loop' around to the other end (i.e.
         * open edges) */
        for (int i=0; i<n; i++){
                ConcatStr(graph_gv_str, "%sH%i [style=invis];\n", n*i); /* analogously for horizontal connections */
                ConcatStr(graph_gv_str, "%sH%i [style=invis];\n", n*i+(n-1));
        }

        for (int i=0; i<n; i++){
                ConcatStr(graph_gv_str, "%sV%i [style=invis];\n", i); /* first line needs vertical connections to the top */
                ConcatStr(graph_gv_str, "%sV%i [style=invis];\n", i+int_pow(n, d)-n); /* and last line */
        }

        /* Do the horizontal connections */ 
        for (int i=0; i < n; i++){

                edge *looping_edge = vertex_find_connecting_edge(draw_torus->vertices[n*i], n*i+n-1);
                double looping_weight_ratio=penwidth*looping_edge->weight/passed_time;
                ConcatStr(graph_gv_str, "%sH%i -- %i[penwidth=%f];\n", n*i, n*i, looping_weight_ratio);

                for (int j=1; j < n; j++){
                        int cur_vertex_index = n*i+j;

                        edge *connecting_edge = vertex_find_connecting_edge(draw_torus->vertices[cur_vertex_index-1],
                                                                           cur_vertex_index);
                        double weight_ratio = penwidth*connecting_edge->weight/passed_time;

                        ConcatStr(graph_gv_str, "%s%i -- %i[penwidth=%f];\n", cur_vertex_index-1, cur_vertex_index, weight_ratio);
                }
                ConcatStr(graph_gv_str, "%s%i -- H%i[penwidth=%f];\n", n*i+n-1, n*i+n-1, looping_weight_ratio);
        }

        /* Now do the vertical connections and define their ranks as same */ 
        for (int i=0; i < n; i++){
                char *same_rank;
                if (asprintf(&same_rank, "V%i, %i", i, i)<0){
                        perror("Could not allocate with asprintf to initialize string same_rank, check with gdb or report bug.");
                        abort();
                }
                
                
                edge *looping_edge = vertex_find_connecting_edge(draw_torus->vertices[i], i+n*(n-1));
                double looping_weight_ratio=penwidth*looping_edge->weight/passed_time;
                ConcatStr(graph_gv_str, "%sV%i -- %i[penwidth=%f];\n", i, i, looping_weight_ratio);

                for (int j=1; j < n; j++){
                        int prev_vertex_index=i+n*(j-1);
                        int cur_vertex_index=i+n*j;

                        edge *connecting_edge = vertex_find_connecting_edge(draw_torus->vertices[prev_vertex_index],
                                                                           cur_vertex_index);

                        double weight_ratio = penwidth*connecting_edge->weight/passed_time;

                        ConcatStr(graph_gv_str, "%s%i -- %i[penwidth=%f];\n", prev_vertex_index, cur_vertex_index, weight_ratio);
                        ConcatStr(same_rank, "%s, %i", cur_vertex_index);
                }
                ConcatStr(graph_gv_str, "%s%i -- V%i[penwidth=%f];\n", i+n*(n-1), i+n*(n-1), looping_weight_ratio);
                ConcatStr(same_rank, "%s, %i, V%i", i+n*(n-1), i+n*(n-1));
                ConcatStr(graph_gv_str, "%s{ rank=same; %s};\n", same_rank);
                free(same_rank);
        }
        ConcatStr(graph_gv_str, "%s}");
        
        /** BEGIN GRAPHVIZ CONVERSION **/
        Agraph_t *g = agmemread(graph_gv_str); /* read the graph from memory */
        GVC_t *gvc = gvContext();

        gvLayout(gvc, g, "dot"); /* layout the gv file using dot */

        for (int i=0; i<duration; i++){
                gvRender(gvc, g, "png", out_stream); /* and convert it to png */
        }
        free(graph_gv_str);
        gvFreeLayout(gvc, g);
        agclose(g);
        gvFreeContext(gvc);
}
