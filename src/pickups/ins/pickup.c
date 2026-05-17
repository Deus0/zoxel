entity spawn_pickup_basic(ecs *world, float3 position) {
    entity e = spawn_cube(world, prefab_pickup_basic, position, 0.125f);
    zox_name("pickup_basic");
    return e;
}

entity spawn_pickup_block(ecs *world, float3 position, entity block) {
#ifdef zox_prefabs_non_textured
    entity e = spawn_cube(world, prefab_pickup_basic, position, 0.125f);
#else
    entity texture = 0;
    if (zox_valid(block)) {
        if (zox_has(block, TextureLinks)) {
            zox_geter(block, TextureLinks, textures)
            if (textures->length) {
                if (textures->length == 1) {
                    texture = textures->value[0];
                } else {
                    texture = textures->value[block_side_down];
                }
            }
        }
        if (!texture && zox_has(block, TextureLink)) {
            texture = zox_get_value(block, TextureLink);
        }
    }
    entity e;
    if (zox_valid(texture)) {
        e = spawn_cube_textured(world, prefab_pickup, position, texture, 0.125f);
    } else {
        e = spawn_cube(world, prefab_pickup_basic, position, 0.125f);
    }
#endif
    zox_name("pickup");
    return e;
}
