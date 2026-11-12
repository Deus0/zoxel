// NOTE: Spawns / Destroys Glyphs of the Text
// #define zoxel_debug_zext_updates
// Dynamically keeps the text characters the right length using entities
void glyph_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextData);
    zox_sys_in(GlyphPrefab);
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
        zox_sys_i(TextData, text_data);
        zox_sys_i(GlyphPrefab, prefab);
        zox_sys_i(TextFontSize, text_size);
        zox_sys_i(FontOutlineColor, outline);
        zox_sys_i(FontFillColor, fill);
        zox_sys_i(FontThickness, fontThickness);
        zox_sys_i(OutlineThickness, fontOutlineThickness);
        zox_sys_i(TextResolution, resolution);
        zox_sys_i(Layer, layer);
        zox_sys_o(RenderDisabled, render_disabled);
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
#ifdef zox_safety_checks
        if (!zox_valid(canvas)) {
            zox_logw("Canvas found on Text");
            continue;
        }
#endif
        uint new_length = (uint) calculate_total_zigels(
            text_data->value,
            text_data->length);
        uint old_length = zox_get_children_count_by_id(
            world,
            e,
            zox_id(Glyph));
        // visibility follows the text, even when the glyph count didn't change
        if (zox_has(e, TextRenderEnabler)) {
            render_disabled->value = new_length == 0;
        }
        if (new_length == old_length) {
            continue;
        }
        float2 position_anchor = float2_half;
        int2 size = int2_single(text_size->value);
        int2 texture_size = int2_single(resolution->value);
        byte thickness = fontThickness->value;
        byte othickness = fontOutlineThickness->value;
        byte zigel_layer = layer->value + 1;
        if (dbg_log) {
            zox_log("%s Text [%s] Glyphs: [%i] -> [%i] - Layer [%i]",
                new_length > old_length ?
                    "+ Growing" : "- Shrinking",
                zox_getn(e),
                old_length,
                new_length,
                zigel_layer);
        }
        if (new_length < old_length) {
            // NOTE: Shrinks the children zigels from the end, by their ChildIndex
            //      - keeps remaining glyphs ordered
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    entity e2 = it2.entities[j];
                    if (!zox_has(e2, Glyph) || !zox_has(e2, ChildIndex)) {
                        continue;
                    }
                    if (zox_getv(e2, ChildIndex) < new_length) {
                        continue;
                    }
                    zox_delete(e2);
                    if (dbg_log) {
                        zox_log("   - Destroyed [%s]",
                            zox_getn(e2));
                    }
                }
            }
        } else if (new_length > old_length) {
            // NOTE: Glyph Data Index just removes new lines out of the data
            byte centred = zox_has(e, CentredGlyph);
            for (uint j = old_length; j < new_length; j++) {
                byte zigel = calculate_zigel_index(text_data->value, text_data->length, j);
                uint child_index = j;
                entity e2 = spawn_zigel(
                    world,
                    prefab->value,
                    e,
                    position_anchor,
                    size,
                    texture_size,
                    thickness,
                    othickness,
                    fill->value,
                    outline->value,
                    zigel,
                    child_index,
                    zigel_layer);
                zox_setv(e2, RenderDisabled, render_disabled->value);
                if (centred) {
                    zox_add(e2, CentredGlyph);
                }
                if (dbg_log) {
                    zox_log("   + Spawn [%i]: [%c]",
                        zigel,
                        convert_to_ascii(zigel));
                }
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(glyph_spawn_system);
