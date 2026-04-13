entity spawn_prefab_elementbar2D_front(ecs* world, entity prefab) {

    zox_prefab_child(prefab)
    zox_prefab_name("elementbar2_bar");
    zox_prefab_set(e, LayoutSize, { int2_one });

    zox_add_tag(e, FixToLayout);
    zox_add_tag(e, FillTexture);
    zox_prefab_set(e, Color, { front_bar_color });
    zox_prefab_set(e, TextureSize, { int2_one });
    zox_prefab_set(e, MeshAlignment, { zox_alignment_left });

    return e;
}
