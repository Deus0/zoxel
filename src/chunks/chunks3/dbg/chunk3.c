extern entity prefab_vox;
entity dbg_chunk3;

entity spawn_test_vox_at(
    ecs* world,
    float3 position,
    float4 rotation,
    float2 scales)
{
    // byte dbg_inspector = 0;
    float distance = frand_range(2.6f, 3.4f);
    float3 spawn_position = move_along_direction(
        position,
        rotation,
        -distance);
    float scale = frand_range(scales.x, scales.y);
    float block_scale = 1.0f / 16.0f;
    byte depth = block_depth_limits.y;
    float spin = rand_range(2, 8);
    float3 euler = (float3) {
        rand() % 100 > 50 ? spin : -spin,
        rand() % 100 > 50 ? spin : -spin,
        rand() % 100 > 50 ? spin : -spin
    };
    int3 size = int3_single(powers_of_two[depth]);
    // spawn chunk3s
    entity e = spawn_chunk3(
        world,
        prefab_vox,
        spawn_position,
        scale,
        depth,
        block_scale,
        size);
    zox_add(e, VoxMesh);
    zox_setv(e, GenerateModel, zox_generate_model_run);
    add_eternal_euler(world, e, euler);
    return e;
}

entity spawn_test_vox(
    ecs* world,
    entity player)
{
    entity camera = zox_get_link(world, player, Camera);
    if (!zox_valid(camera)) {
        return 0;
    }
    return spawn_test_vox_at(
        world,
        zox_getv(camera, Position3D),
        zox_getv(camera, Rotation3D),
        (float2) { 0.1f, 0.3f }
    );
}

entity zox_dbg_spawn_chunk3(ecs* world, byte dbg_inspector) {
    if (zox_valid(dbg_chunk3)) {
        zox_log("Deleting Test: Spawn [Chunk3]");
        zox_delete(dbg_chunk3);
        return 0;
    }
    zox_log("Running Test: Spawn [Chunk3]");
    entity player = dbg_player;
    entity e = spawn_test_vox(world, player);
    zox_set_unique_name(e, "dbg_chunk3");
    if (dbg_inspector) {
        entity canvas = zox_get_link(world, player, Canvas);
        spawn_inspector(
            world,
            canvas,
            player,
            e);
    }
    dbg_chunk3 = e;
    return e;
}

void zox_dbg_spawn_chunk3_button(ecs* world, ClickEventData data) {
    byte dbg_inspector = 0;
    zox_dbg_spawn_chunk3(world, dbg_inspector);
}
