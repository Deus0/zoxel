entity spawn_prefab_elementbar(ecs *world, entity prefab) {
    zox_prefab_child(prefab)
    zox_prefab_name("elementbar");
    zox_add_tag(e, FixToLayout);
    zox_prefab_set(e, ElementBar, { 1 });
    zox_prefab_set(e, ElementBarSize, { float2_zero });
    // zox_prefab_set(e, LayoutSize, { int2_one });
    // zox_add_tag(e, FillTexture);
    // zox_prefab_set(e, TextureSize, { int2_one });
    // zox_prefab_set(e, FillColor, { back_bar_color });
    return e;
}
