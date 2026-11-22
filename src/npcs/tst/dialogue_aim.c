// tst: dialogue_aim, in a dialogue the player's view aims at the middle of the speaker's head (bones/sys/head_look.c)
//  - an npc held 2.5 from the player, the player looks at it as a dialogue has it (look_at, its head's LookTarget), its body faced to it and its
//    camera level as dialogue_face_system has them
//  - settled: the camera's forward is at the npc's head's middle (look_point), within 2 degrees, its height off
//    under a tenth of the head's
//  - was: the head aimed from its neck (its pivot), the camera over it looked parallel, at the top of the head
//  - run: bash bsh/tst.sh dialogue_aim
byte zox_tst_daim_stage = 0;
double zox_tst_daim_time = 0;
entity zox_tst_daim_npc = 0;
byte zox_tst_daim_placed = 0;
float3 zox_tst_daim_place;
float zox_tst_daim_angle = 180, zox_tst_daim_off = 1, zox_tst_daim_head = 0;

byte zox_tst_dialogue_aim_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    zox_tst_progress(zox_tst_daim_stage * 100 + (int) (zox_current_time - zox_tst_daim_time) * 10);
    if (zox_tst_daim_stage == 0) {
        zox_tst_waiting = "an npc with a head, and the player's first person camera";
        if (!zox_valid(camera)) {
            return 0;
        }
        ecs_iter_t it = ecs_each_id(world, zox_id(Character));
        while (ecs_each_next(&it)) {
            for (int i = 0; i < it.count && !zox_tst_daim_npc; i++) {
                const entity npc = it.entities[i];
                const entity s = get_skeleton(world, npc);
                if (npc != character && !zox_has(npc, Dead) && zox_valid(s) && zox_valid(zox_get_link(world, s, HeadBoneLink))) {
                    zox_tst_daim_npc = npc;
                }
            }
        }
        if (!zox_tst_daim_npc) {
            return 0;
        }
        look_at(world, character, zox_tst_daim_npc);
        zox_tst_daim_stage = 1;
        zox_tst_daim_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the player's head to turn to the npc's";
    // the npc held 2.5 ahead of where the player first stood, as close as a dialogue has them
    if (!zox_tst_daim_placed) {
        zox_tst_daim_place = float3_add(zox_getv(character, Position3D), (float3) { 0.6f, 0, 2.4f });
        zox_tst_daim_placed = 1;
    }
    zox_setv(zox_tst_daim_npc, Position3D, zox_tst_daim_place);
    if (zox_has(zox_tst_daim_npc, Velocity3D)) {
        zox_setv(zox_tst_daim_npc, Velocity3D, float3_zero);
    }
    // the body faced to the npc and the camera level, as dialogue_face_system has it in a dialogue
    const float3 toward = float3_sub(zox_getv(zox_tst_daim_npc, Position3D), zox_getv(character, Position3D));
    if (zox_has(character, Euler) && toward.x * toward.x + toward.z * toward.z > 0.0001f) {
        float3 euler = zox_getv(character, Euler);
        euler.y = atan2f(toward.x, toward.z);
        zox_setv(character, Euler, euler);
        zox_setv(character, Rotation3D, quaternion_from_euler(euler));
    }
    zox_setv(camera, LocalRotation3D, euler_to_quaternion((float3) { 0, is_camera_positive_z ? M_PI : 0, 0 }));
    if (zox_current_time - zox_tst_daim_time < 3) {
        return 0;
    }
    const entity npc_head = zox_get_link(world, get_skeleton(world, zox_tst_daim_npc), HeadBoneLink);
    const float3 eye = zox_getv(camera, Position3D);
    const float3 to = float3_sub(look_point(world, zox_tst_daim_npc), eye);
    const float3 forward = float4_rotate_float3(zox_getv(camera, Rotation3D), (float3) { 0, 0, -1 });
    zox_tst_daim_angle = acosf(float_max(-1, float_min(1, float3_dot(float3_normalize(to), forward)))) * radians_to_degrees;
    // the view's height off the head's middle, at its distance
    const float along = float3_dot(to, forward);
    zox_tst_daim_off = float3_length(float3_sub(to, float3_scale(forward, along)));
    zox_tst_daim_head = zox_getv(npc_head, BoneSize).y * 2;
    zox_tst_daim_stage = 2;
    return 1;
}

void zox_tst_dialogue_aim_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_daim_stage == 2, "stopped at stage [%i]", zox_tst_daim_stage);
    zox_log("[tst] [metric] dialogue_aim: the view [%.2f] degrees off the head's middle, [%.3f] off it, the head [%.3f] tall",
        zox_tst_daim_angle, zox_tst_daim_off, zox_tst_daim_head);
    zox_tst_check(zox_tst_daim_angle < 2, "the view is [%.2f] degrees off the npc's head's middle", zox_tst_daim_angle);
    zox_tst_check(zox_tst_daim_off < zox_tst_daim_head * 0.1f, "the view passes [%.3f] off the head's middle, its height [%.3f]",
        zox_tst_daim_off, zox_tst_daim_head);
}

void zox_tsts_dialogue_aim(ecs* world) {
    (void) world;
    zox_tst_add("dialogue_aim", zox_tst_dialogue_aim_ready, zox_tst_dialogue_aim_check, 60);
}
