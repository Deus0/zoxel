// tst: third_person_moves, behind the orbit camera W walks away from it, S towards it, D to its right, A to its left,
//  and the character faces away from it (pure: fun/third_person.c, as player3d_move_system uses them)
//  - the camera at 4 yaws round a target, pitched down at it (camera_look_at, as orbit.c places it)
//  - turned 1 degree at a time all the way round, at 5 pitches: its facing never jumps (a pitched euler flipped
//    half round at some yaws, the walk shook and inverted)
//  - the look (orbit_look_input): the mouse right turns it right, up lowers the camera; a stick (x left) left turns it
//    left, up raises it; both were inverted (2026-11-27, play test)
//  - W walked towards the camera before (2026-11-27, the play test): its yaw came from a pitched euler
//  - run: bash bsh/tst.sh third_person_moves
byte zox_tst_third_person_moves_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_third_person_moves_check(ecs* world) {
    (void) world;
    const float3 target = { 3, 10, -7 };
    for (int i = 0; i < 4; i++) {
        const float yaw = 0.3f + i * M_PI / 2;
        const float3 camera = float3_add(target, (float3) { 6 * sinf(yaw), 2.5f, 6 * cosf(yaw) });
        const float4 rotation = third_person_move_rotation(camera_look_at(camera, target));
        const float3 forward = float3_normalize((float3) { target.x - camera.x, 0, target.z - camera.z });
        const float3 right = { -forward.z, 0, forward.x };     // looking along -z, right is +x
        const float2 sticks[4] = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 } };
        const float3 wanted[4] = { forward, float3_scale(forward, -1), right, float3_scale(right, -1) };
        const char* keys[4] = { "W", "S", "D", "A" };
        for (int k = 0; k < 4; k++) {
            const float3 move = float3_normalize(float4_rotate_float3(rotation, player3_move_local(sticks[k])));
            zox_tst_check(float3_dot(move, wanted[k]) > 0.99f, "camera yaw [%.2f]: [%s] moves [%.2f %.2f %.2f], not [%.2f %.2f %.2f]",
                yaw, keys[k], move.x, move.y, move.z, wanted[k].x, wanted[k].y, wanted[k].z);
        }
        const float3 facing = float4_rotate_float3(rotation, (float3) { 0, 0, is_camera_positive_z ? 1 : -1 });
        zox_tst_check(float3_dot(facing, forward) > 0.99f, "camera yaw [%.2f]: the character faces [%.2f %.2f %.2f], not away from it",
            yaw, facing.x, facing.y, facing.z);
    }
    const float pitches[5] = { 0.05f, 0.35f, 0.8f, 1.2f, -0.25f };
    float worst = 1;
    for (int p = 0; p < 5; p++) {
        float3 last = float3_zero;
        for (int d = 0; d <= 360; d++) {
            const float yaw = d * M_PI / 180;
            const float3 camera = float3_add(target, (float3) { 6 * sinf(yaw) * cosf(pitches[p]), 6 * sinf(pitches[p]), 6 * cosf(yaw) * cosf(pitches[p]) });
            const float3 facing = float4_rotate_float3(third_person_move_rotation(camera_look_at(camera, target)), (float3) { 0, 0, 1 });
            if (d) {
                worst = fminf(worst, float3_dot(facing, last));
            }
            last = facing;
        }
    }
    // the look's signs: yaw + turns it left (round +y), pitch + raises it
    const float2 mouse_right = orbit_look_input((float2) { 10, 0 }, float2_zero, 0);
    const float2 mouse_up = orbit_look_input((float2) { 0, 10 }, float2_zero, 0);
    const float2 stick_left = orbit_look_input(float2_zero, (float2) { 1, 0 }, 0.1f);
    const float2 stick_up = orbit_look_input(float2_zero, (float2) { 0, 1 }, 0.1f);
    zox_tst_check(mouse_right.x < 0 && mouse_up.y < 0, "the mouse turns [%.3f] right, pitches [%.3f] up", mouse_right.x, mouse_up.y);
    zox_tst_check(stick_left.x > 0 && stick_up.y > 0, "a stick turns [%.3f] left, pitches [%.3f] up", stick_left.x, stick_up.y);
    zox_tst_check(worst > 0.99f, "turning the camera a degree turned the character [%.1f] degrees", acosf(fmaxf(-1, worst)) * 180 / M_PI);
}

void zox_tsts_third_person_moves(ecs* world) {
    (void) world;
    zox_tst_add("third_person_moves", zox_tst_third_person_moves_ready, zox_tst_third_person_moves_check, 10);
}
