// its settings, spawned on the app (InitializeEvent)
//  - GPU Meshing: chunks' solid meshes on the gpu (zox_chunks3_gpu_meshing, set/_.c): on with vulkan, off with opengl
//    by default (--gpu_meshing, --cpu_meshing); a chunk mesh takes it when it spawns, so a change shows on chunks
//    loaded after (a new game, a restart), the ones loaded keep theirs (cpu and gpu meshes draw side by side)
//  - Greedy Meshing: vox models' faces merged where they draw the same (zox_greedy_mesh, fun/greedy.c), on by default
//    (--greedy_meshing, --no_greedy_meshing); meshes built after take it
void chunks3_settings(ecs* world, entity e) {
    setting_tags(world, spawn_setting_byte(world, e, "GPU Meshing", zox_chunks3_gpu_meshing), zox_id(SettingTabVideo), zox_id(SettingGpuMeshing));
    setting_tags(world, spawn_setting_byte(world, e, "Greedy Meshing", zox_greedy_mesh), zox_id(SettingTabVideo), zox_id(SettingGreedyMeshing));
}

// a setting changed (its Dirty, added again each frame it changes: a slider dragged)
void chunks3_settings_on_dirty(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingByte) && zox_has(e, SettingGpuMeshing)) {
            zox_chunks3_gpu_meshing = zox_getv(e, SettingByte);
        } else if (zox_has(e, SettingByte) && zox_has(e, SettingGreedyMeshing)) {
            zox_greedy_mesh = zox_getv(e, SettingByte);
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Dirty)
void chunks3_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, chunks3_settings);
    zox_on_add(
        chunks3_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
