// tst: xr_room_scale, walking in the real room moves the head in the world (fun/roam.c), no world needed
//  - the head is the body (the roam) then its place in the room: walked 1m right, it's 1m right of the body
//  - a stick turn turns about the head: it stays put (it swung around the room's centre)
//  - a stick move with the head away from the centre moves the head by the move alone
//  - run: bash bsh/tst.sh xr_room_scale
byte zox_tst_xr_room_ready(ecs* world) {
    return 1;
}

void zox_tst_xr_room_check(ecs* world) {
    free_roam roam = { .position = { 10, 2, -4 }, .euler = { 0, 0.7f, 0 } };
    const float3 room = { 1, 0, 0.5f };     // walked away from the room's centre
    const float3 head = xr_roam_head(&roam, room);
    const float3 expected = float3_add(roam.position, float4_rotate_float3(free_roam_rotation(&roam), room));
    zox_tst_check(float3_distance(head, expected) < 0.0001f, "the head isn't its place in the room on the body");
    zox_tst_check(float3_distance(head, roam.position) > 0.5f, "walking in the room didn't move the head");
    // turns, the stick held right a second
    for (int i = 0; i < 60; i++) {
        xr_roam_step(&roam, float2_zero, (float2) { 1, 0 }, quaternion_identity, room, 1.0f / 60.0f);
    }
    const float drift = float3_distance(xr_roam_head(&roam, room), head);
    zox_log("[tst] xr_room_scale: a turn moved the head [%f]", drift);
    zox_tst_check(drift < 0.001f, "a stick turn moved the head [%f], it turns about the room's centre", drift);
    // moves, forward a second: the head moves as the stick does, as it would at the room's centre
    free_roam centred = roam;
    const float3 before = xr_roam_head(&roam, room);
    const float3 centred_before = xr_roam_head(&centred, float3_zero);
    for (int i = 0; i < 60; i++) {
        xr_roam_step(&roam, (float2) { 0, 1 }, float2_zero, quaternion_identity, room, 1.0f / 60.0f);
        xr_roam_step(&centred, (float2) { 0, 1 }, float2_zero, quaternion_identity, float3_zero, 1.0f / 60.0f);
    }
    const float3 moved = float3_sub(xr_roam_head(&roam, room), before);
    const float3 centred_moved = float3_sub(xr_roam_head(&centred, float3_zero), centred_before);
    zox_tst_check(float3_distance(moved, centred_moved) < 0.001f, "the head moved [%f] off the stick's move",
        float3_distance(moved, centred_moved));
    zox_tst_check(float3_length(moved) > 1, "the stick didn't move the head");
}

void zox_tsts_xr_room(ecs* world) {
    zox_tst_add("xr_room_scale", zox_tst_xr_room_ready, zox_tst_xr_room_check, 30);
}
