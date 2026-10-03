
entity spawn_marker(
    ecs* world,
    entity parent,
    char* character,
    color fill,
    color outline)
{
    color background = color_null;
    byte resolution = 64;
    float ui_scale = zox_ui_scale3 * 8;
    float trail_offset = name_trail_offset * 2.5f;
    entity2 e2 = spawn_label3(
        world,
        character,
        resolution,
        background,
        background,
        fill,
        outline,
        ui_scale,
        parent,
        trail_offset);
    entity marker = e2.x;
    entity text = e2.y;
    zox_set_unique_name(marker, "marker");
    zox_add(marker, CentredGlyph);
    zox_add(text, CentredGlyph);
    // Links
    zox_link(world, parent, MarkerLink, e2.x);
    zox_setv(marker, ElementHolder, parent);
    // add_to_ElementLinks(elements, marker);
    return marker;
}
