entity spawn_element3(ecs *world, entity prefab, entity parent, float3 position) {
    zox_instance(prefab);
    zox_set_unique_name(e, "element3");
    zox_set_parent(world, e, parent);
    zox_set(e, LocalPosition3D, { position });
    return e;
}
