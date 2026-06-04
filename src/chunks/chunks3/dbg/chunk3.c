extern entity prefab_vox;
entity dbg_chunk3;
extern entity spawn_inspector(ecs*, entity, entity, entity);

void zox_dbg_spawn_chunk3(ecs* world, ClickEventData data) {
    if (zox_valid(dbg_chunk3)) {
        zox_log("+ Deleting [Test Chunk3]");
        zox_delete(dbg_chunk3);
        return;
    }
    byte dbg_inspector = 1;
    zox_log("+ Test [Spawn Chunk3]");
    entity player = dbg_player;
    entity canvas = zox_getv(player, CanvasLink);
    entity camera = zox_getv(player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    entity prefab = prefab_vox; // prefab_chunk
    float scale = 1;
    byte depth = block_vox_depth;
    float block_scale = 1.0f / 32.0f;
    int3 size = int3_single(powers_of_two[depth]);
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -2);
    // spawn chu/nk3
    dbg_chunk3 = spawn_chunk3(world, prefab, spawn_position, scale, depth, block_scale, size);
    zox_add_tag(dbg_chunk3, VoxMesh);
    add_eternal_euler(world, dbg_chunk3, (float3) { 25, 25, 0 });
    zox_set_unique_name(dbg_chunk3, "dbg_chunk3");
    if (dbg_inspector) {
        spawn_inspector(world, canvas, player, dbg_chunk3);
    }
}
