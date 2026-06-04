entity spawn_prefab_crosshair(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_add_tag(e, Crosshair);
    zox_set(e, HitType, { 0 });
    zox_set(e, Color, { empty_color });
    zox_set(e, OutlineColor, { crosshair_air });
    zox_set(e, Scale1D, { crosshair_air_scale });
    zox_set(e, FrameCorner, { crosshair_corner });
    zox_set(e, OutlineThickness, { crosshair_thickness });
    zox_add_tag(e, FixToLayout);
    zox_set(e, Layer2D, { 1 });
    return e;
}
