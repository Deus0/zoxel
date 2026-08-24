entity spawn_prefab_glyph(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("glpyh");
    zox_add(e, Zigel);
    zox_add(e, FontTexture);
    // zox_prefab_set(e, ChildIndex, { 0 });
    zox_setv(e, DataIndex, 0); // Position in TextData of parent
    zox_setv(e, ZigelIndex, 0); // Stores the ASCII byte
    zox_setv(e, Seed, 666);
    zox_setv(e, GenerateTexture, zox_generate_texture_run);
    zox_setv(e, FillColor, color_white);
    zox_setv(e, OutlineColor, color_white);
    zox_setv(e, FontThickness, 1);
    zox_setv(e, OutlineThickness, 1);
    // Events
    // zox_prefab_set(e, ZigelDirty, { zox_zigel_dirty_position });
    return e;
}
