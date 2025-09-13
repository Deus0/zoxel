// TODO: Spawn basic Window + Speech Text + Confirm Button
entity spawn_dialogue_ui(
    ecs* world,
    const entity prefab,
    const entity player
) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);

    Children window_children = (Children) { 0 };
    SpawnWindow2 window_data = {
        .header_text = "Dialogue",
        .header_font_size = 32,
        .header_padding = (byte2) { 16, 16 },
        .children = &window_children,
    };
    LayoutParentData canvas_data = (LayoutParentData) {
        .e = canvas,
        .size = canvas_size
    };
    ElementSpawnData element_data = {
        .prefab = prefab,
        .size = (int2) { 400, 200 },
        .anchor = float2_half,
    };
    const entity e = spawn_window2(
        world,
        canvas_data,
        canvas_data,
        element_data,
        window_data,
        // (ClickEvent) { &window_taskbar_close_event },
        (ClickEvent) { NULL },
        1
    );

    // add speech text

    SpawnTextData speech_text_data = {
        .text = "Testing",
        .font_size = 16,
        .font_resolution = 32,
        .font_thickness = 4,
        .font_outline_thickness = 2,
        .font_fill_color = header_font_fill,
        .font_outline_color = header_font_outline,
        .margins = window_data.header_padding,
    };
    SpawnZext speech_text_data2 = {
        .canvas = canvas_data,
        .zext = speech_text_data,
        .parent = { .e = e },
        .element = {
            .prefab = prefab_zext,
            .anchor = float2_half,
        },
    };
    const entity zext = spawn_zext(world, &speech_text_data2);
    add_to_Children(&window_children, zext);

    // add confirm button at bottom right
    // Spawn a small button per choice, for max choices, then enable disable them after text finishes animating

    zox_set_ptr(e, Children, window_children);

    return e;
}
