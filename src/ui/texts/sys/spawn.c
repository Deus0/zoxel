// NOTE: Spawns / Destroys Zigels of the Text
// #define zoxel_debug_zext_updates
// Dynamically keeps the text characters the right length using entities
zox_sys2(ZigelSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(FontOutlineColor);
    zox_sys_in(FontFillColor);
    zox_sys_in(FontThickness);
    zox_sys_in(FontOutlineThickness);
    zox_sys_in(TextResolution);
    zox_sys_in(Layer2D);
    zox_sys_out(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(TextData, tdata);
        zox_sys_i(TextFontSize, textSize);
        zox_sys_i(FontOutlineColor, fontOutlineColor);
        zox_sys_i(FontFillColor, fontFillColor);
        zox_sys_i(FontThickness, fontThickness);
        zox_sys_i(FontOutlineThickness, fontOutlineThickness);
        zox_sys_i(TextResolution, textResolution);
        zox_sys_i(Layer2D, layer);
        zox_sys_o(RenderDisabled, render_disabled);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            zox_logw("Canvas found on Text");
            continue;
        }
        uint new_length = (uint) calculate_total_zigels(tdata->value, tdata->length);
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
                zox_log(" - Shrinking Text!");
            }
            // NOTE: Shrinks the children zigels
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    if (old_length == new_length) {
                        continue;
                    }
                    entity e2 = it2.entities[j];
                    if (!zox_has(e2, Zigel)) {
                        continue;
                    }
                    if (dbg_log) {
                        zox_log("   - Del Zigel [%s]", zox_get_name(e2));
                    }
                    // keep deleting until we arrive at new length;
                    old_length--;
                    zox_delete(e2);
                }
            }
        } else if (new_length > old_length) {
            if (dbg_log) {
                zox_log(" + Growing Text!");
            }
            for (uint j = old_length; j < new_length; j++) {
                byte index = calculate_zigel_index(tdata->value, tdata->length, j);
                entity e2 = spawn_zigel(world, prefab_zigel, e, position_anchor, size, texture_size, index, thickness, othickness, fill, outline);
                zox_set(e2, RenderDisabled, { render_disabled->value });
                zox_set(e2, Layer2D, { layer->value + 1 });
                if (dbg_log) {
                    zox_log("   + Spawn Zigel [%i]", index);
                    // zox_log("zigel [%i] is invisible [%i]", i, data.element.render_disabled);
                }
            }
        }
    }
} zox_sys_end(ZigelSpawnSystem);

