void calculate_perspective_projection_matrix2(
    float4x4 *matrix,
    float left,
    float right,
    float bottom,
    float top,
    float znear,
    float zfar
) {
    float temp, temp2, temp3, temp4;
    temp = 2.0 * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix->x.x = temp / temp2;
    matrix->x.y = 0.0;
    matrix->x.z = 0.0;
    matrix->x.w = 0.0;
    matrix->y.x = 0.0;
    matrix->y.y = temp / temp3;
    matrix->y.z = 0.0;
    matrix->y.w = 0.0;
    matrix->z.x = (right + left) / temp2;
    matrix->z.y = (top + bottom) / temp3;
    matrix->z.z = (-zfar - znear) / temp4;
    matrix->z.w = -1.0;
    matrix->w.x = 0.0;
    matrix->w.y = 0.0;
    matrix->w.z = (-temp * zfar) / temp4;
    matrix->w.w = 0.0;
}

void calculate_perspective_projection_matrix(
    float4x4 *matrix,
    float aspect_ratio,
    float camera_near_distance,
    float camera_far_distance,
    float fov
) {
    float znear = camera_near_distance;
    float zfar = camera_far_distance;
    float ymax = znear * tanf(fov * M_PI / 360.0);
    float xmax = ymax * aspect_ratio;
    calculate_perspective_projection_matrix2(
        matrix,
        -xmax,
        xmax,
        -ymax, ymax, znear, zfar);
}


// This should only update when either ScreenDimensions or FieldOfView changes
void projection_matrix_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(PixelSize);
    zox_sys_in(FieldOfView);
    zox_sys_in(CameraNearDistance);
    zox_sys_in(CameraFarDistance);
    zox_sys_out(ProjectionMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PixelSize, screenDimensions);
        zox_sys_i(FieldOfView, fieldOfView);
        zox_sys_i(CameraNearDistance, cameraNearDistance);
        zox_sys_i(CameraFarDistance, far);
        zox_sys_o(ProjectionMatrix, projectionMatrix);
        if(screenDimensions->value.y <= 0) {
            continue;
        }
        float aspect_ratio = ((float) screenDimensions->value.x) / ((float) screenDimensions->value.y);
        calculate_perspective_projection_matrix(
            &projectionMatrix->value,
            aspect_ratio,
            cameraNearDistance->value,
            far->value,
            fieldOfView->value);
    }
    zox_sys_on_end();
} zoxd_system(projection_matrix_system);
