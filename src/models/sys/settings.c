// its settings, spawned on the app (InitializeEvent)
void models_settings(ecs* world, entity e) {
    spawn_setting_byte_slider(world, e, "Block Res", block_depth, block_depth_limits);
    spawn_setting_byte(world, e, "Outlines", zox_block_outlines);
#ifdef zox_dev
    spawn_setting_byte_slider(world, e, "Terrain Res", terrain_depth, terrain_depth_limits);
#endif
}

// a setting changed (Update, the frame after its Dirty)
void models_settings_on_dirty(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
            }
            if (!strcmp(name->value, "Outlines")) {
                zox_block_outlines = value;
            } else if (!strcmp(name->value, "Block Res")) {
                block_depth = value;
                int2 size = int2_single(powers_of_two[block_depth]);
                int3 size3 = int3_single(powers_of_two[block_depth]);
                zox_set(prefab_texture, TextureSize, { size });
                zox_set(prefab_vox_texture, TextureSize, { size });
                zox_set(prefab_vox, NodeDepth, { block_depth });
                zox_set(prefab_vox, ChunkSize, { size3 });
                zox_set(prefab_vox_generated, NodeDepth, { block_depth });
                zox_set(prefab_vox_generated, ChunkSize, { size3 });
                zox_set(prefab_block_vox, NodeDepth, { block_depth });
                zox_set(prefab_block_vox, ChunkSize, { size3 });
            } else if (!strcmp(name->value, "Terrain Res")) {
                terrain_depth = value;
            }
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Update)
void models_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, models_settings);
    zox_on_add(
        models_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}
