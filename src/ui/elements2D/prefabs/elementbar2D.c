entity spawn_prefab_elementbar2D(ecs *world, entity prefab) {

    zox_prefab_child(prefab)
    zox_prefab_name("elementbar2D");
    zox_add_tag(e, FillTexture);
    zox_add_tag(e, Elementbar2);
    // add_ui_plus_components(world, e);
    zox_prefab_set(e, Color, { back_bar_color });
    zox_prefab_set(e, LayoutSize, { int2_one });
    zox_prefab_set(e, TextureSize, { int2_one });
    zox_prefab_set(e, ElementBar, { 1 });
    zox_prefab_set(e, ElementBarSize, { float2_zero });
    zox_prefab_set(e, Children, { 0, NULL });

    return e;
}