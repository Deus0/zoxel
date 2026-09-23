#include "clone_scale.c"
#include "clone.c"
#include "clear.c"
#include "outline.c"
#include "swap.c"


void delayed_texture_generate(ecs* world, entity e) {
    if (zox_valid(e)) {
        zox_setv(e, GenerateTexture, zox_generate_texture_run);
    }
}
