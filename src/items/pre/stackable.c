entity spawn_prefab_item_stackable(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("item_stackable");
    zox_setv(e, Quantity, 1);
    return e;
}
