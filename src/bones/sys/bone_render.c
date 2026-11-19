void bone_gizmo_system(iter* it) {
    zox_sys_on_begin();
    if (!render_bones) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(BoneSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(BoneSize, size);
        zox_sys_e();
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent)) {
            continue;
        }
        // float3 sposition = zox_getv(skeleton->value, Position3D);
        float3 pposition = zox_getv(parent, Position3D);
        // zox_log("Rendering Bone");
        // spawn_line3(world, float3_add(sposition, position->value), float3_add(sposition, pposition), 1, 15);
        spawn_line3c(
            world,
            position->value,
            pposition,
            4, 0.01,
            color_rgb_gray);
        // its box about its part's middle, turned with it (the bone sits at its joint)
        const float3 centre = float3_add(position->value, float4_rotate_float3(zox_getv(e, Rotation3D),
            float3_sub(zox_getv(e, BoneCentre), zox_getv(e, BonePosition))));
        spawn_cube_lines(
            world,
            centre,
            float3_scale(size->value, 1),
            2,
            color_rgb_black,
            0.01);
    }
    zox_sys_on_end();
} zoxd_system(bone_gizmo_system);
