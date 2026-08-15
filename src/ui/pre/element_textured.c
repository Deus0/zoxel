entity spawn_prefab_ui_textured(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("ui_textured");
    // frame texture
    zox_add(e, FrameTexture);
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    zox_prefab_set(e, TextureDirty, { 0 });
    zox_prefab_set(e, FrameCorner, { default_button_corner });
    zox_prefab_set(e, OutlineThickness, { default_button_frame_thickness });
    zox_prefab_set(e, FillColor, { default_fill_color });
    zox_prefab_set(e, OutlineColor, { default_outline_color });
    return e;
}
