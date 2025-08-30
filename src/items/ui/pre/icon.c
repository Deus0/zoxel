entity spawn_prefab_icon_item(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("icon_item");
    zox_set(e, TooltipEvent, { &tooltip_event_item });
    zox_set(e, IconType, { zox_icon_type_item });
    return e;
}