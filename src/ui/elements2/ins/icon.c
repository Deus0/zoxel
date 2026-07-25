color icon_overlay_fill = { 191, 107, 6, 144 };
color icon_overlay_outline = { 0, 0, 0, 144 };

entity2 spawn_icon(ecs* world, entity prefab, entity parent, int2 position, int2 size, color fill, color outline, byte index) {
    entity e = spawn_uic(world, prefab, parent, float2_half, position, size, size, fill, outline);
    zox_name("icon");
    zox_set(e, IconIndex, { index });
    // add the overlay
    int2 icon_overlay_position = int2_zero;
    int2 icon_overlay_size = size;
    entity icon_overlay = spawn_uic(world, prefab_element_frame, e, float2_half, icon_overlay_position, icon_overlay_size, icon_overlay_size, icon_overlay_fill, icon_overlay_outline);
    return (entity2) { e, icon_overlay };
}
