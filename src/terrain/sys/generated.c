// NOTE: Tell the light systems to update when its all done
zox_sys2(Chunk3GeneratedSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(NodeDepth);
    zox_sys_out(Loaded);
    zox_sys_out(GenerateLights);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, state);
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(Loaded, loaded);
        zox_sys_o(GenerateLights, dirty);
        if (state->value == zox_dirty_end) {
            dirty->value = zox_dirty_trigger;
            if (!loaded->value) {
                entity terrain = zox_get_parent(world, e);
                if (!zox_valid(terrain)) {
                    zox_loge("Missing Parent of Chunk3 Terrain [%s]", zox_get_name(e));
                    continue;
                }
                byte terrain_depth = zox_getv(terrain, NodeDepth);
                if (depth->value == terrain_depth) {
                    loaded->value = 1;
                }
            }
        }
    }
} zox_sys_end(Chunk3GeneratedSystem);
