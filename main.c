#include <stdio.h>
#include "weightedgraph.h"
#include <sys/wait.h>

int main(){
        printf("Start building torus.\n");
        graph *torus = graph_construct_torus(10, 2, 1);
        printf("We have %i vertices.\n", torus->n);
        graph_free(torus);
}

