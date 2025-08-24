entity spawn_prefab_zext(ecs *world, const entity prefab) {

    zox_prefab_child(prefab);
    zox_prefab_name("zext");
    // hierarchy
    zox_prefab_set(e, ParentLink, { 0 });
    // text
    zox_add_tag(e, Zext);
    zox_add_tag(e, Text2D);
    zox_prefab_set(e, TextDirty, { 0 });
    zox_prefab_set(e, TextData, { 0 });
    zox_prefab_set(e, Children, { 0 });
    zox_prefab_set(e, TextPadding, { byte2_zero });
    // used for font
    zox_prefab_set(e, TextSize, { 0 });
    zox_prefab_set(e, TextResolution, { 0 });
    zox_prefab_set(e, FontOutlineColor, { { 255, 0, 0, 255 }});
    zox_prefab_set(e, FontFillColor, { { 0, 255, 0, 255 }});
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, FontOutlineThickness, { 0 });

    return e;
}

entity spawn_zext(
    ecs *world,
    const SpawnZext *data
) {
    int2 texture_size;
    byte font_resolution;
    if (data->zext.font_resolution) {
        font_resolution = data->zext.font_resolution;
        texture_size = int2_single(data->zext.font_resolution);
    } else {
        font_resolution = data->zext.font_size;
        texture_size = int2_single(data->zext.font_size);
    }

    zox_instance(data->element.prefab);
    // zox_name("zext")
    zox_set(e, RenderDisabled, { data->element.render_disabled });
    zox_set(e, TextSize, { data->zext.font_size });
    zox_set(e, TextResolution, { font_resolution });
    zox_set(e, TextPadding, { data->zext.padding });
    zox_set(e, MeshAlignment, { data->zext.alignment });
    zox_set(e, FontFillColor, { data->zext.font_fill_color });
    zox_set(e, FontOutlineColor, { data->zext.font_outline_color });
    if (data->zext.font_thickness) {
        zox_set(e, FontThickness, { data->zext.font_thickness });
    }
    if (data->zext.font_outline_thickness) {
        zox_set(e, FontOutlineThickness, { data->zext.font_outline_thickness });
    }

    const int zext_data_length = data->zext.text != NULL ? strlen(data->zext.text) : 0;

    TextData text_data = (TextData) { 0 };
    initialize_TextData(&text_data, zext_data_length);
    for (int i = 0; i < text_data.length; i++) {
        text_data.value[i] = convert_ascii(data->zext.text[i]);
    }
    zox_set_ptr(e, TextData, text_data);
    zox_set(e, TextDirty, { zox_dirty_trigger });

    const int2 pixel_size = calculate_zext_size(
        text_data.value,
        text_data.length,
        data->zext.font_size,
        data->zext.padding,
        default_line_padding
    );
    initialize_element(
        world,
        e,
        data->parent.e,
        data->canvas.e,
        data->element.position,
        pixel_size,
        texture_size,
        data->element.anchor,
        data->element.layer,
        float2_zero, // position2,
        int2_zero // element_canvas_position
    );

    /*
    Children children = (Children) { 0 };
    const int zigels_count = calculate_total_zigels(
        text_data.value,
        text_data.length);
    initialize_Children(&children, zigels_count);
    SpawnZigel spawn_data = {
        .canvas = data->canvas,
        .parent = {
            .e = e,
            .size = data->element.size
        },
        .zext = {
            .length = zigels_count,
            .text_padding = data->zext.padding,
            .text_alignment = data->zext.alignment,
            .font_thickness = data->zext.font_thickness,
            .font_outline_thickness = data->zext.font_outline_thickness
        },
        .element = {
            .layer = data->element.layer + 1,
            .size = int2_single(data->zext.font_size),
            .texture_size = int2_single(font_resolution)
        },
        .zigel = {
            .outline_color = data->zext.font_outline_color,
            .fill_color = data->zext.font_fill_color,
        }
    };
    for (int i = 0; i < zigels_count; i++) {
        const int data_index = calculate_zigel_data_index(
            text_data.value,
            text_data.length,
            i);
        const byte zigel_index = calculate_zigel_index(
            text_data.value,
            text_data.length,
            i);
        spawn_data.zigel.data_index = data_index;
        spawn_data.zigel.zigel_index = zigel_index;
        children.value[i] = spawn_zext_zigel(
            world,
            &text_data,
            &spawn_data
        );
        zox_set(children.value[i], RenderDisabled, { data->element.render_disabled });
    }
    zox_set_ptr(e, Children, children);
    */
    return e;
}
