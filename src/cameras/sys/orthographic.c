void calculate_orthographic_projection_matrix(float4x4 *matrix, float left, float right, float bottom, float top, float znear, float zfar) {
    matrix->x.x = 2.0 / (right - left);
    matrix->x.y = 0.0;
    matrix->x.z = 0.0;
    matrix->x.w = 0.0;
    matrix->y.x = 0.0;
    matrix->y.y = 2.0 / (top - bottom);
    matrix->y.z = 0.0;
    matrix->y.w = 0.0;
    matrix->z.x = 0.0;
    matrix->z.y = 0.0;
    matrix->z.z = -2.0 / (zfar - znear);
    matrix->z.w = 0.0;
    matrix->w.x = -(right + left) / (right - left);
    matrix->w.y = -(top + bottom) / (top - bottom);
    matrix->w.z = -(zfar + znear) / (zfar - znear);
    matrix->w.w = 1.0;
}

// This should only update when either ScreenDimensions or FieldOfView changes
zox_sys2(OrthographicMatrixSystem) {
    zox_sys_begin();
    zox_sys_in(ScreenDimensions);
    zox_sys_out(ProjectionMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ScreenDimensions, screen);
        zox_sys_o(ProjectionMatrix, matrix);
        if(screen->value.y <= 0) {
            continue;
        }
        float aspect_ratio = ((float) screen->value.x) / ((float) screen->value.y);
        float half_height = 0.5f;
        float half_width = half_height * aspect_ratio;
        calculate_orthographic_projection_matrix(
            &matrix->value,
            -half_width,
            half_width,
            -half_height,
            half_height,
            -1.0f,
            1.0f
        );
    }
} zox_sys_end(OrthographicMatrixSystem);

         /*float zfar = camera_far_distance;
        float znear = near->value;
        float ymax = znear * tanf(fov->value * M_PI / 360.0);
        float xmax = ymax * aspect_ratio;
        ymax = znear * tanf(fov->value * M_PI / 360.0);
        xmax = ymax * aspect_ratio;
        calculate_orthographic_projection_matrix(&matrix->value, -xmax, xmax, -ymax, ymax, znear, zfar);*/

        /*float width = (float) screen->value.x;
        float height = (float) screen->value.y;
        calculate_orthographic_projection_matrix(
            &matrix->value,
            -1.0f,
            1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            1.0f
        );*/