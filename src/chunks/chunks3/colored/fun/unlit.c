// Simple Colors
void add_voxel_face_colors(color_rgb_array_d* colors, color_rgb voxel_color, byte direction) {

    for (byte a = 0; a < voxel_face_vertices_length; a++) {
        color_rgb vertex_color = voxel_color;

        if (direction == direction_down) {
            color_rgb_multiply_float(&vertex_color, 0.33f);
        } else if (direction == direction_front) {
            color_rgb_multiply_float(&vertex_color, 0.44f);
        } else if (direction == direction_left) {
            color_rgb_multiply_float(&vertex_color, 0.55f);
        } else if (direction == direction_back) {
            color_rgb_multiply_float(&vertex_color, 0.66f);
        } else if (direction == direction_right) {
            color_rgb_multiply_float(&vertex_color, 0.76f);
        }

        add_to_color_rgb_array_d(colors, vertex_color);
    }
}