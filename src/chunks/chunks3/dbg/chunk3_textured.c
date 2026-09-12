entity dbg_chunk3_textured;

entity spawn_dbg_chunk3_textured(
    ecs* world,
    entity player)
{
    entity realm = local_realm;
    if (!zox_valid(realm)) {
        // zox_log("Realm invalid, cannot spawn chunk terrain");
        // return 0;
    }
    // byte dbg_inspector = 0;
    lint seed = 666;
    entity camera = zox_get_link(world, player, CameraLink);
    if (!zox_valid(camera)) {
        return 0;
    }
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float distance = frand_range(2.6f, 3.4f);
    float3 spawn_position = move_along_direction(
        position,
        rotation,
        -distance);
    // float scale = frand_range(scales.x, scales.y);
    float block_scale = 1.0f / 16.0f;
    byte depth = block_depth_limits.y;
    ushort length = octree_size(depth);
    int3 size = int3_single(length);
    float spin = rand_range(2, 8);
    float3 euler = (float3) {
        rand() % 100 > 50 ? spin : -spin,
        rand() % 100 > 50 ? spin : -spin,
        rand() % 100 > 50 ? spin : -spin
    };
    // spawn chunk3s
    /*entity spawn_chunk3_textured(
        ecs* world,
        entity prefab,
        entity realm,
        entity terrain,
        entity tilemap,
        lint terrain_seed,
        int3 position,
        byte terrain_depth,
        float terrain_scalev,
        byte render_distance,
        byte render_depth)*/
    entity tilemap = zox_get_link(world, realm, Tilemap);
    entity e = spawn_chunk3_textured(
        world,
        prefab_chunk_textured,
        realm,
        0,
        tilemap,
        seed,
        int3_zero,
        depth,
        block_scale,
        0,
        depth
    );
    zox_setv(e, NodeDepth, depth);
    zox_add(e, NoiseChunk);
    zox_setv(e, GenerateModel, zox_generate_model_run);
    zox_add(e, ColorRGBs);  // temp, for system
    zox_setv(e, Position3D, spawn_position);
    zox_setv(e, Scale1, 1);
    add_eternal_euler(world, e, euler);
    return e;
}

void zox_dbg_spawn_chunk3_textured(
    ecs* world,
    ClickEventData data)
{
    byte dbg_inspector = 1;
    entity player = data.clicker;
    if (zox_valid(dbg_chunk3_textured)) {
        zox_log("+ Deleting [Chunk3 Textured]");
        zox_delete(dbg_chunk3_textured);
        return;
    }
    zox_log("+ Spawning [Chunk3 Textured]");
    entity e = spawn_dbg_chunk3_textured(
        world,
        player);
    zox_set_unique_name(e, "dbg_chunk3");
    if (dbg_inspector) {
        entity canvas = zox_get_link(
            world,
            player,
            Canvas);
        spawn_inspector(
            world,
            canvas,
            player,
            e);
    }
    dbg_chunk3_textured = e;
}
