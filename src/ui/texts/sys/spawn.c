//! Dynamically updates zext by spawning/destroying zigels and updating remaining
// #define zoxel_debug_zext_updates

// Children* children,
void spawn_text2D_zigels(ecs* world, entity e, SpawnZigel data, const TextData* tdata, entity canvas) {

    uint new_length = (uint) calculate_total_zigels(tdata->value, tdata->length);
    entity children[texts_max_children];
    uint old_length = zox_get_children(world, e, children, texts_max_children);

    // Spawn New Zigels
    if (new_length > old_length) {
        for (uint i = old_length; i < new_length; i++) {

            byte zigel_index = calculate_zigel_index(tdata->value, tdata->length, i);
            data.zigel.zigel_index = zigel_index;
            data.element.anchor = float2_half;

            entity e2 = spawn_zigel(world, data, canvas);
            zox_set(e2, RenderDisabled, { data.element.render_disabled });
            zox_set_parent(world, e2, e);

            // new_children[i] = e2;
        }
    }
    // Delete Old Zigels
    else if (new_length < old_length) {
        for (uint i = new_length; i < old_length; i++) {
            zox_delete(children[i]);
        }
    }
}

// Dynamically keeps the text characters the right length using entities
zox_sys2(ZigelSpawnSystem) {
#ifdef zox_disable_zigels
    return;
#endif
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
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
    // zox_sys_out(Children);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(TextData, tdata);
        zox_sys_i(TextFontSize, textSize);
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
        // zox_sys_o(Children, children);
        zox_sys_o(RenderDisabled, render_disabled);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        entity canvas = get_root_canvas(world, e);
        if (!zox_valid(canvas)) {
            zox_log_error("no canvas found on text");
            continue;
        }

        int2 canvas_size = zox_get_value(canvas, LayoutSize)
        byte zext_length = calculate_total_zigels(tdata->value, tdata->length);
        if (zox_has(e, ZextRenderEnabler)) {
            render_disabled->value = tdata->length == 0;
        }

        // set parent to or refactor debug ui?
        SpawnZigel cdata = {
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

        // children,
        spawn_text2D_zigels(world, e, cdata, tdata, canvas);
    }
} zox_sys_end(ZigelSpawnSystem);


    /*for (uint j = 0; j < count; j++) {
        entity e2 = children[j];
        set_position_recursive(world, e2, nposition->value, nrotation->value);
    }*/

    /*int old_children_length = children->length;
    int has_old_children = old_children_length > 0;
    int reuse_count = int_min(old_children_length, length);*/

/*#ifdef zoxel_debug_zext_updates
    zox_log("spawn_zext_zigels :: [%i] -> [%i]; reuse [%i];", children->length, tdata.length, reuse_count)
    if (children->length == tdata.length) {
        zox_log("    - zext remained the same [%i]", tdata.length)
    }
#endif*/

    /*entity *old_children = children->value;
    entity *new_children = NULL;
    if (length > 0) {
        new_children = zalloc(length * sizeof(entity));
    }*/
    // old children needs new
    //  - set old positions, as we are resizing
    /*for (int i = 0; i < reuse_count; i++) {
        entity e = old_children[i];
        new_children[i] = e;
    }*/


/*#ifdef zoxel_debug_zext_updates
        zox_log("    - spawning new_children [%i]", length - old_children_length);
#endif*/
/*#ifdef zoxel_debug_zext_updates
        zox_log("    - deleted old_children [%i]", (old_children_length - new_children_length))
#endif*/

    /*if (has_old_children) {
        dispose_Children(children);
    }
    children->value = new_children;
    children->length = new_children_length;*/
