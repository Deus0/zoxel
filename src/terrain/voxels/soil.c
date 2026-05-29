entity spawn_block_soil(ecs *world, byte index, char* name, color block_color) {
    // vox
    entity vox = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
    zox_set_unique_name(vox, "vox_soil");
    zox_set(vox, VoxType, { vox_type_soil });
    zox_set(vox, VRegions, { 0 });    // we should remove this
    zox_set(vox, VoxUniqueColors, { 8 });
    zox_set(vox, VoxColorRange, { 0.34f });
    // use instanced mesh prefab
    SpawnBlock data = {
        .index = index,
        .seed = generate_voxel_seed(index),
        .prefab = prefab_block_vox_meta,
        .prefab_texture = prefab_vox_texture,
        .name = name,
        .color = block_color,
        .vox = vox,
        .bake_vox = 1,
    };
    return spawn_block_vox_meta(world, data);
}
