// only used be lines atm
// extern void resize_ui_line2D(ecs*, entity, int2);
extern void anchor_element_position2(int2*, const float2, const int2);

// only used be lines atm
float2 get_ui_real_position2_canvas(int2 local_pixel_position, float2 anchor, float2 canvas_size_f, float aspect_ratio) {

    return (float2) { ((local_pixel_position.x  / canvas_size_f.x) - 0.5f + anchor.x) * aspect_ratio, ((local_pixel_position.y  / canvas_size_f.y) - 0.5f + anchor.y) };
}

float2 get_ui_real_position2_parent(int2 local_pixel_position, float2 anchor, float2 parent_position2, int2 parent_pixel_size, float2 canvas_size_f, float aspect_ratio) {

    float2 parent_pixel_ratio = (float2) { parent_pixel_size.x / canvas_size_f.y, parent_pixel_size.y / canvas_size_f.y };
    float2 position2 = parent_position2;
    float2 add_1 = (float2) { (local_pixel_position.x  / canvas_size_f.x) * aspect_ratio, (local_pixel_position.y  / canvas_size_f.y)};
    float2 add_2 = (float2) { - (parent_pixel_ratio.x / 2.0f) + parent_pixel_ratio.x * anchor.x, - (parent_pixel_ratio.y / 2.0f) + parent_pixel_ratio.y * anchor.y };

    float2_add_float2_p(&position2, add_1);
    float2_add_float2_p(&position2, add_2);

    return position2;
}
