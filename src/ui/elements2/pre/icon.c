// a small square frame ui with an icon in it
// used for game - action/skill/stat - uis
entity spawn_prefab_icon(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("icon");
    zox_add(e, Icon);
    zox_add(e, IconTexture);  // general one
    zox_add(e, Tooltipper);
    zox_setv(e, IconType, 0);
    zox_setv(e, IconIndex, 0);
    zox_setv(e, IconRadius, default_icon_radius);
    zox_setv(e, OutlineThickness, 4);
    // Select
    zox_add(e, Selectable);
    zox_add(e, SelectedBrighter);
    zox_setv(e, SelectState, 0);
    // Click
    zox_add(e, Clickable);
    zox_setv(e, ClickState, 0);
    zox_setv(e, Clicker, 0);
    zox_setv(e, ClickEvent, NULL);
    zox_setv(e, ClickDisabled, 0);
    // Colors
    zox_setv(e, FillColor, default_fill_color_icon);
    zox_setv(e, OutlineColor, default_outline_color_icon);
    return e;
}
