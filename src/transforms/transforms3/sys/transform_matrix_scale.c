zox_sys2(TransformMatrixScaleSystem) {
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Scale1);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Scale1, scale);
        zox_sys_o(TransformMatrix, matrix);
        matrix->value = float4x4_transform_scale(position->value, rotation->value, scale->value);
    }
} zox_sys_end(TransformMatrixScaleSystem);

zox_sys2(TransformMatrixScale3System) {
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Scale3);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Scale3, scale);
        zox_sys_o(TransformMatrix, matrix);
        matrix->value = float4x4_transform_scale3(position->value, rotation->value, scale->value);
    }
} zox_sys_end(TransformMatrixScale3System);

