entity spawn_zigel(ecs* world, entity prefab, entity parent, float2 position_anchor, int2 size, int2 texture_size, byte index, byte thickness, byte othickness, color fill, color outline) {
    zox_instance(prefab);
    zox_name("zigel");
    zox_set_parent(world, e, parent);
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, LayoutSize, { size });
    zox_set(e, TextureSize, { texture_size });
    zox_set(e, ZigelIndex, { index });
    zox_set(e, FontThickness, { thickness });
    zox_set(e, FontOutlineThickness, { othickness });
    zox_set(e, Color, { fill });
    zox_set(e, SecondaryColor, { outline });
    zox_set(e, Layer2D, { 3 });
    return e;
}
