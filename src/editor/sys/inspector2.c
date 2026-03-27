void fetch_entity_components(ecs* world, entity_array_d* ids, text_group_dynamic_array_d* labels, entity target) {

    const uint tlength = inspector_component_size_buffer;

    const ecs_type_t* type = ecs_get_type(world, target);

    // add name first
    add_entity_to_labels(world, target, labels, ids, 0);

    for (int i = 0; i < type->count; i++) {
        ecs_id_t id = type->array[i];

        entity e = 0;
        char* text = malloc(tlength);

        if (zox_is_override(id)) {

            e = id & ECS_COMPONENT_MASK;

            if (!zox_valid(e)) {
                snprintf(text, tlength, "Invalid");
            } else {
                snprintf(text, tlength, "*[%s]*", zox_get_name(e));
            }

        } else if (ECS_HAS_ID_FLAG(id, PAIR)) {

            entity pair1 = ecs_pair_first(world, id);
            entity pair2 = ecs_pair_second(world, id);

            if (!zox_valid(pair1) || !zox_valid(pair2)) {
                snprintf(text, tlength, "[bad pair]");
            } else {
                snprintf(text, tlength, "%s -=- %s", zox_get_name(pair1), zox_get_name(pair2));
            }

        } else {
            e = id & ECS_COMPONENT_MASK;
            if (!zox_valid(e)) {
                snprintf(text, tlength, "Invalid");
            } else {
                get_component_label(world, target, e, text, tlength);
            }
            // snprintf(text, tlength, "%s", zox_get_name(e));
        }

        /*if (!zox_has(e, ZoxName)) {
            snprintf(text, hierarchy_max_line_characters, "%s", zox_get_name(e));
        } else {
            zox_geter(e, ZoxName, zox_name);
            snprintf(text, hierarchy_max_line_characters, "%s", zox_name->value);
        }*/

        add_to_text_group_dynamic_array_d(labels, (text_group_dynamic) { text = text });
        add_to_entity_array_d(ids, e);
    }
}

zox_sys2(InspectorSpawnSystem) {

    const ClickEvent on_click = (ClickEvent) { NULL };
    // &button_event_clicked_hierarchy };

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InspectorDirty);
    zox_sys_in(CanvasLink);
    zox_sys_in(EditorTarget);
    zox_sys_in(ScrollviewLink);
    zox_sys_in(ElementFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InspectorDirty, dirty);
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
        zox_geter_value(list_ui, ListVisible, byte, visible);

        // 1: Fetch Target Hierarchy Data
        entity_array_d* ids = create_entity_array_d(16);
        text_group_dynamic_array_d* labels = create_text_group_dynamic_array_d(16);
        fetch_entity_components(world, ids, labels, target->value);

        // 3: Initialize Data
        zox_geter_value(list_ui, TextPadding, byte2, text_padding);
        ElementSpawnData child_element_data = {
            .prefab = prefab_button,
            .layer = scrollview_layer + 1,
            .anchor = float2_half,
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
            entity target = ids->data[j];

            child_element_data.render_disabled = j >= visible;
            entity e2 = spawn_button(world, canvas_data, child_parent_data, child_element_data, child_text_data, child_button_data);

            zox_set(e2, ClickEvent, { on_click.value });
            zox_set(e2, EntityTarget, { target });

            add_to_Children(children, e2);
        }

        // 6: Set ListDirty for positioning / hiding etc
        zox_set(list_ui, ListStart, { 0 });
        zox_set(list_ui, ListDirty, { zox_dirty_trigger });
        zox_set(list_ui, ListPositionDirty, { zox_dirty_trigger });

        // 7: Debug
        zox_logv("Inspector Refreshed [%i]", children->length);
        zox_logv("   - Elements [%i]", labels->size);
        zox_logv("   - Visible [%i]", visible);
        for (size_t j = 0; j < labels->size; j++) {
            zox_logv("   + %i: [%s]", j, labels->data[j].text);
        }

        // 8: Dispose of dynamic arrays
        for (size_t j = 0; j < labels->size; j++) {
            free(labels->data[j].text);
        }
        dispose_text_group_dynamic_array_d(labels);
        dispose_entity_array_d(ids);
    }
} zox_sys_end(InspectorSpawnSystem);
