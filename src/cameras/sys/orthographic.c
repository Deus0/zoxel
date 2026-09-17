static inline float4x4 calculate_orthographic_matrix(
    int2 screen_size)
{
    float aspect_ratio = ((float) screen_size.x) / ((float) screen_size.y);
    float half_height = 0.5f;
    float half_width = half_height * aspect_ratio;
    float left = -half_width;
    float right = half_width;
    float bottom = -half_height;
    float top = half_height;
    float znear = -1;
    float zfar = 1;
    float4x4 matrix;
    matrix.x.x = 2.0 / (right - left);
    matrix.x.y = 0.0;
    matrix.x.z = 0.0;
    matrix.x.w = 0.0;
    matrix.y.x = 0.0;
    matrix.y.y = 2.0 / (top - bottom);
    matrix.y.z = 0.0;
    matrix.y.w = 0.0;
    matrix.z.x = 0.0;
    matrix.z.y = 0.0;
    matrix.z.z = -2.0 / (zfar - znear);
    matrix.z.w = 0.0;
    matrix.w.x = -(right + left) / (right - left);
    matrix.w.y = -(top + bottom) / (top - bottom);
    matrix.w.z = -(zfar + znear) / (zfar - znear);
    matrix.w.w = 1.0;
    return matrix;
}

// This should only update when either ScreenDimensions or FieldOfView changes
void orthographic_matrix_system(iter* it) {
    zox_sys_begin();
    zox_sys_in(PixelSize);
    zox_sys_out(ProjectionMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PixelSize, screen);
        zox_sys_o(ProjectionMatrix, matrix);
        if(screen->value.y <= 0) {
            continue;
        }
        matrix->value = calculate_orthographic_matrix(screen->value);
    }
} zoxd_system(orthographic_matrix_system);
