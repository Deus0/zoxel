zox_sys2(HierarchySpawnSystem) {

    const ClickEvent on_click = (ClickEvent) { &button_event_clicked_hierarchy };

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(HierarchyUIDirty);
    zox_sys_in(CanvasLink);
    zox_sys_in(EditorTarget);
    zox_sys_in(ScrollviewLink);
    zox_sys_in(ElementFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(HierarchyUIDirty, dirty);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(EditorTarget, target);
        zox_sys_i(ScrollviewLink, scrollview);
        zox_sys_i(ElementFontSize, font_size);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(scrollview->value)) {
            zox_log_error("Scrollview Link is invalid.");
            continue;
        }

        if (!zox_has(scrollview->value, ListUILink)) {
            zox_log_error("Scrollview [%s] has no list link", zox_get_name(scrollview->value));
            continue;
        }

        // 2: Fetch our scrollview data
        entity list_ui = zox_gett_value(scrollview->value, ListUILink);

        if (!zox_valid(list_ui)) {
            zox_log_error("Scrollview [%s] Invalid ListUI", zox_get_name(scrollview->value));
            continue;
        }


        if (!zox_has(list_ui, Layer2D) || !zox_has(list_ui, ListVisible)) {
            zox_log_error("List UI [%s] Invalid Components", zox_get_name(list_ui));
            continue;
        }

        zox_geter_value(list_ui, Layer2D, byte, scrollview_layer);

        // 1: Fetch Target Hierarchy Data
        entity_array_d* entities = create_entity_array_d(16);
        text_group_dynamic_array_d* labels = create_text_group_dynamic_array_d(16);
        editor_fetch_children(world, entities, labels, target->value);

        // 3: Initialize Data
        // byte2 button_padding = (byte2) { (int) (font_size->value * 0.46f), (int) (font_size->value * 0.3f) };
        // fetch this from list data
        zox_geter_value(list_ui, TextPadding, byte2, text_padding);
        // byte2 button_padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
        ElementSpawnData child_element_data = {
            .prefab = prefab_button,
            .layer = scrollview_layer + 1,
            .anchor = float2_half,
            .render_disabled = 1, // hide until list set
        };
        SpawnTextData child_text_data = {
            .font_size = font_size->value,
            .margins = text_padding,
            .font_resolution = button_font_resolution,
            .font_fill_color = button_font_fill,
            .font_outline_color = button_font_outline,
            .font_thickness = button_font_thickness_fill,
            .font_outline_thickness = button_font_thickness_outline,
        };
        SpawnButtonData child_button_data = {
            .prefab_zext = prefab_zext,
            .fill = button_fill,
            .outline = button_outline,
        };
        LayoutParentData canvas_data = { .e = canvas->value };
        LayoutParentData child_parent_data = { .e = list_ui };

        // 4: Delete old list elements
        zox_muter(list_ui, Children, children);
        for (int j = 0; j < children->length; j++) {
            zox_delete(children->value[j]);
        }

        // 5: Spawn new buttons
        resize_Children(children, 0);
        for (size_t j = 0; j < labels->size; j++) {
            child_text_data.text = labels->data[j].text;
            entity target = entities->data[j];

            entity e2 = spawn_button(world, canvas_data, child_parent_data, child_element_data, child_text_data, child_button_data);

            zox_set(e2, ClickEvent, { on_click.value });
            zox_set(e2, EntityTarget, { target });

            add_to_Children(children, e2);
        }

        // 6: Set ListDirty for positioning / hiding etc
        zox_set(list_ui, ListStart, { 0 });
        zox_set(list_ui, ListDirty, { zox_dirty_trigger });

        // 7: Debug
        zox_geter_value(list_ui, ListVisible, byte, visible);
        zox_log("Hierarchy Refreshed");
        zox_log("   - Found: total [%i]", labels->size);
        zox_log("   - Visible [%i]", visible);
        for (size_t j = 0; j < labels->size; j++) {
            zox_log("   + %i: [%s]", j, labels->data[j].text);
        }

        // 8: Dispose of dynamic arrays
        for (size_t j = 0; j < labels->size; j++) {
            free(labels->data[j].text);
        }
        dispose_text_group_dynamic_array_d(labels);
        dispose_entity_array_d(entities);
    }
} zox_sys_end(HierarchySpawnSystem);
