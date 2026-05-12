void resize_text3D(ecs *world, entity e, entity* children, uint children_length, const TextData* text, Zigel3DData zigel_data, int new_children_length) {
    // Set old zigels
    int reuse_count = int_min(children_length, new_children_length);
    for (uint i = 0; i < reuse_count; i++) {     // Reposition old zigels!
        uint data_index = calculate_zigel_data_index(text->value, text->length, i);
        float3 zigel_position = calculate_zigel3D_position(zigel3D_size, data_index, new_children_length, zigel_data.scale);

        entity e2 = children[i];
        zox_set(e2, LocalPosition3D, { zigel_position });
        // new_children[i] = e;
        // zox_log_text3D("    > reusing [%i] zigel [%s]", i, zox_get_name(e))
    }
    // Spawn if extended text
    if (new_children_length > children_length) {
        for (uint i = children_length; i < new_children_length; i++) {
            uint index = calculate_zigel_data_index(text->value, text->length, i);
            byte zigel_index = calculate_zigel_index(text->value, text->length, i);
            zigel_data.zigel_index = zigel_index;
            zigel_data.position = calculate_zigel3D_position(zigel3D_size, index, new_children_length, zigel_data.scale);
            entity e2 = spawn_zigel3(world, zigel_data);
            zox_set_parent(world, e2, e);
            // new_children[i] = e;
            // zox_log_text3D("    + spawned [%i] zigel [%s]", i, zox_get_name(e))
        }
    }
    // Remove if shortening text
    else if (new_children_length < children_length) {
        for (uint i = new_children_length; i < children_length; i++) {
            entity e2 = children[i];
            //zox_log_text3D("    - deleting [%i] zigel [%s]", i, zox_get_name(e))
            zox_delete(e2);
        }
    }
}

// todo: split up into update system, and resize system
// note: update system can be generically for 2D and 3D text
zox_sys2(Text3DResizeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(FontOutlineColor);
    zox_sys_in(FontFillColor);
    zox_sys_in(FontThickness);
    zox_sys_in(RenderDisabled);
    zox_sys_in(Text3DScale);
    zox_sys_in(TextFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(FontThickness, fontThickness);
        zox_sys_i(FontFillColor, fontFillColor);
        zox_sys_i(FontOutlineColor, fontOutlineColor);
        zox_sys_i(RenderDisabled, renderDisabled);
        zox_sys_i(Text3DScale, text3DScale);
        zox_sys_i(TextFontSize, textSize);
        zox_sys_i(TextData, text);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        int new_length = calculate_total_zigels(text->value, text->length);
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        if (children_length == new_length) {
            continue;
        }
        Zigel3DData zigel_data = {
            .prefab = prefab_zigel3D,
            .resolution = textSize->value, // 128,
        };
        zigel_data.parent = e;
        zigel_data.font_thickness = fontThickness->value;
        zigel_data.fill_color = fontFillColor->value;
        zigel_data.outline_color = fontOutlineColor->value;
        zigel_data.render_disabled = renderDisabled->value;
        zigel_data.scale = text3DScale->value;
        resize_text3D(world, e, children, children_length, text, zigel_data, new_length);
    }
} zox_sys_end(Text3DResizeSystem);
