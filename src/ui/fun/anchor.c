// todo: refactor this extern

void anchor_element_position2_with_header(
    int2 *position,
    const float2 position_anchor,
    const int2 window_size,
    const float header_height
) {
    position->x += (window_size.x / 2.0f) - window_size.x * position_anchor.x;
    position->y += (window_size.y / 2.0f) - (window_size.y + header_height) * position_anchor.y;
}

void reverse_anchor_element_position2_with_header(
    int2 *position,
    const float2 position_anchor,
    const int2 window_size,
    const float header_height
) {
    position->x -= (window_size.x / 2.0f) - window_size.x * position_anchor.x;
    position->y -= (window_size.y / 2.0f) - (window_size.y + header_height) * position_anchor.y;
}