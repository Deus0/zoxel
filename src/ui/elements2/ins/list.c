static inline int2 calculate_header_size(
    byte length,
    byte font_size,
    byte2 padding)
{
    return (int2) {
        length * font_size + padding.x * 2,
        font_size + padding.y * 2
    };
}

// TODO: Set scrollbar visible/invisible based on list count
// TODO: spawn list panel, and scrollbar as children of list entity
entity spawn_list(
    ecs *world,
    LayoutParentData canvas_data,
    LayoutParentData parent_data,
    ElementSpawnData element_data,
    SpawnList list_data,
    byte alignment,
    entity* elements)
{
    byte slider_handle_width = 16 * ui_scale;
    zox_instance(element_data.prefab);
    zox_name("list");
    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);
    zox_setv(e, ListAlignment, alignment);
    zox_setv(e, ListVisible, list_data.visible_count);
    zox_setv(e, ListMargins, list_data.margins);
    zox_setv(e, ListPadding, list_data.padding);
    zox_setv(e, TextPadding, list_data.button_padding);
    // now spawn elements to fit our window
    LayoutParentData child_parent_data = { .e = e };
    for (int i = 0; i < list_data.count; i++) {
        byte visible = (i >= 0 && i < list_data.visible_count);
        SpawnListElement child_data = list_data.elements[i];
        ElementSpawnData child_element_data = {
            .prefab = prefab_button,
            .anchor = float2_half,
            .render_disabled = !visible,
        };
        entity child = 0;
        if (child_data.type == list_element_type_button) {
            child = spawn_button(
                world,
                prefab_button,
                e,
                child_data.text,
                int2_zero,
                int2_zero,
                float2_half,
                zox_alignment_centre,
                list_data.font_size,
                list_data.button_padding,
                button_fill,
                button_outline,
                button_font_fill,
                button_font_outline).x;
            if (child_data.on_click.value) {
                zox_setv(child, ClickEvent, child_data.on_click.value);
            }
            zox_add(child, TextLabel);
            if (child_data.save_path) {
                FolderPath path = { };
                size_t len = strlen(child_data.save_path);
                if (len >= 512) {
                    len = 512 - 1;
                }
                memcpy(path.value, child_data.save_path, len);
                path.value[len] = '\0';
                zox_set_ptr(child, FolderPath, path);
                free(child_data.save_path);
            }
        } else if (child_data.type == list_element_type_slider) {
            // zox_log("Spawning Slider %s v[%i]", child_data.text, visible);
            child_element_data.prefab = prefab_slider;
            child_element_data.size = (int2) {
                element_data.size.x - list_data.margins.x * 2 - list_data.slider_padding,
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
                button_fill,
                button_outline,
                list_data.font_size,
                button_font_fill,
                button_font_outline);
            if (child_data.on_slide.value) {
                zox_setv(e2.y, SlideEvent, child_data.on_slide.value);
            }
            child = e2.x;
        } else if (child_data.type == list_element_type_toggle) {
            // Why so many for a button...
            child_element_data.prefab = prefab_button;
            SpawnTextData child_text_data = {
                .text = child_data.text,
                .font_size = list_data.font_size,
                .margins = list_data.button_padding,
                .font_resolution = list_data.font_size,
                .font_fill_color = button_font_fill,
                .font_outline_color = button_font_outline,
                .font_thickness = button_font_thickness_fill,
                .font_outline_thickness = button_font_thickness_outline,
            };
            SpawnButtonData child_button_data = {
                .prefab_text = prefab_text,
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
                child_data.value);
            if (child_data.on_toggle.value) {
                zox_setv(toggle, ToggleEvent, child_data.on_toggle.value);
            }
            zox_setv(toggle, OptionLabel, child_data.text);
            child = toggle;
        }
        if (child) {
            zox_setv(child, ChildIndex, i);
            zox_set_parent(world, child, e);
        } else {
            zox_loge("No child at [%i] in spawn_list", i);
        }
        if (elements) {
            elements[i] = child;
        }
    }
    return e;
}
