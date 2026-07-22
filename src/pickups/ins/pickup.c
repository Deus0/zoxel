entity spawn_pickup_basic(ecs *world, float3 position) {
    entity e = spawn_cube(world, prefab_pickup_basic, position, 0.125f);
    zox_name("pickup_basic");
    return e;
}

entity spawn_pickup_block(ecs *world, float3 position, entity block, float scale) {
    if (zox_disable_textured_items) {
        entity e = spawn_cube(world, prefab_pickup_basic, position, scale);
        zox_name("pickup");
        return e;
    }
    entity texture = 0;
    if (zox_valid(block)) {
        if (zox_has(block, TextureLinks)) {
            zox_geter(block, TextureLinks, textures)
            if (textures->length) {
                if (textures->length == 1) {
                    texture = textures->value[0];
                } else {
                    texture = textures->value[direction_down];
                }
            }
        }
        if (!zox_valid(texture) && zox_has(block, TextureLink)) {
            texture = zox_getv(block, TextureLink);
        }
    }
    entity e;
    if (zox_valid(texture)) {
        e = spawn_cube_textured(world, prefab_pickup, texture, position, scale);
    } else {
        e = spawn_cube(world, prefab_pickup_basic, position, scale);
    }
    zox_name("pickup");
    return e;
}
