entity spawn_prefab_mouse(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("mouse");
    zox_add_tag(e, Mouse);
    zox_prefab_set(e, MouseLock, { 0 });
    return e;
}

entity spawn_mouse(ecs *world) {
    zox_instance(prefab_mouse);
    zox_name("mouse");
    entity e2 = spawn_zevice_pointer(world, e, 0, 0);
    zox_set(e2, ZevicePointerRight, { 0 });
    zox_set(e2, ZeviceWheel, { int2_zero });
    zox_set_parent(world, e2, e);
    return e;
}
