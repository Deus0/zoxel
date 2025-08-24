entity spawn_menu_start(
    ecs* world,
    const entity player,    // hmm
    const entity canvas
) {
    const byte font_size = 128;
    const byte layer = 3;

    SpawnButton spawnButton = {
        .canvas = {
            .e = canvas
        },
        .parent = {
            .e = canvas
        },
        .element = {
            .prefab = prefab_button,
            .layer = layer,
            .anchor = float2_half
        },
        .zext = {
            .text = label_start,
            .font_size = font_size,
            .font_thickness = 4,
            .font_fill_color = default_font_fill_color,
            .font_outline_color = default_font_outline_color,
            .padding = (byte2) { 32, 16 },
        },
        .button = {
            .prefab_zext = prefab_zext,
            .fill = button_fill,
            .outline = button_outline,
        }};

    const entity e = spawn_button(
        world,
        spawnButton.canvas,
        spawnButton.parent,
        spawnButton.element,
        spawnButton.zext,
        spawnButton.button
    );
    zox_add_tag(e, MenuStart);
    zox_set(e, ClickEvent, { &button_event_menu_start });
    zox_name("main_start");
    return e;
}