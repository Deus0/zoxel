byte grass_variants = 8;
byte max_model_lods = 5;
const byte default_unique_colors = 6;
const float default_color_range = 0.14f;

void set_block_depth(ecs* world, void* value) {
    (void) world;
    int new_value = *(int*) value;
    block_vox_depth = new_value;

    // zox_log("Set Block Depth [%i]", new_value);

    if (!prefab_texture) return;

    int2 size = int2_single(powers_of_two[block_vox_depth]);
    zox_set(prefab_texture, TextureSize, { size });
    zox_set( prefab_vox_texture, TextureSize, { size });

    zox_set(prefab_vox, NodeDepth, { block_vox_depth });
    zox_set(prefab_vox_generated, NodeDepth, { block_vox_depth });
    zox_set(prefab_block_vox, NodeDepth, { block_vox_depth });

}

void initialize_settings_models(ecs *world) {
    zoxs_new_int_lim("block depth", set_block_depth, block_vox_depth, 2, 5);
}
