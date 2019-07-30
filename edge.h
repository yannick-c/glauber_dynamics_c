/* define the edge struct */
typedef struct edge {
        int v1;                /* one end of edge */
        int v2;                /* other end of edge */
        double weight;          /* weight for the edge */
} edge;

edge *edge_new(int v1, int v2, int weight);
void edge_free(edge *e);
