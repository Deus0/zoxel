entity spawn_canvas3(ecs *world, entity prefab, float3 position, float scale, int2 size) {
    zox_instance(prefab);
    zox_set_unique_name(e, "canvas3");
    zox_setv(e, Position3D, position);
    zox_setv(e, Scale1, scale);
    //zox_setv(e, Scale3, scale);
    zox_setv(e, LayoutSize, size);
    ui3_root_spawned(e);    // its children 3D this frame, before its components land (deferred: ui/fun/prefab3.c)
    return e;
}
