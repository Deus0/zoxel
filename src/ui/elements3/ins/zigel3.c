entity spawn_zigel3(ecs *world, Zigel3DData data) {
    if (!zox_valid(data.prefab)) {
        zox_log_error("prefab_zigel3 is invalid")
        return 0;
    }
    zox_instance(data.prefab);
    zox_set_unique_name(e, "zigel3");
    zox_set_parent(world, e, data.parent);
    // zox_set(e, ParentLink, { data.parent });
    zox_set(e, ZigelIndex, { data.zigel_index });
    zox_set(e, FontThickness, { data.font_thickness });
    zox_set(e, FontOutlineThickness, { data.font_outline });
    zox_set(e, LocalPosition3D, { data.position });
    zox_set(e, RenderDisabled, { data.render_disabled });
    zox_set(e, TextureSize, { int2_single(data.resolution) });
    if (!is_color_null(data.fill_color)) {
        zox_set(e, FillColor, { data.fill_color });
    }
    if (!is_color_null(data.outline_color)) {
        zox_set(e, SecondaryColor, { data.outline_color });
    }
    if (data.scale) {
        zox_set(e, Scale1D, { data.scale });
    }
    return e;
}
