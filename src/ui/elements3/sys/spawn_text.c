// todo: split up into update system, and resize system
// note: update system can be generically for 2D and 3D text
/*zox_sys2(Text3DResizeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(FontOutlineColor);
    zox_sys_in(FontFillColor);
    zox_sys_in(FontThickness);
    zox_sys_in(OutlineThickness);
    zox_sys_in(RenderDisabled);
    zox_sys_in(Text3DScale);
    zox_sys_in(TextFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(FontThickness, thickness);
        zox_sys_i(OutlineThickness, outline_thickness);
        zox_sys_i(FontFillColor, fill);
        zox_sys_i(FontOutlineColor, outline);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(Text3DScale, scale);
        zox_sys_i(TextFontSize, resolution);
        zox_sys_i(TextData, text);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        uint new_length = calculate_total_zigels(text->value, text->length);
        uint old_length = zox_get_children_count_by_id(world, e, zox_id(Zigel));
        int child_index = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && child_index < new_length; j++, child_index++) {
                entity e2 = it2.entities[j];
#ifdef zox_safety_checks
                if (!zox_has(e2, Zigel)) {
                    zox_loge("Zigel [%s] is Invalid", zox_get_name(e2));
                    continue;
                }
#endif
                // zox_log("Set Zigel3D to dirty! %s:%i", zox_getn(e), child_index);
                zox_setv(e2, ZigelDirty, zox_zigel_dirty_update);
            }
        }
        if (old_length == new_length) {
            if (dbg_log) {
                zox_log("[%s] Text [%s] Dirty at same length [%i]", zox_getn(e), text->value, new_length);
                // print_entity_zext(world, e);
            }
            continue;
        }
        if (new_length < old_length) {
            if (dbg_log) {
                zox_log(" - Shrinking Text [%s]! %i -> %i", zox_getn(e), new_length, old_length);
            }
            int child_index = old_length - 1;
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    entity e2 = it2.entities[j];
#ifdef zox_safety_checks
                    if (!zox_has(e2, Zigel)) {
                        zox_loge("Zigel [%s] is Invalid", zox_get_name(e2));
                        continue;
                    }
#endif
                    // NOTE: When shrinking the children we need to adjust the child indexes
                    if (child_index < new_length) {
                        // here we can set child indexes
                        zox_setv(e2, ChildIndex, child_index);
                        // Reposition as no system for this atm
                        uint index = child_index_to_text_array_index(text->value, text->length, child_index);
                        float3 position = calculate_zigel3D_position(zigel3D_size, index, new_length, scale->value);
                        zox_set(e2, LocalPosition3D, { position });
                        if (dbg_log) {
                            zox_log("Spawned Zigel [%i]", child_index);
                            zox_log("[%s] Updated [%i] zigel3 [%lu] child_index[%i] index [%i]", zox_getn(e), i, (e2), child_index, index);
                        }
                        child_index--;
                        continue;
                    }
                    if (dbg_log) {
                        zox_log("   - Deleted Zigel [%s]", zox_get_name(e2));
                    }
                    // keep deleting until we arrive at new length;
                    zox_delete(e2);
                    // child_index--;
                }
            }
        } else if (new_length > old_length) {
            if (dbg_log) {
                if (old_length) {
                    zox_log(" + Growing Text [%s]! %i -> %i", zox_getn(e), old_length, new_length);
                } else {
                    zox_log(" = Creating Text [%s]! %i -> %i", zox_getn(e), old_length, new_length);
                }
            }
            // Reposition them all
            uint child_index = 0;
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    entity e2 = it2.entities[j];
                    if (!zox_has(e2, Zigel)) {
                        continue;
                    }
                    zox_setv(e2, ChildIndex, child_index);
                    uint index = child_index_to_text_array_index(text->value, text->length, child_index);
                    float3 position = calculate_zigel3D_position(zigel3D_size, index, new_length, scale->value);
                    zox_set(e2, LocalPosition3D, { position });
                    child_index++;
                }
            }
            // NOTE: Zigel Data Index just removes new lines out of the data
            for (uint j = old_length; j < new_length; j++) {
                uint child_index = j;
                uint index = child_index_to_text_array_index(text->value, text->length, child_index);
                byte zigel_index = calculate_zigel_index(text->value, text->length, j);
                float3 position = calculate_zigel3D_position(zigel3D_size, index, new_length, scale->value);
                entity e2 = spawn_zigel3(world, prefab_zigel3, e, child_index, zigel_index, position, scale->value, render_disabled->value, thickness->value, outline_thickness->value, resolution->value, fill->value, outline->value);
                if (zox_has(e, CentredZigel)) {
                    zox_add(e2, CentredZigel);
                }
                if (dbg_log) {
                    zox_log("[%s] Spawned [%i] zigel3 [%lu] child_index[%i] index [%i] zigel [%i] at [%fx%fx%f]", zox_getn(e), i, (e2), child_index, index, zigel_index, position.x, position.y, position.z);
                }
            }
        }
        // Set old zigels
    }
} zox_sys_end(Text3DResizeSystem);*/

        /*int reuse_count = int_min(children_length, new_length);
        for (uint i = 0; i < reuse_count; i++) {     // Reposition old zigels!
            uint data_index = child_index_to_text_array_index(text->value, text->length, i);
            float3 zigel_position = calculate_zigel3D_position(zigel3D_size, data_index, new_length, zigel_data.scale);
            entity e2 = children[i];
            zox_set(e2, LocalPosition3D, { zigel_position });
            // new_children[i] = e;
            // zox_log_text3D("    > reusing [%i] zigel [%s]", i, zox_get_name(e))
        }
        // Spawn if extended text
        if (new_length > children_length) {
            for (uint i = children_length; i < new_length; i++) {
                uint index = child_index_to_text_array_index(text->value, text->length, i);
                byte zigel_index = calculate_zigel_index(text->value, text->length, i);
                zigel_data.zigel_index = zigel_index;
                zigel_data.position = calculate_zigel3D_position(zigel3D_size, index, new_length, zigel_data.scale);
                entity e2 = spawn_zigel3(world, zigel_data);
                zox_set_parent(world, e2, e);
                if (dbg_log) {
                    zox_log("[%s] Spawned [%i] zigel3 [%lu]", zox_getn(e), i, (e2));
                }
            }
        }
        // Remove if shortening text
        else if (new_length < children_length) {
            for (uint i = new_length; i < children_length; i++) {
                entity e2 = children[i];
                if (dbg_log) {
                    zox_log("[%s] Deleted [%i] zigel [%s]", zox_getn(e), i, zox_getn(e2));
                }
                zox_delete(e2);
            }
        }*/