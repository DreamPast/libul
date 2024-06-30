#include "ulrand.h"
#include <stdio.h>
#include <time.h>

#define T 10000
#define N 10

int main(void) {
    ulrand32_t rng;
    int times[N] = { 0 };
    long sum = 0;
    ulrand32_init(&rng, (uint64_t)(time(NULL)));

    printf("Test uniform distribution :\n");
    for(int i = 0; i < T; ++i) {
        ++times[ulrand32_boundage(&rng, N)];
    }
    for(int i = 0; i < N; ++i) sum += times[i];
    for(int i = 0; i < N; ++i) {
        printf("\t%d: %d\t%lf\n", i, times[i], (double)times[i] / sum);
    }

    do {
        ulrand32_t rng2 = rng;
        for(int i = 0; i < T; ++i) ulrand32_step(&rng);
        ulrand32_advance(&rng2, T);
        printf("Test `step` and `advance`: %lu <=> %lu\n",
            (unsigned long)ulrand32_gen(&rng), (unsigned long)ulrand32_gen(&rng2));
    } while(0);

    return 0;
}
