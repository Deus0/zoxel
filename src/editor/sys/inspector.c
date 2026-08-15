void fetch_entity_components(ecs* world, entity_array_d* entitys, entity_array_d* components, byte_array_d* types, text_group_dynamic_array_d* labels, entity target) {
    if (!zox_valid(target)) {
        return;
    }
    // Add name first
    byte_array_d_add(types, zox_type_name);
    entity_array_d_add(components, 0);
    add_entity_to_labels(world, target, labels, entitys, 0);
    // Add parent if exists
    entity_array_d_add(components, 0);
    byte_array_d_add(types, zox_type_parent);
    entity parent = zox_get_parent(world, target);
    // Get parent name if exists
    const char* parent_name = !zox_valid(parent) ? "None" : zox_get_name(parent);
    text_group_dynamic_array_d_add(labels, (text_group_dynamic) { .text = zox_copy_string(parent_name) });
    uint children_length = zox_get_children_count(world, target);
    char children_label[TooltipText_length];
    sprintf(children_label, "children [%i]", children_length);
    entity_array_d_add(components, 0);
    byte_array_d_add(types, zox_type_children);
    text_group_dynamic_array_d_add(labels, (text_group_dynamic) { .text = zox_copy_string(children_label) });
    const ecs_type_t* type = ecs_get_type(world, target);
    for (int i = 0; i < type->count; i++) {
        ecs_id_t component_id = type->array[i];
        // NOTE: Can set this tto null and see button size failing to resize
        char* text = fetch_compoent_label(world, target, component_id);
        byte type = fetch_component_type(world, target, component_id);
        text_group_dynamic_array_d_add(labels, (text_group_dynamic) { text = text });
        entity_array_d_add(entitys, target);
        entity_array_d_add(components, component_id);
        byte_array_d_add(types, type);
    }
}

void button_event_clicked_inspector(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity clicked = event.clicked;
    if (!zox_has(clicked, ComponentTarget) || !zox_has(clicked, EntityTarget) || !zox_has(clicked, ComponentType)) {
        zox_log_error("Clicked [%s] Invalid Components", zox_get_name(clicked));
        return;
    }
    zox_geter_value(clicked, ComponentType, byte, ctype);
    zox_geter_value(clicked, EntityTarget, entity, target);
    zox_geter_value(clicked, ComponentTarget, entity, target_id);
    zox_log("Clicked [%s] type [%i]:[%s]", zox_get_name(target), ctype, zox_valid(target_id) ? zox_get_name(target_id) : "None");
    if (ctype != zox_type_entity || !zox_valid(target_id)) {
        return;
    }
    const EntityTarget* link = zox_get_id(target, target_id);
    zox_log("   -> Linked [%s]", link->value ? zox_get_name(link->value) : "None");
    inspector_select_target(world, player, link->value);
}

static inline color blend_color(color a, color b, float m) {
    return (color) {
        (byte) (a.r * m + (1.0f - m) * b.r),
        (byte) (a.g * m + (1.0f - m) * b.g),
        (byte) (a.b * m + (1.0f - m) * b.b),
        (byte) (a.a * m + (1.0f - m) * b.a) };
}

