entity spawn_prefab_element_ready(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("element_ready");
    zox_prefab_set(e, TextureDirty, { 0 });
    zox_prefab_set(e, Color, { color_white });   // do i need this here? does basic lack texture info for generation?
    return e;
}
