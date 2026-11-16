// in a dialogue, the player turns to face who they talk to (as the npc walks up), and their camera's pitch
//  levels out, so the head's look (bones/sys/head_look.c) is at the speaker's head, not where they clicked
const float dialogue_face_speed = 6;

void dialogue_face_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float blend = float_min(1, zox_delta_time * dialogue_face_speed);
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_dialogue_begin && state->value != zox_player_state_dialogue_active) {
            continue;
        }
        const entity character = zox_get_link(world, e, CharacterLink);
        if (!zox_valid(character) || !zox_has(character, Euler)) {
            continue;
        }
        const entity target = looking_at(world, character);
        if (!zox_valid(target)) {
            continue;
        }
        // the body's yaw, to the speaker (+z ahead, as ai/sys/rotate_towards_system.c)
        const float3 to = float3_sub(zox_getv(target, Position3D), zox_getv(character, Position3D));
        if (to.x * to.x + to.z * to.z > 0.0001f) {
            zox_muter(character, Euler, euler);
            zox_muter(character, Rotation3D, rotation);
            const float error = remainderf(atan2f(to.x, to.z) - euler->value.y, 2.0f * M_PI);
            euler->value.y += error * blend;
            rotation->value = quaternion_from_euler(euler->value);
        }
        // the camera levels, its look left to the head
        const entity skeleton = get_skeleton(world, character);
        const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
        const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
        if (zox_valid(camera)) {
            zox_muter(camera, LocalRotation3D, camera_rotation);
            const float4 level = euler_to_quaternion((float3) { 0, is_camera_positive_z ? M_PI : 0, 0 });
            camera_rotation->value = quaternion_slerp(camera_rotation->value, level, blend);
        }
    }
    zox_sys_on_end();
} zoxd_system(dialogue_face_system);
