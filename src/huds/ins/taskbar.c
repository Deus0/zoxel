void taskbar_button_click_event(ecs *world, ClickEventData event) {

    if (!zox_has(event.clicked, IconIndex)) {
        zox_log_error("Clicked [%s] does not have IconIndex", zox_get_name(event.clicked));
        return;
    }

    // This IconIndex links a taskbar to a Window
    // TODO: Refactor taskbar and make entities
    zox_geter_value(event.clicked, IconIndex, byte, index);
    if (index >= hook_taskbars->size) {
        zox_log_error("taskbar button index [%i] out of bounds [%zu]", index, hook_taskbars->size);
        return;
    }
    hook_taskbar hook = hook_taskbars->data[index];

    entity window_ui = toggle_ui_with_id(world, *hook.spawn, hook.component_id, event.clicker);
    entity frame = zox_get_parent(world, event.clicked);
    // zox_geter_value(event.clicked, ParentLink, entity, frame);
    if (!zox_valid(frame) || !zox_has(frame, ActiveState)) {
        zox_log_error("Invalid frame.");
        return;
    }

    byte window_state = zox_valid(window_ui);
    zox_set(frame, ActiveState, { window_state });
    zox_set(frame, ActiveStateDirty, { zox_dirty_trigger });

    if (window_ui) {
        zox_set(window_ui, TaskbarToggleLink, { frame });
    }
}

entity spawn_taskbar(ecs *world, entity canvas) {

    byte taskbar_count = hook_taskbars->size;
    int frame_size = (default_frame_size / 4) * ui_scale;
    int icon_size = (default_icon_size / 4) * ui_scale;
    byte2 padding = byte2_single(1 * ui_scale);
    int margins = 2 * ui_scale;
    byte header_font_size = 4 * ui_scale;
    byte2 header_padding = byte2_single(2 * ui_scale);
    int2 body_size = (int2) { padding.x + (frame_size + padding.x) * taskbar_count + margins * 2, frame_size + padding.y * 2 };
    int2 window_size = (int2) { body_size.x, body_size.y + header_font_size + header_padding.y * 2 };
    int2 position = (int2) { 0, - window_size.y / 2 };
    float2 anchor = (float2) { 0.5f, 1 };

    entity e = spawn_window(world, prefab_window_textured, canvas, canvas, zox_window_taskbar, position, window_size, anchor);
    zox_set_unique_name(e, "taskbar");
    zox_add_tag(e, Taskbar);

    Children children = (Children) { 0 };

    entity header = spawn_window_header(world, canvas, e, window_size, "taskbar", header_font_size, header_padding, 2, 1, (ClickEvent) { NULL });
    add_to_Children(&children, header);

    ElementSpawn spawn_frame_data = {
        .canvas = { canvas },
        .parent = {
            .e = e,
            .position = position,
        },
        .element = {
            .prefab = prefab_frame_taskbar,
            .anchor = float2_half,
            .size = int2_single(frame_size),
        },
        .texture = {
            .fill_color = default_fill_color_frame,
            .outline_color = default_outline_color_frame,
        }
    };

    ElementSpawn spawn_icon_data = {
        .canvas = { canvas },
        .parent = {
            .size = spawn_frame_data.element.size
        },
        .element = {
            .prefab = prefab_icon,
            .anchor = float2_half,
            .size = int2_single(icon_size)
        },
        .texture = {
            .fill_color = default_fill_color_icon,
            .outline_color = default_outline_color_icon,
        }
    };

    for (int i = 0; i < taskbar_count; i++) {

        // hook data
        int hook_index = -1;
        for (int j = 0; j < taskbar_count; j++) {
            hook_taskbar new_hook = hook_taskbars->data[j];
            if (new_hook.index == i) {
                hook_index = j;
                break;
            }
        }

        if (hook_index == -1) {
            zox_log_error("hook_index [%i] not found.", i);
            continue;
        }
        hook_taskbar hook = hook_taskbars->data[hook_index];

        spawn_frame_data.element.position = (int2) {
            (int) ((i - (taskbar_count / 2.0f) + 0.5f) * (frame_size + padding.x)),
            0
        };

        entity frame = spawn_element(world, spawn_frame_data);
        zox_set_unique_name(frame, "taskbar_frame");

        Children frame_children = (Children) { 0 };
        initialize_Children(&frame_children, 1);
        spawn_icon_data.parent.e = frame;
        spawn_icon_data.parent.position = spawn_frame_data.element.position;

        // Icon
        entity icon = spawn_element(world, spawn_icon_data);
        zox_set_unique_name(icon, "taskbar_icon");
        frame_children.value[0] = icon;

        entity window = find_child_with_tag2(world, canvas,  hook.component_id);
        if (zox_valid(window)) {
            zox_set(frame, ActiveState, { 1 });
            zox_set(frame, ActiveStateDirty, { zox_dirty_trigger });
            if (window) {
                zox_set(window, TaskbarToggleLink, { frame });
            }
        }

        zox_set(icon, ClickEvent, { &taskbar_button_click_event });
        zox_prefab_set(icon, TooltipEvent, { &tooltip_event_taskbar_icon });
        zox_set(icon, IconIndex, { hook_index });

        // Set Tooltip
        char tooltip_text[64];
        sprintf(tooltip_text, "%s", hook.tooltip_text );
        set_TooltipText(world, icon, tooltip_text);

        // texture
        char* icon_texture_name = hook.texture_name;
        clone_texture_file_to_entity(world, icon, icon_texture_name);

        zox_set_ptr(frame, Children, frame_children);
        // children.value[i] = frame;
        add_to_Children(&children, frame);
    }

    zox_set_ptr(e, Children, children);

    return e;
}
