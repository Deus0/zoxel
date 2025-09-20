entity spawn_prefab_icon_quest(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("icon_quest");
    zox_prefab_set(e, TooltipEvent, { &tooltip_event_quest });
    return e;
}
