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

entity spawn_taskbar(ecs* world, entity canvas) {
    byte2 margins = (byte2) { 8 * ui_scale, 6 * ui_scale };
    byte2 padding = byte2_single(4 * ui_scale);
    entity prefab_frame = prefab_frame_selectable;
    byte taskbar_count = hook_taskbars->size;
    int frame_size = (default_frame_size / 4) * ui_scale;
    int icon_size = (default_icon_size / 4) * ui_scale;
    int2 fsize = int2_single(frame_size);
    int2 isize = int2_single(icon_size);
    int2 size = (int2) { padding.x + (frame_size + padding.x) * taskbar_count + margins.x * 2, frame_size + margins.y * 2 };
    int2 position = (int2) { 0, - size.y / 4 };
    float2 anchor = (float2) { 0.5f, 1 };
    byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    entity3 e2 = spawn_window(
        world,
        prefab_window,
        prefab_body,
        "",
        canvas,
        position,
        size,
        anchor,
        header_font_size,
        header_padding,
        NULL);
    entity e = e2.x;
    entity body = e2.z;
    zox_set_unique_name(e, "taskbar");
    zox_add(e, Taskbar);
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
        int2 position = (int2) { (int) ((i - (taskbar_count / 2.0f) + 0.5f) * (frame_size + padding.x)), 0 };
        // entity frame = spawn_element(world, spawn_frame_data);
        entity frame = spawn_uic(
            world,
            prefab_frame,
            body,
            float2_half,
            position,
            fsize,
            fsize,
            default_fill_color_frame,
            default_outline_color_frame);
        zox_set_parent(world, frame, body);
        zox_set_unique_name(frame, "taskbar_frame");
        // A link to the window ID
        zox_set(frame, TaskbarWindowID, { hook.component_id });
        // Icon
        entity icon = spawn_ui(world, prefab_icon, frame, float2_half, int2_zero, isize, isize);
        // entity icon = spawn_element(world, spawn_icon_data);
        zox_set_unique_name(icon, "taskbar_icon");
        zox_set_parent(world, icon, frame);
        entity window = zox_get_child_by_id(world, canvas,  hook.component_id);
        if (zox_valid(window)) {
            zox_set(frame, ActiveState, { 1 });
            zox_set(frame, ActiveStateDirty, { zox_dirty_trigger });
            if (zox_valid(window)) {
                zox_set(window, TaskbarToggleLink, { frame });
            }
        }
        zox_set(icon, ClickEvent, { &taskbar_button_click_event });
        zox_setv(icon, TooltipEvent, &tooltip_text_event);
        zox_set(icon, IconIndex, { hook_index });
        // Set Tooltip
        char tooltip_text[64];
        sprintf(tooltip_text, "%s", hook.tooltip_text );
        set_TooltipText(world, icon, tooltip_text);
        // texture
        char* icon_texture_name = hook.texture_name;
        clone_texture_file_to_entity(world, icon, icon_texture_name);
    }
    return e;
}
