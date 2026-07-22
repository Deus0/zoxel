void calculate_perspective_projection_matrix2(float4x4 *matrix,
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

void calculate_perspective_projection_matrix(float4x4 *matrix,
    const float aspect_ratio,
    const float camera_near_distance,
    const float camera_far_distance,
    const float fov
) {
    const float znear = camera_near_distance;
    const float zfar = camera_far_distance;
    const float ymax = znear * tanf(fov * M_PI / 360.0);
    const float xmax = ymax * aspect_ratio;
    calculate_perspective_projection_matrix2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}


// This should only update when either ScreenDimensions or FieldOfView changes
zox_sys2(ProjectionMatrixSystem) {
    zox_sys_begin();
    zox_sys_in(ScreenDimensions);
    zox_sys_in(FieldOfView);
    zox_sys_in(CameraNearDistance);
    zox_sys_out(ProjectionMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ScreenDimensions, screenDimensions);
        zox_sys_i(FieldOfView, fieldOfView);
        zox_sys_i(CameraNearDistance, cameraNearDistance);
        zox_sys_o(ProjectionMatrix, projectionMatrix);
        if(screenDimensions->value.y <= 0) {
            continue;
        }
        float aspect_ratio = ((float) screenDimensions->value.x) / ((float) screenDimensions->value.y);
        calculate_perspective_projection_matrix(&projectionMatrix->value, aspect_ratio, cameraNearDistance->value, camera_far_distance, fieldOfView->value);
    }
} zox_sys_end(ProjectionMatrixSystem);
