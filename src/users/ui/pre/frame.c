entity spawn_prefab_frame_user(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("frame_user");
    zox_prefab_set(e, DataLink, { 0 });
    zox_prefab_add(e, TooltipEvent);
    return e;
}