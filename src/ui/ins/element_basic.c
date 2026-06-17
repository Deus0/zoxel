entity spawn_element_basic(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 pixel_size, int2 texture_size, float2 anchor, byte layer, int2 parent_position, int2 parent_size) {
    zox_instance(p);
    zox_name("element_basic");
    initialize_element(world, e, parent, canvas, position, pixel_size, texture_size, anchor, layer);
    return e;
}

entity spawn_element_texture(ecs *world, entity prefab, entity canvas, entity parent, int2 position, int2 size, float2 anchor, byte layer, entity texture) {
    zox_instance(prefab);
    zox_name("element");
    zox_geter_value(texture, TextureSize, int2, tsize);
    zox_geter(texture, TextureData, source_data);
    initialize_element(world, e, parent, canvas, position, size, tsize, anchor, layer);
    zox_set_ptr(e, TextureData, *source_data);
    zox_set(e, TextureDirty, { zox_dirty_trigger });
    zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });
    return e;
}
