entity spawn_zigel3(ecs *world, entity prefab, entity parent, uint child_index, byte zigel_index, float3 position, float scale, byte render_disabled, byte thickness, byte outline_thickness, byte resolution, color fill, color outline) {
    zox_instance(prefab);
    zox_set_unique_name(e, "zigel3");
    zox_set_parent(world, e, parent);
    zox_set(e, LocalPosition3D, { position });
    zox_set(e, ChildIndex, { child_index });
    zox_set(e, ZigelIndex, { zigel_index });
    zox_set(e, FontThickness, { thickness });
    zox_set(e, FontOutlineThickness, { outline_thickness });
    zox_set(e, RenderDisabled, { render_disabled });
    zox_set(e, TextureSize, { int2_single(resolution) });
    zox_set(e, FillColor, { fill });
    zox_set(e, OutlineColor, { outline });
    if (scale) {
        zox_set(e, Scale1, { scale });
    }
    return e;
}
