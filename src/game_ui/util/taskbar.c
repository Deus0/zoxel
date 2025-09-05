// need functions:
//      add_taskbar_button
//      - spawn function, component_type, texture_name, etc

typedef struct {
    byte index;
    byte window_type;
    entity component_id;
    char *texture_name;
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

// set active stat based on ui component id
/*void taskbar_set_icons(
    ecs *world,
    const entity canvas,
    const entity e,
    const int i
) {
    hook_taskbar hook = hook_taskbars->data[i];
    if_has_child_with_id(canvas, hook.component_id) {
        zox_set(e, ActiveState, { 1 });
        zox_set(e, ActiveStateDirty, { zox_dirty_trigger });
    }
}*/

// todo: make tooltip function just return a string
byte tooltip_event_taskbar_icon(
    ecs *world,
    const TooltipEventData *data
) {
    if (!data->triggered || !zox_has(data->triggered, TooltipText)) {
        zox_log("! issue with ui, on tooltip\n")
        return 0;
    }
    zox_geter(data->triggered, TooltipText, tooltip_text);
    char *result = convert_zext_to_text(tooltip_text->value, tooltip_text->length);
    // char *result = "opens a game ui";
    set_entity_text(world, data->tooltip, result);
    free(result);
    return 1;
}

/*for (int i = 0; i < load_shader_functions->size; i++) {
    if (load_shader_functions->data[i].value != NULL) (*load_shader_functions->data[i].value)(world);
 }*/