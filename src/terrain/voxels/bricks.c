entity spawn_block_bricks(ecs *world, byte index, char* name, color bcolor) {
    entity vox = spawn_vox_generated_invisible(world, prefab_vox_generated, bcolor);
    zox_set_unique_name(vox, "block_bricks");
    zox_set(vox, VoxType, { vox_type_bricks });
    // use instanced mesh prefab
    SpawnBlock data = {
        .index = index,
        .seed = generate_voxel_seed(index),
        .prefab = prefab_block_vox_meta,
        .prefab_texture = prefab_vox_texture,
        .name = name,
        .color = bcolor,
        .model = vox,
        .bake_vox = 1,
    };
    return spawn_block_vox_meta(world, data);
}
