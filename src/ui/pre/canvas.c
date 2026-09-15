entity spawn_prefab_canvas(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("canvas");
    zox_add(e, Canvas);
    // Canvas Rendering
    zox_prefab_set(e, ScreenToCanvas, { float4_zero });
    return e;
}
