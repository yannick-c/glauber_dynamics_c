#include <stdlib.h> // malloc
#include "edge.h"

edge *edge_new(int v1, int v2, int weight){
        edge *out = malloc(sizeof(edge));
        *out = (edge){.v1=v1, .v2=v2, .weight=weight};
        return out;
}

void edge_free(edge *e){ free(e); }
