entity spawn_prefab_cloud(ecs* world) {
    zox_prefab_child(prefab_cube);
    zox_prefab_name("cloud");
    zox_add(e, Cloud);
    zox_setv(e, Scale1, 1);
    zox_setv(e, Color, color_white);
    return e;
}
