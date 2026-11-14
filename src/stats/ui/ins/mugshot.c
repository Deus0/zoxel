// a character's mug shot: just its head, rendered into a frame on parent (its id window)
//  - a copy of its skeleton mesh far below the world, a spot of its own, a camera in front of its head (its head bone,
//    the face is +z) close enough to frame just the head; the copy goes with the frame
entity spawn_character_mugshot(ecs* world, entity parent, entity character, float2 anchor, int2 position, int2 size) {
    const entity skeleton = get_skeleton(world, character);
    const entity source = zox_valid(skeleton) ? zox_get_link(world, skeleton, ActiveMesh) : 0;
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    if (!zox_valid(source) || !zox_valid(head)) {
        return 0;
    }
    const float3 at = { randf_range(-100, 100), -200, randf_range(-100, 100) };
    const entity copy = spawn_mesh3_clone(world, source);
    if (!copy) {
        return 0;
    }
    zox_add(copy, VoxMesh);
    zox_set_unique_name(copy, "mugshot_mesh");
    zox_setv(copy, Position3D, at);
    zox_setv(copy, Rotation3D, quaternion_identity);
    // in front of its face, framing its head (bone sizes are half sizes)
    const float3 head_at = float3_add(at, zox_getv(head, BonePosition));
    const float3 head_size = zox_getv(head, BoneSize);
    const float3 camera_at = float3_add(head_at, (float3) { 0, head_size.y * 0.3f, fmaxf(head_size.x, head_size.y) * 4 });
    // alpha under 1: the alpha render texture material (the opaque one shows nothing, todo), the gray behind it clear
    const entity frame = spawn_render_frame(world, parent, anchor, position, size, copy, camera_at, quaternion_identity, 1, 0.95f);
    zox_set_unique_name(frame, "mugshot");
    zox_set_parent(world, copy, frame);
    return frame;
}
