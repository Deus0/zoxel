const float4 single_screen_to_canvas = (float4) { 1, 1, 0, 0 };

// rename screen to viewport
int2 screen_to_canvas_size(int2 size, float4 screen_to_canvas) {
    return (int2) { screen_to_canvas.x * size.x, screen_to_canvas.y * size. y };
}

// rename screen to viewport
int2 screen_to_canvas_position(int2 size, float4 screen_to_canvas) {
    return (int2) { screen_to_canvas.z * size.x, screen_to_canvas.w * size. y };
}
