// todo: why i have to reverse  rotation?? for npcs, fix this shit!
zox_sys2(TransformMatrixSystem) {
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_out(TransformMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_o(TransformMatrix, matrix);
        matrix->value = float4x4_transform(position->value, rotation->value);
    }
} zox_sys_end(TransformMatrixSystem);