extern entity prefab_vox;

void zox_dbg_spawn_chunk3(ecs* world, ClickEventData data) {
    zox_log("+ Testing [chunk3]");
    entity camera = zox_getv(dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    entity prefab = prefab_vox; // prefab_chunk
    float scale = 1;
    byte depth = block_vox_depth;
    float block_scale = 1.0f / 8.0f;
    int3 size = int3_single(powers_of_two[depth]);
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -2);
    // spawn chunk3
    spawn_chunk3(world, prefab, spawn_position, scale, depth, block_scale, size);
}
