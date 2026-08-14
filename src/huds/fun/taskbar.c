// need functions:
//      add_taskbar_button
//      - spawn function, component_type, texture_name, etc

typedef struct {
    byte index;
    byte window_type;
    entity component_id;
    char *texture_name;
    char *tooltip_text;
    entity (*spawn)(ecs *, const entity);
} hook_taskbar;

zoxel_dynamic_array(hook_taskbar)
hook_taskbar_array_d* hook_taskbars;

void initialize_hook_taskbar() {
    hook_taskbars = create_hook_taskbar_array_d(initial_dynamic_array_size);
}

void dispose_hook_taskbar() {
    dispose_hook_taskbar_array_d(hook_taskbars);
}

void add_taskbar_button(const hook_taskbar data) {
    hook_taskbar_array_d_add(hook_taskbars, data);
}

// todo: make tooltip function just return a string
/*byte tooltip_event_taskbar_icon(ecs *world, const TooltipEventData *data) {
    if (!data->ui || !zox_has(data->ui, TooltipText)) {
        zox_loge("Invalid Tooltip UI");
        return 0;
    }
    zox_geter(data->ui, TooltipText, tooltip_text);
    set_tooltip_text(world, data->ui, data->tooltip, tooltip_text->value);
    return 1;
}*/

// nested function (GCC extension)
void on_closed_taskbar_window(ecs *world, ClickEventData data) {
    entity window = zox_get_parent_by_id(world, data.clicked, zox_id(Window));
    if (!zox_valid(window)) {
        zox_loge("Close button couldn't find window [%s]", zox_get_name(data.clicked));
        return;
    }
    if (!zox_has(window, TaskbarToggleLink)) {
        zox_log_error("Window [%s] Missing [TaskbarToggleLink]", zox_get_name(window));
        return;
    }
    zox_delete(window);
    entity toggle = zox_getv(window, TaskbarToggleLink);
    if (zox_valid(toggle)) {
        zox_set(toggle, ActiveState, { 0 });
        zox_set(toggle, ActiveStateDirty, { zox_dirty_trigger });
    }
}

// NOTE: window has just spawned, we can only use set with it
// TODO: Move to a system for TaskbarWindow or regular window..!
void link_window_to_taskbar(ecs *world, entity window, entity canvas, entity window_id) {
    byte dbg_log = 0;
    if (!zox_valid(window)) {
        zox_loge("[on_spawned_taskbar_window] Invalid Window");
        return;
    }
    // entity canvas = zox_get_parent_by_id(world, window, zox_id(Canvas));
    if (!zox_valid(canvas)) {
        zox_loge("[on_spawned_taskbar_window] Invalid Canvas");
        return;
    }
    entity taskbar = zox_get_child_by_id(world, canvas, zox_id(Taskbar));
    if (!zox_valid(taskbar)) {
        zox_loge("[on_spawned_taskbar_window] Invalid Taskbar");
        return;
    }
    entity taskbar_body = zox_get_child_by_id(world, taskbar, zox_id(WindowBody));
    if (!zox_valid(taskbar_body)) {
        zox_loge("[on_spawned_taskbar_window] Invalid taskbar_body");
        return;
    }
    // get children taskbar icons and see if window has any of them
    entity toggle = 0;
    entity frames[zox_children_capacity];
    uint length = zox_get_children_by_id(world, taskbar_body, frames, zox_children_capacity, zox_id(TaskbarWindowID));
    for (uint j = 0; j < length; j++) {
        entity e2 = frames[j];
        if (!zox_valid(e2)) {
            continue;
        }
        entity id = zox_getv(e2, TaskbarWindowID);
        if (dbg_log) {
            zox_log("Frame [%s] had ID [%s] =? [%s]", zox_get_name(e2), zox_get_name(id), zox_get_name(window_id));
        }
        if (window_id == id) { // zox_has_id(window, id)) {
            toggle = e2;
            break;
        }
    }
    if (toggle) {
        zox_set(window, TaskbarToggleLink, { toggle });
        zox_set(toggle, ActiveState, { 1 });
        zox_set(toggle, ActiveStateDirty, { zox_dirty_trigger });
    }
}
