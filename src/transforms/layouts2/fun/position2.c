// only used be lines atm
extern void resize_ui_line2D(ecs *world, entity e, int2 canvas_size);
extern void anchor_element_position2(int2 *position, const float2 position_anchor, const int2 window_size);
extern void anchor_element_size2D(int2 *size, const float2 anchor, const int2 parent_size);

// only used be lines atm
float2 get_ui_real_position2_canvas(
    const int2 local_pixel_position,
    const float2 anchor,
    const float2 canvas_size_f,
    const float aspect_ratio
) {
    return (float2) { ((local_pixel_position.x  / canvas_size_f.x) - 0.5f + anchor.x) * aspect_ratio, ((local_pixel_position.y  / canvas_size_f.y) - 0.5f + anchor.y) };
}

float2 get_ui_real_position2_parent(
    const int2 local_pixel_position,
    const float2 anchor,
    const float2 parent_position2,
    const int2 parent_pixel_size,
    const float2 canvas_size_f,
    const float aspect_ratio
) {
    const float2 parent_pixel_ratio = (float2) { parent_pixel_size.x / canvas_size_f.y, parent_pixel_size.y / canvas_size_f.y };
    float2 position2 = parent_position2;
    const float2 add_1 = (float2) { (local_pixel_position.x  / canvas_size_f.x) * aspect_ratio, (local_pixel_position.y  / canvas_size_f.y)};
    const float2 add_2 = (float2) { - (parent_pixel_ratio.x / 2.0f) + parent_pixel_ratio.x * anchor.x, - (parent_pixel_ratio.y / 2.0f) + parent_pixel_ratio.y * anchor.y };
    float2_add_float2_p(&position2, add_1);
    float2_add_float2_p(&position2, add_2);
    return position2;
}

void on_element_parent_updated(
    ecs *world,
    const entity e,
    const int2 local_pixel_position,
    const float2 anchor,
    const float2 parent_position,
    const int2 parent_pixel_size,
    const float2 canvas_size_f
) {
    const float aspect_ratio = canvas_size_f.x / canvas_size_f.y;
    const float2 position2 = get_ui_real_position2_parent(local_pixel_position, anchor, parent_position, parent_pixel_size, canvas_size_f, aspect_ratio);
    const int2 global_pixel_position = (int2) { ceil(((position2.x / aspect_ratio) + 0.5f) * canvas_size_f.x), ((position2.y + 0.5f) * canvas_size_f.y) };
    // zox_set(e, Position2, { position2 })
    // zox_set(e, CanvasPosition, { global_pixel_position })
}

// called by CanvasResizeSystem
void set_ui_transform(
    ecs *world,
    const entity e,
    const int2 canvas_size,
    const int2 parent_position,
    const int2 parent_size
) {
    if (!zox_valid(e)) {
        zox_log_error("invalid ui in set_ui_transform")
        return;
    }

    const float2 canvasSizef = int2_to_float2(canvas_size);

    int2 pixel_size = int2_zero;
    if (zox_has(e, AnchorSize)) {
        const float2 anchor_size = zox_get_value(e, AnchorSize)
        anchor_element_size2D(&pixel_size, anchor_size, parent_size);
        zox_set(e, PixelSize, { pixel_size });
    } else if (zox_has(e, PixelSize)) {
        pixel_size = zox_get_value(e, PixelSize);
    }

    resize_ui_line2D(world, e, canvas_size);
    if (zox_has(e, BoundToCanvas)) {
        const float2 anchor = zox_get_value(e, Anchor);
        set_window_bounds_to_canvas(world, e, canvas_size, pixel_size, anchor);
    }

    limit_element(world, e); // check limited elements - bounded


    // set scale of mesh again
    /*if (!headless && zox_has(e, MeshVertices2D)) {
        zox_muter(e, MeshVertices2D, meshVertices2D)
        const byte mesh_alignment = zox_get_value(e, MeshAlignment)
        const float2 size2D = (float2) {
            pixel_size.x / canvasSizef.y,
            pixel_size.y / canvasSizef.y
        };
        set_mesh_vertices_scale2D(meshVertices2D, get_aligned_mesh2D(mesh_alignment), 4, size2D);
        zox_set(e, MeshDirty, { mesh_state_trigger })
    }*/

    /*if (zox_has(e, PixelPosition)) {
        const float2 anchor = zox_get_value(e, Anchor)
        int2 pixel_position = zox_get_value(e, PixelPosition)
        // todo: make this more widespread, used atm just for game_ui
        const int2 position_in_canvas = get_element_pixel_positionv(parent_position, parent_size, pixel_position, anchor);
        const float2 positionf = get_element_position(position_in_canvas, canvas_size);
        anchor_element_position2(&pixel_position, anchor, pixel_size);
        zox_set(e, Position2, { positionf });
        zox_set(e, CanvasPosition, { position_in_canvas });

        if (zox_has(e, Children)) {
            const Children *children = zox_get(e, Children)
            for (int i = 0; i < children->length; i++) {
                set_ui_transform(world,
                    //e,
                    children->value[i],
                    canvas_size,
                    position_in_canvas,
                    pixel_size);
            }
        }
    }*/
}