zox_sys2(MarkerSpawnSystem) {
    // TODO: Check if has quest to give
    // TODO: Use texture quad instead - simpler
    byte dbg_log = 0;
    byte resolution = 64;
    // byte outline_thickness = 6;
    // byte scale = 10;
    float ui_scale = zox_ui_scale3 * 3;
    // float offset_x = 0; // 0.046f;
    color fill = color_yellow;
    color outline = color_black;
    color background = color_null;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_out(ElementLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_o(ElementLinks, elements);
        if (state->value != zox_dirty_end) {
            continue;
        }
        uint quests = zox_get_children_count_by_id(world, e, zox_id(Quest));
        if (!quests) {
            continue;
        }
        char* text = "!";
        entity2 e2 = spawn_label3(world, text, resolution, background, background, fill, outline, ui_scale, e, name_trail_offset * 2);
        zox_set_unique_name(e2.x, "marker");
        zox_set(e2.x, ElementHolder, { e });
        add_to_ElementLinks(elements, e2.x);
        // Text
        zox_add_tag(e2.y, CentredZigel);
        if (dbg_log) {
            zox_log("Spawned Marker Label on [%s]: %s", zox_getn(e), text);
        }
    }
} zox_sys_end(MarkerSpawnSystem);
