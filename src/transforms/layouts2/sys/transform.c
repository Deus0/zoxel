// This calculcates the matrix from Position2 and Scale1 when Layout is dirty
zox_sys2(LayoutTransform2System) {
    zox_sys_begin();
    zox_sys_in(LayoutPositionDirty);
    zox_sys_in(Position2);
    zox_sys_in(Scale1D);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutPositionDirty, dirty);
        zox_sys_i(Position2, position);
        zox_sys_i(Scale1D, scale);
        zox_sys_o(TransformMatrix, matrix);
        // Sets immediately when dirty
        if (dirty->value != zox_dirty_trigger && dirty->value != zox_dirty_active) {
            continue;
        }
        matrix->value = float4x4_transform_scale(float2_to_float3(position->value), quaternion_identity, scale->value);
    }
} zox_sys_end(LayoutTransform2System);
