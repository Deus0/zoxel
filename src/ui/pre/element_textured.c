entity spawn_prefab_ui_textured(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("ui_textured");
    zox_add(e, FrameTexture);
    zox_setv(e, Seed, 666);
    zox_setv(e, FrameCorner, default_button_corner);
    zox_setv(e, OutlineThickness, default_button_frame_thickness);
    zox_setv(e, FillColor, default_fill_color);
    zox_setv(e, OutlineColor, default_outline_color);
    zox_setv(e, GenerateTexture, zox_generate_texture_run);
    return e;
}
