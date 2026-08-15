entity spawn_prefab_popup3(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("popup3");
    zox_add(e, FillTexture);
    zox_prefab_set(e, LayoutSize, { int2_one });
    zox_prefab_set(e, TextureSize, { int2_one });
    zox_prefab_set(e, DestroyInTime, { 0 });
    return e;
}
