entity spawn_prefab_glyph(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("zigel");
    zox_add(e, Zigel);
    // zox_prefab_set(e, ChildIndex, { 0 });
    zox_prefab_set(e, DataIndex, { 0 }); // Position in TextData of parent
    zox_prefab_set(e, ZigelIndex, { 0 }); // Stores the ASCII byte
    zox_add(e, FontTexture);
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    zox_prefab_set(e, FillColor, { color_white });
    zox_prefab_set(e, OutlineColor, { color_white });
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, FontOutlineThickness, { 2 });
    // Events
    // zox_prefab_set(e, ZigelDirty, { zox_zigel_dirty_position });
    return e;
}
