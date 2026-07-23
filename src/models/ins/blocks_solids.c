entity spawn_model_soil(ecs* world, const char* name, entity parent, lint seed, color block_color, float noise, byte voxtype) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
    {
        char name2[64];
        sprintf(name2, "model_%s", name);
        zox_name(name2);
    }
    zox_set_parent(world, e, parent);
    zox_add_tag(e, BakeModel);
    zox_set(e, Seed, { seed });
    zox_set(e, VoxType, { voxtype });
    zox_set(e, VoxUniqueColors, { 8 });
    zox_set(e, VoxColorRange, { 0.34f });
    zox_set(e, VRegions, { 0 });    // TODO: Remove property
    return e;
}

entity spawn_model_soil_grass(ecs* world, entity parent, lint seed, color soil_color, color grass_color, float noise) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, grass_color);
    zox_set_unique_name(e, "model_soil_grass");
    zox_set_parent(world, e, parent);
    zox_add_tag(e, BakeModel);
    zox_set(e, VoxType, { vox_type_blended });
    zox_set(e, SecondaryColor, { soil_color });
    zox_set(e, VoxUniqueColors, { 6 });
    zox_set(e, VoxColorRange, { noise });
    zox_set(e, VRegions, { 0 });
    return e;
}

entity spawn_model_wood(ecs *world, entity parent, lint seed, color block_color) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
    zox_set_unique_name(e, "model_wood");
    zox_add_tag(e, BakeModel);
    zox_set(e, VoxType, { vox_type_wood });
    zox_set(e, VoxUniqueColors, { 8 });
    zox_set(e, VoxColorRange, { 0.34f });
    return e;
}

entity spawn_model_stone(ecs *world, entity parent, lint seed, color block_color) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
    zox_set_unique_name(e, "model_stone");
    zox_add_tag(e, BakeModel);
    zox_set(e, VoxType, { vox_type_soil });
    zox_set(e, VRegions, { 80 });
    zox_set(e, VoxColorRange, { 0.15f });
    return e;
}

entity spawn_model_bricks(ecs *world, entity parent, lint seed, color block_color, color cracks_color) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
    zox_set_unique_name(e, "model_bricks");
    zox_add_tag(e, BakeModel);
    zox_set(e, VoxType, { vox_type_bricks });
    zox_set(e, SecondaryColor, { cracks_color });
    return e;
}

entity spawn_model_road(ecs *world, entity parent, lint seed, color block_color) {
    entity e = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
    zox_set_unique_name(e, "model_wood");
    zox_add_tag(e, BakeModel);
    zox_set(e, VoxType, { vox_type_road });
    zox_set(e, VoxUniqueColors, { 8 });
    zox_set(e, VoxColorRange, { 0.24f });
    return e;
}
