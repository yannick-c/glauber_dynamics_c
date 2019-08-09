#include <stdio.h>

#include "pcg-c/include/pcg_variants.h"
#include "pcg-c/extras/entropy.h"

pcg32_random_t exponential_rng, uniform_rng;

int main(){
        uint64_t seeds1[2], seeds2[2];
        entropy_getbytes((void*)seeds1, sizeof(seeds1));
        entropy_getbytes((void*)seeds2, sizeof(seeds2));
        pcg32_srandom_r(&exponential_rng, seeds1[0], seeds1[1]);
        pcg32_srandom_r(&uniform_rng, seeds2[0], seeds2[1]);
        for (int i = 0; i<10; i++){
                printf("%i\n", pcg32_boundedrand_r(&exponential_rng, 50));
        }
        printf("New\n\n\n");
        for (int i = 0; i<10; i++){
                printf("%i\n", pcg32_boundedrand_r(&uniform_rng, 50));
        }
}
