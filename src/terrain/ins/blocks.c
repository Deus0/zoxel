/*entity spawn_block_wood(ecs *world, entity parent, lint seed, char* name, color block_color) {
    entity model;
    {
        model = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
        zox_set_unique_name(model, "block_wood");
        zox_set(model, VoxType, { vox_type_wood });
        zox_set(model, VoxUniqueColors, { 8 });
        zox_set(model, VoxColorRange, { 0.34f });
    }
    return spawn_realm_block_solid(world, prefab_block_vox_meta, parent, seed, name, block_color, model);
}

entity spawn_block_stone(ecs *world, entity parent, lint seed, char* name, color block_color) {
    entity model;
    {
        model = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
        zox_set_unique_name(model, "block_stone");
        zox_set(model, VoxType, { vox_type_soil });
        zox_set(model, VRegions, { 80 });
        zox_set(model, VoxColorRange, { 0.15f });
    }
    return spawn_realm_block_solid(world, prefab_block_vox_meta, parent, seed, name, block_color, model);
}

entity spawn_block_bricks(ecs *world, entity parent, char* name, color block_color, lint seed) {
    entity model;
    {
        model = spawn_vox_generated_invisible(world, prefab_vox_generated, block_color);
        zox_set_unique_name(model, "block_bricks");
        zox_set(model, VoxType, { vox_type_bricks });
    }
    return spawn_realm_block_solid(world, prefab_block_vox_meta, parent, seed, name, block_color, model);
}*/
