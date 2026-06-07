// a small square frame ui with an icon in it
// used for game - action/skill/stat - uis
entity spawn_prefab_icon(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("icon");
    zox_add_tag(e, Icon);
    zox_add_tag(e, IconTexture);  // general one
    zox_add_tag(e, Tooltipper);
    zox_prefab_set(e, IconType, { 0 });
    zox_prefab_set(e, IconIndex, { 0 });
    zox_prefab_set(e, IconRadius, { default_icon_radius });
    zox_prefab_set(e, OutlineThickness, { 4 });
    // Select
    zox_add_tag(e, Selectable);
    zox_add_tag(e, SelectedBrighter);
    zox_prefab_set(e, SelectState, { 0 });
    // Click
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    // Colors
    zox_prefab_set(e, FillColor, { default_fill_color_icon });
    zox_prefab_set(e, OutlineColor, { default_outline_color_icon });
    // zox_prefab_set(e, ElementFillColor, { default_fill_color_icon });
    // zox_prefab_set(e, ElementOutlineColor, { default_outline_color_icon });
    // zox_prefab_set(e, SelectedFillColor, { button_fill_selected });
    // zox_prefab_set(e, SelectedOutlineColor, { button_outline_selected });
    return e;
}
