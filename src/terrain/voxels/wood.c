entity spawn_block_wood(ecs *world, byte index, char* name, color bcolor) {
    // vox
    entity v = spawn_vox_generated_invisible(
        world,
        prefab_vox_generated,
        bcolor
    );
    zox_set_unique_name(v, "block_bricks");
    zox_set(v, VoxType, { vox_type_wood });
    zox_set(v, VoxUniqueColors, { 8 });
    zox_set(v, VoxColorRange, { 0.34f });

    // use instanced mesh prefab
    SpawnBlock data = {
        .index = index,
        .seed = generate_voxel_seed(index),
        .prefab = prefab_block_vox_meta,
        .prefab_texture = prefab_vox_texture,
        .name = name,
        .color = bcolor,
        .vox = v,
        .bake_vox = 1,
    };
    return spawn_block_vox_meta(world, data);
}