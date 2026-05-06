zox_sys2(BoneRenderSystem) {
    if (!render_bones) {
        return;
    }

    zox_sys_world();
    zox_sys_begin();
    //zox_sys_in(ParentLink);
    zox_sys_in(Position3D);
    zox_sys_in(BoneSize);
    for (int i = 0; i < it->count; i++) {
        //zox_sys_i(ParentLink, parent);
        zox_sys_i(Position3D, position);
        zox_sys_i(BoneSize, size);

        zox_sys_e();
        entity parent = zox_get_parent(world, e);

        if (!zox_valid(parent)) {
            continue;
        }

        // zox_geter_value(skeleton->value, Position3D, float3, sposition);
        zox_geter_value(parent, Position3D, float3, pposition);

        // zox_log("Rendering Bone");

        // spawn_line3(world, float3_add(sposition, position->value), float3_add(sposition, pposition), 1, 15);

        spawn_line3c(world, position->value, pposition, 4, 0.01, color_rgb_gray);

        spawn_cube_lines(world, position->value, float3_scale(size->value, 1), 2, 0.01, color_rgb_black);

    }
} zox_sys_end(BoneRenderSystem);
