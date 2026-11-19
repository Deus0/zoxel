// Names and tags the head bone when the skeleton is updated
//  - its bob is the body's, in step (arm_control.c); a random oscillation of its own bobbed out of step
void head_animate_system(iter* it) {
    zox_sys_on_begin();
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
        zox_add(head_bone, HeadBone);
    }
    zox_sys_on_end();
} zoxd_system(head_animate_system);
