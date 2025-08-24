// for any elements with parent that is canvas
extern int2 get_line_element_mid_point(ecs *world, entity e);
extern void set_line_element_real_position2(ecs *world, entity e, float2 real_position2, int2 canvas_size, int2 pixel_position);

void set_element_position(
    ecs *world,
    const entity e,
    const int2 parent_position,
    const int2 parent_size,
    const int2 canvas_size
) {
    if (!zox_valid(e)) {
        return;
    }
    int2 position = int2_zero;
    if (zox_has(e, LayoutPosition)) {
        position = zox_get_value(e, LayoutPosition)
    } else {
        position = get_line_element_mid_point(world, e);
    }
    float2 anchor = float2_zero;    // should i pass this in from parent?
    if (zox_has(e, Anchor)) {
        anchor = zox_get_value(e, Anchor)
    }

    // calculate pixel and real positions
    const int2 position_in_canvas = get_element_pixel_positionv(parent_position, parent_size, position, anchor);
    const float2 position2 = get_element_position(position_in_canvas, canvas_size);
    if (zox_has(e, Position2)) {
        Position2 *position2_component = zox_get_mut(e, Position2)
        position2_component->value = position2;
        zox_modified(e, Position2);
    }
    if (zox_has(e, CanvasPosition)) {
        CanvasPosition *canvasPosition = zox_get_mut(e, CanvasPosition)
        canvasPosition->value = position_in_canvas;
        zox_modified(e, CanvasPosition);
    }
    set_line_element_real_position2(world, e, position2, canvas_size, position);

    if (zox_has(e, Children)) {
        int2 size = parent_size;
        if (zox_has(e, LayoutSize)) {
            size = zox_get_value(e, LayoutSize)
        }
        const Children *children = zox_get(e, Children)
        if (!children->value) return;
        for (int i = 0; i < children->length; i++) {
            const entity child = children->value[i];
            set_element_position(world, child, position_in_canvas, size, canvas_size);
        }
    }
}


void ElementPositionSystem(iter *it) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(LayoutPosition)
    zox_sys_in(LayoutSize)
    zox_sys_in(ParentLink)
    zox_sys_in(Anchor)
    zox_sys_in(CanvasLink)
    zox_sys_out(Position2)
    zox_sys_out(CanvasPosition)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(LayoutPosition, pixelPosition)
        zox_sys_i(LayoutSize, pixelSize)
        zox_sys_i(ParentLink, parentLink)
        zox_sys_i(Anchor, anchor)
        zox_sys_i(CanvasLink, canvasLink)
        zox_sys_o(Position2, position2)
        zox_sys_o(CanvasPosition, canvasPosition)
        if (!zox_valid(canvasLink->value) || parentLink->value != canvasLink->value) {
            continue;
        }
        const int2 canvas_size = zox_get_value(canvasLink->value, LayoutSize)
        const int2 pixel_position = pixelPosition->value;
        int2 position_in_canvas = get_element_pixel_positionv(
            int2_half(canvas_size),
            canvas_size,
            pixel_position,
            anchor->value
        );
        const float2 positionf = get_element_position(
            position_in_canvas,
            canvas_size
        );
        canvasPosition->value = position_in_canvas;
        position2->value = positionf;
        if (zox_has(e, Children)) {
            zox_geter(e, Children, children)
            for (int j = 0; j < children->length; j++) {
                const entity child = children->value[j];
                set_element_position(world, child, position_in_canvas, pixelSize->value, canvas_size);
            }
        }
    }
} zoxd_system(ElementPositionSystem)
