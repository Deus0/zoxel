entity spawn_prefab_quad_lines(ecs *world) {
    zox_prefab();
    zox_prefab_name("quad_lines");
    zox_add(e, QuadLines);
    zox_prefab_set(e, Color, { color_black });
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, LineThickness, { 1 });
    zox_prefab_set(e, QuadLineSize, { 1 });
    zox_prefab_set(e, DestroyInTime, { 0 });
    return e;
}

entity spawn_quad_lines(ecs* world, entity prefab, color c, float3 position, float4 rotation, float thickness, float size, float lifetime) {
    zox_instance(prefab);
    zox_set(e, Position3D, { position });
    zox_set(e, Rotation3D, { rotation });
    zox_set(e, LineThickness, { thickness });
    zox_set(e, QuadLineSize, { size });
    if (!color_equals(c, color_black)) {
        zox_set(e, Color, { c });
    }
    if (lifetime) {
        zox_set(e, DestroyInTime, { lifetime });
    }
    return e;
}
