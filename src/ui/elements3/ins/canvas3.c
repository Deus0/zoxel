entity spawn_canvas3(ecs *world, entity prefab, float3 position) {
    zox_instance(prefab);
    zox_set_unique_name(e, "canvas3");
    zox_set(e, Position3D, { position });
    return e;
}
