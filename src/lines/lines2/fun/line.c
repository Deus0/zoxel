int4 get_new_line_position(float2 real_position2, float2 canvas_size_f, float aspect_ratio, int2 parent_position, int4 local_position) {
    int2 new_mid_point = (int2) { ceil((real_position2.x / aspect_ratio + 0.5f) * canvas_size_f.x), ((real_position2.y + 0.5f) * canvas_size_f.y) };
    int2 delta = int2_sub(new_mid_point, parent_position);
    int4 output = local_position;
    int4_add_int2(&output, delta);
    return output;
}

// setting our canvas line points
void set_line_element_real_position2(ecs *world, entity e, float2 positionf, int2 canvas_size, int2 parent_position) {
    /*if (zox_has(e, LinePosition2)) {
        float2 canvas_size_f = int2_to_float2(canvas_size);
        float aspect_ratio = canvas_size_f.x / canvas_size_f.y;
        zox_geter(e, LineLocalPosition2, localPoints)
        int4 points = get_new_line_position(positionf, canvas_size_f, aspect_ratio, parent_position, localPoints->value);
        zox_muter(e, LinePosition2, linePosition2);
        linePosition2->value = points;
    }*/
}

int2 get_line_element_mid_point(ecs *world, entity e) {
    if (zox_has(e, LayoutLinePoints)) {
        const LayoutLinePoints *local_points = zox_get(e, LayoutLinePoints);
        int2 xy_line = local_points->start;
        int2 position = local_points->end;
        position = int2_subtract(position, xy_line);
        int2_divide_int_p(&position, 2);
        int2_add_p(&position, xy_line);
        return position;
    }
    return int2_zero;
}
