void taskbar_button_click_event(
    ecs *world,
    ClickEventData event)
{
     /*if (!zox_has(event.clicked, IconIndex)) {
        zox_loge("Clicked [%s] does not have IconIndex",
            zox_getn(event.clicked));
        return;
    }
    // This IconIndex links a taskbar to a Window
    // TODO: Refactor taskbar and make entities
    byte index = zox_getv(event.clicked, IconIndex);
    if (index >= TaskbarDatas->size) {
        zox_loge("taskbar button index [%i] out of bounds [%zu]",
            index,
            TaskbarDatas->size);
        return;
    }*/
    entity frame = zox_get_parent(world, event.clicked);
    if (!zox_valid(frame) ||
        !zox_has(frame, TaskbarWindowID) ||
        !zox_has(frame, SpawnWindowEvent) ||
        !zox_has(frame, ActiveState))
    {
        zox_loge("[taskbar_button_click_event] Invalid [frame]");
        return;
    }
    entity window_id = zox_getv(frame, TaskbarWindowID);
    entity_fun window_fun = zox_getv(frame, SpawnWindowEvent);
    entity window = toggle_ui_with_id(
        world,
        window_fun,
        window_id,
        event.clicker);
    byte window_state = window;
    zox_setv(frame, ActiveState, window_state);
    zox_setv(frame, ActiveStateDirty, zox_dirty_trigger);
    if (window_state) {
        zox_link(world, window_state, Toggle, frame);
    }
}

entity spawn_taskbar(
    ecs* world,
    entity canvas)
{
    byte dbg_log = 0;
    byte taskbar_count = zox_get_children_count(world, taskbar_manager);
    entity prefab_frame = prefab_frame_selectable;
    byte2 margins = (byte2) {
        8 * ui_scale,
        6 * ui_scale
    };
    byte2 padding = byte2_single(4 * ui_scale);
    int frame_size = (default_frame_size / 4) * ui_scale;
    int icon_size = (default_icon_size / 4) * ui_scale;
    int2 fsize = int2_single(frame_size);
    int2 isize = int2_single(icon_size);
    int2 size = (int2) {
        (frame_size + padding.x) * taskbar_count - padding.x + margins.x * 2,
        frame_size + margins.y * 2
    };
    int2 position = (int2) {
        0,
        - size.y / 4
    };
    float2 anchor = (float2) {
        0.5f,
        1
    };
    byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) {
        10 * ui_scale,
        4 * ui_scale
    };
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
    int i = 0;
    entity frames[taskbar_count];
    memset(frames, 0, sizeof(entity) * taskbar_count);
    iter it2 = zox_children(world, taskbar_manager);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_has(e2, TaskbarData)) {
                continue;
            }
            const TaskbarData* data = zox_get(e2, TaskbarData);
            if (!zox_valid(data->component_id) ||
                !data->spawn
            ) {
                zox_loge("[%s]::[%s] TaskbarData Invalid [%i]:[%s]",
                    data->tooltip_text,
                    zox_getn(e2),
                    i,
                    zox_getn(data->component_id));
                i++;
                continue;
            }
            int2 position = (int2) {
                (int) ((i - (taskbar_count / 2.0f) + 0.5f) * (frame_size + padding.x)),
                0
            };
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
            zox_set_unique_name(frame, "taskbar_frame");
            // A link to the window ID
            zox_setv(frame, TaskbarWindowID, data->component_id);
            zox_setv(frame, SpawnWindowEvent, data->spawn);
            // Icon
            entity icon = spawn_ui(
                world,
                prefab_icon,
                frame,
                float2_half,
                int2_zero,
                isize,
                isize);
            zox_set_unique_name(icon, "taskbar_icon");
            // zox_set_parent(world, icon, frame);
            zox_setv(icon, ClickEvent, &taskbar_button_click_event);
            zox_setv(icon, TooltipEvent, &tooltip_text_event);
            // Set Tooltip
            char tooltip_text[64];
            sprintf(tooltip_text, "%s", data->tooltip_text);
            set_TooltipText(world, icon, tooltip_text);
            // texture
            char* icon_texture_name = data->texture_name;
            clone_texture_file_to_entity(
                world,
                icon,
                icon_texture_name);
            // Link to active window
            entity window = zox_get_child_by_id(
                world,
                canvas,
                data->component_id);
            if (zox_valid(window)) {
                zox_setv(frame, ActiveState, 1);
                zox_setv(frame, ActiveStateDirty, zox_dirty_trigger);
                if (zox_valid(window)) {
                    zox_link(world, window, Toggle, frame);
                    // zox_setv(window, TaskbarToggleLink, frame);
                }
            }
            if (frames[data->index]) {
                zox_loge("Overriding Taskbar Frame [%i]",
                    data->index);
            }
            // frames[data->index] = frame;
            i++;
            if (dbg_log) {
                zox_log("TaskbarData [%s]::[%s] - [%i]:[%i] - [%s]",
                    data->tooltip_text,
                    zox_getn(e2),
                    data->index,
                    i,
                    zox_getn(data->component_id));
            }
        }
    }
    /*for (int i = 0; i < taskbar_count; i++) {
        entity frame = frames[i];
        if (!frame) {
            zox_loge("[spawn_taskbar] Frame Invalid [%i]", i);
            continue;
        }
        zox_set_parent(world, frame, body);
    }*/
    return e;
}

// hook data
//int hook_index = -1;
/*for (int j = 0; j < taskbar_count; j++) {
 T askbarData new_hook = TaskbarDatas->data[j];   *
 if (new_hook.index == i) {
     hook_index = j;
     break;
     }
     }
     if (hook_index == -1) {
         zox_log_error("hook_index [%i] not found.", i);
         continue;
         }
         TaskbarData hook = TaskbarDatas->data[hook_index];*/
