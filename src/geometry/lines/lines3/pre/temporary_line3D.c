entity spawn_prefab_temporary_line3D(ecs *world) {
    zox_prefab_child(prefab_line3D)
    zox_prefab_name("temporary_line3D")
    zox_prefab_set(e, DestroyInTime, { 0 })
    return e;
}
