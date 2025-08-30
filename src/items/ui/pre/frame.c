entity spawn_prefab_frame_item(
    ecs*world,
    const entity prefab
) {
    zox_prefab_child(prefab)
    zox_prefab_name("frame_item");
    zox_add_tag(e, IconLabel);
    return e;
}