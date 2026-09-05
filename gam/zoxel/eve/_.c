// we should just add dirty hook to the settings entities

void app_initialize_zoxel(ecs* world, entity app) {
    entity e2 = spawn_test_vox_at(
        world,
        float3_zero,
        quaternion_identity,
        (float2) { 1.6f, 2.6f });
    zox_set_parent(world, e2, app);
}

void zox_events_zoxel(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_zoxel);
}
