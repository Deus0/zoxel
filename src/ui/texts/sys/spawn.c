// NOTE: Spawns / Destroys Zigels of the Text
// #define zoxel_debug_zext_updates
// Dynamically keeps the text characters the right length using entities
zox_sys2(ZigelSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(ZigelPrefab);
    zox_sys_in(TextFontSize);
    zox_sys_in(FontOutlineColor);
    zox_sys_in(FontFillColor);
    zox_sys_in(FontThickness);
    zox_sys_in(OutlineThickness);
    zox_sys_in(TextResolution);
    zox_sys_in(Layer);
    zox_sys_out(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextDirty, text_dirty);
        zox_sys_i(TextData, text_data);
        zox_sys_i(ZigelPrefab, prefab);
        zox_sys_i(TextFontSize, textSize);
        zox_sys_i(FontOutlineColor, fontOutlineColor);
        zox_sys_i(FontFillColor, fontFillColor);
        zox_sys_i(FontThickness, fontThickness);
        zox_sys_i(OutlineThickness, fontOutlineThickness);
        zox_sys_i(TextResolution, textResolution);
        zox_sys_i(Layer, layer);
        zox_sys_o(RenderDisabled, render_disabled);
        if (text_dirty->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
#ifdef zox_safety_checks
        if (!zox_valid(canvas)) {
            zox_logw("Canvas found on Text");
            continue;
        }
#endif
        uint new_length = (uint) calculate_total_zigels(text_data->value, text_data->length);
        if (zox_has(e, ZextRenderEnabler)) {
            render_disabled->value = new_length == 0;
        }
        float2 position_anchor = float2_half;
        int2 size = int2_single(textSize->value);
        int2 texture_size = int2_single(textResolution->value);
        byte thickness = fontThickness->value;
        byte othickness = fontOutlineThickness->value;
        color fill = fontFillColor->value;
        color outline = fontOutlineColor->value;
        uint old_length = zox_get_children_count_by_id(world, e, zox_id(Zigel));
        if (dbg_log) {
            zox_log("Updating Text [%s] [%i -> %i]", zox_get_name(e), old_length, new_length);
        }
        if (new_length < old_length) {
            if (dbg_log) {
                zox_log("[%s] Shrinking Text [%i] from [%i]", zox_getn(e), new_length, old_length);
            }
            // NOTE: Shrinks the children zigels
            // int child_index = old_length - 1;
            uint deleted_count = 0;
            uint deleted_target = old_length - new_length;
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && deleted_count < deleted_target; j++) {
                    entity e2 = it2.entities[j];
#ifdef zox_safety_checks
                    if (!zox_has(e2, Zigel)) {
                        zox_loge("Zigel [%s] is Invalid", zox_get_name(e2));
                        continue;
                    }
#endif
                    // keep deleting until we arrive at new length;
                    zox_delete(e2);
                    deleted_count++;
                    if (dbg_log) {
                        zox_log("   - Deleted Zigel [%s]", zox_get_name(e2));
                    }
                }
            }
        } else if (new_length > old_length) {
            if (dbg_log) {
                zox_log(" + Growing Text!");
            }
            byte zigel_layer = layer->value + 1;
            // NOTE: Zigel Data Index just removes new lines out of the data
            byte centred = zox_has(e, CentredZigel);
            for (uint j = old_length; j < new_length; j++) {
                byte zigel = calculate_zigel_index(text_data->value, text_data->length, j);
                uint child_index = j;
                entity e2 = spawn_zigel(world, prefab->value, e, position_anchor, size, texture_size, thickness, othickness, fill, outline, zigel, child_index, zigel_layer);
                zox_setv(e2, RenderDisabled, render_disabled->value);
                if (centred) {
                    zox_add(e2, CentredZigel);
                }
                if (dbg_log) {
                    zox_log("Spawn Zigel [%i:%c] - Layer [%i]", zigel, convert_to_ascii(zigel), zigel_layer);
                }
            }
        }
    }
} zox_sys_end(ZigelSpawnSystem);


        /*int child_index = 0;
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
                    if (new_length == old_length) {
                        zox_setv(e2, ZigelDirty, zox_zigel_dirty_update);
                    } else {
                        zox_setv(e2, ZigelDirty, zox_zigel_dirty_position);
                    }
                    // zox_set(e2, ZigelDirty, { zox_zigel_dirty_position });
                    // child_index++;
                }
            }
        }*/
                    // NOTE: When shrinking the children we need to adjust the child indexes
                    /*if (child_index < new_length) {
                        // here we can set child indexes
                        // zox_setv(e2, ChildIndex, child_index);
                        // child_index--;
                        continue;
                    }*/