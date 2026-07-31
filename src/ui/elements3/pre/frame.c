entity spawn_prefab_frame3(ecs *world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("frame3");
    zox_add_tag(e, FrameTexture);
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_run });
    zox_prefab_set(e, FrameCorner, { 1 });
    zox_prefab_set(e, OutlineThickness, { 1 });
    zox_prefab_set(e, FillColor, { default_fill_color });
    zox_prefab_set(e, OutlineColor, { default_outline_color });
    return e;
}
