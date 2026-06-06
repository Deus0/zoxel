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
    add_to_hook_taskbar_array_d(hook_taskbars, data);
}

// todo: make tooltip function just return a string
byte tooltip_event_taskbar_icon(ecs *world, const TooltipEventData *data) {

    if (!data->triggered || !zox_has(data->triggered, TooltipText)) {
        zox_log_error("Invalid Tooltip UI");
        return 0;
    }

    zox_geter(data->triggered, TooltipText, tooltip_text);

    set_entity_text(world, data->tooltip, tooltip_text->value);

    return 1;
}

// nested function (GCC extension)
void on_closed_taskbar_window(ecs *world, ClickEventData data) {
    entity window = zox_get_parent_by_id(world, data.clicked, zox_id(Window));
    if (!zox_valid(window)) {
        return;
    }
    if (!zox_has(window, TaskbarToggleLink)) {
        zox_log_error("Window [%s] Missing [TaskbarToggleLink]", zox_get_name(window));
        return;
    }
    zox_geter_value(window, TaskbarToggleLink, entity, button);
    if (zox_valid(button)) {
        zox_set(button, ActiveState, { 0 });
        zox_set(button, ActiveStateDirty, { zox_dirty_trigger });
    }
    zox_delete(window);
}
