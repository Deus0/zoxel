// spawn random pickup
entity dbg_item_pickup;

entity zox_tst_spawn_item_pickup(
    ecs* world,
    entity player)
{
    byte filter_voxes = 1;
    float rotate_speed = 16;
    float distance = frand_range(0.6f, 0.9f);
    if (zox_valid(dbg_item_pickup)) {
        zox_log("Deleting [dbg_item_pickup]");
        zox_delete(dbg_item_pickup);
        dbg_item_mesh = 0;
        return 0;
    }
    if (!zox_valid(player)) {
        zox_loge("[dbg_item_pickup] Invalid [player]");
        return 0;
    }
    entity game = zox_get_parent(world, player);
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(game)) {
        zox_loge("[dbg_item_pickup] Invalid [game]");
        return 0;
    }
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[dbg_item_pickup] Invalid [realm]");
        return 0;
    }
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(
        position,
        rotation,
        -distance);
    uint count = 0;
    entity items[256];
    // for children of ItemVox
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity child = it2.entities[j];
            if (!zox_has(child, Item)) {
                continue;
            }
            if (filter_voxes) {
                if (!zox_has(child, ItemVox)) {
                    continue;
                }
            }
            items[count++] = child;
        }
    }
    if (!count) {
        zox_loge("No models found in blocks.");
        return 0;
    }
    entity item = items[rand_range(0, count)];
    if (!item) {
        zox_loge("[zox_dbg_spawn_item_mesh] Invalid Item Model");
        return 0;
    }
    zox_log("Spawning World Item [%s]",
        zox_getn(item));
    entity e = spawn_item_world_from_meta(
        world,
        realm,
        item,
        spawn_position,
        item_pickup_scale);
    zox_setv(e, Quantity, 1);
    dbg_item_pickup = e;
    return e;
}

void zox_tst_spawn_item_pickup2(ecs* world, ClickEventData data) {
    zox_tst_spawn_item_pickup(world, data.clicker);
}
