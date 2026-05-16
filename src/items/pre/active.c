entity spawn_prefab_item_active(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("item_active");
    zox_prefab_set(e, Activate, { 0 });
    zox_prefab_set(e, ActivateBegin, { 0 });
    zox_prefab_addc_user_timings(world, e);
    return e;
}
