// This calculcates the matrix from Position2 and Scale2 when Layout is dirty
zox_sys2(Transform2Scale2System) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_in(Position2);
    zox_sys_in(Scale2D);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position2, position);
        zox_sys_i(Scale2D, scale2);
        zox_sys_o(TransformMatrix, matrix);
        matrix->value = float4x4_transform_scale2(float2_to_float3(position->value), quaternion_identity, scale2->value);
        if (dbg_log) {
            zox_sys_world();
            zox_sys_e();
            zox_log("Scaleyo Transform set [%s] [%fx%f]", zox_getn(e), scale2->value.x, scale2->value.y);
        }
    }
} zox_sys_end(Transform2Scale2System);
