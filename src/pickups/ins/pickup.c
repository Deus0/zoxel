entity spawn_pickup_basic(
    ecs *world,
    float3 position)
{
    entity e = spawn_cube(
        world,
        prefab_pickup_basic,
        position,
        0.125f);
    zox_name("pickup_basic");
    return e;
}

entity2 spawn_pickup_cube_texture(
    ecs* world,
    float3 position,
    float scale,
    entity texture)
{
    entity e = zox_ins(world, prefab_pickup);
    zox_name("pickup");
    zox_setv(e, Position3D, position);
    entity cube;
    if (zox_valid(texture)) {
        cube = spawn_cube_textured(
            world,
            prefab_cube_textured,
            texture,
            float3_zero,
            scale);
    } else {
        cube = spawn_cube(
            world,
            prefab_cube,
            float3_zero,
            scale);
    }
    zox_set_parent(world, cube, e);
    // tag local transform
    zox_setv(cube, LocalPosition3D, float3_zero);
    zox_setv(cube, LocalScale1, scale);
    return (entity2) {
        e,
        cube
    };
}

entity2 spawn_pickup_block(
    ecs* world,
    entity block,
    float3 position,
    float scale)
{
    entity texture = 0;
    if (zox_valid(block) && !zox_disable_textured_items) {
        if (zox_has(block, TextureLinks)) {
            zox_geter(block, TextureLinks, textures);
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
    return spawn_pickup_cube_texture(
        world,
        position,
        scale,
        texture);
}
