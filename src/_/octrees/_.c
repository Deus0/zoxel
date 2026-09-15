#include "data.c"
#include "base.c"
#include "root.c"
#include "tree.c"
#include "components.c"
#include "linked.c"
#include "macros.c"

static inline byte3 get_scaled_size(
    byte depth,
    float3 ratio)
{
    return byte3_scale3f(
        byte3_single(octree_size(depth)),
        ratio);
}
