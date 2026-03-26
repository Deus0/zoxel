

// like text, sets the list of text onto the ui element list
void set_ui_list_hierarchy(ecs *world, Children *children, entity window_entity, entity canvas, int elements_visible, text_group_dynamic_array_d* labels, entity_array_d* entities, int labels_count, ClickEvent click_event, byte button_layer, byte2 button_padding, int button_inner_margins, byte font_size, byte list_start, int2 list_margins, byte is_scrollbar, int scrollbar_width, int scrollbar_margins, float2 window_position, int2 window_pixel_positionv, int2 window_size, int2 canvas_size) {

    (void) window_position;
    // resize scrollbar
    resize_window_scrollbar(world, children, window_size, canvas_size, elements_visible, labels_count);
    // refresh elements
    int childrens_length = list_start + labels_count;

    // destroy previous ones
    for (int j = list_start; j < children->length; j++) {
        zox_delete(children->value[j])
    }

    // set new elements size
    resize_Children(children, childrens_length);
    for (int j = 0; j < labels_count; j++) {
        byte render_disabled = !(j >= 0 && j < elements_visible);

        int2 label_position = get_element_label_position(j, font_size, button_padding, button_inner_margins, window_size, list_margins, is_scrollbar, scrollbar_width, scrollbar_margins);

        entity e2 = spawn_button_old(world, window_entity, canvas, label_position, button_padding, float2_half, labels->data[j].text, font_size, button_layer, window_pixel_positionv, window_size, canvas_size, render_disabled);

        zox_set(e2, ClickEvent, { click_event.value })
        zox_set(e2, EntityTarget, { entities->data[j] })
        children->value[list_start + j] = e2;
    }
}


zox_sys2(HierarchyRefreshSystem) {

    byte is_header = 1;
    byte is_scrollbar = 1;
    byte list_start = is_header + is_scrollbar;
    const ClickEvent click_event = (ClickEvent) { &button_event_clicked_hierarchy };

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position2);
    zox_sys_in(CanvasPosition);
    zox_sys_in(Layer2D);
    zox_sys_in(Anchor);
    zox_sys_in(ListUIMax);
    zox_sys_in(ElementFontSize);
    zox_sys_in(CanvasLink);
    zox_sys_in(EditorTarget);
    zox_sys_out(HierarchyUIDirty);
    zox_sys_out(LayoutPosition);
    zox_sys_out(LayoutSize);
    zox_sys_out(TextureSize);
    zox_sys_out(Children);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position2, position2);
        zox_sys_i(CanvasPosition, canvasPosition);
        zox_sys_i(Layer2D, layer2D);
        zox_sys_i(Anchor, anchor);
        zox_sys_i(ListUIMax, max);
        zox_sys_i(ElementFontSize, font_size);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(EditorTarget, target);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LayoutSize, size);
        zox_sys_o(TextureSize, tsize);
        zox_sys_o(HierarchyUIDirty, dirty);
        zox_sys_o(Children, children);

        if (!dirty->value || !children->value || children->length < 2) {
            continue; // children issues
        }

        entity header = children->value[0];
        entity scrollbar = children->value[is_header];
        if (!header || !scrollbar) {
            continue; // no scrollbar
        }

        zox_geter_value(canvas->value, LayoutSize, int2, canvas_size)
        int elements_visible = max->value;
        byte button_layer = layer2D->value + 1;
        int scrollbar_margins = zox_gett_value(scrollbar, ElementMargins).x;
        int scrollbar_width = zox_gett_value(scrollbar, LayoutSize).x;
        byte2 button_padding = (byte2) { (int) (font_size->value * 0.46f), (int) (font_size->value * 0.3f) };
        int2 list_margins = (int2) { (int) (font_size->value * 0.8f), (int) (font_size->value * 0.8f) };
        int button_inner_margins = (int) (font_size->value * 0.5f);

        // our label data
        entity_array_d* entities = create_entity_array_d(32);
        text_group_dynamic_array_d* labels = create_text_group_dynamic_array_d(32);
        editor_fetch_children(world, entities, labels, target->value);
        // editor_fetch_children(world, entities, labels, local_realm);

        // resize window
        int labels_count = labels->size;
        // first pass, limit it, some reason flecs table glitches here
