entity spawn_prefab_mouse(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("mouse");
    zox_add_tag(e, Mouse);
    zox_prefab_set(e, MouseLock, { 0 });
    return e;
}

entity spawn_prefab_mouse_pointer(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("mouse_pointer");
    zox_add(e, MousePointer);
    zox_setv(e, ZevicePointerRight, 0);
    zox_setv(e  , ZeviceWheel, int2_zero);
    return e;
}