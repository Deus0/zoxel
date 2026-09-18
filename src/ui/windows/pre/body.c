entity spawn_prefab_body(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("body");
    zox_add(e, WindowBody);
    // Texture
    zox_add(e, FixToLayout);
    zox_add(e, FrameTexture);
    zox_setv(e, FrameCorner, window_corner_size);
    zox_setv(e, OutlineThickness, window_outline_size);
    zox_setv(e, FillColor, window_fill);
    zox_setv(e, OutlineColor, window_outline);
    // Selection
    zox_add(e, Selectable);
    zox_setv(e, SelectState, 0);
    // Clicking
    zox_add(e, Clickable);
    zox_setv(e, ClickState, 0);
    zox_setv(e, Clicker, 0);
    zox_setv(e, ClickEvent, NULL);
    zox_setv(e, ClickDisabled, 0);
    return e;
}
