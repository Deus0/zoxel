// Calculates our camera to world matrix
zox_sys2(ViewProjectionMatrixSystem) {
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(ProjectionMatrix);
    zox_sys_out(ViewProjectionMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TransformMatrix, transform);
        zox_sys_i(ProjectionMatrix, projection);
        zox_sys_o(ViewProjectionMatrix, view_projection);
        const float4x4 view_matrix = float4x4_inverse(transform->value);
        view_projection->value = float4x4_multiply(view_matrix, projection->value);
    }
} zox_sys_end(ViewProjectionMatrixSystem);