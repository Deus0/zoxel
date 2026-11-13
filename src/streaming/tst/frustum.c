// tst: camera_frustum, which boxes a camera sees (camera_sees_bounds, the chunk_frustum_system check)
//  - planes and bounds made the way the camera systems make them (view_projection_matrix_system)
//  - known boxes: ahead, behind, past the side, over the side's edge, past the far plane, around the camera,
//      and a camera with its own short far distance
//  - seeded random cameras (camera_look_at) and points, against where the view projection puts them:
//      a seen point culled is a chunk that vanishes on screen (the bad one), a hidden point kept is just work
//  - pure logic, on the main menu; run: bash bsh/tst.sh camera_frustum

typedef struct {
    plane planes[zox_camera_planes];
    float6 bounds;
    float4x4 view_projection;
} zox_tst_frustum;

zox_tst_frustum zox_tst_frustum_make(float3 position, float4 rotation, float aspect, float fov, float far) {
    zox_tst_frustum f;
    f.view_projection = zox_tst_camera_view_projection(position, rotation, aspect, fov, far);
    double3 corners[8];
    calculate_frustum_corners_d3(f.view_projection, corners);
    calculate_frustum_bounds_d3(corners, &f.bounds);
    frustum_to_planes_d3(corners, f.planes);
    return f;
}

static inline byte zox_tst_frustum_sees(const zox_tst_frustum* f, float3 centre, float extent) {
    return camera_sees_bounds(f->planes, f->bounds, (bounds) { centre, float3_single(extent) });
}

byte zox_tst_camera_frustum_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_camera_frustum_check(ecs* world) {
    (void) world;
    const float aspect = 16 / 9.0f;
    const float fov = 60;
    // looking down -z from the origin
    const zox_tst_frustum f = zox_tst_frustum_make(float3_zero, quaternion_identity, aspect, fov, camera_far_distance);
    const float half_width = 10 * tanf(fov * 0.5f * degreesToRadians) * aspect;   // at 10 ahead
    zox_tst_check(zox_tst_frustum_sees(&f, (float3) { 0, 0, -10 }, 0.5f), "a box ahead isn't seen");
    zox_tst_check(!zox_tst_frustum_sees(&f, (float3) { 0, 0, 10 }, 0.5f), "a box behind is seen");
    zox_tst_check(!zox_tst_frustum_sees(&f, (float3) { -half_width * 3, 0, -10 }, 0.5f), "a box past the left is seen");
    zox_tst_check(!zox_tst_frustum_sees(&f, (float3) { 0, 20, -10 }, 0.5f), "a box above the view is seen");
    zox_tst_check(zox_tst_frustum_sees(&f, (float3) { -half_width - 0.4f, 0, -10 }, 0.5f), "a box over the left edge isn't seen");
    zox_tst_check(zox_tst_frustum_sees(&f, (float3) { 0, 0, 0 }, 100), "a box around the camera isn't seen");
    zox_tst_check(zox_tst_frustum_sees(&f, (float3) { 0, 0, -(camera_far_distance - 10) }, 1), "a box before the far plane isn't seen");
    zox_tst_check(!zox_tst_frustum_sees(&f, (float3) { 0, 0, -(camera_far_distance + 50) }, 1), "a box past the far plane is seen");
    // a camera's own far distance culls (CameraFarDistance, i.e. a short sighted minimap or mirror camera)
    const zox_tst_frustum near_sighted = zox_tst_frustum_make(float3_zero, quaternion_identity, aspect, fov, 50);
    zox_tst_check(zox_tst_frustum_sees(&near_sighted, (float3) { 0, 0, -40 }, 1), "a far 50 camera doesn't see a box 40 ahead");
    zox_tst_check(!zox_tst_frustum_sees(&near_sighted, (float3) { 0, 0, -80 }, 1), "a far 50 camera sees a box 80 ahead");
    // random cameras and points, against the projection
    uint seed = 777;
    int missed = 0;
    int kept = 0;
    int seen_count = 0;
    int tested = 0;
    float3 missed_example = float3_zero;
    for (int c = 0; c < 40; c++) {
        float values[6];
        for (int v = 0; v < 6; v++) {
            seed ^= seed << 13;
            seed ^= seed >> 17;
            seed ^= seed << 5;
            values[v] = (float) (seed % 20001) / 10000.0f - 1.0f;  // -1 to 1
        }
        const float3 position = { values[0] * 100, values[1] * 100, values[2] * 100 };
        const float3 target = float3_add(position, (float3) { values[3], values[4] * 0.8f, values[5] });
        const zox_tst_frustum fc = zox_tst_frustum_make(position, camera_look_at(position, target), aspect, fov, camera_far_distance);
        const float3 forward = float3_normalize(float3_subtract(target, position));
        for (int p = 0; p < 400; p++) {
            float3 offset;
            for (int v = 0; v < 3; v++) {
                seed ^= seed << 13;
                seed ^= seed >> 17;
                seed ^= seed << 5;
                const float value = (float) (seed % 20001) / 10000.0f - 1.0f;
                if (v == 0) offset.x = value;
                if (v == 1) offset.y = value;
                if (v == 2) offset.z = value;
            }
            // mostly ahead, out to past the far plane
            offset = float3_add(float3_scale(offset, 300), float3_scale(forward, 400));
            const float3 point = float3_add(position, offset);
            const float4 seen = camera_project_point(fc.view_projection, point);
            const float depth = float3_dot(offset, forward);
            // skip points near an edge, either answer is fine there
            const float margin = 0.05f;
            if (fabsf(fabsf(seen.x) - 1) < margin || fabsf(fabsf(seen.y) - 1) < margin ||
                fabsf(depth - camera_far_distance) < camera_far_distance * margin || fabsf(depth) < 1) {
                continue;
            }
            const byte visible = seen.w > 0 && fabsf(seen.x) < 1 && fabsf(seen.y) < 1 && depth < camera_far_distance;
            const byte sees = zox_tst_frustum_sees(&fc, point, 0.01f);
            tested++;
            seen_count += visible;
            if (visible && !sees) {
                missed++;
                missed_example = point;
            } else if (!visible && sees) {
                kept++;
            }
        }
    }
    zox_log("[tst] frustum: [%i] points, [%i] in view, [%i] culled while seen, [%i] kept while hidden",
        tested, seen_count, missed, kept);
    zox_tst_check(seen_count > 100, "only [%i] random points were in view, the test isn't testing", seen_count);
    zox_tst_check(!missed, "[%i] points in view were culled (i.e. [%.1f %.1f %.1f]): chunks would vanish",
        missed, missed_example.x, missed_example.y, missed_example.z);
    zox_tst_check(!kept, "[%i] points out of view were kept", kept);
}

void zox_tsts_camera_frustum() {
    zox_tst_add("camera_frustum", zox_tst_camera_frustum_ready, zox_tst_camera_frustum_check, 30);
    zox_tst_set_setup("camera_frustum", NULL, 1);
}
