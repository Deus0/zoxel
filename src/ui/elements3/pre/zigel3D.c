typedef struct {
    entity prefab;
    entity parent;
    byte zigel_index; // zox character code
    color fill_color;
    color outline_color;
    byte font_thickness;
    byte font_outline_thickness;
    byte font_outline;
    byte resolution;
    float3 position;
    byte render_disabled; // zox character code
    float scale;
} Zigel3DData;

entity spawn_prefab_zigel3D(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("zigel3");
    zox_add_tag(e, Zigel);
    zox_add_tag(e, FontTexture);
    zox_prefab_set(e, ChildIndex, { 0 });
    zox_prefab_set(e, DataIndex, { 0 }); // Position in TextData of parent
    zox_prefab_set(e, ZigelIndex, { 0 }); // Stores the ASCII byte
    zox_prefab_set(e, FillColor, { color_white });
    zox_prefab_set(e, OutlineColor, { color_black });
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, FontOutlineThickness, { 1 });
    // OH this is why scales were weirdaf
    prefab_set_mesh3D_vertices(world, e, square_vertices, 4, zigel3D_size);
    zox_set(e, TextureSize, { int2_single(2) });
    // States
    zox_prefab_set(e, ZigelDirty, { zox_zigel_dirty_position });
    return e;
}
