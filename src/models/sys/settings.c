zox_sys2(ModelsSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InitializeEntity, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        spawn_setting_byte(world, e, "Outlines", is_generate_vox_outlines);
        spawn_setting_byte_slider(world, e, "Resolution", block_vox_depth, (byte2) { 1, 5 });
    }
} zox_sys_end(ModelsSettingsSystem);

zox_sys2(ModelsSettingsDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SettingDirty);
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SettingDirty, state);
        zox_sys_i(ZoxName, name);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
            }
            if (!strcmp(name->value, "Outlines")) {
                is_generate_vox_outlines = value;
            } else if (!strcmp(name->value, "Resolution")) {
                block_vox_depth = value;
                int2 size = int2_single(powers_of_two[block_vox_depth]);
                int3 size3 = int3_single(powers_of_two[block_vox_depth]);
                zox_set(prefab_texture, TextureSize, { size });
                zox_set(prefab_vox_texture, TextureSize, { size });
                zox_set(prefab_vox, NodeDepth, { block_vox_depth });
                zox_set(prefab_vox, ChunkSize, { size3 });
                zox_set(prefab_vox_generated, NodeDepth, { block_vox_depth });
                zox_set(prefab_vox_generated, ChunkSize, { size3 });
                zox_set(prefab_block_vox, NodeDepth, { block_vox_depth });
                zox_set(prefab_block_vox, ChunkSize, { size3 });
            }
        }
    }
} zox_sys_end(ModelsSettingsDirtySystem);
