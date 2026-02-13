byte calculate_list_max_characters(const SpawnList data) {
    byte max_list_characters = 0; // get max text length out of all of the words
    for (byte i = 0; i < data.count; i++) {
        SpawnListElement child_data = data.elements[i];
        byte txt_size = child_data.text ? strlen(child_data.text) : 0;
        if (txt_size > max_list_characters) {
            max_list_characters = txt_size;
        }
    }
    return max_list_characters;
}

static inline int2 calculate_list_size(
    byte max_characters,
    const SpawnList data
) {
    const int list_element_height = data.font_size + data.button_padding.y * 2;
    return (int2) {
        max_characters * data.font_size +
            data.button_padding.x * 2 + data.margins.x * 2,
        data.visible_count * list_element_height +
            ((data.visible_count - 1) * data.padding.y) +
            data.margins.y * 2
    };
}

static inline int2 calculate_header_size(
    byte length,
    byte font_size,
    byte2 padding
) {
    return (int2) {
        length * font_size + padding.x * 2,
        font_size + padding.y * 2
    };
}


// TODO: Set scrollbar visible/invisible based on list count
// TODO: spawn list panel, and scrollbar as children of list entity
entity spawn_list(
    ecs *world,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    const ElementSpawnData element_data,
    const SpawnList list_data
) {
    byte slider_handle_width = 8 * ui_scale;

    zox_instance(element_data.prefab);
    set_element_spawn_data(
        world,
        e,
        canvas_data,
        parent_data,
        element_data
    );
    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }
    zox_name("list");
    zox_set(e, ListVisible, { list_data.visible_count });
    zox_set(e, ListMargins, { list_data.margins });
    zox_set(e, ListPadding, { list_data.padding });
    zox_set(e, Color, { list_data.fill });
    zox_set(e, OutlineColor, { list_data.outline });

    Children children = (Children) { 0 };

    // now spawn elements to fit our window
    LayoutParentData child_parent_data = {
        .e = e,
        .position = element_data.position_in_canvas,
        .size = element_data.size
    };

    for (int i = 0; i < list_data.count; i++) {
        byte visible = (i >= 0 && i < list_data.visible_count);
        SpawnListElement child_data = list_data.elements[i];
        ElementSpawnData child_element_data = {
            .prefab = prefab_button,
            .layer = element_data.layer + 1,
            .anchor = float2_half,
            .render_disabled = !visible,
        };
        entity child = 0;
        if (child_data.type == list_element_type_button) {
            SpawnTextData child_text_data = {
                .text = child_data.text,
                .font_size = list_data.font_size,
                .margins = list_data.button_padding,
                .font_resolution = button_font_resolution,
                .font_fill_color = button_font_fill,
                .font_outline_color = button_font_outline,
                .font_thickness = button_font_thickness_fill,
                .font_outline_thickness = button_font_thickness_outline,
            };
            SpawnButtonData child_button_data = {
                .prefab_zext = prefab_zext,
                .fill = button_fill,
                .outline = button_outline,
            };
            child = spawn_button(
                world,
                canvas_data,
                child_parent_data,
                child_element_data,
                child_text_data,
                child_button_data
            );
            if (child_data.on_click.value) {
                zox_set(child, ClickEvent, { child_data.on_click.value });
            }
            zox_add_tag(child, ZextLabel);
            if (child_data.save_path) {
                SaveGamePath path = { };

                size_t len = strlen(child_data.save_path);
                if (len >= 512) {
                    len = 512 - 1;
                }
                memcpy(path.value, child_data.save_path, len);
                path.value[len] = '\0';

                zox_set_ptr(child, SaveGamePath, path);
                free(child_data.save_path);
            }

        } else if (child_data.type == list_element_type_slider) {
            // zox_log("Spawning Slider %s v[%i]", child_data.text, visible);
            child_element_data.prefab = prefab_slider;
            child_element_data.size = (int2) {
                element_data.size.x - list_data.slider_padding,
                list_data.slider_height
            };
            SpawnSliderData slider_data = (SpawnSliderData) {
                .name = child_data.text,
                .prefab_handle = prefab_handle,
                .type = zox_slider_type_float,
                .value = child_data.value,
                .bounds = child_data.value_bounds,
                .handle_width = slider_handle_width,
            };

            entity2 e2 = spawn_slider(
                world,
                canvas_data,
                child_parent_data,
                child_element_data,
                slider_data,
                list_data.font_size,
                button_font_fill,
                button_font_outline
            );

            if (child_data.on_slide.value) {
                zox_set(e2.y, SlideEvent, { child_data.on_slide.value })
            }
            child = e2.x;

        } else if (child_data.type == list_element_type_toggle) {
            // Why so many for a button...
            SpawnTextData child_text_data = {
                .text = child_data.text,
                .font_size = list_data.font_size,
                .margins = list_data.button_padding,
                .font_resolution = button_font_resolution,
                .font_fill_color = button_font_fill,
                .font_outline_color = button_font_outline,
                .font_thickness = button_font_thickness_fill,
                .font_outline_thickness = button_font_thickness_outline,
            };
            SpawnButtonData child_button_data = {
                .prefab_zext = prefab_zext,
                .fill = button_fill,
                .outline = button_outline,
            };
            entity toggle = spawn_toggle(
                world,
                canvas_data,
                child_parent_data,
                child_element_data,
                child_text_data,
                child_button_data,
                child_data.value
            );
            if (child_data.on_toggle.value) {
                zox_set(toggle, ToggleEvent, { child_data.on_toggle.value });
            }
            zox_set(toggle, OptionLabel, { child_data.text });

            child = toggle;
        }
        add_to_Children(&children, child);
    }
    zox_set_ptr(e, Children, children);
    return e;
}
