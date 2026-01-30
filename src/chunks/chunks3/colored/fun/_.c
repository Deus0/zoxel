// #define DISABLE_AO
#define AO_MULTIPLIER 1.4f // 2

#define ao_no_neighbors 1
#define ao_small_darkness 0.8f
#define ao_edge_darkness 0.6f
#define ao_corner_darkness 0.25f

#include "quad.c"
#include "unlit.c"
#include "ao6.c"
#include "ao27.c"