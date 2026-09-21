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
    byte target_direction = direction_down;
    entity pickup_texture = 0;
    if (zox_valid(block) && !zox_disable_textured_items) {
        iter it2 = zox_children(world, block);
        while (zox_children_next(it2)) {
            for (int k = 0; k < it2.count; k++) {
                entity texture = it2.entities[k];
                if (!zox_has(texture, Texture)) {
                    continue;
                }
                // Vox item?
                if (!zox_has(texture, ChildIndex)) {
                    pickup_texture = texture;
                    continue;
                }
                if (zox_getv(texture, ChildIndex) == target_direction) {
                    pickup_texture = texture;
                }
            }
            if (!zox_valid(pickup_texture) &&
                zox_has(block, TextureLink))
            {
                pickup_texture = zox_get_link(world,block, TextureLink);
            }
        }
    }
    return spawn_pickup_cube_texture(
        world,
        position,
        scale,
        pickup_texture);
}
