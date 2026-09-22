// todo: pass player entity through here
entity zox_tst_item_block;

void zox_tst_spawn_item_block(
    ecs *world,
    ClickEventData data)
{
    byte dbg_inspector = 1;
    if (zox_valid(zox_tst_item_block)) {
        zox_log("Cleaning Test [zox_tst_item_block]");
        zox_delete(zox_tst_item_block);
        zox_tst_item_block = 0;
        return;
    }
    entity player = dbg_player;
    entity character = zox_get_link(world, player, CharacterLink);
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(character)) {
        zox_logw("No Player Character.");
        return;
    }
    entity terrain = zox_get_link(world, character, TerrainLink);
    if (!zox_valid(terrain)) {
        return;
    }
    entity realm = zox_get_link(world, terrain, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    entity block = blocks->value[rand_range(0, blocks->length)];
    // NOTE: Non instanced is broken atm?
    float3 position = zox_getv(camera, Position3D);
    // float4 srotation = zox_getv(camera, Rotation3D);
    entity2 e2 = spawn_pickup_block(
        world,
        block,
        position,
        item_pickup_scale);
    zox_tst_item_block = e2.x;
    inspect_entity(world, e2.x);
}
