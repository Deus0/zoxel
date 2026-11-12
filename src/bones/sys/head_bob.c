// NOTE: Sets Head animation when skeleton is updated
void head_animate_system(iter* it) {
    zox_sys_on_begin();
    float2 headbob_range = (float2) { 0.004f, 0.006f };
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity head_bone = zox_get_link(world, e, HeadBoneLink);
        if (!zox_valid(head_bone)) {
            zox_loge("Head bone invalid.");
            continue;
        }
        zox_set_unique_name(head_bone, "bone_head");
        float3 position = zox_getv(head_bone, LocalPosition3D);
        float3 delta = (float3) {
            0,
            randf_range(headbob_range.x, headbob_range.y),
            0
        };
        float3 start_position = float3_add(position, delta);
        zox_add(head_bone, HeadBone);
        zox_add(head_bone, OscillatePosition3D);
        zox_setv(head_bone, OscillateStartPosition3D, start_position);
        zox_setv(head_bone, OscillateDeltaPosition3D, delta);
        // zox_log("+ Animating head from [%fx%fx%f] delta [%f]", start_position.x, start_position.y, start_position.z, delta.y);
    }
    zox_sys_on_end();
} zoxd_system(head_animate_system);
