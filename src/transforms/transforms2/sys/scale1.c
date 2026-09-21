// This calculcates the matrix from Position2 and Scale1 when Layout is dirty
zox_sys2(Transform2Scale1System) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position2);
    zox_sys_in(Scale1);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position2, position);
        zox_sys_i(Scale1, scale);
        zox_sys_o(TransformMatrix, matrix);
        matrix->value = float4x4_transform_scale(float2_to_float3(position->value), quaternion_identity, scale->value);
        if (dbg_log) {
            zox_log("Transform2Scale1System [%s]", zox_getn(e));
        }
    }
} zox_sys_end(Transform2Scale1System);

zox_sys2(Transform2Rotation2Scale1System) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position2);
    zox_sys_in(Rotation2);
    zox_sys_in(Scale1);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position2, position);
        zox_sys_i(Rotation2, rotation);
        zox_sys_i(Scale1, scale);
        zox_sys_o(TransformMatrix, matrix);
        matrix->value = float4x4_transform_scale(
            float2_to_float3(position->value),
            rotation2_to_quaternion(rotation->value),
            scale->value);
        if (dbg_log) {
            zox_log("Transform2Rotation2Scale1System [%s] Rot [%f] Scale [%f]",
                zox_getn(e),
                rotation->value,
                scale->value);
        }
    }
} zox_sys_end(Transform2Rotation2Scale1System);