zox_sys2(InspectorSpawnSystem) {
    const ClickEvent on_click = (ClickEvent) { button_event_clicked_inspector };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InspectorDirty);
    zox_sys_in(EntityTarget);
    zox_sys_in(ScrollviewLink);
    zox_sys_in(FontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InspectorDirty, dirty);
        zox_sys_i(EntityTarget, target);
        zox_sys_i(ScrollviewLink, scrollview);
        zox_sys_i(FontSize, font_size);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
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
        entity list_ui = zox_getv(scrollview->value, ListUILink);
        if (!zox_valid(list_ui)) {
            zox_log_error("Scrollview [%s] Invalid ListUI", zox_get_name(scrollview->value));
            continue;
        }
        if (!zox_has(list_ui, Layer2D) || !zox_has(list_ui, ListVisible)) {
            zox_log_error("List UI [%s] Invalid Components", zox_get_name(list_ui));
            continue;
        }
        // zox_geter_value(list_ui, Layer2D, byte, scrollview_layer);
        // 1: Fetch Target Hierarchy Data
        byte_array_d* types = create_byte_array_d(4);
        entity_array_d* entitys = create_entity_array_d(4);
        entity_array_d* components = create_entity_array_d(4);
        text_group_dynamic_array_d* labels = create_text_group_dynamic_array_d(4);
        fetch_entity_components(world, entitys, components, types, labels, target->value);
        // 3: Initialize Data
        zox_geter_value(list_ui, TextPadding, byte2, text_padding);
        /*ElementSpawnData child_element_data = {
            .prefab = prefab_button,
            .layer = scrollview_layer + 1,
            .anchor = float2_half,
        };*/
        /*SpawnTextData child_text_data = {
            .font_size = font_size->value,
            .font_resolution = font_size->value,
            .margins = text_padding,
            .font_fill_color = editor_color_font,
            .font_thickness = editor_font_thickness,
            .font_outline_color = editor_color_fonto,
            .font_outline_thickness = editor_fonto_thickness
        };*/
        /*SpawnButtonData child_button_data = {
            .prefab_text = prefab_text,
            .fill = editor_window_fill,
            .outline = editor_window_outline,
        };*/
        // LayoutParentData canvas_data = { .e = canvas };
        // LayoutParentData child_parent_data = { .e = list_ui };
        // Delete old list elements
        entity list_children[layouts2_children_capacity];
        uint list_children_length = zox_get_children(world, list_ui, list_children, layouts2_children_capacity);
        for (uint j = 0; j < list_children_length; j++) {
            entity e2 = list_children[j];
            zox_delete(e2);
        }
        // Spawn new buttons
        byte visible = zox_getv(list_ui, ListVisible);
        byte button_layer = zox_getv(list_ui, Layer2D) + 1;
        byte button_text_layer = button_layer + 1;
        for (size_t j = 0; j < labels->size; j++) {
            const char* text = labels->data[j].text;
            entity target = entitys->data[j];
            byte type = types->data[j];
            entity component_id = components->data[j];
            // Color code our element outlines
            color outline = editor_button_outline;
            if (type == zox_type_name || type == zox_type_parent) {
                outline = blend_color(button_outline, color_black, 0.5f);
            } else if (type == zox_type_tag) {
                outline = blend_color(button_outline, color_green, 0.8f);
            } else if (type == zox_type_byte || type == zox_type_byte2 || type == zox_type_byte3) {
                outline = blend_color(button_outline, color_red, 0.5f);
            } else if (type == zox_type_int || type == zox_type_int2 || type == zox_type_int3) {
                outline = blend_color(button_outline, color_blue, 0.5f);
            } else if (type == zox_type_uint || type == zox_type_uint2) {
                outline = blend_color(button_outline, color_green, 0.5f);
            } else if (type == zox_type_float || type == zox_type_float2 || type == zox_type_float3 || type == zox_type_float4) {
                outline = blend_color(button_outline, color_white, 0.5f);
            } else if (type == zox_type_double || type == zox_type_double2 || type == zox_type_double3) {
                outline = blend_color(button_outline, color_white, 0.5f);
            } else if (type == zox_type_lint) {
                outline = blend_color(button_outline, color_white, 0.5f);
            } else if (type == zox_type_color || type == zox_type_color_rgb) {
                outline = blend_color(button_outline, color_yellow, 0.5f);
            }  else if (type == zox_type_entity) {
                outline = blend_color(button_outline, color_purple, 0.5f);
            } else if (type == zox_type_entities) {
                outline = blend_color(button_outline, color_purple, 0.5f);
            }
            // child_element_data.render_disabled = j >= visible;
            // our spawning
            entity2 e2 = spawn_button(world, prefab_button, list_ui, text, int2_zero, int2_zero, float2_half, zox_alignment_left, font_size->value, text_padding, editor_button_fill, outline, editor_color_font, editor_color_fonto);
            // Set tooltips
            char tooltip_text[inspector_component_size_buffer];
            sprintf(tooltip_text, "%s", text);
            set_TooltipText(world, e2.x, tooltip_text);
            zox_set(e2.x, TooltipEvent, { &inspector_label_tooltip });
            zox_add(e2.x, InspectorLabel);
            zox_set(e2.x, ComponentType, { type });
            zox_set(e2.x, EntityTarget, { target });
            zox_set(e2.x, ComponentTarget, { component_id });
            zox_set(e2.x, ClickEvent, { on_click.value });
            zox_set(e2.x, Layer2D, { button_layer });
            zox_set(e2.y, Layer2D, { button_text_layer });
            // zox_set_parent(world, e2, list_ui);
            byte rendered = j < visible;
            zox_set(e2.x, RenderDisabled, { !rendered });
            zox_set(e2.y, RenderDisabled, { !rendered });
        }
        // 6: Set ListDirty for positioning / hiding etc
        zox_set(list_ui, ListStart, { 0 });
        zox_set(list_ui, ListDirty, { zox_dirty_trigger });
        zox_set(list_ui, ListPositionDirty, { zox_dirty_trigger });
        // 7: Debug
        zox_logv("Inspector Refreshed [%i]", list_children_length);
        zox_logv("   - Elements [%i]", labels->size);
        zox_logv("   - Visible [%i]", visible);
        for (size_t j = 0; j < labels->size; j++) {
            zox_logv("   + %i: [%s] type [%i]", j, labels->data[j].text, types->data[j]);
        }
        // 8: Dispose of dynamic arrays
        for (size_t j = 0; j < labels->size; j++) {
            free(labels->data[j].text);
        }
        dispose_text_group_dynamic_array_d(labels);
        dispose_entity_array_d(entitys);
        dispose_byte_array_d(types);
        dispose_entity_array_d(components);
    }
} zox_sys_end(InspectorSpawnSystem);
