// The taskbar: a frame a ui (TaskbarData, taskbar_manager), its icon toggles it; a frame's ActiveState is its ui open
//  - 3D huds (zox_huds3D): the same frames on a canvas3 row above the menus (spawn_taskbar3), its panel the body
//  - LB and RB (a headset's grips) step through its uis, one open at a time (huds/sys/navigator.c)

// toggles a taskbar frame's ui for the player, returns it if opened
entity taskbar_frame_toggle(
    ecs* world,
    entity frame,
    entity player)
{
    if (!zox_valid(frame) ||
        !zox_has(frame, TaskbarWindowID) ||
        !zox_has(frame, SpawnWindowEvent) ||
        !zox_has(frame, ActiveState))
    {
        zox_loge("[taskbar_frame_toggle] Invalid [frame]");
        return 0;
    }
    entity window_id = zox_getv(frame, TaskbarWindowID);
    entity_fun window_fun = zox_getv(frame, SpawnWindowEvent);
    entity window = toggle_ui_with_id(
        world,
        window_fun,
        window_id,
        player);
    byte window_state = zox_valid(window);
    zox_setv(frame, ActiveState, window_state);
    zox_setv(frame, ActiveStateDirty, zox_dirty_trigger);
    if (window_state) {
        zox_link(world, window, ToggleLink, frame);
    }
    return window;
}

void taskbar_button_click_event(
    ecs *world,
    ClickEventData event)
{
    taskbar_frame_toggle(world, zox_get_parent(world, event.clicked), event.clicker);
}

// the taskbar's frames on body, a ui each, in a row (2D or 3D: the prefabs)
static void taskbar_spawn_frames(
    ecs* world,
    entity canvas,
    entity body,
    entity prefab_frame,
    entity prefab_icon_,
    int frame_size,
    int icon_size,
    int padding,
    byte taskbar_count)
{
    byte dbg_log = 0;
    const int2 fsize = int2_single(frame_size);
    const int2 isize = int2_single(icon_size);
    int i = 0;
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
                (int) ((i - (taskbar_count / 2.0f) + 0.5f) * (frame_size + padding)),
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
            zox_setv(frame, ChildIndex, i);     // its place in the row, LB and RB step by it
            // A link to the window ID
            zox_setv(frame, TaskbarWindowID, data->component_id);
            zox_setv(frame, SpawnWindowEvent, data->spawn);
            // Icon
            entity icon = spawn_ui(
                world,
                prefab_icon_,
                frame,
                float2_half,
                int2_zero,
                isize,
                isize);
            zox_set_unique_name(icon, "taskbar_icon");
            zox_setv(icon, ClickEvent, &taskbar_button_click_event);
            if (zox_has(icon, TooltipEvent)) {
                zox_setv(icon, TooltipEvent, &tooltip_text_event);
                char tooltip_text[64];
                sprintf(tooltip_text, "%s", data->tooltip_text);
                set_TooltipText(world, icon, tooltip_text);
            }
            clone_texture_file_to_entity(
                world,
                icon,
                data->texture_name);
            // Link to active window
            entity window = zox_get_child_by_id(
                world,
                canvas,
                data->component_id);
            if (zox_valid(window)) {
                zox_setv(frame, ActiveState, 1);
                zox_setv(frame, ActiveStateDirty, zox_dirty_trigger);
                zox_link(world, window, ToggleLink, frame);
            }
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
}

// 3D huds: the taskbar a row of frames on a canvas3 above the menus (huds3_pose), a child of the canvas as the 2D
//  one is (found and closed the same), its panel the body (WindowBody: its frames found as the 2D one's)
#define taskbar3_height 0.09f       // metres
#define taskbar3_above 0.36f        // metres above the menus' middle

entity spawn_taskbar3(
    ecs* world,
    entity canvas,
    entity player)
{
    const byte taskbar_count = zox_get_children_count(world, taskbar_manager);
    const int frame_size = default_frame_size / 4;
    const int icon_size = default_icon_size / 4;
    const int padding = 4;
    const int margin = 6;
    const int2 size = {
        (frame_size + padding) * taskbar_count - padding + margin * 2,
        frame_size + margin * 2
    };
    float3 position;
    float4 rotation;
    huds3_pose(world, player, huds3_distance, &position, &rotation);
    position = float3_add(position, float4_rotate_float3(rotation, (float3) { 0, taskbar3_above, 0 }));
    entity e = spawn_canvas3(world, prefab_canvas3, position, taskbar3_height, size);
    zox_set_unique_name(e, "taskbar");
    zox_setv(e, Rotation3D, rotation);
    zox_add(e, Taskbar);
    zox_set_parent(world, e, canvas);
    entity body = spawn_uic(world, prefab_frame3, e, float2_centre, int2_zero, size, size, window_fill, window_outline);
    zox_set_unique_name(body, "taskbar_body");
    zox_add(body, WindowBody);
    taskbar_spawn_frames(world, canvas, body, prefab_taskbar_frame3, prefab_taskbar_icon3,
        frame_size, icon_size, padding, taskbar_count);
    return e;
}

entity spawn_taskbar(
    ecs* world,
    entity canvas)
{
    byte taskbar_count = zox_get_children_count(world, taskbar_manager);
    entity prefab_frame = prefab_frame_selectable;
    byte2 margins = (byte2) {
        8 * ui_scale,
        6 * ui_scale
    };
    byte2 padding = byte2_single(4 * ui_scale);
    int frame_size = (default_frame_size / 4) * ui_scale;
    int icon_size = (default_icon_size / 4) * ui_scale;
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
    taskbar_spawn_frames(world, canvas, body, prefab_frame, prefab_icon, frame_size, icon_size, padding.x, taskbar_count);
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
         zox_loge("hook_index [%i] not found.", i);
         continue;
         }
         TaskbarData hook = TaskbarDatas->data[hook_index];*/
