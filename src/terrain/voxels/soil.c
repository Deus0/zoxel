entity spawn_block_soil(ecs *world, entity parent, char* name, color block_color, lint seed) {
    entity vox;
    {
        vox = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
        zox_set_unique_name(vox, "vox_soil");
        zox_set_parent(world, vox, parent);
        zox_set(vox, VoxType, { vox_type_soil });
        zox_set(vox, VRegions, { 0 });    // we should remove this
        zox_set(vox, VoxUniqueColors, { 8 });
        zox_set(vox, VoxColorRange, { 0.34f });
    }
    // use instanced mesh prefab
    SpawnBlock data = {
        .seed = seed,
        .prefab = prefab_block_vox_meta,
        .prefab_texture = prefab_vox_texture,
        .name = name,
        .color = block_color,
        .model = vox,
        .bake_vox = 1,
    };
    entity e = spawn_block_vox_meta(world, data);
    zox_set_parent(world, e, parent);
    return e;
}

entity spawn_block_soil_grass(ecs *world, entity parent, char* name, color bottom_color, color top_color, lint seed) {
    entity v;
    {
        v = spawn_vox_generated_invisible(world, prefab_vox_generated, top_color);
        zox_set_unique_name(v, "block_soil_grass");
        zox_set_parent(world, v, parent);
        zox_set(v, VoxType, { vox_type_blended });
        zox_set(v, SecondaryColor, { bottom_color });
        zox_set(v, VRegions, { 0 });
        zox_set(v, VoxUniqueColors, { 6 });
        zox_set(v, VoxColorRange, { 0.22f });
    }
    // use instanced mesh prefab
    SpawnBlock data = {
        .seed = seed,
        .name = name,
        .model = v,
        .prefab = prefab_block_vox_meta,
        .prefab_texture = prefab_vox_texture,
        .color = top_color,
        .bake_vox = 1,
    };
    entity e = spawn_block_vox_meta(world, data);
    zox_set_parent(world, e, parent);
    return e;
}

