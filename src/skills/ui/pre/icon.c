extern byte tooltip_event_skill(ecs*, const TooltipEventData*);

entity spawn_prefab_icon_skill(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("icon_skill");
    zox_prefab_set(e, TooltipEvent, { &tooltip_event_skill });
    zox_set(e, IconType, { zox_icon_type_skill });
    return e;
}