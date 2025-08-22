entity spawn_taskbar(
    ecs *world,
    const entity prefab,
    const entity canvas,
    const entity parent,
    const byte layer
) {
    byte taskbar_count = hook_taskbars->size;
    float2 anchor = (float2) { 0.5f, 1 };
    const int frame_size = default_frame_size; //  * zox_ui_scale;
    const int icon_size = default_icon_size; // * zox_ui_scale;
    const int padding_x = 4; // * zox_ui_scale;
    const int padding_y = 4; // * zox_ui_scale;
    const int margins = frame_size / 4;
    const int2 canvas_size = zox_get_value(canvas, PixelSize)
    const int2 actionbar_size = (int2) {
        padding_x + (frame_size + padding_x) * taskbar_count + margins * 2,
        frame_size + padding_y * 2
    };
    int2 position = (int2) {
        0,
        -12 - actionbar_size.y / 2
    };
    ElementSpawn data = {
        .canvas = {
            .e = canvas,
            .size = canvas_size
        },
        .parent = {
            .e = parent,
            .position = int2_half(canvas_size),
            .size = canvas_size
        },
        .element = {
            .prefab = prefab,
            .layer = layer,
            .anchor = anchor,
            .position = position,
            .size = actionbar_size
        },
        .texture = {
            .fill_color = window_fill,
            .outline_color = window_outline,
        }
    };

    const entity e = spawn_element(world, &data);
    zox_set_unique_name(e, "taskbar");

    Children children = (Children) { 0 };
    initialize_Children(&children, taskbar_count);

    ElementSpawn spawn_frame_data = {
        .canvas = data.canvas,
        .parent = {
            .e = e,
            .position = position,
            .size = actionbar_size
        },
        .element = {
            .prefab = prefab_frame_taskbar,
            .layer = layer + 1,
            .anchor = float2_half,
            .size = int2_single(frame_size),
        },
        .texture = {
            .fill_color = default_fill_color_frame,
            .outline_color = default_outline_color_frame,
        }
    };

    ElementSpawn spawn_icon_data = {
        .canvas = data.canvas,
        .parent = {
            .size = spawn_frame_data.element.size
        },
        .element = {
            .prefab = prefab_icon,
            .layer = layer + 2,
            .anchor = float2_half,
            .size = int2_single(icon_size)
        },
        .texture = {
            .fill_color = default_fill_color_icon,
            .outline_color = default_outline_color_icon,
        }
    };

    for (int i = 0; i < taskbar_count; i++) {

        spawn_frame_data.element.position = (int2) {
            (int) ((i - (taskbar_count / 2.0f) + 0.5f) * (frame_size + padding_x)),
            0
        };

        const entity frame = spawn_element(world, &spawn_frame_data);
        zox_set_unique_name(frame, "taskbar_frame");

        Children frame_children = (Children) { 0 };
        initialize_Children(&frame_children, 1);
        spawn_icon_data.parent.e = frame;
        spawn_icon_data.parent.position = spawn_frame_data.element.position;
        const entity icon = spawn_element(
            world,
            &spawn_icon_data
        );
        // zox_remove(icon, GenerateTexture);
        zox_set_unique_name(icon, "taskbar_icon");
        frame_children.value[0] = icon;
        zox_set(icon, IconIndex, { i });

        // hook data
        hook_taskbar hook = hook_taskbars->data[i];
        taskbar_set_icons(world, canvas, frame, i);
        zox_set(icon, ClickEvent, { &taskbar_button_click_event });
        zox_prefab_set(icon, TooltipEvent, { &tooltip_event_taskbar_icon });
        char tooltip_text[64];
        sprintf(tooltip_text, "toggles a [%s] game ui", ecs_get_name(world, hook.component_id));
        zox_prefab_set(icon, TooltipText, { text_to_zext(tooltip_text) });
        // texture
        char* icon_texture_name = hook.texture_name;
        clone_texture_to_entity(world, icon, icon_texture_name);
        zox_set_ptr(frame, Children, frame_children);

        // Active State
        if (i == 0) {
            zox_set(frame, ActiveState, { 1 });
            zox_set(frame, ActiveStateDirty, { zox_dirty_trigger });
        }
        children.value[i] = frame;
    }
    zox_set_ptr(e, Children, children);
    return e;
}