// a bone's paint, as bone.vert paints it (Paint Weights): hues spread by the golden ratio, so neighbouring bones differ
static inline color_rgb bone_paint(int bone) {
    const float hue = fmodf(bone * 0.618034f, 1);
    const float offsets[3] = { 0, 2.0f / 3.0f, 1.0f / 3.0f };
    byte rgb[3];
    for (int k = 0; k < 3; k++) {
        const float channel = fabsf(fmodf(hue + offsets[k], 1) * 6 - 3) - 1;
        rgb[k] = (byte) ((clampf(channel, 0, 1) * 0.8f + 0.2f) * 255);
    }
    return (color_rgb) { rgb[0], rgb[1], rgb[2] };
}

// Bone gizmos (the bones debug view, fun/debug.c): a line from each bone's joint to its parent's, a box round its part
//  turned with it, in its paint (the skin painted by bone under it matches)
void bone_gizmo_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(BoneSize);
    for (int i = 0; i < it->count && render_bones; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(BoneSize, size);
        zox_sys_e();
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent)) {
            continue;
        }
        spawn_line3c(world, position->value, zox_getv(parent, Position3D), 4, 0.01, color_rgb_white);
        // its index in its skeleton's bones, the shader's
        const entity skeleton = zox_get_link(world, e, SkeletonLink);
        const BoneLinks* bones = zox_valid(skeleton) ? zox_get(skeleton, BoneLinks) : NULL;
        int index = 0;
        for (int j = 0; bones && j < bones->length; j++) {
            if (bones->value[j] == e) {
                index = j;
                break;
            }
        }
        const float4 rotation = zox_getv(e, Rotation3D);
        const float3 centre = float3_add(position->value, float4_rotate_float3(rotation,
            float3_sub(zox_getv(e, BoneCentre), zox_getv(e, BonePosition))));
        const entity box = spawn_cube_lines(world, centre, size->value, 2, bone_paint(index), 0.01);
        zox_setv(box, Rotation3D, rotation);
    }
    zox_sys_on_end();
} zoxd_system(bone_gizmo_system);
