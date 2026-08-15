entity spawn_prefab_bar2(ecs *world, entity prefab) {
    zox_prefab_child(prefab)
    zox_prefab_name("bar2");
    zox_add(e, FixToLayout);
    zox_add(e, Elementbar);
    zox_setv(e, BarLevel, 1);
    // zox_setv(e, ElementBarSize, float2_zero);
    return e;
}