#ifdef zox_glitch_fix_hierarchy_labels
        if (is_first_hierarchy_spawn) { // children->length == 2
            is_first_hierarchy_spawn = 0;
            labels_count = int_min(max_hierarchy_labels, labels_count);
        }
#endif

        int max_characters = get_max_characters_d("hierarchy", labels);
        float2 window_position = position2->value;
        int2 window_pixel_positionv = canvasPosition->value;
        int2 old_window_size = size->value;

        int2 new_window_size = { (font_size->value) * max_characters + button_padding.x * 2 + list_margins.x * 2, old_window_size.y };
        if (is_scrollbar) {
            new_window_size.x += scrollbar_width + scrollbar_margins * 2;
        }

        if (new_window_size.x != old_window_size.x) {
            int header_height = zox_gett_value(header, LayoutSize).y;
            reverse_anchor_element_position2_with_header(&position->value, anchor->value, old_window_size, header_height);
            size->value = new_window_size;
            tsize->value = new_window_size;
            anchor_element_position2_with_header(&position->value, anchor->value, size->value, header_height);
        }

        // refresh elements
        set_ui_list_hierarchy(world, children, e, canvas->value, elements_visible,
            labels, entities, labels_count, click_event, button_layer, button_padding, button_inner_margins, font_size->value, list_start, list_margins, is_scrollbar, scrollbar_width, scrollbar_margins, window_position, window_pixel_positionv, new_window_size, canvas_size);

        // dispose allocated things
        for (size_t j = 0; j < labels->size; j++) {
            free(labels->data[j].text);
        }

        dispose_text_group_dynamic_array_d(labels);
        dispose_entity_array_d(entities);

        dirty->value = 0;
    }
} zox_sys_end(HierarchyRefreshSystem);




    /*add_entity_to_labels(world, target, labels, entities, 0);
    add_entity_children_to_labels(world, target, labels, entities, 0);
    add_to_labels_voxel_links(world, target, labels, entities, 0);
#ifdef zoxm_stats
    add_to_labels_stat_links(world, target, labels, entities, 0);
#endif*/

    // prefabs
    /*add_entity_to_labels(world, prefab_app, labels, entities, 0);
    add_entity_to_labels(world, prefab_window, labels, entities, 0);
    add_entity_to_labels(world, prefab_button, labels, entities, 0);
    add_entity_to_labels(world, prefab_zext, labels, entities, 0);
    add_entity_to_labels(world, prefab_zigel, labels, entities, 0);
    add_entity_to_labels(world, prefab_realm, labels, entities, 0);
    add_entity_to_labels(world, prefab_block, labels, entities, 0);
    add_entity_to_labels(world, prefab_texture, labels, entities, 0);*/

    /*add_entity_to_labels(world, local_music, labels, entities, 0);
    for (int k = 0; k < main_cameras_count; k++) {
        add_entity_to_labels(world, main_cameras[k], labels, entities, 0);
        add_entity_to_labels(world, ui_cameras[k], labels, entities, 0);
    }
    // add_entity_children_to_labels(world, zox_players[0], labels, entities, 0);
    add_entity_children_to_labels(world, local_keyboard, labels, entities, 0);
    if (local_mouse) {
        add_entity_children_to_labels(world, local_mouse, labels, entities, 0);
    }
    add_entity_children_to_labels(world, gamepad_entity, labels, entities, 0);
    add_entity_children_to_labels(world, local_touchscreen, labels, entities, 0);
    // add_entity_children_to_labels(world, local_character3D, labels, entities, 0);
    add_entity_to_labels(world, local_terrain, labels, entities, 0);
    add_entity_children_to_labels(world, canvas, labels, entities, 0);*/
