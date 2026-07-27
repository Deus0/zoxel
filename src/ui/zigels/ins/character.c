entity spawn_zigel(ecs* world, entity prefab, entity parent, float2 position_anchor, int2 size, int2 texture_size, byte thickness, byte othickness, color fill, color outline, byte zigel, ushort child_index, byte layer) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "zigel_%c", convert_to_ascii(zigel));
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, ZigelIndex, { zigel });
    zox_set(e, ChildIndex, { child_index });
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, LayoutSize, { size });
    zox_set(e, TextureSize, { texture_size });
    zox_set(e, FontThickness, { thickness });
    zox_set(e, FontOutlineThickness, { othickness });
    zox_set(e, FillColor, { fill });
    zox_set(e, OutlineColor, { outline });
    zox_set(e, Layer2D, { layer });
    return e;
}
