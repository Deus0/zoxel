byte fetch_compoent_label(ecs *world, entity target, entity component, char* buffer, uint length) {

    byte type = zox_type_none;

    if (!zox_valid(target) || !zox_valid(component)) {
        zox_logw("invalid e or c in inspector label");
        return type;
    }

    // int buffer_size = inspector_component_size_buffer;
    int index = 0;
    ecs_id_t id = component & ECS_COMPONENT_MASK;
    index += snprintf(buffer + index, length, "%s", zox_get_name(component));

    #define add_component_label(T)\
        else if (is_component_type_##T(id)) { \
            index = get_type_label_##T(world, target, id, buffer, length, index); \
            type = zox_type_##T; \
        }

    if (is_component_type_byte(id)) {
        index = get_type_label_byte(world, target, id, buffer, length, index);
        type = zox_type_byte;
    }
    add_component_label(byte2)
    add_component_label(byte3)
    add_component_label(int)
    add_component_label(int2)
    add_component_label(int3)
    add_component_label(int4)
    add_component_label(uint)
    add_component_label(uint2)
    add_component_label(float)
    add_component_label(float2)
    add_component_label(float3)
    add_component_label(float4)
    add_component_label(float6)
    add_component_label(double)
    add_component_label(lint)
    add_component_label(entity)
    add_component_label(color)
    add_component_label(color_rgb)
    add_component_label(text)

    else {

        const EcsComponent* component = (EcsComponent*) ecs_get(world, id, EcsComponent);

        uint component_size = component !=  NULL ? component->size : 0;

        if (!component_size) {
            index += snprintf(buffer + index, length - index, " [T]");
            type = zox_type_tag;
        } else {
            index += snprintf(buffer + index, length - index, " [?]");
        }
    }

    return type;
}

void fetch_entity_components(ecs* world, entity_array_d* entity_ids, entity_array_d* component_ids, byte_array_d* types, text_group_dynamic_array_d* labels, entity target) {

    if (!zox_valid(target)) {
        return;
    }

    const uint tlength = inspector_component_size_buffer;

    const ecs_type_t* type = ecs_get_type(world, target);

    // add name first
    add_to_entity_array_d(component_ids, 0);
    add_to_byte_array_d(types, zox_type_name);
    add_entity_to_labels(world, target, labels, entity_ids, 0);

    for (int i = 0; i < type->count; i++) {
        ecs_id_t id = type->array[i];

        entity component_id = 0;
        byte type = zox_type_none;
        char* text = malloc(tlength);

        if (zox_is_override(id)) {

            component_id = id & ECS_COMPONENT_MASK;

            if (!zox_valid(component_id)) {
                snprintf(text, tlength, "Invalid");
            } else {
                type = zox_type_override;
                snprintf(text, tlength, "*[%s]*", zox_get_name(component_id));
            }

        } else if (ECS_HAS_ID_FLAG(id, PAIR)) {

            entity pair1 = ecs_pair_first(world, id);
            entity pair2 = ecs_pair_second(world, id);

            if (!zox_valid(pair1) || !zox_valid(pair2)) {
                snprintf(text, tlength, "[bad pair]");
            } else {
                type = zox_type_pair;
                snprintf(text, tlength, "%s -=- %s", zox_get_name(pair1), zox_get_name(pair2));
            }

        } else {
            component_id = id & ECS_COMPONENT_MASK;

            if (!zox_valid(component_id)) {
                snprintf(text, tlength, "Invalid");
            } else {
                type = fetch_compoent_label(world, target, component_id, text, tlength);
            }
            // snprintf(text, tlength, "%s", zox_get_name(e));
        }

        add_to_text_group_dynamic_array_d(labels, (text_group_dynamic) { text = text });
        add_to_entity_array_d(entity_ids, target);
        add_to_byte_array_d(types, type);
        add_to_entity_array_d(component_ids, component_id);
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
    zox_sys_in(CanvasLink);
    zox_sys_in(EntityTarget);
    zox_sys_in(ScrollviewLink);
    zox_sys_in(ElementFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InspectorDirty, dirty);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(EntityTarget, target);
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
        byte_array_d* types = create_byte_array_d(16);
        entity_array_d* entity_ids = create_entity_array_d(16);
        entity_array_d* component_ids = create_entity_array_d(16);
        text_group_dynamic_array_d* labels = create_text_group_dynamic_array_d(16);
        fetch_entity_components(world, entity_ids, component_ids, types, labels, target->value);

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
            entity target = entity_ids->data[j];
            byte type = types->data[j];
            entity component_id = component_ids->data[j];

            // Color code our element outlines
            child_button_data.outline = button_outline;

            if (type == zox_type_name) {
                child_button_data.outline = blend_color(button_outline, color_black, 0.3f);
            } else if (type == zox_type_tag) {
                child_button_data.outline = blend_color(button_outline, color_cyan, 0.8f);
            } else if (type == zox_type_entity) {
                child_button_data.outline = blend_color(button_outline, color_purple, 0.7f);
            } else if (type == zox_type_byte || type == zox_type_byte2 || type == zox_type_byte3) {
                child_button_data.outline = blend_color(button_outline, color_red, 0.9f);
            } else if (type == zox_type_int || type == zox_type_int2 || type == zox_type_int3) {
                child_button_data.outline = blend_color(button_outline, color_blue, 0.9f);
            } else if (type == zox_type_uint || type == zox_type_uint2) {
                child_button_data.outline = blend_color(button_outline, color_green, 0.6f);
            } else if (type == zox_type_float || type == zox_type_float2 || type == zox_type_float3 || type == zox_type_float4) {
                child_button_data.outline = blend_color(button_outline, color_green, 0.9f);
            } else if (type == zox_type_double || type == zox_type_double2 || type == zox_type_double3) {
                child_button_data.outline = blend_color(button_outline, color_yellow, 0.7f);
            } else if (type == zox_type_lint) {
                child_button_data.outline = blend_color(button_outline, color_white, 0.7f);
            } else if (type == zox_type_color || type == zox_type_color_rgb) {
                child_button_data.outline = blend_color(button_outline, color_yellow, 0.8f);
            }

            child_element_data.render_disabled = j >= visible;
            entity e2 = spawn_button(world, canvas_data, child_parent_data, child_element_data, child_text_data, child_button_data);

            zox_set(e2, ClickEvent, { on_click.value });
            zox_set(e2, ComponentType, { type });
            zox_set(e2, EntityTarget, { target });
            zox_set(e2, ComponentTarget, { component_id });

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
            zox_logv("   + %i: [%s] type [%i]", j, labels->data[j].text, types->data[j]);
        }

        // 8: Dispose of dynamic arrays
        for (size_t j = 0; j < labels->size; j++) {
            free(labels->data[j].text);
        }
        dispose_text_group_dynamic_array_d(labels);
        dispose_entity_array_d(entity_ids);
        dispose_byte_array_d(types);
        dispose_entity_array_d(component_ids);
    }
} zox_sys_end(InspectorSpawnSystem);
