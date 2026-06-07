entity spawn_prefab_label3D(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("label3D");
    zox_add_tag(e, FillTexture);
    zox_set(e, UITrail, {{ 0, 0.23f, 0 }});
    zox_prefab_set(e, LayoutSize, { int2_one });
    zox_prefab_set(e, TextureSize, { int2_one });
    zox_prefab_set(e, FillColor, { label3D_fill });
    prefab_set_mesh3D_vertices(world, e, square_vertices, 4, statbar_back_mesh_scale);
    return e;
}
