entity spawn_zigel(ecs *world, const SpawnZigel *data) {

    zox_instance(prefab_zigel);
    zox_name("zigel");

    zox_set(e, ZigelIndex, { data->zigel.zigel_index });
    zox_set(e, FontThickness, { data->zext.font_thickness });
    zox_set(e, Color, { data->zigel.fill_color });
    zox_set(e, SecondaryColor, { data->zigel.outline_color });
    zox_set(e, FontOutlineThickness, { data->zext.font_outline_thickness });

    initialize_element(world, e, data->parent.e, data->canvas.e, data->element.position, data->element.size, data->element.texture_size, data->element.anchor, data->element.layer);

    return e;
}
