entity spawn_prefab_crosshair(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_set(e, HitType, { 0 });
    zox_set(e, Color, { empty_color });
    zox_set(e, OutlineColor, { crosshair_air });
    zox_set(e, FrameCorner, { crosshair_corner });
    zox_set(e, OutlineThickness, { crosshair_thickness });
    return e;
}