entity spawn_zigel_prefab(ecs *world, const entity prefab) {

    zox_prefab_child(prefab);
    zox_prefab_name("zigel");
    // zigel
    zox_add_tag(e, Zigel);
    zox_prefab_set(e, ZigelIndex, { 0 });
    // font texture
    zox_add_tag(e, FixToLayout);
    zox_add_tag(e, FontTexture);
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_generate_texture_trigger });
    zox_prefab_set(e, Color, { color_white });
    zox_prefab_set(e, SecondaryColor, { color_white });
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, FontOutlineThickness, { 2 });

    return e;
}

entity spawn_zigel(ecs *world, const SpawnZigel *data) {

    zox_instance(prefab_zigel);
    zox_name("zigel");
    zox_set(e, ZigelIndex, { data->zigel.zigel_index });
    zox_set(e, FontThickness, { data->zext.font_thickness });
    zox_set(e, Color, { data->zigel.fill_color });
    zox_set(e, SecondaryColor, { data->zigel.outline_color });
    if (data->zext.font_outline_thickness) {
        zox_set(e, FontOutlineThickness, { data->zext.font_outline_thickness });
    }

    initialize_element(
        world,
        e,
        data->parent.e,
        data->canvas.e,
        data->element.position,
        data->element.size,
        data->element.texture_size,
        data->element.anchor,
        data->element.layer,
        float2_zero,
        int2_zero
    );

    return e;
}
