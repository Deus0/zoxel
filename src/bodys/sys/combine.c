entity item_get_max_depth_vox(ecs* world, entity part) {
    if (!zox_has(part, ModelLink)) {
        zox_log_error("Part has no Model Link [%s]", zox_get_name(part));
        return 0;
    }
    zox_geter_value_non_const(part, ModelLink, entity, vox);
    if (!zox_valid(vox)) {
        zox_log_error("[player body]: Invalid Part %s Model", zox_get_name(part));
        return 0;
    }
    if (!zox_has(vox, MaxRenderDepth)) {
        zox_log_error("Part Vox Invalid Components %s Model %s", zox_get_name(part), zox_get_name(vox));
        return 0;
    }
    zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
    if (zox_valid(vox) && zox_has(vox, ModelLods)) {
        zox_geter(vox, ModelLods, mlods);
        entity vox_lod = mlods->value[max_render_depth];
        if (zox_valid(vox_lod)) {
            vox = vox_lod;
        } else {
            zox_log_error("[player body]: Invalid Vox Model Lod [%s] Depth [%i]", zox_get_name(vox), max_render_depth);
        }
    }
    if (!zox_valid(vox)) {
        zox_logw("Part [%s] has invalid Vox", zox_get_name(part));
    }
    return vox;
}

// for now just set vox, later we spawn item and set it from BodyDirty
zox_sys2(BodyCombineSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodyDirty);
    zox_sys_out(CombineList);
    zox_sys_out(CombinePositions);
    zox_sys_out(CombineVox);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodyDirty, state);
        zox_sys_o(CombineList, voxes);
        zox_sys_o(CombinePositions, positions);
        zox_sys_o(CombineVox, output);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity core_part = zox_get_child_by_id(world, e, zox_id(BodyPart));
        entity_array_d* flat_parts = create_entity_array_d(1);
        add_to_entity_array_d(flat_parts, core_part);
        fetch_parts_recursive(world, flat_parts, core_part);
        // Clear combine data
        resize_CombineList(voxes, 0);
        resize_CombinePositions(positions, 0);
        for (int j = 0; j < flat_parts->size; j++) {
            entity part = flat_parts->data[j];
            entity vox = item_get_max_depth_vox(world, part);
            zox_geter_value(part, VoxelPosition, int3, vposition);
            add_to_CombineList(voxes, vox);
            add_to_CombinePositions(positions, int3_to_byte3(vposition));
        }
        dispose_entity_array_d(flat_parts);
        output->value = zox_dirty_trigger;
    }
} zox_sys_end(BodyCombineSystem);
