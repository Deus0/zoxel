extern entity prefab_vox;
entity dbg_chunk3;
extern entity spawn_inspector(ecs*, entity, entity, entity);

entity zox_dbg_spawn_chunk3(ecs* world, byte dbg_inspector) {
    if (zox_valid(dbg_chunk3)) {
        zox_log("Deleting Test: Spawn [Chunk3]");
        zox_delete(dbg_chunk3);
        return 0;
    }
    zox_log("Running Test: Spawn [Chunk3]");
    entity player = dbg_player;
    entity canvas = zox_getv(player, CanvasLink);
    entity camera = zox_getv(player, CameraLink);
    if (!zox_valid(camera)) {
        return 0;
    }
    entity prefab = prefab_vox; // prefab_chunk
    float scale = frand_range(0.8f, 1.2f);
    float distance = frand_range(2.6f, 3.4f);
    float block_scale = 1.0f / 16.0f;
    byte depth = block_vox_depth_limits.y;
    float spin = rand_range(2, 8);
    float3 euler = (float3) { rand() % 100 > 50 ? spin : -spin, rand() % 100 > 50 ? spin : -spin, rand() % 100 > 50 ? spin : -spin };
    int3 size = int3_single(powers_of_two[depth]);
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -distance);
    // spawn chunk3s
    entity e = spawn_chunk3(world, prefab, spawn_position, scale, depth, block_scale, size);
    zox_set(e, GenerateModel, { zox_generate_model_run });
    zox_add_tag(e, VoxMesh);
    add_eternal_euler(world, e, euler);
    zox_set_unique_name(e, "dbg_chunk3");
    if (dbg_inspector) {
        spawn_inspector(world, canvas, player, e);
    }
    dbg_chunk3 = e;
    return e;
}

void zox_dbg_spawn_chunk3_button(ecs* world, ClickEventData data) {
    byte dbg_inspector = 1;
    zox_dbg_spawn_chunk3(world, dbg_inspector);
}
