entity spawn_pickup_basic(
    ecs *world,
    float3 position)
{
    entity e = spawn_cube(
        world,
        prefab_pickup_basic,
        position,
        item_pickup_scale); // 0.125f);
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
    zox_setv(e, Scale1, scale);
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
    zox_setv(cube, LocalScale1, 1); // scale);
    return (entity2) {
        e,
        cube
    };
}

entity get_item_block_texture(
    ecs* world,
    entity block)
{
    byte target_direction = direction_down;
    entity pickup_texture = 0;
    if (zox_valid(block) &&
        !zox_disable_textured_items)
    {
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
            if (!zox_valid(pickup_texture)) {
                pickup_texture = zox_get_link(world, block, TextureLink);
            }
        }
    }
    return pickup_texture;
}

entity2 spawn_pickup_block(
    ecs* world,
    entity block,
    float3 position,
    float scale)
{
    entity pickup_texture = get_item_block_texture(world, block);
    return spawn_pickup_cube_texture(
        world,
        position,
        scale,
        pickup_texture);
}

entity spawn_item_mesh(
    ecs* world,
    entity parent,
    entity meta,
    float scale)
{
    entity mesh;
    if (zox_has(meta, ItemVox)) {
        mesh = spawn_item_vox_mesh(world, meta);
        if (mesh) {
            scale *= 3;
        } else {
            mesh = spawn_cube(
                world,
                prefab_cube,
                float3_zero,
                1);
        }
        // zox_setv(mesh, LocalScale1, 0.5f);
        // zox_setv(parent, Scale1, 1); // 0.5f);
    } else {
        entity block = zox_get_link(world, meta, BlockLink);
        entity texture = get_item_block_texture(world, block);
        if (block && texture) {
            mesh = spawn_cube_textured(
                world,
                prefab_cube_textured,
                texture,
                float3_zero,
                1);
        } else {
            mesh = spawn_cube(
                world,
                prefab_cube,
                float3_zero,
                1);
        }
    }
    if (mesh) {
        zox_set_parent(world, mesh, parent);
        zox_setv(mesh, LocalPosition3D, float3_zero);
        zox_setv(mesh, LocalScale1, scale);
        zox_setv(mesh, Scale1, 1);
    } else {
        zox_loge("ItemVox mesh didn't spawn [%s]",
            zox_getn(meta));
    }
    return mesh;
}

entity spawn_item_world_from_meta(
    ecs* world,
    entity realm,
    entity meta,
    float3 position,
    float scale)
{
    // TODO: Spawn model clone here too
    entity pickup = zox_ins(world, prefab_pickup);
    zox_set_unique_name(pickup, "pickup");
    zox_setv(pickup, Position3D, position);
    zox_link(world, pickup, ItemLink, meta);
    zox_set_parent(world, pickup, realm);
    spawn_item_mesh(world, pickup, meta, scale);
    return pickup;
}

entity spawn_item_world(
    ecs* world,
    entity realm,
    entity item,
    float3 position,
    float scale)
{
    entity meta = zox_get_prefab(world, item);
    entity pickup = spawn_item_world_from_meta(
        world,
        realm,
        meta,
        position,
        scale);
    if (zox_has(item, Quantity)) {
        byte quantity = zox_getv(item, Quantity);
        zox_setv(pickup, Quantity, quantity);
    }
    return pickup;
}
