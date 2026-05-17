// a small square frame ui with an icon in it
// used for game - action/skill/stat - uis
entity spawn_prefab_icon(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("icon");
    zox_add_tag(e, Icon);
    zox_add_tag(e, IconTexture);  // general one
    zox_prefab_set(e, IconType, { 0 });
    zox_prefab_set(e, IconIndex, { 0 });
    zox_prefab_set(e, Color, { default_fill_color_icon });
    zox_prefab_set(e, OutlineColor, { default_outline_color_icon });
    zox_prefab_set(e, IconRadius, { default_icon_radius });
    zox_prefab_set(e, OutlineThickness, { 4 });
    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { zox_select_state_none });
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    zox_add_tag(e, Tooltipper);
    return e;
}
