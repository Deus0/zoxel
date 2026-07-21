zox_sys2(MarkerSpawnSystem) {
    // TODO: Check if has quest to give
    // TODO: Use texture quad instead - simpler
    byte dbg_log = 0;
    byte resolution = 64;
    byte outline_thickness = 2 * 4;
    byte scale = 10;
    float offset_x = 0.046f;
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
        char* result = "!";
        Text3DData label3D_text_data = {
            .prefab = prefab_text3D,
            .text = result,
        };
        Zigel3DData label3D_zigel_data = {
            .prefab = prefab_zigel3D,
            .scale = scale,
            .font_thickness = 1,
            .font_outline = outline_thickness,
            .resolution = resolution,
            .fill_color = fill,
            .outline_color = outline
        };
        SpawnDataElement3 label3D_spawn_data = {
            .ui_holder = e,
            .prefab = prefab_label3D,
            .base_color = background,
            .outline_color = background
        };
        entity e2 = spawn_label3D(world, label3D_spawn_data, label3D_text_data, label3D_zigel_data, (float3) { offset_x, name_trail_offset * 2, 0 });
        zox_set_unique_name(e2, "marker");
        zox_set(e2, ElementHolder, { e });
        add_to_ElementLinks(elements, e2);
        if (dbg_log) {
            zox_log("Spawned Marker Label on [%s]: %s", zox_getn(e), result);
        }
    }
} zox_sys_end(MarkerSpawnSystem);
