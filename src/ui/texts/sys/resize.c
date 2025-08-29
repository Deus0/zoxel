//! Dynamically updates zext by spawning/destroying zigels and updating remaining
// #define zoxel_debug_zext_updates

// For reusing a zigel, set all positions again to position entire text
void set_zigel_position(
    ecs *world,
    const TextData *textData,
    const entity e,
    const int data_index,
    const int font_size,
    const byte text_alignment,
    const byte2 text_padding,
    float2 anchor,
    const int2 parent_position,
    const int2 parent_size,
    const int2 canvas_size
) {
    const int2 pixel_position = calculate_zigel_position(
        textData->value,
        textData->length,
        data_index, font_size,
        text_alignment,
        text_padding,
        default_line_padding
    );
    zox_set(e, LayoutPosition, { pixel_position });
    zox_set(e, LayoutPositionDirty, { zox_dirty_trigger });
}

void spawn_text2D_zigels(
    ecs* world,
    SpawnZigel* data,
    Children* children,
    const TextData* text_data
) {
    const int old_children_length = children->length;
    const int new_children_length = calculate_total_zigels(text_data->value, text_data->length);
    const int has_old_children = old_children_length > 0;
    const int reuse_count = int_min(old_children_length, new_children_length);
#ifdef zoxel_debug_zext_updates
    zox_log("spawn_zext_zigels :: [%i] -> [%i]; reuse [%i];", children->length, text_data->length, reuse_count)
    if (children->length == text_data->length) {
        zox_log("    - zext remained the same [%i]", text_data->length)
    }
#endif
    entity *old_children = children->value;
    entity *new_children = NULL;
    if (new_children_length > 0) {
        new_children = zalloc(new_children_length * sizeof(entity));
    }
    // old children needs new
    //  - set old positions, as we are resizing
    for (int i = 0; i < reuse_count; i++) {
        const entity e = old_children[i];
        const int data_index = calculate_zigel_data_index(text_data->value, text_data->length, i);
        set_zigel_position(
            world,
            text_data,
            e,
            data_index,
            data->element.size.y,
            data->zext.text_alignment,
            data->zext.text_padding,
            data->element.anchor,
            // new_children_length,
            data->parent.position,
            data->parent.size,
            data->canvas.size
        );
        new_children[i] = e;
    }
    // Spawn New Zigels
    if (new_children_length > old_children_length) {
#ifdef zoxel_debug_zext_updates
        zox_log("    - spawning new_children [%i]", new_children_length - old_children_length)
#endif
        for (int i = old_children_length; i < new_children_length; i++) {
            const byte zigel_index = calculate_zigel_index(text_data->value, text_data->length, i);
            const int data_index = calculate_zigel_data_index(text_data->value, text_data->length, i);
            data->zigel.zigel_index = zigel_index;
            data->zigel.data_index = data_index;
            const entity zigel = spawn_zext_zigel(
                world,
                text_data,
                data
            );
            zox_set(zigel, RenderDisabled, { data->element.render_disabled });
            new_children[i] = zigel;
        }
    }
    // Delete Old Zigels
    else if (new_children_length < old_children_length) {
        for (int i = new_children_length; i < old_children_length; i++) {
            zox_delete(old_children[i]);
        }
#ifdef zoxel_debug_zext_updates
        zox_log("    - deleted old_children [%i]", (old_children_length - new_children_length))
#endif
    }
    if (has_old_children) {
        dispose_Children(children);
    }
    children->value = new_children;
    children->length = new_children_length;
}

//! When ui text updates, spawn/destroy font entities
void TextResizeSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextData);
    zox_sys_in(TextSize);
    zox_sys_in(TextPadding);
    zox_sys_in(Layer2D);
    zox_sys_in(CanvasPosition);
    zox_sys_in(LayoutSize);
    zox_sys_in(MeshAlignment);
    zox_sys_in(FontOutlineColor);
    zox_sys_in(FontFillColor);
    zox_sys_in(FontThickness);
    zox_sys_in(FontOutlineThickness);
    zox_sys_in(TextResolution);
    zox_sys_in(TextDirty);
    zox_sys_out(RenderDisabled);
    zox_sys_out(Children);

    for (int i = 0; i < it->count; i++) {

        zox_sys_e();
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(TextData, text_data);
        zox_sys_i(TextSize, textSize);
        zox_sys_i(TextPadding, textPadding);
        zox_sys_i(Layer2D, layer2D);
        zox_sys_i(CanvasPosition, canvasPosition);
        zox_sys_i(LayoutSize, pixelSize);
        zox_sys_i(MeshAlignment, meshAlignment);
        zox_sys_i(FontOutlineColor, fontOutlineColor);
        zox_sys_i(FontFillColor, fontFillColor);
        zox_sys_i(FontThickness, fontThickness);
        zox_sys_i(FontOutlineThickness, fontOutlineThickness);
        zox_sys_i(TextResolution, textResolution);
        zox_sys_o(Children, children);
        zox_sys_o(RenderDisabled, render_disabled);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        const entity canvas = get_root_canvas(world, e);
        if (!zox_valid(canvas)) {
            zox_log_error("no canvas found on text")
            continue;
        }
        const int2 canvas_size = zox_get_value(canvas, LayoutSize)
        const byte zext_length = calculate_total_zigels(text_data->value, text_data->length);
        if (zox_has(e, ZextRenderEnabler)) {
            render_disabled->value = text_data->length == 0;
        }
        // set parent to or refactor debug ui?
        SpawnZigel spawn_data = {
            .canvas = {
                .e = canvas,
                .size = canvas_size
            },
            .parent = {
                .e = e,
                .position = canvasPosition->value,
                .size = pixelSize->value
            },
            .element = {
                .layer = layer2D->value + 1,
                .anchor = float2_half,
                .render_disabled = render_disabled->value,
                .size = int2_single(textSize->value),
                .texture_size = int2_single(textResolution->value),
            },
            .zext = {
                .font_thickness = fontThickness->value,
                .font_outline_thickness = fontOutlineThickness->value,
                .length = zext_length,
                .text_padding = textPadding->value,
                .text_alignment = meshAlignment->value,
            },
            .zigel = {
                .outline_color = fontOutlineColor->value,
                .fill_color = fontFillColor->value
            }
        };
        spawn_text2D_zigels(
            world,
            &spawn_data,
            children,
            text_data
        );
    }
} zoxd_system2(TextResizeSystem);