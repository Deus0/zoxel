// tst: held_view, a held block from first person, to look at and checked (actions/sys/hold.c, bones/ik/sys/swing.c)
//  - holds a plain block (a cube, as held_block), raised; first person looking down 25, level, up 50: a screenshot each,
//    tst/held_view_down.ppm, held_view_level.ppm, held_view_up.ppm (--nohud from tst.sh)
//  - at each: the block in the view, on the hand's inner side (nearer the view's middle than the hand: it was
//    behind the hand, out of sight), clear of the view's middle (its nearest edge, it covered half the view), and upright in it (its up within 35 degrees of the view's up)
//  - run: bash bsh/tst.sh held_view, then look at the three
#define zox_tst_hv_views 3
const float zox_tst_hv_pitches[zox_tst_hv_views] = { -25, 0, 50 };
const char* zox_tst_hv_names[zox_tst_hv_views] = { "down", "level", "up" };
byte zox_tst_hv_view = 0;
byte zox_tst_hv_shot = 0;
double zox_tst_hv_time = 0;
float3 zox_tst_hv_block[zox_tst_hv_views];   // the block's middle in the camera's space (it faces -z)
float3 zox_tst_hv_hand[zox_tst_hv_views];    // the hand's
float3 zox_tst_hv_up[zox_tst_hv_views];      // the block's up in the camera's space
float zox_tst_hv_size[zox_tst_hv_views];     // the block's half diagonal, its reach round its middle
byte zox_tst_hv_measured = 0;

byte zox_tst_held_view_ready(ecs* world) {
    // held_item's steps: a block given, selected, raised
    zox_tst_held_block = 1;
    zox_tst_held_cube = 1;
    if (zox_tst_held_stage < 3) {
        zox_tst_held_item_ready(world);
        zox_tst_hv_time = zox_current_time;
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    const entity hand = zox_valid(skeleton) ? zox_get_link(world, skeleton, HandBoneLink) : 0;
    const entity held = zox_valid(hand) ? zox_get_child_by_id(world, hand, zox_id(HeldAction)) : 0;
    zox_tst_progress(zox_tst_hv_view * 100 + (int) ((zox_current_time - zox_tst_hv_time) * 10));
    if (!zox_valid(camera) || !zox_valid(held)) {
        zox_tst_waiting = "the held block and the first person camera";
        return 0;
    }
    if (zox_tst_hv_view >= zox_tst_hv_views) {
        return !zox_screenshot_request[0];
    }
    zox_tst_waiting = "the held block at each look";
    const float pitch = zox_tst_hv_pitches[zox_tst_hv_view] * degrees_to_radians;
    zox_setv(camera, LocalRotation3D, euler_to_quaternion((float3) { pitch, is_camera_positive_z ? M_PI : 0, 0 }));
    if (zox_screenshot_request[0] || zox_current_time - zox_tst_hv_time < 1.5) {
        return 0;
    }
    // measured, a screenshot taken, the next
    const float4 inverse = quaternion_inverse(zox_getv(camera, Rotation3D));
    const float3 eye = zox_getv(camera, Position3D);
    const byte v = zox_tst_hv_view;
    // its mesh's middle (its vertices', turned and scaled as it's held), not its origin
    const MeshVertices* vertices = zox_get(held, MeshVertices);
    float3 low = float3_single(1000000), high = float3_single(-1000000);
    for (int i = 0; i < vertices->length; i++) {
        low = (float3) { fminf(low.x, vertices->value[i].x), fminf(low.y, vertices->value[i].y), fminf(low.z, vertices->value[i].z) };
        high = (float3) { fmaxf(high.x, vertices->value[i].x), fmaxf(high.y, vertices->value[i].y), fmaxf(high.z, vertices->value[i].z) };
    }
    const float3 middle = float3_add(zox_getv(held, Position3D), float4_rotate_float3(zox_getv(held, Rotation3D),
        float3_scale(float3_mid(low, high), zox_getv(held, LocalScale1))));
    zox_tst_hv_block[v] = float4_rotate_float3(inverse, float3_sub(middle, eye));
    zox_tst_hv_size[v] = float3_length(float3_scale(float3_sub(high, low), zox_getv(held, LocalScale1))) / 2;
    zox_tst_hv_hand[v] = float4_rotate_float3(inverse, float3_sub(bone_centre_now(world, hand), eye));
    zox_tst_hv_up[v] = float4_rotate_float3(quaternion_rotate(inverse, zox_getv(held, Rotation3D)), float3_up);
    zox_tst_hv_measured |= 1 << v;
    char name[64];
    snprintf(name, sizeof(name), "held_view_%s.ppm", zox_tst_hv_names[v]);
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(name));
    zox_tst_hv_view++;
    zox_tst_hv_time = zox_current_time;
    return 0;
}

void zox_tst_held_view_check(ecs* world) {
    (void) world;
    for (byte v = 0; v < zox_tst_hv_views; v++) {
        const float3 b = zox_tst_hv_block[v], h = zox_tst_hv_hand[v], u = zox_tst_hv_up[v];
        zox_log("[tst] held_view [%s]: block [%.3f %.3f %.3f] hand [%.3f %.3f %.3f] the block's up [%.2f %.2f %.2f]",
            zox_tst_hv_names[v], b.x, b.y, b.z, h.x, h.y, h.z, u.x, u.y, u.z);
        if (!(zox_tst_hv_measured & (1 << v))) {
            zox_tst_check(0, "[%s]: never measured", zox_tst_hv_names[v]);
            continue;
        }
        // in view (ahead, on a 16:9 screen), nearer the view's middle than the hand, upright
        zox_tst_check(b.z < 0 && fabsf(b.x) < -b.z * 1.6f && fabsf(b.y) < -b.z, "[%s]: the block is out of view [%.3f %.3f %.3f]",
            zox_tst_hv_names[v], b.x, b.y, b.z);
        zox_tst_check(b.x / -b.z < h.x / -h.z, "[%s]: the block is behind the hand, not on its inner side (block [%.2f] hand [%.2f] across)",
            zox_tst_hv_names[v], b.x / -b.z, h.x / -h.z);
        // it doesn't block the view: its nearest edge well off its middle (at 0.7 of the reach out it covered half
        //  the view, unusable)
        const float across = sqrtf(b.x * b.x + b.y * b.y) / -b.z, span = zox_tst_hv_size[v] / -b.z;
        zox_log("[tst] held_view [%s]: its middle [%.2f] off the view's, its reach [%.2f] (tan)", zox_tst_hv_names[v], across, span);
        zox_tst_check(across - span > 0.3f, "[%s]: the block covers the view, [%.2f] off its middle reaching [%.2f]",
            zox_tst_hv_names[v], across, span);
        zox_tst_check(u.y > 0.82f, "[%s]: the block isn't upright, its up [%.2f %.2f %.2f]", zox_tst_hv_names[v], u.x, u.y, u.z);
    }
}
