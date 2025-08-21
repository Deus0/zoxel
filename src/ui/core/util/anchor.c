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

void anchor_element_size2D(
    int2 *size,
    const float2 anchor,
    const int2 parent_size
) {
    size->x = ceil(parent_size.x * anchor.x);
    size->y = ceil(parent_size.y * anchor.y);
    // zox_log("+ new size [%ix%i] from parent [%ix%i] anchor [%fx%f]", size->x, size->y, parent_size.x, parent_size.y, anchor.x, anchor.y)
}