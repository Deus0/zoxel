extern entity prefab_vox;
entity dbg_chunk3_mass;

void zox_dbg_spawn_chunk3_mass(ecs* world, ClickEventData data) {
    if (zox_valid(dbg_chunk3_mass)) {
        zox_log("+ Deleting: Mass Spawn [Chunk3]");
        zox_delete(dbg_chunk3_mass);
        return;
    }
    // byte dbg_inspector = 1;
    zox_log("Running Test: Mass Spawn [Chunk3]");
    entity player = dbg_player;
    // entity canvas = zox_getv(player, CanvasLink);
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
    // spawn chunk3
    uint test_count = 512;
    float test_range = 8;
    zox_log("Test Count [%i]\nTest Range [%f]", test_count, test_range);
    entity e = zox_new();
    zox_set_unique_name(e, "dbg_chunk3_mass");
    dbg_chunk3_mass = e;
    for (int i = 0; i < test_count; i++) {
        float3 e2_position = spawn_position;
        e2_position.x += frand_range(-test_range, test_range);
        e2_position.y += frand_range(-test_range, test_range);
        e2_position.z += frand_range(-test_range, test_range);
        entity e2 = spawn_chunk3(world, prefab, e2_position, scale, depth, block_scale, size);
        zox_set(e, Seed, { i * 10000 });
        zox_set(e2, GenerateModel, { zox_generate_model_run });
        zox_add(e2, VoxMesh);
        add_eternal_euler(world, e2, (float3) { 25, 25, 0 });
        zox_set_unique_name(e2, "dbg_chunk3_mass");
        zox_set_parent(world, e2, e);
    }

}
