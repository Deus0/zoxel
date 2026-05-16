#include "clone_scale.c"
#include "clone.c"
#include "clear.c"
// #include "swap.c"

void swap_textures(ecs *world, entity e1, entity e2) {
    zox_muter(e1, TextureData, texture_a);
    zox_muter(e1, TextureSize, texture_size_a);
    zox_muter(e2, TextureData, texture_b);
    zox_muter(e2, TextureSize, texture_size_b);
    const TextureData temp_data = (TextureData) { texture_a->length, texture_a->value };
    texture_a->value = texture_b->value;
    texture_a->length = texture_b->length;
    texture_b->value = temp_data.value;
    texture_b->length = temp_data.length;
    const int2 temp_size = texture_size_a->value;
    texture_size_a->value = texture_size_b->value;
    texture_size_b->value = temp_size;
    zox_set(e1, TextureDirty, { 1 });
    zox_set(e2, TextureDirty, { 1 });
}
