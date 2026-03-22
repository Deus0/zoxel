// TODO: HeadBoneLink?
zox_sys2(HeadAnimateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkeletonDirty);
    zox_sys_in(BoneLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SkeletonDirty, state);
        zox_sys_i(BoneLinks, bones);

        if (state->value != zox_dirty_active || bones->length <= 1) {
            continue;
        }

        entity head_bone = bones->value[1];

        if (!zox_valid(head_bone)) {
            zox_log_error("Head bone invalid.");
            continue;
        }

        zox_geter_value(head_bone, LocalPosition3D, float3, position);
        float3 delta = (float3) { 0, randf_range(0.01f, 0.02f), 0 };
        float3 start_position = float3_add(position, delta);

        zox_add_tag(head_bone, HeadBone);
        zox_add_tag(head_bone, OscillatePosition3D);
        zox_set(head_bone, OscillateStartPosition3D, { start_position });
        zox_set(head_bone, OscillateDeltaPosition3D, { delta });

        zox_log("+ Animating head from [%fx%fx%f] delta [%f]", start_position.x, start_position.y, start_position.z, delta.y);
    }
} zox_sys_end(HeadAnimateSystem);
