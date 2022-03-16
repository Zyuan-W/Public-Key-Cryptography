#include "randstate.h"
#include <stdio.h>

gmp_randstate_t state;
// initialize the global random state
void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}

// clear and free memory
void randstate_clear(void) {
    gmp_randclear(state);
}
