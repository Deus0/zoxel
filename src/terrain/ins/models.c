entity spawn_model_soil(ecs* world, entity parent, lint seed, color block_color, float noise) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
    zox_set_unique_name(e, "model_soil");
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, VoxType, { vox_type_soil });
    zox_set(e, VoxUniqueColors, { 8 });
    zox_set(e, VoxColorRange, { 0.34f });
    zox_set(e, VRegions, { 0 });    // TODO: Remove property
    return e;
}

entity spawn_model_soil_grass(ecs* world, entity parent, lint seed, color soil_color, color grass_color, float noise) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, grass_color);
    zox_set_unique_name(e, "model_soil_grass");
    zox_set_parent(world, e, parent);
    zox_set(e, VoxType, { vox_type_blended });
    zox_set(e, SecondaryColor, { soil_color });
    zox_set(e, VoxUniqueColors, { 6 });
    zox_set(e, VoxColorRange, { noise });
    zox_set(e, VRegions, { 0 });
    return e;
}
